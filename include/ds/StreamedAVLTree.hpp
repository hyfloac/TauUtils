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
    uSys _allocPages;

    uSys _branchReservedPages;
    uSys _heightReservedPages;
    uSys _valueReservedPages;

    uSys _branchCommittedPages;
    uSys _heightCommittedPages;
    uSys _valueCommittedPages;

    uSys _allocIndex;
    uSys _firstFree;
    uSys _lastFree;

    IndexT* _leftTree;
    IndexT* _rightTree;
    HeightT* _heightTree;
    T* _valueTree;
public:
    StreamedAVLTreeAllocator(const uSys maxElements, const uSys allocPages = 4) noexcept
        : _allocPages(nextPowerOf2(allocPages))
        , _branchReservedPages(_alignTo((maxElements * sizeof(IndexT)) / PageAllocator::pageSize() + 1, _allocPages))
        , _heightReservedPages(_alignTo((maxElements * sizeof(HeightT)) / PageAllocator::pageSize() + 1, _allocPages))
        , _valueReservedPages(_alignTo((maxElements * sizeof(T)) / PageAllocator::pageSize() + 1, _allocPages))
        , _branchCommittedPages(0)
        , _heightCommittedPages(0)
        , _valueCommittedPages(0)
        , _allocIndex(0)
        , _firstFree(INVALID_VALUE)
        , _lastFree(INVALID_VALUE)
        , _leftTree(reinterpret_cast<IndexT*>(PageAllocator::reserve(_branchReservedPages)))
        , _rightTree(reinterpret_cast<IndexT*>(PageAllocator::reserve(_branchReservedPages)))
        , _heightTree(reinterpret_cast<HeightT*>(PageAllocator::reserve(_heightReservedPages)))
        , _valueTree(reinterpret_cast<T*>(PageAllocator::reserve(_valueReservedPages)))
    { }

    ~StreamedAVLTreeAllocator() noexcept
    {
        PageAllocator::free(_leftTree);
        PageAllocator::free(_rightTree);
        PageAllocator::free(_heightTree);
        PageAllocator::free(_valueTree);
    }

    [[nodiscard]] IndexT*    leftTree() noexcept { return _leftTree;   }
    [[nodiscard]] IndexT*   rightTree() noexcept { return _rightTree;  }
    [[nodiscard]] HeightT* heightTree() noexcept { return _heightTree; }
    [[nodiscard]] T*        valueTree() noexcept { return _valueTree;  }

    [[nodiscard]] const IndexT*    leftTree() const noexcept { return _leftTree;   }
    [[nodiscard]] const IndexT*   rightTree() const noexcept { return _rightTree;  }
    [[nodiscard]] const HeightT* heightTree() const noexcept { return _heightTree; }
    [[nodiscard]] const T*        valueTree() const noexcept { return _valueTree;  }

    [[nodiscard]] uSys allocate() noexcept
    {
        if(_firstFree != INVALID_VALUE)
        {
            const uSys ret = _firstFree;
            if(_firstFree == _lastFree)
            {
                _firstFree = INVALID_VALUE;
                _lastFree = INVALID_VALUE;
            }
            else
            { _firstFree = _leftTree[_firstFree]; }

            return ret;
        }

        if(!assertSize())
        { return INVALID_VALUE; }

        return _allocIndex++;
    }

    void deallocate(const uSys index) noexcept
    {
        if(index == INVALID_VALUE)
        { return; }

        _valueTree[index].~_T();

#if !defined(TAU_PRODUCITON)
        _leftTree[index] = DESTROYED_VALUE;
        _rightTree[index] = DESTROYED_VALUE;
#endif

        if(index == _allocIndex)
        {
            --_allocIndex;
            attemptRelease();
            return;
        }

        if(_lastFree != INVALID_VALUE)
        {
            leftTree()[_lastFree] = index;
            _lastFree = index;
            return;
        }

        _firstFree = index;
        _lastFree = index;
    }
private:
    [[nodiscard]] bool assertSize() noexcept
    {
        {
            const uSys branchPageBytes = _branchCommittedPages * PageAllocator::pageSize();
            if(_allocIndex + sizeof(IndexT) > branchPageBytes)
            {
                if(_branchCommittedPages == _branchReservedPages)
                { return false; }

                (void) PageAllocator::commitPages(reinterpret_cast<u8*>(_leftTree) + branchPageBytes, _allocPages);
                (void) PageAllocator::commitPages(reinterpret_cast<u8*>(_rightTree) + branchPageBytes, _allocPages);
                _branchCommittedPages += _allocPages;
            }
        }

        {
            const uSys heightPageBytes = _heightCommittedPages * PageAllocator::pageSize();
            if(_allocIndex + sizeof(HeightT) > heightPageBytes)
            {
                if(_heightCommittedPages == _heightReservedPages)
                { return false; }

                (void) PageAllocator::commitPages(reinterpret_cast<u8*>(_heightTree) + heightPageBytes, _allocPages);
                _heightCommittedPages += _allocPages;
            }
        }

        {
            const uSys valuePageBytes = _valueCommittedPages * PageAllocator::pageSize();
            if(_allocIndex + sizeof(T) > valuePageBytes)
            {
                if(_valueCommittedPages == _valueReservedPages)
                { return false; }

                (void) PageAllocator::commitPages(reinterpret_cast<u8*>(_valueTree) + valuePageBytes, _allocPages);
                _valueCommittedPages += _allocPages;
            }
        }

        return true;
    }

    void attemptRelease() noexcept
    {
        // If there are `_allocPages` empty pages, release the block of pages.
        {
            const uSys branchPageBytes = (_branchCommittedPages - _allocPages) * PageAllocator::pageSize();
            if(branchPageBytes - sizeof(IndexT) <= (PageAllocator::pageSize() * _allocPages))
            {
                (void) PageAllocator::decommitPages(reinterpret_cast<u8*>(_leftTree) + branchPageBytes, _allocPages);
                (void) PageAllocator::decommitPages(reinterpret_cast<u8*>(_rightTree) + branchPageBytes, _allocPages);
                _branchCommittedPages -= _allocPages;
            }
        }

        {
            const uSys heightPageBytes = (_heightCommittedPages - _allocPages) * PageAllocator::pageSize();
            if(heightPageBytes - sizeof(HeightT) <= (PageAllocator::pageSize() * _allocPages))
            {
                (void) PageAllocator::decommitPages(reinterpret_cast<u8*>(_heightTree) + heightPageBytes, _allocPages);
                _heightCommittedPages -= _allocPages;
            }
        }

        {
            const uSys valuePageBytes = (_valueCommittedPages - _allocPages) * PageAllocator::pageSize();
            if(valuePageBytes - sizeof(T) <= (PageAllocator::pageSize() * _allocPages))
            {
                (void) PageAllocator::decommitPages(reinterpret_cast<u8*>(_valueTree) + valuePageBytes, _allocPages);
                _valueCommittedPages -= _allocPages;
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
    StreamedAVLTreeAllocator<T, IndexT, HeightT> _allocator;

    IndexT _root;
public:
    StreamedAVLTree(const uSys maxElements) noexcept
        : _allocator(maxElements)
        , _root(INVALID_VALUE)
    { }

    ~StreamedAVLTree() noexcept
    { disposeTree(); }

    [[nodiscard]] IndexT root() const noexcept { return _root; }

    [[nodiscard]] IndexT*    leftTree() noexcept { return _allocator.leftTree();   }
    [[nodiscard]] IndexT*   rightTree() noexcept { return _allocator.rightTree();  }
    [[nodiscard]] HeightT* heightTree() noexcept { return _allocator.heightTree(); }
    [[nodiscard]] T*        valueTree() noexcept { return _allocator.valueTree();  }

    [[nodiscard]] const IndexT*    leftTree() const noexcept { return _allocator.leftTree();   }
    [[nodiscard]] const IndexT*   rightTree() const noexcept { return _allocator.rightTree();  }
    [[nodiscard]] const HeightT* heightTree() const noexcept { return _allocator.heightTree(); }
    [[nodiscard]] const T*        valueTree() const noexcept { return _allocator.valueTree();  }

    template<typename _Search>
    [[nodiscard]] T* find(const _Search& search) noexcept
    {
        const IndexT node = find(_root, search);
        if(node == INVALID_VALUE)
        { return nullptr; }
        return &valueTree()[node];
    }

    template<typename _Search>
    [[nodiscard]] const T* find(const _Search& search) const noexcept
    {
        const IndexT node = find(_root, search);
        if(node == INVALID_VALUE)
        { return nullptr; }
        return &valueTree()[node];
    }

    template<typename _SearchT>
    [[nodiscard]] T* findClosestMatchAbove(const _SearchT& search) noexcept
    {
        const IndexT node = findClosestMatchAbove(_root, search);
        if(node == INVALID_VALUE)
        { return nullptr; }
        return &valueTree()[node];
    }

    template<typename _SearchT>
    [[nodiscard]] const T* findClosestMatchAbove(const _SearchT& search) const noexcept
    {
        const IndexT node = findClosestMatchAbove(_root, search);
        if(node == INVALID_VALUE)
        { return nullptr; }
        return &valueTree()[node];
    }

    template<typename _SearchT>
    [[nodiscard]] T* findClosestMatchBelow(const _SearchT& search) noexcept
    {
        const IndexT node = findClosestMatchBelow(_root, search);
        if(node == INVALID_VALUE)
        { return nullptr; }
        return &valueTree()[node];
    }

    template<typename _SearchT>
    [[nodiscard]] const T* findClosestMatchBelow(const _SearchT& search) const noexcept
    {
        const IndexT node = findClosestMatchBelow(_root, search);
        if(node == INVALID_VALUE)
        { return nullptr; }
        return &valueTree()[node];
    }
    
    template<typename _Search>
    [[nodiscard]] uSys get(const _Search& search) const noexcept
    { return find(_root, search); }

    template<typename _SearchT>
    [[nodiscard]] uSys getClosestMatchAbove(const _SearchT& search) const noexcept
    { return findClosestMatchAbove(_root, search); }

    template<typename _SearchT>
    [[nodiscard]] uSys getClosestMatchBelow(const _SearchT& search) const noexcept
    { return findClosestMatchBelow(_root, search); }

    void insert(const T& value) noexcept
    {
        if(_root == INVALID_VALUE)
        {
            _root = _allocator.allocate();
            leftTree()[_root] = INVALID_VALUE;
            rightTree()[_root] = INVALID_VALUE;
            heightTree()[_root] = 0;
            new(&valueTree()[_root]) T(value);
        }
        else
        {
            const IndexT newNode = _allocator.allocate();
            leftTree()[newNode] = INVALID_VALUE;
            rightTree()[newNode] = INVALID_VALUE;
            heightTree()[newNode] = 0;
            new(&valueTree()[newNode]) T(value);

            _root = insert(_root, newNode);
        }
    }

    void insert(T&& value) noexcept
    {
        if(_root == INVALID_VALUE)
        {
            _root = _allocator.allocate();
            leftTree()[_root] = INVALID_VALUE;
            rightTree()[_root] = INVALID_VALUE;
            heightTree()[_root] = 0;
            new(&valueTree()[_root]) T(TauAllocatorUtils::_move(value));
        }
        else
        {
            const IndexT newNode = _allocator.allocate();
            leftTree()[newNode] = INVALID_VALUE;
            rightTree()[newNode] = INVALID_VALUE;
            heightTree()[newNode] = 0;
            new(&valueTree()[newNode]) T(TauAllocatorUtils::_move(value));

            _root = insert(_root, newNode);
        }
    }
    
    template<typename... _Args>
    void emplace(_Args&&... args) noexcept
    {
        if(_root == INVALID_VALUE)
        {
            _root = _allocator.allocate();
            leftTree()[_root] = INVALID_VALUE;
            rightTree()[_root] = INVALID_VALUE;
            heightTree()[_root] = 0;
            new(&valueTree()[_root]) T(TauAllocatorUtils::_forward<_Args>(args)...);
        }
        else
        {
            const IndexT newNode = _allocator.allocate();
            leftTree()[newNode] = INVALID_VALUE;
            rightTree()[newNode] = INVALID_VALUE;
            heightTree()[newNode] = 0;
            new(&valueTree()[newNode]) T(TauAllocatorUtils::_forward<_Args>(args)...);

            _root = insert(_root, newNode);
        }
    }

    template<typename _SearchT>
    void remove(const _SearchT& search) noexcept
    { _root = remove(&_root, search); }

    void remove(const IndexT search) noexcept
    { _root = remove(&_root, search); }

    void disposeTree() noexcept
    { disposeTree(_root); }
private:
    template<typename _SearchT>
    [[nodiscard]] IndexT find(const IndexT tree, const _SearchT& search) const noexcept
    {
        IndexT node = tree;

        while(node != INVALID_VALUE)
        {
            const T& value = valueTree()[node];
            if(search == value)
            { return node; }

            if(search > value)
            { node = rightTree()[node]; }
            else
            { node = leftTree()[node]; }
        }

        return INVALID_VALUE;
    }

    template<typename _SearchT>
    [[nodiscard]] IndexT findClosestMatchAbove(const IndexT tree, const _SearchT& search) const noexcept
    {
        IndexT contender = tree;
        IndexT node = tree;

        const T* contenderValue = valueTree()[contender];

        while(node != INVALID_VALUE)
        {
            const T& value = &valueTree()[node];
            if(search == value)
            { return node; }

            if(search > value)
            { node = rightTree()[node]; }
            else
            {
                if(*contenderValue > value)
                {
                    contender = node;
                    contenderValue = value;
                }
                node = leftTree()[node];
            }
        }

        return contender;
    }

    template<typename _SearchT>
    [[nodiscard]] IndexT findClosestMatchBelow(const IndexT tree, const _SearchT& search) const noexcept
    {
        IndexT contender = tree;
        IndexT node = tree;
        
        const T* contenderValue = valueTree()[contender];

        while(node != INVALID_VALUE)
        {
            const T& value = valueTree()[node];
            if(search == *value)
            { return node; }

            if(search > value)
            {
                if(*contenderValue < value)
                {
                    contender = node;
                    contenderValue = value;
                }
                node = rightTree()[node];
            }
            else
            { node = leftTree()[node]; }
        }

        return contender;
    }

    [[nodiscard]] IndexT rotateRight(const IndexT pivot) noexcept
    {
        const IndexT newRoot = leftTree()[pivot];
        const IndexT transferNode = rightTree()[newRoot];

        rightTree()[newRoot] = pivot;
        leftTree()[pivot] = transferNode;

        heightTree()[pivot] = maxT(height(leftTree()[pivot]), height(rightTree()[pivot])) + 1;
        heightTree()[newRoot] = maxT(height(leftTree()[newRoot]), height(rightTree()[newRoot])) + 1;

        return newRoot;
    }

    [[nodiscard]] IndexT rotateLeft(const IndexT pivot) noexcept
    {
        const IndexT newRoot = rightTree()[pivot];
        const IndexT transferNode = leftTree()[newRoot];

        leftTree()[newRoot] = pivot;
        rightTree()[pivot] = transferNode;

        heightTree()[pivot] = maxT(height(leftTree()[pivot]), height(rightTree()[pivot])) + 1;
        heightTree()[newRoot] = maxT(height(leftTree()[newRoot]), height(rightTree()[newRoot])) + 1;

        return newRoot;
    }

    [[nodiscard]] HeightT height(const IndexT tree) const noexcept
    {
        if(tree == INVALID_VALUE)
        { return 0; }
        return heightTree()[tree];
    }

    [[nodiscard]] int computeBalance(const IndexT tree) const noexcept
    {
        if(tree == INVALID_VALUE)
        { return 0; }
        return static_cast<int>(height(leftTree()[tree]) - height(rightTree()[tree]));
    }

    [[nodiscard]] IndexT insert(const IndexT tree, const IndexT newNode) noexcept
    {
        if(tree == INVALID_VALUE)
        { return newNode; }

        const T& newValue = valueTree()[newNode];
        const T& treeValue = valueTree()[tree];

        if(newValue < treeValue)
        { leftTree()[tree] = insert(leftTree()[tree], newNode); }
        else if(newValue > treeValue)
        { rightTree()[tree] = insert(rightTree()[tree], newNode); }
        else
        {
            if constexpr(InsertMethod == InsertMethod::Ignore)
            {
                _allocator.deallocate(newNode);
                return tree;
            }
            else if constexpr(InsertMethod == InsertMethod::Replace)
            {
                leftTree()[newNode] = leftTree()[tree];
                rightTree()[newNode] = rightTree()[tree];
                heightTree()[newNode] = heightTree()[tree];
                _allocator.deallocate(tree);
                return newNode;
            }
            else if constexpr(InsertMethod == InsertMethod::Greater)
            { rightTree()[tree] = insert(rightTree()[tree], newNode); }
            else if constexpr(InsertMethod == InsertMethod::Lesser)
            { leftTree()[tree] = insert(leftTree()[tree], newNode); }
            else
            { return tree; }
        }

        const IndexT leftBranch = leftTree()[tree];
        const IndexT rightBranch = rightTree()[tree];

        const T* lValue = &valueTree()[leftBranch];
        const T* rValue = &valueTree()[rightBranch];

        heightTree()[tree] = maxT(height(leftBranch), height(rightBranch)) + 1;

        const int balance = computeBalance(tree);

        // Left Left
        if(balance > 1 && newValue < *lValue)
        { return rotateRight(tree); }

        // Right Right
        if(balance < -1 && newValue > *rValue)
        { return rotateLeft(tree); }

        // Left Right
        if(balance > 1 && newValue > *lValue)
        {
            leftTree()[tree] = rotateLeft(leftBranch);
            return rotateRight(tree);
        }
        
        // Right Left
        if(balance < -1 && newValue < *rValue)
        {
            rightTree()[tree] = rotateRight(rightBranch);
            return rotateLeft(tree);
        }

        return tree;
    }

    [[nodiscard]] IndexT* minValueNode(IndexT* tree) noexcept
    {
        IndexT parent = INVALID_VALUE;
        IndexT current = *tree;
        while(leftTree()[current] != INVALID_VALUE)
        {
            parent = current;
            current = leftTree()[current];
        }

        if(parent == INVALID_VALUE)
        { return tree; }
        return &leftTree()[parent];
    }

    template<typename _SearchT>
    [[nodiscard]] IndexT remove(IndexT* rootHolder, const _SearchT& search) noexcept
    {
        if(!rootHolder || *rootHolder == INVALID_VALUE)
        { return INVALID_VALUE; }

        IndexT root = *rootHolder;

        T& rValue = valueTree()[root];

        IndexT* rLeft = &leftTree()[root];
        IndexT* rRight = &rightTree()[root];

        if(search < rValue)
        { *rLeft = remove(rLeft, search); }
        else if(search > rValue)
        { *rRight = remove(rRight, search);}
        else
        {
            if(*rLeft == INVALID_VALUE || *rRight == INVALID_VALUE)
            {
                IndexT tmp = *rLeft != INVALID_VALUE ? *rLeft : *rRight;
                if(tmp == INVALID_VALUE)
                { *rootHolder = INVALID_VALUE; }
                else
                { *rootHolder = tmp; }
                _allocator.deallocate(root);
                root = *rootHolder;
            }
            else
            {
                IndexT* tmp = minValueNode(rRight);

                *rootHolder = *tmp;                 // Replace root
                *tmp = rightTree()[*tmp];           // Store tmp's right branch in tmp's parent left branch
                leftTree()[*rootHolder] = *rLeft;   // Set tmp's left branch to the old root's left branch
                rightTree()[*rootHolder] = *rRight; // Set tmp's right branch to the old root's right branch
                _allocator.deallocate(root);        // Destroy root 
                root = *rootHolder;                 // Update the actual root variable
            }

            rLeft = &leftTree()[root];
            rRight = &rightTree()[root];
        }

        if(root == INVALID_VALUE)
        { return INVALID_VALUE; }
        
        heightTree()[root] = maxT(height(*rLeft), height(*rRight)) + 1;

        const int balance = computeBalance(root);

        // Left Left
        if(balance > 1 && computeBalance(*rLeft) >= 0)
        { return rotateRight(root); }

        // Left Right
        if(balance > 1 && computeBalance(*rLeft) < 0)
        {
            *rLeft = rotateLeft(*rLeft);
            return rotateRight(root);
        }

        // Right Right
        if(balance < -1 && computeBalance(*rRight) <= 0)
        { return rotateLeft(root); }

        // Right Left
        if(balance < -1 && computeBalance(*rRight) > 0)
        {
            *rRight = rotateRight(*rRight);
            return rotateLeft(root);
        }

        return root;
    }

    [[nodiscard]] IndexT remove(IndexT* rootHolder, const IndexT search) noexcept
    {
        if(!rootHolder || *rootHolder == INVALID_VALUE)
        { return INVALID_VALUE; }

        IndexT root = *rootHolder;

        T& rValue = valueTree()[root];

        IndexT* rLeft = &leftTree()[root];
        IndexT* rRight = &rightTree()[root];

        if(search->value < rValue)
        { *rLeft = remove(rLeft, search); }
        else if(search->value > rValue)
        { *rRight = remove(rRight, search); }
        else if(search != root)
        {
            if constexpr(InsertMethod == InsertMethod::Ignore || InsertMethod == InsertMethod::Replace)
            { return root; }
            else if constexpr(InsertMethod == InsertMethod::Greater)
            { *rRight = remove(rRight, search); }
            else if constexpr(InsertMethod == InsertMethod::Lesser)
            { *rLeft = remove(rLeft, search); }
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
                _allocator.deallocate(root);
                root = *rootHolder;
            }
            else
            {
                IndexT* tmp = minValueNode(rRight);

                *rootHolder = *tmp;                 // Replace root
                *tmp = rightTree()[*tmp];           // Store tmp's right branch in tmp's parent left branch
                leftTree()[*rootHolder] = *rLeft;   // Set tmp's left branch to the old root's left branch
                rightTree()[*rootHolder] = *rRight; // Set tmp's right branch to the old root's right branch
                _allocator.deallocate(root);        // Destroy root 
                root = *rootHolder;                 // Update the actual root variable
            }
        }
        
        if(root == INVALID_VALUE)
        { return INVALID_VALUE; }
        
        heightTree()[root] = maxT(height(*rLeft), height(*rRight)) + 1;

        const int balance = computeBalance(root);
        
        // Left Left
        if(balance > 1 && computeBalance(*rLeft) >= 0)
        { return rotateRight(root); }

        // Left Right
        if(balance > 1 && computeBalance(*rLeft) < 0)
        {
            *rLeft = rotateLeft(*rLeft);
            return rotateRight(root);
        }

        // Right Right
        if(balance < INVALID_VALUE && computeBalance(*rRight) <= 0)
        { return rotateLeft(root); }

        // Right Left
        if(balance < INVALID_VALUE && computeBalance(*rRight) > 0)
        {
            *rRight = rotateRight(*rRight);
            return rotateLeft(root);
        }

        return root;
    }

    void disposeTree(const uSys tree) noexcept
    {
        if(tree == INVALID_VALUE)
        { return; }

        disposeTree(leftTree()[tree]);
        disposeTree(rightTree()[tree]);

        _allocator.deallocate(tree);
    }
};
