
// Clang Tutorial: Finding Declarations
// 1 The "Hello World" Program
// Little bit updated
// https://xinhuang.github.io/posts/2014-10-19-clang-tutorial-finding-declarations.html#org2946e57


#include "test_data_02.h"


typedef int int_type;



class MyClass
{
  int_type foo = 0;  // member declaration

public:

  MyClass()                             = default;
  MyClass( const MyClass &)             = default;
  MyClass( MyClass &&)                  = default;
  MyClass& operator=( const MyClass &)  = default;
  MyClass& operator=( MyClass &&)       = default;

  MyClass( int _foo ) : foo(_foo) {}
  MyClass& operator=( int _foo )
  {
      foo = _foo;
      return *this;
  }

  int bar() const { return foo; } // method declaration w/implementation

};


inline
MyClass operator+(const MyClass mc1, const MyClass mc2)
{
    MyClass res = MyClass( mc1.bar()+mc2.bar());
    return res;
}

inline
bool operator>(const MyClass mc1, const MyClass mc2)
{
    return mc1.bar() > mc2.bar();
}

inline
bool operator<(const MyClass mc1, const MyClass mc2)
{
    return mc1.bar() < mc2.bar();
}

inline
bool operator==(const MyClass mc1, const MyClass mc2)
{
    return mc1.bar() == mc2.bar();
}


// function declaration w/implementation

MyClass foobar( int f )
{
  MyClass a = f;
  return a;
}


struct OtherClass
{
    int dummy = 0;

}; // class OtherClass


#define CALL_FOOBAR(i)   foobar(i)


namespace test
{

template< typename ClassType > inline
ClassType makeClass( int i )
{
    ClassType c = ClassType(i);
    return c;
}

template< > inline
OtherClass makeClass<OtherClass>( int i )
{
    OtherClass o;
    o.dummy = i;
    return o;
}

} // namespace test


namespace foo {
    namespace bar {
         namespace baz {
             
             int qux = 42;

             enum OldStyleEnum{ ose1 };
             enum class EnumClass{ ec1 };

         }
    }
}

namespace fbz = foo::bar::baz;
 

namespace t = test;

using t::makeClass;


int main(int argc, char* argv[])
{
    auto fb1 = CALL_FOOBAR(4);
    auto fb2 = test::makeClass<MyClass>(7);
    auto fb3 = fb1 + fb2;

    auto fb4 = test::makeClass<OtherClass>(9);

    int  i1 = 2 + 3;

    bool b1 = fb3<i1;

    return 0;
}

