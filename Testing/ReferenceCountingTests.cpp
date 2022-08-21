#include <ReferenceCountingPointer.hpp>
#include <iostream>
#include <ConPrinter.hpp>

using namespace std;

struct Dummy
{
    int _x, _y;

    Dummy(const int x, const int y)
        : _x(x)
        , _y(y)
    {
        ConPrinter::Print(u"Initialized. ({}, {})\n", _x, _y);
    }

    ~Dummy()
    {
        ConPrinter::Print(u"Destroyed. ({}, {})\n", _x, _y);
    }

    Dummy(const Dummy& copy)
        : _x(copy._x)
        , _y(copy._y)
    {
        ConPrinter::Print(u"Copied. ({}, {})\n", _x, _y);
    }

    Dummy(Dummy&& move) noexcept
        : _x(move._x)
        , _y(move._y)
    {
        move._x = 0;
        move._y = 0;

        ConPrinter::Print(u"Moved. ({}, {})\n", _x, _y);
    }

    Dummy& operator=(const Dummy& copy)
    {
        if(this == &copy)
        {
            return *this;
        }

        _x = copy._x;
        _y = copy._y;

        ConPrinter::Print(u"Copy Assigned. ({}, {})\n", _x, _y);

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

        ConPrinter::Print(u"Move Assigned. ({}, {})\n", _x, _y);

        return *this;
    }
};

void ReferenceCounterTestCreateDestroy()
{
    ConPrinter::Print(u"Begin ReferenceCounterTestCreateDestroy\n");
    {
        ReferenceCounter* pRefCounter;

        {
            ReferenceCounter refCounter;
            pRefCounter = &refCounter;

            ConPrinter::Print("RefCount PostCreate: {}\n", pRefCounter->RefCount());
        }

        ConPrinter::Print("RefCount PostDestroy: {}\n", pRefCounter->RefCount());
    }
    ConPrinter::Print(u"End ReferenceCounterTestCreateDestroy\n");
}

void ReferenceCounterTestCreateFrom0Destroy()
{
    ConPrinter::Print(u"Begin ReferenceCounterTestCreateFrom0Destroy\n");
    {
        ReferenceCounter* pRefCounter;
        ReferenceCounter::Type* pRefCount = new(::std::nothrow) ReferenceCounter::Type(0);

        {
            ReferenceCounter refCounter(pRefCount);
            pRefCounter = &refCounter;

            ConPrinter::Print("RefCount PostCreate: {}\n", pRefCounter->RefCount());
        }

        ConPrinter::Print("RefCount PostDestroy: {}\n", pRefCounter->RefCount());
    }
    ConPrinter::Print(u"End ReferenceCounterTestCreateFrom0Destroy\n");
}

void ReferenceCounterTestCreateFrom1Destroy()
{
    ConPrinter::Print(u"Begin ReferenceCounterTestCreateFrom1Destroy\n");
    {
        ReferenceCounter* pRefCounter;
        ReferenceCounter::Type* pRefCount = new(::std::nothrow) ReferenceCounter::Type(1);

        {
            ReferenceCounter refCounter(pRefCount);
            pRefCounter = &refCounter;

            ConPrinter::Print("RefCount PostCreate: {}\n", pRefCounter->RefCount());
        }

        ConPrinter::Print("RefCount PostDestroy: {}\n", pRefCounter->RefCount());

        delete pRefCount;
    }
    ConPrinter::Print(u"End ReferenceCounterTestCreateFrom1Destroy\n");
}

void ReferenceCounterTestCreateFrom2Destroy()
{
    ConPrinter::Print(u"Begin ReferenceCounterTestCreateFrom2Destroy\n");
    {
        ReferenceCounter* pRefCounter;
        ReferenceCounter::Type* pRefCount = new(::std::nothrow) ReferenceCounter::Type(2);

        {
            ReferenceCounter refCounter(pRefCount);
            pRefCounter = &refCounter;

            ConPrinter::Print("RefCount PostCreate: {}\n", pRefCounter->RefCount());
        }

        ConPrinter::Print("RefCount PostDestroy: {}\n", pRefCounter->RefCount());

        delete pRefCount;
    }
    ConPrinter::Print(u"End ReferenceCounterTestCreateFrom2Destroy\n");
}

void ReferenceCounterTestCreateCopy()
{
    ConPrinter::Print(u"Begin ReferenceCounterTestCreateCopy\n");
    {
        ReferenceCounter* pRefCounter;
        {
            ReferenceCounter refCounter;
            pRefCounter = &refCounter;

            ConPrinter::Print("RefCount PostCreate: {}\n", pRefCounter->RefCount());

            {
                const ReferenceCounter copy(refCounter);
                ConPrinter::Print("RefCount copy PostCopy: {}\n", copy.RefCount());
                ConPrinter::Print("RefCount PostCopy: {}\n", pRefCounter->RefCount());
            }
            ConPrinter::Print("RefCount PostCopyDestroy: {}\n", pRefCounter->RefCount());
        }

        ConPrinter::Print("RefCount PostDestroy: {}\n", pRefCounter->RefCount());
    }
    ConPrinter::Print(u"End ReferenceCounterTestCreateCopy\n");
}

void ReferenceCounterTestCreateMove()
{
    ConPrinter::Print(u"Begin ReferenceCounterTestCreateMove\n");
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
    ConPrinter::Print(u"End ReferenceCounterTestCreateMove\n");
}

void ReferenceCounterTestCreateCopyAssign()
{
    ConPrinter::Print(u"Begin ReferenceCounterTestCreateCopyAssign\n");
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
    ConPrinter::Print(u"End ReferenceCounterTestCreateCopyAssign\n");
}

void ReferenceCounterTestCreateMoveAssign()
{
    ConPrinter::Print(u"Begin ReferenceCounterTestCreateMoveAssign\n");
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
    ConPrinter::Print(u"End ReferenceCounterTestCreateMoveAssign\n");
}

void ReferenceCounterTestCreateCopyAssignSelf()
{
    ConPrinter::Print(u"Begin ReferenceCounterTestCreateCopyAssignSelf\n");
    {
        ReferenceCounter* pRefCounter;
        {
            ReferenceCounter refCounter;
            pRefCounter = &refCounter;

            ConPrinter::Print("RefCount PostCreate: {}\n", pRefCounter->RefCount());

            {
                refCounter = refCounter;
                ConPrinter::Print("RefCount PostCopy: {}\n", pRefCounter->RefCount());
            }
            ConPrinter::Print("RefCount PostCopyDestroy: {}\n", pRefCounter->RefCount());
        }

        ConPrinter::Print("RefCount PostDestroy: {}\n", pRefCounter->RefCount());
    }
    ConPrinter::Print(u"End ReferenceCounterTestCreateCopyAssignSelf\n");
}

void ReferenceCounterTestCreateMoveAssignSelf()
{
    ConPrinter::Print(u"Begin ReferenceCounterTestCreateMoveAssignSelf\n");
    {
        ReferenceCounter* pRefCounter;
        {
            ReferenceCounter refCounter;
            pRefCounter = &refCounter;

            ConPrinter::Print("RefCount PostCreate: {}\n", pRefCounter->RefCount());
            ConPrinter::Print("pRefCount PostCreate: 0x{}\n", pRefCounter->RefCountPtr());

            {
                refCounter = ::std::move(refCounter);
                ConPrinter::Print("RefCount PostMove: {}\n", pRefCounter->RefCount());
                ConPrinter::Print("pRefCount PostMove: 0x{}\n", pRefCounter->RefCountPtr());
            }
            ConPrinter::Print("RefCount PostMoveDestroy: {}\n", pRefCounter->RefCount());
        }

        ConPrinter::Print("RefCount PostDestroy: {}\n", pRefCounter->RefCount());
        ConPrinter::Print("pRefCount PostDestroy: 0x{}\n", pRefCounter->RefCountPtr());
    }
    ConPrinter::Print(u"End ReferenceCounterTestCreateMoveAssignSelf\n");
}

void RefTest0()
{
    wcout << "Begin RefTest0" << endl;
    {
        ReferenceCountingPointer<Dummy> ref(32, 17);
    }
    wcout << "End RefTest0" << endl;
}

void StrongRefTest0()
{
    wcout << "Begin StrongRefTest0" << endl;
    {
        StrongReferenceCountingPointer<Dummy> ref(32, 17);
    }
    wcout << "End StrongRefTest0" << endl;
}

void WeakRefTest0()
{
    wcout << "Begin WeakRefTest0" << endl;
    {
        const StrongReferenceCountingPointer<Dummy> strong(32, 17);
        WeakReferenceCountingPointer<Dummy> weak = strong;
    }
    wcout << "End WeakRefTest0" << endl;
}

static void DummyCopy(ReferenceCountingPointer<Dummy> ref)
{
    wcout << "Copy: (" << ref->_x << ", " << ref->_y << ")" << endl;
}

static void DummyCopy(StrongReferenceCountingPointer<Dummy> ref)
{
    wcout << "Copy: (" << ref->_x << ", " << ref->_y << ")" << endl;
}

static void DummyCopy(WeakReferenceCountingPointer<Dummy> ref)
{
    wcout << "Copy: (" << ref->_x << ", " << ref->_y << ")" << endl;
}

static void DummyRefBase(const TReferenceCountingPointerBase<Dummy>& ref)
{
    wcout << "Reference: (" << ref->_x << ", " << ref->_y << ")" << endl;
}

void RefTest1()
{
    wcout << "Begin RefTest1" << endl;
    {
        const ReferenceCountingPointer<Dummy> ref(32, 17);
        wcout << "Passing as copy." << endl;
        DummyCopy(ref);
        wcout << "Passing as reference." << endl;
        DummyRefBase(ref);
    }
    wcout << "End RefTest1" << endl;
}

void StrongRefTest1()
{
    wcout << "Begin StrongRefTest1" << endl;
    {
        const StrongReferenceCountingPointer<Dummy> ref(32, 17);
        wcout << "Passing as copy." << endl;
        DummyCopy(ref);
        wcout << "Passing as reference." << endl;
        DummyRefBase(ref);
    }
    wcout << "End StrongRefTest1" << endl;
}

void WeakRefTest1()
{
    wcout << "Begin WeakRefTest1" << endl;
    {
        const StrongReferenceCountingPointer<Dummy> strong(32, 17);
        const WeakReferenceCountingPointer<Dummy> weak = strong;
        wcout << "Passing as copy." << endl;
        DummyCopy(weak);
        wcout << "Passing as reference." << endl;
        DummyRefBase(weak);
    }
    wcout << "End WeakRefTest1" << endl;
}

void StrongWeakRefTest0()
{
    wcout << "Begin StrongWeakRefTest0" << endl;
    {
        WeakReferenceCountingPointer<Dummy> weak;
        {
            const StrongReferenceCountingPointer<Dummy> strong(32, 17);
            wcout << "Saving to weak." << endl;
            weak = strong;
        }
        wcout << "After Scope." << endl;
        wcout << "Weak is valid: " << (weak ? "true" : "false") << endl;
        wcout << "Strong Ref Count " << weak.StrongRefCount() << endl;
        wcout << "Weak Ref Count " << weak.WeakRefCount() << endl;

    }
    wcout << "End StrongWeakRefTest1" << endl;
}

void RefTests()
{
    ReferenceCounterTestCreateDestroy();
    wcout << endl;
    ReferenceCounterTestCreateFrom0Destroy();
    wcout << endl;
    ReferenceCounterTestCreateFrom1Destroy();
    wcout << endl;
    ReferenceCounterTestCreateFrom2Destroy();
    wcout << endl;
    ReferenceCounterTestCreateCopy();
    wcout << endl;
    ReferenceCounterTestCreateMove();
    wcout << endl;
    ReferenceCounterTestCreateCopyAssign();
    wcout << endl;
    ReferenceCounterTestCreateMoveAssign();
    wcout << endl;
    ReferenceCounterTestCreateCopyAssignSelf();
    wcout << endl;
    ReferenceCounterTestCreateMoveAssignSelf();
    wcout << endl;
    RefTest0();
    wcout << endl;
    RefTest1();
    wcout << endl;
    StrongRefTest0();
    wcout << endl;
    StrongRefTest1();
    wcout << endl;
    WeakRefTest0();
    wcout << endl;
    WeakRefTest1();
    wcout << endl;
    StrongWeakRefTest0();
}
