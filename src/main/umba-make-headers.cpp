

// http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/p0061r1.html

#define UMBA_MAKE_HEADERS_STUB()                         \
    int main( int arc, char* argv[])                     \
    {                                                    \
        std::cout << "umba-make-headers: just a stub" << std::endl;   \
        return 0;                                        \
    }


// #if 1

#if __has_include("..\..\..\umba-make-headers\umba-make-headers.cpp")

    #include "..\..\..\umba-make-headers\umba-make-headers.cpp"

#else

    #include <iostream>

    UMBA_MAKE_HEADERS_STUB()
    

#endif