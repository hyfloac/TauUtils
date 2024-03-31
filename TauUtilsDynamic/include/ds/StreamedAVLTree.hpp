#pragma once

#include "Objects.hpp"
#include "NumTypes.hpp"
#include "allocator/TauAllocator.hpp"
#include "allocator/FixedBlockAllocator.hpp"
#include "allocator/PageAllocator.hpp"
#include "TreeUtils.hpp"

#ifndef SAVL_USE_TRACKING
  #define SAVL_USE_TRACKING !defined(TAU_PRODUCTION)
#endif

template<typename T, typename IndexT, typename HeightT>
class StreamedAVLTreeAllocator final
{
    DELETE_CM(StreamedAVLTreeAllocator);
public:
    static constexpr IndexT INVALID_VALUE = static_cast<IndexT>(-1);
    static constexpr IndexT DESTROYED_VALUE = static_cast<IndexT>(0xCCCCCCCCCCCCCCCC);
private:
    uSys m_AllocPages;

    uSys m_BranchReservedPages;
    uSys m_HeightReservedPages;
    uSys m_ValueReservedPages;

    uSys m_BranchCommittedPages;
    uSys m_HeightCommittedPages;
    uSys m_ValueCommittedPages;

    uSys m_AllocIndex;
    uSys m_FirstFree;
    uSys m_LastFree;

    IndexT* m_LeftTree;
    IndexT* m_RightTree;
    HeightT* m_HeightTree;
    T* m_ValueTree;
public:
    StreamedAVLTreeAllocator(const uSys maxElements, const uSys allocPages = 4) noexcept
        : m_AllocPages(nextPowerOf2(allocPages))
        , m_BranchReservedPages(AlignTo((maxElements * sizeof(IndexT)) / PageAllocator::PageSize() + 1, m_AllocPages))
        , m_HeightReservedPages(AlignTo((maxElements * sizeof(HeightT)) / PageAllocator::PageSize() + 1, m_AllocPages))
        , m_ValueReservedPages(AlignTo((maxElements * sizeof(T)) / PageAllocator::PageSize() + 1, m_AllocPages))
        , m_BranchCommittedPages(0)
        , m_HeightCommittedPages(0)
        , m_ValueCommittedPages(0)
        , m_AllocIndex(0)
        , m_FirstFree(INVALID_VALUE)
        , m_LastFree(INVALID_VALUE)
        , m_LeftTree(reinterpret_cast<IndexT*>(PageAllocator::Reserve(m_BranchReservedPages)))
        , m_RightTree(reinterpret_cast<IndexT*>(PageAllocator::Reserve(m_BranchReservedPages)))
        , m_HeightTree(reinterpret_cast<HeightT*>(PageAllocator::Reserve(m_HeightReservedPages)))
        , m_ValueTree(reinterpret_cast<T*>(PageAllocator::Reserve(m_ValueReservedPages)))
    { }

    ~StreamedAVLTreeAllocator() noexcept
    {
        PageAllocator::Free(m_LeftTree);
        PageAllocator::Free(m_RightTree);
        PageAllocator::Free(m_HeightTree);
        PageAllocator::Free(m_ValueTree);
    }

    [[nodiscard]] IndexT*    LeftTree() noexcept { return m_LeftTree;   }
    [[nodiscard]] IndexT*   RightTree() noexcept { return m_RightTree;  }
    [[nodiscard]] HeightT* HeightTree() noexcept { return m_HeightTree; }
    [[nodiscard]] T*        ValueTree() noexcept { return m_ValueTree;  }

    [[nodiscard]] const IndexT*    LeftTree() const noexcept { return m_LeftTree;   }
    [[nodiscard]] const IndexT*   RightTree() const noexcept { return m_RightTree;  }
    [[nodiscard]] const HeightT* HeightTree() const noexcept { return m_HeightTree; }
    [[nodiscard]] const T*        ValueTree() const noexcept { return m_ValueTree;  }

    [[nodiscard]] uSys Allocate() noexcept
    {
        if(m_FirstFree != INVALID_VALUE)
        {
            const uSys ret = m_FirstFree;
            if(m_FirstFree == m_LastFree)
            {
                m_FirstFree = INVALID_VALUE;
                m_LastFree = INVALID_VALUE;
            }
            else
            { m_FirstFree = m_LeftTree[m_FirstFree]; }

            return ret;
        }

        if(!AssertSize())
        { return INVALID_VALUE; }

        return m_AllocIndex++;
    }

    void Deallocate(const uSys index) noexcept
    {
        if(index == INVALID_VALUE)
        { return; }

        m_ValueTree[index].~_T();

#if !defined(TAU_PRODUCITON)
        m_LeftTree[index] = DESTROYED_VALUE;
        m_RightTree[index] = DESTROYED_VALUE;
#endif

        if(index == m_AllocIndex)
        {
            --m_AllocIndex;
            AttemptRelease();
            return;
        }

        if(m_LastFree != INVALID_VALUE)
        {
            LeftTree()[m_LastFree] = index;
            m_LastFree = index;
            return;
        }

        m_FirstFree = index;
        m_LastFree = index;
    }
private:
    [[nodiscard]] bool AssertSize() noexcept
    {
        {
            const uSys branchPageBytes = m_BranchCommittedPages * PageAllocator::PageSize();
            if(m_AllocIndex + sizeof(IndexT) > branchPageBytes)
            {
                if(m_BranchCommittedPages == m_BranchReservedPages)
                { return false; }

                (void) PageAllocator::CommitPages(reinterpret_cast<u8*>(m_LeftTree) + branchPageBytes, m_AllocPages);
                (void) PageAllocator::CommitPages(reinterpret_cast<u8*>(m_RightTree) + branchPageBytes, m_AllocPages);
                m_BranchCommittedPages += m_AllocPages;
            }
        }

        {
            const uSys heightPageBytes = m_HeightCommittedPages * PageAllocator::PageSize();
            if(m_AllocIndex + sizeof(HeightT) > heightPageBytes)
            {
                if(m_HeightCommittedPages == m_HeightReservedPages)
                { return false; }

                (void) PageAllocator::CommitPages(reinterpret_cast<u8*>(m_HeightTree) + heightPageBytes, m_AllocPages);
                m_HeightCommittedPages += m_AllocPages;
            }
        }

        {
            const uSys valuePageBytes = m_ValueCommittedPages * PageAllocator::PageSize();
            if(m_AllocIndex + sizeof(T) > valuePageBytes)
            {
                if(m_ValueCommittedPages == m_ValueReservedPages)
                { return false; }

                (void) PageAllocator::CommitPages(reinterpret_cast<u8*>(m_ValueTree) + valuePageBytes, m_AllocPages);
                m_ValueCommittedPages += m_AllocPages;
            }
        }

        return true;
    }

    void AttemptRelease() noexcept
    {
        // If there are `_allocPages` empty pages, release the block of pages.
        {
            const uSys branchPageBytes = (m_BranchCommittedPages - m_AllocPages) * PageAllocator::PageSize();
            if(branchPageBytes - sizeof(IndexT) <= (PageAllocator::PageSize() * m_AllocPages))
            {
                PageAllocator::DecommitPages(reinterpret_cast<u8*>(m_LeftTree) + branchPageBytes, m_AllocPages);
                PageAllocator::DecommitPages(reinterpret_cast<u8*>(m_RightTree) + branchPageBytes, m_AllocPages);
                m_BranchCommittedPages -= m_AllocPages;
            }
        }

        {
            const uSys heightPageBytes = (m_HeightCommittedPages - m_AllocPages) * PageAllocator::PageSize();
            if(heightPageBytes - sizeof(HeightT) <= (PageAllocator::PageSize() * m_AllocPages))
            {
                PageAllocator::DecommitPages(reinterpret_cast<u8*>(m_HeightTree) + heightPageBytes, m_AllocPages);
                m_HeightCommittedPages -= m_AllocPages;
            }
        }

        {
            const uSys valuePageBytes = (m_ValueCommittedPages - m_AllocPages) * PageAllocator::PageSize();
            if(valuePageBytes - sizeof(T) <= (PageAllocator::PageSize() * m_AllocPages))
            {
                PageAllocator::DecommitPages(reinterpret_cast<u8*>(m_ValueTree) + valuePageBytes, m_AllocPages);
                m_ValueCommittedPages -= m_AllocPages;
            }
        }
    }
};

template<typename T, typename IndexT, typename HeightT = IndexT, InsertMethod InsertMethod = InsertMethod::Ignore>
class StreamedAVLTree final
{
    DELETE_CM(StreamedAVLTree);
public:
#if SAVL_USE_TRACKING
    using FBAllocator = FixedBlockAllocator<AllocationTracking::DoubleDeleteCount>;
#else
    using FBAllocator = FixedBlockAllocator<AllocationTracking::None>;
#endif

    static constexpr IndexT INVALID_VALUE = static_cast<IndexT>(-1);
private:
    StreamedAVLTreeAllocator<T, IndexT, HeightT> m_Allocator;

    IndexT m_Root;
public:
    StreamedAVLTree(const uSys maxElements) noexcept
        : m_Allocator(maxElements)
        , m_Root(INVALID_VALUE)
    { }

    ~StreamedAVLTree() noexcept
    { DisposeTree(); }

    [[nodiscard]] IndexT Root() const noexcept { return m_Root; }

    [[nodiscard]] IndexT*    LeftTree() noexcept { return m_Allocator.LeftTree();   }
    [[nodiscard]] IndexT*   RightTree() noexcept { return m_Allocator.RightTree();  }
    [[nodiscard]] HeightT* HeightTree() noexcept { return m_Allocator.HeightTree(); }
    [[nodiscard]] T*        ValueTree() noexcept { return m_Allocator.ValueTree();  }

    [[nodiscard]] const IndexT*    LeftTree() const noexcept { return m_Allocator.LeftTree();   }
    [[nodiscard]] const IndexT*   RightTree() const noexcept { return m_Allocator.RightTree();  }
    [[nodiscard]] const HeightT* HeightTree() const noexcept { return m_Allocator.HeightTree(); }
    [[nodiscard]] const T*        ValueTree() const noexcept { return m_Allocator.ValueTree();  }

    template<typename Search>
    [[nodiscard]] T* Find(const Search& search) noexcept
    {
        const IndexT node = Find(m_Root, search);
        if(node == INVALID_VALUE)
        { return nullptr; }

        return &ValueTree()[node];
    }

    template<typename Search>
    [[nodiscard]] const T* Find(const Search& search) const noexcept
    {
        const IndexT node = Find(m_Root, search);
        if(node == INVALID_VALUE)
        { return nullptr; }
        return &ValueTree()[node];
    }

    template<typename Search>
    [[nodiscard]] T* FindClosestMatchAbove(const Search& search) noexcept
    {
        const IndexT node = FindClosestMatchAbove(m_Root, search);
        if(node == INVALID_VALUE)
        { return nullptr; }
        return &ValueTree()[node];
    }

    template<typename Search>
    [[nodiscard]] const T* FindClosestMatchAbove(const Search& search) const noexcept
    {
        const IndexT node = FindClosestMatchAbove(m_Root, search);
        if(node == INVALID_VALUE)
        { return nullptr; }
        return &ValueTree()[node];
    }

    template<typename Search>
    [[nodiscard]] T* FindClosestMatchBelow(const Search& search) noexcept
    {
        const IndexT node = FindClosestMatchBelow(m_Root, search);
        if(node == INVALID_VALUE)
        { return nullptr; }
        return &ValueTree()[node];
    }

    template<typename Search>
    [[nodiscard]] const T* FindClosestMatchBelow(const Search& search) const noexcept
    {
        const IndexT node = FindClosestMatchBelow(m_Root, search);
        if(node == INVALID_VALUE)
        { return nullptr; }
        return &ValueTree()[node];
    }
    
    template<typename Search>
    [[nodiscard]] uSys Get(const Search& search) const noexcept
    { return Find(m_Root, search); }

    template<typename Search>
    [[nodiscard]] uSys GetClosestMatchAbove(const Search& search) const noexcept
    { return FindClosestMatchAbove(m_Root, search); }

    template<typename Search>
    [[nodiscard]] uSys GetClosestMatchBelow(const Search& search) const noexcept
    { return FindClosestMatchBelow(m_Root, search); }

    void Insert(const T& value) noexcept
    {
        if(m_Root == INVALID_VALUE)
        {
            m_Root = m_Allocator.Allocate();
            LeftTree()[m_Root] = INVALID_VALUE;
            RightTree()[m_Root] = INVALID_VALUE;
            HeightTree()[m_Root] = 0;
            new(&ValueTree()[m_Root]) T(value);
        }
        else
        {
            const IndexT newNode = m_Allocator.Allocate();
            LeftTree()[newNode] = INVALID_VALUE;
            RightTree()[newNode] = INVALID_VALUE;
            HeightTree()[newNode] = 0;
            new(&ValueTree()[newNode]) T(value);

            m_Root = Insert(m_Root, newNode);
        }
    }

    void Insert(T&& value) noexcept
    {
        if(m_Root == INVALID_VALUE)
        {
            m_Root = m_Allocator.Allocate();
            LeftTree()[m_Root] = INVALID_VALUE;
            RightTree()[m_Root] = INVALID_VALUE;
            HeightTree()[m_Root] = 0;
            new(&ValueTree()[m_Root]) T(TauAllocatorUtils::Move(value));
        }
        else
        {
            const IndexT newNode = m_Allocator.Allocate();
            LeftTree()[newNode] = INVALID_VALUE;
            RightTree()[newNode] = INVALID_VALUE;
            HeightTree()[newNode] = 0;
            new(&ValueTree()[newNode]) T(TauAllocatorUtils::Move(value));

            m_Root = Insert(m_Root, newNode);
        }
    }
    
    template<typename... Args>
    void Emplace(Args&&... args) noexcept
    {
        if(m_Root == INVALID_VALUE)
        {
            m_Root = m_Allocator.Allocate();
            LeftTree()[m_Root] = INVALID_VALUE;
            RightTree()[m_Root] = INVALID_VALUE;
            HeightTree()[m_Root] = 0;
            new(&ValueTree()[m_Root]) T(TauAllocatorUtils::Forward<Args>(args)...);
        }
        else
        {
            const IndexT newNode = m_Allocator.Allocate();
            LeftTree()[newNode] = INVALID_VALUE;
            RightTree()[newNode] = INVALID_VALUE;
            HeightTree()[newNode] = 0;
            new(&ValueTree()[newNode]) T(TauAllocatorUtils::Forward<Args>(args)...);

            m_Root = Insert(m_Root, newNode);
        }
    }

    template<typename Search>
    void Remove(const Search& search) noexcept
    { m_Root = Remove(&m_Root, search); }

    void Remove(const IndexT search) noexcept
    { m_Root = Remove(&m_Root, search); }

    void DisposeTree() noexcept
    { DisposeTree(m_Root); }
private:
    template<typename Search>
    [[nodiscard]] IndexT Find(const IndexT tree, const Search& search) const noexcept
    {
        IndexT node = tree;

        while(node != INVALID_VALUE)
        {
            const T& value = ValueTree()[node];
            if(search == value)
            { return node; }

            if(search > value)
            { node = RightTree()[node]; }
            else
            { node = LeftTree()[node]; }
        }

        return INVALID_VALUE;
    }

    template<typename Search>
    [[nodiscard]] IndexT FindClosestMatchAbove(const IndexT tree, const Search& search) const noexcept
    {
        IndexT contender = tree;
        IndexT node = tree;

        const T* contenderValue = ValueTree()[contender];

        while(node != INVALID_VALUE)
        {
            const T& value = &ValueTree()[node];
            if(search == value)
            { return node; }

            if(search > value)
            { node = RightTree()[node]; }
            else
            {
                if(*contenderValue > value)
                {
                    contender = node;
                    contenderValue = value;
                }
                node = LeftTree()[node];
            }
        }

        return contender;
    }

    template<typename Search>
    [[nodiscard]] IndexT FindClosestMatchBelow(const IndexT tree, const Search& search) const noexcept
    {
        IndexT contender = tree;
        IndexT node = tree;
        
        const T* contenderValue = ValueTree()[contender];

        while(node != INVALID_VALUE)
        {
            const T& value = ValueTree()[node];
            if(search == *value)
            { return node; }

            if(search > value)
            {
                if(*contenderValue < value)
                {
                    contender = node;
                    contenderValue = value;
                }
                node = RightTree()[node];
            }
            else
            { node = LeftTree()[node]; }
        }

        return contender;
    }

    [[nodiscard]] IndexT RotateRight(const IndexT pivot) noexcept
    {
        const IndexT newRoot = LeftTree()[pivot];
        const IndexT transferNode = RightTree()[newRoot];

        RightTree()[newRoot] = pivot;
        LeftTree()[pivot] = transferNode;

        HeightTree()[pivot] = maxT(Height(LeftTree()[pivot]), Height(RightTree()[pivot])) + 1;
        HeightTree()[newRoot] = maxT(Height(LeftTree()[newRoot]), Height(RightTree()[newRoot])) + 1;

        return newRoot;
    }

    [[nodiscard]] IndexT RotateLeft(const IndexT pivot) noexcept
    {
        const IndexT newRoot = RightTree()[pivot];
        const IndexT transferNode = LeftTree()[newRoot];

        LeftTree()[newRoot] = pivot;
        RightTree()[pivot] = transferNode;

        HeightTree()[pivot] = maxT(Height(LeftTree()[pivot]), Height(RightTree()[pivot])) + 1;
        HeightTree()[newRoot] = maxT(Height(LeftTree()[newRoot]), Height(RightTree()[newRoot])) + 1;

        return newRoot;
    }

    [[nodiscard]] HeightT Height(const IndexT tree) const noexcept
    {
        if(tree == INVALID_VALUE)
        { return 0; }

        return HeightTree()[tree];
    }

    [[nodiscard]] int computeBalance(const IndexT tree) const noexcept
    {
        if(tree == INVALID_VALUE)
        { return 0; }

        return static_cast<int>(Height(LeftTree()[tree]) - Height(RightTree()[tree]));
    }

    [[nodiscard]] IndexT Insert(const IndexT tree, const IndexT newNode) noexcept
    {
        if(tree == INVALID_VALUE)
        { return newNode; }

        const T& newValue = ValueTree()[newNode];
        const T& treeValue = ValueTree()[tree];

        if(newValue < treeValue)
        { LeftTree()[tree] = Insert(LeftTree()[tree], newNode); }
        else if(newValue > treeValue)
        { RightTree()[tree] = Insert(RightTree()[tree], newNode); }
        else
        {
            if constexpr(InsertMethod == InsertMethod::Ignore)
            {
                m_Allocator.Deallocate(newNode);
                return tree;
            }
            else if constexpr(InsertMethod == InsertMethod::Replace)
            {
                LeftTree()[newNode] = LeftTree()[tree];
                RightTree()[newNode] = RightTree()[tree];
                HeightTree()[newNode] = HeightTree()[tree];
                m_Allocator.Deallocate(tree);
                return newNode;
            }
            else if constexpr(InsertMethod == InsertMethod::Greater)
            { RightTree()[tree] = Insert(RightTree()[tree], newNode); }
            else if constexpr(InsertMethod == InsertMethod::Lesser)
            { LeftTree()[tree] = Insert(LeftTree()[tree], newNode); }
            else
            { return tree; }
        }

        const IndexT leftBranch = LeftTree()[tree];
        const IndexT rightBranch = RightTree()[tree];

        const T* lValue = &ValueTree()[leftBranch];
        const T* rValue = &ValueTree()[rightBranch];

        HeightTree()[tree] = maxT(Height(leftBranch), Height(rightBranch)) + 1;

        const int balance = ComputeBalance(tree);

        // Left Left
        if(balance > 1 && newValue < *lValue)
        { return RotateRight(tree); }

        // Right Right
        if(balance < -1 && newValue > *rValue)
        { return RotateLeft(tree); }

        // Left Right
        if(balance > 1 && newValue > *lValue)
        {
            LeftTree()[tree] = RotateLeft(leftBranch);
            return RotateRight(tree);
        }
        
        // Right Left
        if(balance < -1 && newValue < *rValue)
        {
            RightTree()[tree] = RotateRight(rightBranch);
            return RotateLeft(tree);
        }

        return tree;
    }

    [[nodiscard]] IndexT* MinValueNode(IndexT* tree) noexcept
    {
        IndexT parent = INVALID_VALUE;
        IndexT current = *tree;
        while(LeftTree()[current] != INVALID_VALUE)
        {
            parent = current;
            current = LeftTree()[current];
        }

        if(parent == INVALID_VALUE)
        { return tree; }

        return &LeftTree()[parent];
    }

    template<typename Search>
    [[nodiscard]] IndexT Remove(IndexT* rootHolder, const Search& search) noexcept
    {
        if(!rootHolder || *rootHolder == INVALID_VALUE)
        { return INVALID_VALUE; }

        IndexT root = *rootHolder;

        T& rValue = ValueTree()[root];

        IndexT* rLeft = &LeftTree()[root];
        IndexT* rRight = &RightTree()[root];

        if(search < rValue)
        { *rLeft = Remove(rLeft, search); }
        else if(search > rValue)
        { *rRight = Remove(rRight, search);}
        else
        {
            if(*rLeft == INVALID_VALUE || *rRight == INVALID_VALUE)
            {
                IndexT tmp = *rLeft != INVALID_VALUE ? *rLeft : *rRight;
                if(tmp == INVALID_VALUE)
                { *rootHolder = INVALID_VALUE; }
                else
                { *rootHolder = tmp; }
                m_Allocator.Deallocate(root);
                root = *rootHolder;
            }
            else
            {
                IndexT* tmp = MinValueNode(rRight);

                *rootHolder = *tmp;                 // Replace root
                *tmp = RightTree()[*tmp];           // Store tmp's right branch in tmp's parent left branch
                LeftTree()[*rootHolder] = *rLeft;   // Set tmp's left branch to the old root's left branch
                RightTree()[*rootHolder] = *rRight; // Set tmp's right branch to the old root's right branch
                m_Allocator.Deallocate(root);        // Destroy root 
                root = *rootHolder;                 // Update the actual root variable
            }

            rLeft = &LeftTree()[root];
            rRight = &RightTree()[root];
        }

        if(root == INVALID_VALUE)
        { return INVALID_VALUE; }
        
        HeightTree()[root] = maxT(Height(*rLeft), Height(*rRight)) + 1;

        const int balance = ComputeBalance(root);

        // Left Left
        if(balance > 1 && ComputeBalance(*rLeft) >= 0)
        { return RotateRight(root); }

        // Left Right
        if(balance > 1 && ComputeBalance(*rLeft) < 0)
        {
            *rLeft = RotateLeft(*rLeft);
            return RotateRight(root);
        }

        // Right Right
        if(balance < -1 && ComputeBalance(*rRight) <= 0)
        { return RotateLeft(root); }

        // Right Left
        if(balance < -1 && ComputeBalance(*rRight) > 0)
        {
            *rRight = RotateRight(*rRight);
            return RotateLeft(root);
        }

        return root;
    }

    [[nodiscard]] IndexT Remove(IndexT* rootHolder, const IndexT search) noexcept
    {
        if(!rootHolder || *rootHolder == INVALID_VALUE)
        { return INVALID_VALUE; }

        IndexT root = *rootHolder;

        T& rValue = ValueTree()[root];

        IndexT* rLeft = &LeftTree()[root];
        IndexT* rRight = &RightTree()[root];

        if(search->value < rValue)
        { *rLeft = Remove(rLeft, search); }
        else if(search->value > rValue)
        { *rRight = Remove(rRight, search); }
        else if(search != root)
        {
            if constexpr(InsertMethod == InsertMethod::Ignore || InsertMethod == InsertMethod::Replace)
            { return root; }
            else if constexpr(InsertMethod == InsertMethod::Greater)
            { *rRight = Remove(rRight, search); }
            else if constexpr(InsertMethod == InsertMethod::Lesser)
            { *rLeft = Remove(rLeft, search); }
        }
        else
        {
            if(*rLeft == INVALID_VALUE || *rRight == INVALID_VALUE)
            {
                IndexT tmp = *rLeft != INVALID_VALUE ? *rLeft : *rRight;
                if(tmp == INVALID_VALUE)
                { *rootHolder = INVALID_VALUE; }
                else
                { *rootHolder = tmp; }
                m_Allocator.Deallocate(root);
                root = *rootHolder;
            }
            else
            {
                IndexT* tmp = MinValueNode(rRight);

                *rootHolder = *tmp;                 // Replace root
                *tmp = RightTree()[*tmp];           // Store tmp's right branch in tmp's parent left branch
                LeftTree()[*rootHolder] = *rLeft;   // Set tmp's left branch to the old root's left branch
                RightTree()[*rootHolder] = *rRight; // Set tmp's right branch to the old root's right branch
                m_Allocator.Deallocate(root);        // Destroy root 
                root = *rootHolder;                 // Update the actual root variable
            }
        }
        
        if(root == INVALID_VALUE)
        { return INVALID_VALUE; }
        
        HeightTree()[root] = maxT(Height(*rLeft), Height(*rRight)) + 1;

        const int balance = ComputeBalance(root);
        
        // Left Left
        if(balance > 1 && ComputeBalance(*rLeft) >= 0)
        { return RotateRight(root); }

        // Left Right
        if(balance > 1 && ComputeBalance(*rLeft) < 0)
        {
            *rLeft = RotateLeft(*rLeft);
            return RotateRight(root);
        }

        // Right Right
        if(balance < INVALID_VALUE && ComputeBalance(*rRight) <= 0)
        { return RotateLeft(root); }

        // Right Left
        if(balance < INVALID_VALUE && ComputeBalance(*rRight) > 0)
        {
            *rRight = RotateRight(*rRight);
            return RotateLeft(root);
        }

        return root;
    }

    void DisposeTree(const uSys tree) noexcept
    {
        if(tree == INVALID_VALUE)
        { return; }

        DisposeTree(LeftTree()[tree]);
        DisposeTree(RightTree()[tree]);

        m_Allocator.Deallocate(tree);
    }
};
