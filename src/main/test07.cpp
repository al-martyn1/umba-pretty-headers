#include "umba/umba.h"
#include "umba/simple_formatter.h"
#include "umba/char_writers.h"

#include "dbghlp.h"

#include <iostream>
#include <iomanip>
#include <string>
// #include <cstdio>
#include <filesystem>

#include "umba/debug_helpers.h"
#include "umba/string_plus.h"
#include "umba/program_location.h"
#include "umba/scope_exec.h"



umba::StdStreamCharWriter coutWriter(std::cout);
umba::StdStreamCharWriter cerrWriter(std::cerr);
umba::NulCharWriter       nulWriter;

umba::SimpleFormatter logMsg(&coutWriter);
umba::SimpleFormatter logErr(&cerrWriter);
umba::SimpleFormatter logNul(&nulWriter);

bool logWarnType   = true;
bool logGccFormat  = false;
bool logSourceInfo = false;


#include "log.h"

#include "compile_flags_parser.h"



umba::program_location::ProgramLocation<std::string>   programLocationInfo;


// bool                    quet = false;
// std::set<std::string>   argsNeedHelp;
// bool                    hasHelpOption   = false;
// bool                    skipCheckUpdate = false;

//PrintHelpStyle printHelpStyle = PrintHelpStyle::normal;

// std::string inputFilename;
// std::string outputFilename;

//bool disableBuiltins = false;
//bool disableBuiltinIncludes = false;



#include "umba/cmd_line.h"


#include "app_ver_config.h"
#include "print_ver.h"

#include "arg_parser.h"











// ..\src\main\test01.cpp test01.cpp.txt

int main(int argc, char* argv[])
{

    auto argsParser = umba::command_line::makeArgsParser( ArgParser(), CommandLineOptionCollector(), argc, argv );

    // Force set CLI arguments while running under debugger
    if (umba::isDebuggerPresent())
    {
        argsParser.args.clear();
        argsParser.args.push_back("@..\\test_data\\umba-pretty-headers.rsp");
        // argsParser.args.push_back(umba::string_plus::make_string(""));
        // argsParser.args.push_back(umba::string_plus::make_string(""));
        // argsParser.args.push_back(umba::string_plus::make_string(""));
    }

    programLocationInfo = argsParser.programLocationInfo;

    // Job completed - may be, --where option found
    if (argsParser.mustExit)
        return 0;

    if (!argsParser.quet)
    {
        printNameVersion();
    }

    if (!argsParser.parseStdBuiltins())
        return 1;
    if (argsParser.mustExit)
        return 0;

    if (!argsParser.parse())
        return 1;
    if (argsParser.mustExit)
        return 0;


    std::cout << "# AppConfig:\n---\n" << appConfig << "\n";

    appConfig = appConfig.getAdjustedConfig(programLocationInfo);

    std::cout << "# Adjusted AppConfig:\n---\n" << appConfig << "\n";


    std::set<std::string> allCompileFlagFiles;
    auto compileFlagFilesAutoDeleter = umba::makeLeaveScopeExec
                                       (
                                           [&]()
                                           {
                                               if (appConfig.keepCompileFlags) return;

                                               for( auto f: allCompileFlagFiles)
                                               {
                                                   // std::remove(f.c_str());
                                                   std::filesystem::remove(f);
                                               }
                                           }
                                       );
    


    for(auto compileFlagsTxt : appConfig.clangCompileFlagsTxtFilename)
    {
        std::map<std::string, std::vector<std::string> > cflags;
        std::vector<std::string>                         commonLines;

        if (!parseCompileFlags(compileFlagsTxt, cflags, commonLines))
            return 1;


        std::vector<std::string> generatedFiles;

        generateCompileFlags(compileFlagsTxt, cflags, commonLines, generatedFiles);

        allCompileFlagFiles.insert(generatedFiles.begin(), generatedFiles.end());
        
    }




    return 0;
}

