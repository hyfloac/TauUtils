#include <ReferenceCountingPointer.hpp>
#include <iostream>
#include <ConPrinter.hpp>

using namespace std;

struct Dummy
{
    int _x, _y;

    Dummy(int x, int y)
        : _x(x)
        , _y(y)
    {
        wcout << "Initialized. (" << x << ", " << y << ")" << endl;
    }

    ~Dummy()
    {
        wcout << "Destroyed. (" << _x << ", " << _y << ")" << endl;
    }

    Dummy(const Dummy& copy)
        : _x(copy._x)
        , _y(copy._y)
    {
        ConPrinter::Print("Copied. ({}, {})\n", _x, _y);
    }

    Dummy(Dummy&& move) noexcept
        : _x(move._x)
        , _y(move._y)
    {
        move._x = 0;
        move._y = 0;

        ConPrinter::Print("Moved. ({}, {})\n", _x, _y);
    }

    Dummy& operator=(const Dummy& copy)
    {
        if(this == &copy)
        {
            return *this;
        }

        _x = copy._x;
        _y = copy._y;

        ConPrinter::Print("Copy Assigned. ({}, {})\n", _x, _y);

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

        ConPrinter::Print("Move Assigned. ({}, {})\n", _x, _y);

        return *this;
    }
};

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
