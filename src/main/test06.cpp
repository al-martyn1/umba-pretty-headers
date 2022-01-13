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



// ..\src\main\test01.cpp test01.cpp.txt

int main(int argc, char* argv[])
{

    programLocationInfo = umba::program_location::getProgramLocation(argc, argv);

    std::cout << "App Location Info" << "\n";
    std::cout << programLocationInfo;
    std::cout << "---";
    std::cout << "\n";
    std::cout << "Test Subfolder Name             : " << programLocationInfo.getAppRootSubName("include") << "\n";
    std::cout << "Test Subfolder NameExt          : " << programLocationInfo.getAppRootSubName("include","user") << "\n";
    std::cout << "Test Subfolder ExtOnly          :" << programLocationInfo.getAppRootSubName("","user") << "\n";
    std::cout << "---\n";
    std::cout << "Test makeAbsPath                : " << programLocationInfo.makeAbsPath("..\\test_data\\compile_flags.txt") << "\n";
    std::cout << "-------------\n";

    std::cout << "\n";

    return 0;
}

