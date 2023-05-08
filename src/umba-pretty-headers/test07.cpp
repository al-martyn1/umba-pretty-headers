/*! \file
    \brief Реализация и проверка базовых функций сборка данных для сканирования хидеров
*/

#include "umba/umba.h"
#include "umba/simple_formatter.h"
#include "umba/char_writers.h"

#include "umba/debug_helpers.h"

#include <iostream>
#include <iomanip>
#include <string>
// #include <cstdio>
#include <filesystem>

#include "umba/debug_helpers.h"
#include "umba/string_plus.h"
#include "umba/program_location.h"
#include "umba/scope_exec.h"
#include "umba/macro_helpers.h"
#include "umba/macros.h"
#include "umba/info_log.h"
#include "umba/scanners.h"

#include "umba/time_service.h"



umba::StdStreamCharWriter coutWriter(std::cout);
umba::StdStreamCharWriter cerrWriter(std::cerr);
umba::NulCharWriter       nulWriter;

umba::SimpleFormatter umbaLogStreamMsg(&coutWriter);
umba::SimpleFormatter umbaLogStreamErr(&cerrWriter);
umba::SimpleFormatter umbaLogStreamNul(&nulWriter);

bool logWarnType   = true;
bool umbaLogGccFormat  = false;
bool umbaLogSourceInfo = false;


#include "../common/log.h"
#include "../common/compile_flags_parser.h"
#include "umba/scanners.h"
#include "app_config.h"
#include "umba/info_log.h"

umba::program_location::ProgramLocation<std::string>   programLocationInfo;
AppConfig    appConfig;



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
#include "umba/cli_tool_helpers.h"

#include "arg_parser.h"











// ..\src\main\test01.cpp test01.cpp.txt

int main(int argc, char* argv[])
{
    umba::time_service::init();
    umba::time_service::start();

    umba::time_service::TimeTick startTick = umba::time_service::getCurTimeMs();


    using namespace umba::omanip;


    auto argsParser = umba::command_line::makeArgsParser( ArgParser(), CommandLineOptionCollector(), argc, argv );

    // Force set CLI arguments while running under debugger
    if (umba::isDebuggerPresent())
    {
        argsParser.args.clear();
        argsParser.args.push_back("--help");
        // argsParser.args.push_back("@..\\tests\\data\\umba-pretty-headers.rsp");
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
        umba::cli_tool_helpers::printNameVersion(umbaLogStreamMsg);
    }

    if (!argsParser.parseStdBuiltins())
        return 1;
    if (argsParser.mustExit)
        return 0;

    if (!argsParser.parse())
        return 1;
    if (argsParser.mustExit)
        return 0;




    umba::info_log::printSectionHeader(umbaLogStreamMsg, "App Config");
    appConfig.print(umbaLogStreamMsg) << "\n";

    appConfig = appConfig.getAdjustedConfig(programLocationInfo);

    // printInfoLogSectionHeader(umbaLogStreamMsg, "Adjusted App Config") << appConfig << "\n";
    umba::info_log::printSectionHeader(umbaLogStreamMsg, "Adjusted App Config");
    appConfig.print(umbaLogStreamMsg) << "\n";

    umba::info_log::printSectionHeader(umbaLogStreamMsg, "### Normal Output") << "\n";


    #include "zz_generation.h"


    std::vector<std::string> foundFiles, excludedFiles;
    std::set<std::string>    foundExtentions;
    umba::scanners::scanFolders(appConfig, umbaLogStreamMsg, foundFiles, excludedFiles, foundExtentions);



    if (!appConfig.testVerbosity(VerbosityLevel::detailed))
    {
        if (!foundFiles.empty())
            umba::info_log::printSectionHeader(umbaLogStreamMsg, "Files for Processing");

        for(const auto & name : foundFiles)
        {
            umbaLogStreamMsg << name << endl;
        }


        if (!excludedFiles.empty())
            umba::info_log::printSectionHeader(umbaLogStreamMsg, "Files Excluded from Processing");

        for(const auto & name : excludedFiles)
        {
            umbaLogStreamMsg << name << endl;
        }


        if (!foundExtentions.empty())
            umba::info_log::printSectionHeader(umbaLogStreamMsg, "Found File Extentions");

        for(const auto & ext : foundExtentions)
        {
            if (ext.empty())
                umbaLogStreamMsg << "<EMPTY>" << endl;
            else
                umbaLogStreamMsg << "." << ext << endl;
        }
    }

    // Phases: Init, Scaning, Processing, Generating

    if (!appConfig.testVerbosity(VerbosityLevel::normal))
    {
        if (!foundFiles.empty())
        {
            umba::info_log::printSectionHeader(umbaLogStreamMsg, "Scaning completed");
            auto tickDiff = umba::time_service::getCurTimeMs() - startTick;
            umbaLogStreamMsg << "Time elapsed: " << tickDiff << "ms" << "\n";
            startTick = umba::time_service::getCurTimeMs();
        }
    }

    return 0;
}

