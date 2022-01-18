/*! \file
    \brief Чистый проект, без каких либо библиотек. Проверка, будут ли передаваться аргументы из среды в main под отладкой. Не проканало. Баг в MSVC 2019?
*/


#include <iostream>
#include <iomanip>





// ..\src\main\test00.cpp test00.cpp.txt

int main(int argc, char* argv[])
{
    using std::cout;
    using std::endl;

    cout << "Argc: " << argc << endl;

    return 0;
}

