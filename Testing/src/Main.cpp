#include <ReferenceCountingPointer.hpp>
#include <iostream>
#include <Console.hpp>

using namespace std;

void RefTests();
extern void StringTests();
extern void DragonTests();

int main(int argCount, char* args[])
{
    Console::Create();
    Console::Init();
    // RefTests();
    StringTests();
    DragonTests();

    return 0;
}

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