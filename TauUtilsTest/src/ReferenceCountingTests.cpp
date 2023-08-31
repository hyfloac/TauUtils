#include <ReferenceCountingPointer.hpp>
#include <iostream>
#include <ConPrinter.hpp>
#include <TauUnit.hpp>

struct Dummy
{
    int _x, _y;

    Dummy(const int x, const int y)
        : _x(x)
        , _y(y)
    {
        ConPrinter::PrintLn(u"Initialized. ({}, {})", _x, _y);
    }

    ~Dummy()
    {
        ConPrinter::PrintLn(u"Destroyed. ({}, {})", _x, _y);
    }

    Dummy(const Dummy& copy)
        : _x(copy._x)
        , _y(copy._y)
    {
        ConPrinter::PrintLn(u"Copied. ({}, {})", _x, _y);
    }

    Dummy(Dummy&& move) noexcept
        : _x(move._x)
        , _y(move._y)
    {
        move._x = 0;
        move._y = 0;

        ConPrinter::PrintLn(u"Moved. ({}, {})", _x, _y);
    }

    Dummy& operator=(const Dummy& copy)
    {
        if(this == &copy)
        {
            return *this;
        }

        _x = copy._x;
        _y = copy._y;

        ConPrinter::PrintLn(u"Copy Assigned. ({}, {})", _x, _y);

        return *this;
    }

    Dummy& operator=(Dummy&& move) noexcept
    {
        if(this == &move)
        {
            return *this;
        }

        _x = move._x;
        _y = move._y;

        move._x = 0;
        move._y = 0;

        ConPrinter::PrintLn(u"Move Assigned. ({}, {})", _x, _y);

        return *this;
    }
};

void ReferenceCounterTestCreateDestroy()
{
    TAU_UNIT_TEST();
    
    {
        ReferenceCounter* pRefCounter;

        {
            ReferenceCounter refCounter;
            pRefCounter = &refCounter;

            ConPrinter::Print("RefCount PostCreate: {}\n", pRefCounter->RefCount());
        }

        ConPrinter::Print("RefCount PostDestroy: {}\n", pRefCounter->RefCount());
    }
}

void ReferenceCounterTestCreateFrom0Destroy()
{
    TAU_UNIT_TEST();

    {
        ReferenceCounter* pRefCounter;
        ReferenceCounter::Type* pRefCount = TU_NEW(ReferenceCounter::Type, 0);

        {
            ReferenceCounter refCounter(pRefCount);
            pRefCounter = &refCounter;

            ConPrinter::Print("RefCount PostCreate: {}\n", pRefCounter->RefCount());
        }

        ConPrinter::Print("RefCount PostDestroy: {}\n", pRefCounter->RefCount());
    }
}

void ReferenceCounterTestCreateFrom1Destroy()
{
    TAU_UNIT_TEST();

    {
        ReferenceCounter* pRefCounter;
        ReferenceCounter::Type* pRefCount = TU_NEW(ReferenceCounter::Type, 1);

        {
            ReferenceCounter refCounter(pRefCount);
            pRefCounter = &refCounter;

            ConPrinter::Print("RefCount PostCreate: {}\n", pRefCounter->RefCount());
        }

        ConPrinter::Print("RefCount PostDestroy: {}\n", pRefCounter->RefCount());

        TU_DELETE(pRefCount);
    }
}

void ReferenceCounterTestCreateFrom2Destroy()
{
    TAU_UNIT_TEST();

    {
        ReferenceCounter* pRefCounter;
        ReferenceCounter::Type* pRefCount = TU_NEW(ReferenceCounter::Type, 2);

        {
            ReferenceCounter refCounter(pRefCount);
            pRefCounter = &refCounter;

            ConPrinter::Print("RefCount PostCreate: {}\n", pRefCounter->RefCount());
        }

        ConPrinter::Print("RefCount PostDestroy: {}\n", pRefCounter->RefCount());

        TU_DELETE(pRefCount);
    }
}

void ReferenceCounterTestCreateCopy()
{
    TAU_UNIT_TEST();

    {
        ReferenceCounter* pRefCounter;
        {
            ReferenceCounter refCounter;
            pRefCounter = &refCounter;

            ConPrinter::Print("RefCount PostCreate: {}\n", pRefCounter->RefCount());

            {
                const ReferenceCounter copy(refCounter);  // NOLINT(performance-unnecessary-copy-initialization)
                ConPrinter::Print("RefCount copy PostCopy: {}\n", copy.RefCount());
                ConPrinter::Print("RefCount PostCopy: {}\n", pRefCounter->RefCount());
            }
            ConPrinter::Print("RefCount PostCopyDestroy: {}\n", pRefCounter->RefCount());
        }

        ConPrinter::Print("RefCount PostDestroy: {}\n", pRefCounter->RefCount());
    }
}

void ReferenceCounterTestCreateMove()
{
    TAU_UNIT_TEST();

    {
        ReferenceCounter* pRefCounter;
        {
            ReferenceCounter refCounter;
            pRefCounter = &refCounter;

            ConPrinter::Print("RefCount PostCreate: {}\n", pRefCounter->RefCount());
            ConPrinter::Print("pRefCount PostCreate: 0x{}\n", pRefCounter->RefCountPtr());

            {
                const ReferenceCounter move(::std::move(refCounter));
                ConPrinter::Print("RefCount move PostMove: {}\n", move.RefCount());
                ConPrinter::Print("RefCount PostMove: {}\n", pRefCounter->RefCount());
                ConPrinter::Print("pRefCount PostMove: 0x{}\n", pRefCounter->RefCountPtr());
            }
            ConPrinter::Print("RefCount PostMoveDestroy: {}\n", pRefCounter->RefCount());
        }

        ConPrinter::Print("RefCount PostDestroy: {}\n", pRefCounter->RefCount());
        ConPrinter::Print("pRefCount PostDestroy: 0x{}\n", pRefCounter->RefCountPtr());
    }
}

void ReferenceCounterTestCreateCopyAssign()
{
    TAU_UNIT_TEST();

    {
        ReferenceCounter* pRefCounter;
        {
            ReferenceCounter refCounter;
            pRefCounter = &refCounter;

            ConPrinter::Print("RefCount PostCreate: {}\n", pRefCounter->RefCount());

            {
                ReferenceCounter copy;
                ConPrinter::Print("RefCount copy PostCreate: {}\n", copy.RefCount());
                copy = refCounter;
                ConPrinter::Print("RefCount copy PostCopy: {}\n", copy.RefCount());
                ConPrinter::Print("RefCount PostCopy: {}\n", pRefCounter->RefCount());
            }
            ConPrinter::Print("RefCount PostCopyDestroy: {}\n", pRefCounter->RefCount());
        }

        ConPrinter::Print("RefCount PostDestroy: {}\n", pRefCounter->RefCount());
    }
}

void ReferenceCounterTestCreateMoveAssign()
{
    TAU_UNIT_TEST();

    {
        ReferenceCounter* pRefCounter;
        {
            ReferenceCounter refCounter;
            pRefCounter = &refCounter;

            ConPrinter::Print("RefCount PostCreate: {}\n", pRefCounter->RefCount());
            ConPrinter::Print("pRefCount PostCreate: 0x{}\n", pRefCounter->RefCountPtr());

            {
                ReferenceCounter move;
                ConPrinter::Print("RefCount move PostCreate: {}\n", move.RefCount());
                move = ::std::move(refCounter);
                ConPrinter::Print("RefCount move PostMove: {}\n", move.RefCount());
                ConPrinter::Print("RefCount PostMove: {}\n", pRefCounter->RefCount());
                ConPrinter::Print("pRefCount PostMove: 0x{}\n", pRefCounter->RefCountPtr());
            }
            ConPrinter::Print("RefCount PostMoveDestroy: {}\n", pRefCounter->RefCount());
        }

        ConPrinter::Print("RefCount PostDestroy: {}\n", pRefCounter->RefCount());
        ConPrinter::Print("pRefCount PostDestroy: 0x{}\n", pRefCounter->RefCountPtr());
    }
}

void ReferenceCounterTestCreateCopyAssignSelf()
{
    TAU_UNIT_TEST();

    {
        ReferenceCounter* pRefCounter;
        {
            ReferenceCounter refCounter;
            pRefCounter = &refCounter;

            ConPrinter::Print("RefCount PostCreate: {}\n", pRefCounter->RefCount());

            {
#pragma warning(push)
#pragma clang diagnostic ignored "-Wself-assign-overloaded"
                // ReSharper disable once CppIdenticalOperandsInBinaryExpression
                refCounter = refCounter;
#pragma warning(pop)
                ConPrinter::Print("RefCount PostCopy: {}\n", pRefCounter->RefCount());
            }
            ConPrinter::Print("RefCount PostCopyDestroy: {}\n", pRefCounter->RefCount());
        }

        ConPrinter::Print("RefCount PostDestroy: {}\n", pRefCounter->RefCount());
    }
}

void ReferenceCounterTestCreateMoveAssignSelf()
{
    TAU_UNIT_TEST();

    {
        ReferenceCounter* pRefCounter;
        {
            ReferenceCounter refCounter;
            pRefCounter = &refCounter;

            ConPrinter::Print("RefCount PostCreate: {}\n", pRefCounter->RefCount());
            ConPrinter::Print("pRefCount PostCreate: 0x{}\n", pRefCounter->RefCountPtr());

            {
#pragma warning(push)
#pragma clang diagnostic ignored "-Wself-move"
                refCounter = ::std::move(refCounter);
#pragma warning(pop)
                ConPrinter::Print("RefCount PostMove: {}\n", pRefCounter->RefCount());
                ConPrinter::Print("pRefCount PostMove: 0x{}\n", pRefCounter->RefCountPtr());
            }
            ConPrinter::Print("RefCount PostMoveDestroy: {}\n", pRefCounter->RefCount());
        }

        ConPrinter::Print("RefCount PostDestroy: {}\n", pRefCounter->RefCount());
        ConPrinter::Print("pRefCount PostDestroy: 0x{}\n", pRefCounter->RefCountPtr());
    }
}

void RefTest0()
{
    TAU_UNIT_TEST();
    
    {
        ReferenceCountingPointer<Dummy> ref(32, 17);
    }
}

void StrongRefTest0()
{
    TAU_UNIT_TEST();

    {
        StrongReferenceCountingPointer<Dummy> ref(32, 17);
    }
}

void WeakRefTest0()
{
    TAU_UNIT_TEST();

    {
        const StrongReferenceCountingPointer<Dummy> strong(32, 17);
        WeakReferenceCountingPointer<Dummy> weak = strong;
    }
}

static void DummyCopy(ReferenceCountingPointer<Dummy> ref)
{
    ConPrinter::PrintLn("Copy: ({}, {})", ref->_x, ref->_y);
}

static void DummyCopy(StrongReferenceCountingPointer<Dummy> ref)
{
    ConPrinter::PrintLn("Copy: ({}, {})", ref->_x, ref->_y);
}

static void DummyCopy(WeakReferenceCountingPointer<Dummy> ref)
{
    ConPrinter::PrintLn("Copy: ({}, {})", ref->_x, ref->_y);
}

static void DummyRefBase(const TReferenceCountingPointerBase<Dummy>& ref)
{
    ConPrinter::PrintLn("Reference: ({}, {})", ref->_x, ref->_y);
}

void RefTest1()
{
    TAU_UNIT_TEST();

    {
        const ReferenceCountingPointer<Dummy> ref(32, 17);
        ConPrinter::PrintLn("Passing as copy.");
        DummyCopy(ref);
        ConPrinter::PrintLn("Passing as reference.");
        DummyRefBase(ref);
    }
}

void StrongRefTest1()
{
    TAU_UNIT_TEST();

    {
        const StrongReferenceCountingPointer<Dummy> ref(32, 17);
        ConPrinter::PrintLn("Passing as copy.");
        DummyCopy(ref);
        ConPrinter::PrintLn("Passing as reference.");
        DummyRefBase(ref);
    }
}

void WeakRefTest1()
{
    TAU_UNIT_TEST();

    {
        const StrongReferenceCountingPointer<Dummy> strong(32, 17);
        const WeakReferenceCountingPointer<Dummy> weak = strong;
        ConPrinter::PrintLn("Passing as copy.");
        DummyCopy(weak);
        ConPrinter::PrintLn("Passing as reference.");
        DummyRefBase(weak);
    }
}

void StrongWeakRefTest0()
{
    TAU_UNIT_TEST();

    {
        WeakReferenceCountingPointer<Dummy> weak;
        {
            const StrongReferenceCountingPointer<Dummy> strong(32, 17);
            ConPrinter::PrintLn("Saving to weak.");
            weak = strong;
        }
        ConPrinter::PrintLn("After Scope.");
        ConPrinter::PrintLn("Weak is valid: {}", (weak ? "true" : "false"));
        ConPrinter::PrintLn("Strong Ref Count: {}", weak.StrongRefCount());
        ConPrinter::PrintLn("Weak Ref Count: {}", weak.WeakRefCount());
    }
}

void RefTests()
{
    ReferenceCounterTestCreateDestroy();
    ReferenceCounterTestCreateFrom0Destroy();
    ReferenceCounterTestCreateFrom1Destroy();
    ReferenceCounterTestCreateFrom2Destroy();
    ReferenceCounterTestCreateCopy();
    ReferenceCounterTestCreateMove();
    ReferenceCounterTestCreateCopyAssign();
    ReferenceCounterTestCreateMoveAssign();
    ReferenceCounterTestCreateCopyAssignSelf();
    ReferenceCounterTestCreateMoveAssignSelf();
    RefTest0();
    RefTest1();
    StrongRefTest0();
    StrongRefTest1();
    WeakRefTest0();
    WeakRefTest1();
    StrongWeakRefTest0();
}
