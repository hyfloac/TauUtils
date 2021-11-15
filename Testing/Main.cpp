#include <ReferenceCountingPointer.hpp>
#include <iostream>

using namespace std;

void RefTests();

int main(int argCount, char* args[])
{
    RefTests();

    return 0;
}

struct Dummy
{
    int _x, _y;

    Dummy(int x, int y)
        : _x(x)
        , _y(y)
    {
        cout << "Initialized. (" << x << ", " << y << ")" << endl;
    }

    ~Dummy()
    {
        cout << "Destroyed. (" << _x << ", " << _y << ")" << endl;
    }
};

void RefTest0()
{
    cout << "Begin RefTest0" << endl;
    {
        ReferenceCountingPointer<Dummy> ref(32, 17);
    }
    cout << "End RefTest0" << endl;
}

void StrongRefTest0()
{
    cout << "Begin StrongRefTest0" << endl;
    {
        StrongReferenceCountingPointer<Dummy> ref(32, 17);
    }
    cout << "End StrongRefTest0" << endl;
}

void WeakRefTest0()
{
    cout << "Begin WeakRefTest0" << endl;
    {
        const StrongReferenceCountingPointer<Dummy> strong(32, 17);
        WeakReferenceCountingPointer<Dummy> weak = strong;
    }
    cout << "End WeakRefTest0" << endl;
}

static void DummyCopy(ReferenceCountingPointer<Dummy> ref)
{
    cout << "Copy: (" << ref->_x << ", " << ref->_y << ")" << endl;
}

static void DummyCopy(StrongReferenceCountingPointer<Dummy> ref)
{
    cout << "Copy: (" << ref->_x << ", " << ref->_y << ")" << endl;
}

static void DummyCopy(WeakReferenceCountingPointer<Dummy> ref)
{
    cout << "Copy: (" << ref->_x << ", " << ref->_y << ")" << endl;
}

static void DummyRefBase(const TReferenceCountingPointerBase<Dummy>& ref)
{
    cout << "Reference: (" << ref->_x << ", " << ref->_y << ")" << endl;
}

void RefTest1()
{
    cout << "Begin RefTest1" << endl;
    {
        const ReferenceCountingPointer<Dummy> ref(32, 17);
        cout << "Passing as copy." << endl;
        DummyCopy(ref);
        cout << "Passing as reference." << endl;
        DummyRefBase(ref);
    }
    cout << "End RefTest1" << endl;
}

void StrongRefTest1()
{
    cout << "Begin StrongRefTest1" << endl;
    {
        const StrongReferenceCountingPointer<Dummy> ref(32, 17);
        cout << "Passing as copy." << endl;
        DummyCopy(ref);
        cout << "Passing as reference." << endl;
        DummyRefBase(ref);
    }
    cout << "End StrongRefTest1" << endl;
}

void WeakRefTest1()
{
    cout << "Begin WeakRefTest1" << endl;
    {
        const StrongReferenceCountingPointer<Dummy> strong(32, 17);
        const WeakReferenceCountingPointer<Dummy> weak = strong;
        cout << "Passing as copy." << endl;
        DummyCopy(weak);
        cout << "Passing as reference." << endl;
        DummyRefBase(weak);
    }
    cout << "End WeakRefTest1" << endl;
}

void StrongWeakRefTest0()
{
    cout << "Begin StrongWeakRefTest0" << endl;
    {
        WeakReferenceCountingPointer<Dummy> weak;
        {
            const StrongReferenceCountingPointer<Dummy> strong(32, 17);
            cout << "Saving to weak." << endl;
            weak = strong;
        }
        cout << "After Scope." << endl;
        cout << "Weak is valid: " << (weak ? "true" : "false") << endl;
        cout << "Strong Ref Count " << weak.StrongRefCount() << endl;
        cout << "Weak Ref Count " << weak.WeakRefCount() << endl;

    }
    cout << "End StrongWeakRefTest1" << endl;
}

void RefTests()
{
    RefTest0();
    cout << endl;
    RefTest1();
    cout << endl;
    StrongRefTest0();
    cout << endl;
    StrongRefTest1();
    cout << endl;
    WeakRefTest0();
    cout << endl;
    WeakRefTest1();
    cout << endl;
    StrongWeakRefTest0();
}