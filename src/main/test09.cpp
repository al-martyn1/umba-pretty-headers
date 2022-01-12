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
#include "umba/macro_helpers.h"
#include "umba/macros.h"

#include "umba/time_service.h"



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
#include "utils.h"
#include "scan_folders.h"


umba::program_location::ProgramLocation<std::string>   programLocationInfo;


#include "umba/cmd_line.h"


#include "app_ver_config.h"
#include "print_ver.h"

#include "arg_parser.h"



int main(int argc, char* argv[])
{
    umba::time_service::init();
    umba::time_service::start();

    umba::time_service::TimeTick startTick = umba::time_service::getCurTimeMs();


    using namespace umba::omanip;


    auto argsParser = umba::command_line::makeArgsParser( ArgParser()
                                                        , CommandLineOptionCollector()
                                                        , argc, argv
                                                        , umba::program_location::getProgramLocation
                                                            ( argc, argv
                                                            , false // useUserFolder = false
                                                            , "umba-pretty-headers" // overrideExeName
                                                            )
                                                        );

    // Force set CLI arguments while running under debugger
    if (umba::isDebuggerPresent())
    {
        argsParser.args.clear();
        argsParser.args.push_back("@..\\test_data\\umba-pretty-headers-09.rsp");
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




    printInfoLogSectionHeader(logMsg, "App Config");
    appConfig.print(logMsg) << "\n";

    appConfig = appConfig.getAdjustedConfig(programLocationInfo);

    // printInfoLogSectionHeader(logMsg, "Adjusted App Config") << appConfig << "\n";
    printInfoLogSectionHeader(logMsg, "Adjusted App Config");
    appConfig.print(logMsg) << "\n";

    printInfoLogSectionHeader(logMsg, "### Normal Output") << "\n";

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

        generateCompileFlags(appConfig, compileFlagsTxt, cflags, commonLines, generatedFiles);

        allCompileFlagFiles.insert(generatedFiles.begin(), generatedFiles.end());
        
    }

    // Phases: Init, Scaning, Processing, Generating

    if (!appConfig.getOptQuet())
    {
        printInfoLogSectionHeader(logMsg, "Initialization completed");
        auto tickDiff = umba::time_service::getCurTimeMs() - startTick;
        logMsg << "Time elapsed: " << tickDiff << "ms" << "\n";
        startTick = umba::time_service::getCurTimeMs();
    }


    std::vector<std::string> foundFiles, excludedFiles;
    std::set<std::string>    foundExtentions;
    scanFolders(appConfig, foundFiles, excludedFiles, foundExtentions);



    if (!appConfig.getOptQuet())
    {
        if (!foundFiles.empty())
            printInfoLogSectionHeader(logMsg, "Files for Processing");

        for(const auto & name : foundFiles)
        {
            logMsg << name << endl;
        }


        if (!excludedFiles.empty())
            printInfoLogSectionHeader(logMsg, "Files Excluded from Processing");

        for(const auto & name : excludedFiles)
        {
            logMsg << name << endl;
        }


        if (!foundExtentions.empty())
            printInfoLogSectionHeader(logMsg, "Found File Extentions");

        for(const auto & ext : foundExtentions)
        {
            if (ext.empty())
                logMsg << "<EMPTY>" << endl;
            else
                logMsg << "." << ext << endl;
        }
    }

    // Phases: Init, Scaning, Processing, Generating

    if (!appConfig.getOptQuet())
    {
        if (!foundFiles.empty())
        {
            printInfoLogSectionHeader(logMsg, "Scaning completed");
            auto tickDiff = umba::time_service::getCurTimeMs() - startTick;
            logMsg << "Time elapsed: " << tickDiff << "ms" << "\n";
            startTick = umba::time_service::getCurTimeMs();
        }
    }

    return 0;
}

