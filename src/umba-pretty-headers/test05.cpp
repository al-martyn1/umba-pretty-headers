/*! \file
    \brief Тестики umba::filename - немного подпиливал её
*/

#include "umba/umba.h"
#include "umba/simple_formatter.h"
#include "umba/char_writers.h"

#include "umba/debug_helpers.h"

#include <iostream>
#include <iomanip>
#include <string>

#include "umba/debug_helpers.h"
#include "umba/string_plus.h"
#include "umba/program_location.h"



umba::StdStreamCharWriter coutWriter(std::cout);
umba::StdStreamCharWriter cerrWriter(std::cerr);
umba::NulCharWriter       nulWriter;

umba::SimpleFormatter logMsg(&coutWriter);
umba::SimpleFormatter logErr(&cerrWriter);
umba::SimpleFormatter logNul(&nulWriter);


umba::program_location::ProgramLocation<std::string>   programLocationInfo;


#include "umba/cmd_line.h"


// ..\src\main\test01.cpp test01.cpp.txt

int main(int argc, char* argv[])
{

    // Тестики по случаю - операции с извлечением частей из полного имени
    auto testUmbaFilename = [](std::string testPathName)
    {
        std::cout << "Full path: " << testPathName << "\n";
        std::cout << "Path     : " << umba::filename::getPath(testPathName) << "\n";
        std::cout << "Filename : " << umba::filename::getFileName(testPathName) << "\n";
        std::cout << "Name     : " << umba::filename::getName(testPathName) << "\n";
        std::cout << "Ext      : " << umba::filename::getExt(testPathName) << "\n";
        std::cout << "-------------";
        std::cout << "\n";
    };

    testUmbaFilename("Z:\\umba_pretty_headers.h\\bin\\umba_pretty_headers");
    testUmbaFilename("\\umba_pretty_headers");
    testUmbaFilename("umba_pretty_headers");
        
    testUmbaFilename("Z:\\umba_pretty_headers.h\\bin\\umba_pretty_headers.exe");
    testUmbaFilename("\\umba_pretty_headers.exe");
    testUmbaFilename("umba_pretty_headers.exe");

    testUmbaFilename("Z:\\umba_pretty_headers.h\\bin\\umba_pretty_headers.");
    testUmbaFilename("\\umba_pretty_headers.");
    testUmbaFilename("umba_pretty_headers.");

    testUmbaFilename("Z:\\umba_pretty_headers.h\\bin\\.exe");
    testUmbaFilename("\\.exe");
    testUmbaFilename(".exe");


    return 0;
}

