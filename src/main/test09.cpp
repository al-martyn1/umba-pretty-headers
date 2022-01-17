#include "clang.h"

#if defined(_MSC_VER)
    #pragma hdrstop
#endif

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

#include "umba/time_service.h"


#include "utils.h"
#include "clang.h"
#include "marty_clang_helpers.h"


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

#include "scan_sources.h"

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
        argsParser.args.push_back("@..\\tests\\data\\umba-pretty-headers-09.rsp");
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


    appConfig = appConfig.getAdjustedConfig(programLocationInfo);
    pAppConfig = &appConfig;

    if (appConfig.getOptShowConfig())
    {
        printInfoLogSectionHeader(logMsg, "Actual Config");
        // logMsg << appConfig;
        appConfig.print(logMsg) << "\n";
    }

    if (appConfig.outputPath.empty())
    {
        LOG_ERR_OPT << "output path not taken (--output-path)" << endl;
        return 1;
    }



    #include "zz_generation.h"


    std::vector<std::string> foundFiles, excludedFiles;
    std::set<std::string>    foundExtentions;
    scanFolders(appConfig, foundFiles, excludedFiles, foundExtentions);



    if (appConfig.testVerbosity(VerbosityLevel::normal))
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

    if (appConfig.testVerbosity(VerbosityLevel::normal))
    {
        printInfoLogSectionHeader(logMsg, "Scaning for input files completed");
        auto tickDiff = umba::time_service::getCurTimeMs() - startTick;
        logMsg << "Time elapsed: " << tickDiff << "ms" << "\n";
        //startTick = umba::time_service::getCurTimeMs();
    }

    if (foundFiles.empty())
    {
        LOG_WARN_OPT("input-empty") << "no files for processing";
    }



    for(auto compileFlagsTxtFile: generatedCompileFlagsTxtFiles)
    {
    
        // std::vector<std::string> foundFiles
        std::string errRecipientStr;
       
        auto pcdb = clang::tooling::FixedCompilationDatabase::loadFromFile(compileFlagsTxtFile, errRecipientStr);
         
        if (pcdb==0 || !errRecipientStr.empty())
        {
            marty::clang::helpers::printError( llvm::errs(), errRecipientStr );
            return -1;
        }

        std::string srcPath = umba::filename::getPath(compileFlagsTxtFile);
        std::string srcName = umba::filename::getName(compileFlagsTxtFile);
        std::string srcFullName = umba::filename::appendPath( srcPath, umba::filename::appendExt(srcName, std::string("cpp")) );

        std::ofstream cppStream(srcFullName);
        if (!cppStream)
        {
            LOG_ERR_OPT << "failed to create C++ source file: " << srcFullName << endl;
            continue;
        }

        allGeneratedFiles.insert(srcFullName);

        currentSourceFullName = srcFullName;
        currentSourcePath     = umba::filename::getPath(srcFullName);


        if (appConfig.testVerbosity(VerbosityLevel::normal))
        {
            printInfoLogSectionHeader(logMsg, "Generating C++ source");
            // logMsg << endl << "Generating C++ source: " << srcFullName << endl << endl;
            logMsg << "File: " << srcFullName << endl << endl;
            logMsg << "Processing..." << endl << endl;
        }
        

        // if (!appConfig.getOptQuet())
        // {
        //     logMsg << endl << "Generating C++ source: " << srcFullName << endl << endl;
        //     //printInfoLogSectionHeader(logMsg, "Generating C++ source");
        // }


        std::vector<std::string> incPaths;
        std::map< std::string, std::vector<std::string> >::const_iterator pit = generatedCompileFlagsIncPaths.find(compileFlagsTxtFile);
        if (pit!=generatedCompileFlagsIncPaths.end())
        {
            incPaths = pit->second;
        }


        curProcessedFiles.clear();

        for(auto file : foundFiles)
        {
            std::string incName = file;

            curProcessedFiles.insert(umba::filename::makeCanonicalForCompare( incName, '/' )); // Вставляем сначала полное имя файла

            for(const auto &incPath : incPaths)
            {
                if (umba::filename::isSubPathName(incPath, incName, &incName, '/'))
                    break;
            }

            //cppStream << "#include \"" << incName << "\"\n";
            cppStream << "#include <" << incName << ">\n";

            curProcessedFiles.insert(umba::filename::makeCanonicalForCompare( incName, '/' )); // Вставляем имя файла, как оно инклудится
        }

        cppStream << "\n";
        cppStream.close();

        // Now we are ready to process source

        std::vector<std::string> inputFiles;
        inputFiles.push_back(srcFullName);



        auto pActionFactory = clang::tooling::newFrontendActionFactory
                                  < marty::clang::helpers::DeclFindingActionTemplate
                                      < marty::clang::helpers::DeclFinderTemplate
                                          < DeclVisitor
                                          , marty::clang::helpers::DeclFinderMode::handleAll // handleAllAndPrintFilename // printSourceFilename
                                          >
                                      > 
                                  >(); // std::unique_ptr
       
        auto pActionFactoryRawPtr = pActionFactory.get();

        clang::tooling::ClangTool clangTool(*pcdb, inputFiles);
        
        auto res = clangTool.run( pActionFactoryRawPtr ); // pass raw ptr

        if (res)
        {
            LOG_ERR_OPT << "Clang returns error: " << res << endl;
            return res;
        }

    }

    auto printDeclStat = [&]( const std::map< clang::Decl::Kind, unsigned > &m, const char *title )
    {
        //if (!appConfig.getOptVerbose())
        if (!appConfig.testVerbosity(VerbosityLevel::normal))
            return;

        if (m.empty())
            return;

        using namespace umba::omanip;

        printInfoLogSectionHeader(logMsg, title);

        for( auto [key,val] : m )
        {
            logMsg << width(36) << marty::clang::helpers::getClangDeclKindName(key) << ": " << val <<endl;
        }

    };

    printDeclStat( declUsageMapHandled  , "Handled Declaration Types"   );
    printDeclStat( declUsageMapSkipped  , "Skipped Declaration Types"   );
    printDeclStat( declUsageMapUnhandled, "Unhandled Declaration Types" );
    printDeclStat( declUsageMapUnknowns , "Unknown Declaration Types"   );


    if (!foundDeclarations.empty() && appConfig.testVerbosity(VerbosityLevel::detailed))
    {
        {
            std::ostringstream oss;
            oss << "Found User Declarations (Total: " << foundDeclarations.size() << ")";
            printInfoLogSectionHeader(logMsg, oss.str());
        }

        for(const auto& [name, info] : foundDeclarations)
        {
            logMsg << endl;

            auto fileNameForCppName = cppNameToFileName(name);
            logMsg << name <<  " - (" << fileNameForCppName << ")"; // << endl;
            if (isCppSpecialName(name))
                logMsg << " - C++ special name";
            logMsg << endl;

            logMsg << "Kinds:" << endl;
            for( auto kind : info.nameKinds )
            {
                logMsg << "    " << marty::clang::helpers::getClangDeclKindName(kind) 
                       << " - "  << marty::clang::helpers::DeclKindOfKind_toStdString(marty::clang::helpers::declKind_toKindOfKind(kind))
                       << endl;
            }

            logMsg << "Files:" << endl;
            for( const auto & [filenameKey, locFileInfo]: info.locationFiles )
            {
                logMsg << "    " << marty::clang::helpers::getClangDeclKindName(locFileInfo.kind)
                       <<   ": " << locFileInfo.locationFilename
                       <<  " - " << locFileInfo.fullFilename
                       << endl;
            }

        }
        
    }

    if (appConfig.testVerbosity(VerbosityLevel::normal))
    {
        printInfoLogSectionHeader(logMsg, "Scaning C++ headers completed");
        auto tickDiff = umba::time_service::getCurTimeMs() - startTick;
        logMsg << "Time elapsed: " << tickDiff << "ms" << "\n";
        //startTick = umba::time_service::getCurTimeMs();
    }


    std::map<std::string,std::regex>  regexes;
    std::map<std::string,std::string> originalMasks;

    for(auto excludeNameMask : appConfig.excludeNamesMaskList)
    {
        auto regexStr = expandSimpleMaskToEcmaRegex(excludeNameMask);
        regexes      [regexStr] = std::regex(regexStr);
        originalMasks[regexStr] = excludeNameMask;
    }


    for(const auto& [cxxName, info] : foundDeclarations)
    {
        if (isCppSpecialName(cxxName))
        {
            continue; // skip operatorXX etc
        }

        bool validKindFound = false;

        //std::string kindsStr;
        marty::clang::helpers::DeclKindOfKind allNameKinds = marty::clang::helpers::DeclKindOfKind::none;

        for( auto kind : info.nameKinds )
        {
            auto kindOfKind = marty::clang::helpers::declKind_toKindOfKind(kind);
            allNameKinds |= kindOfKind;
            // if (!kindsStr.empty())
            //      kindsStr.append(1,',');
            // kindsStr.append(marty::clang::helpers::DeclKindOfKind_toStdString(kindOfKind));

            if (appConfig.isDeclKindAllowed(kindOfKind))
            {
                validKindFound = true;
                break;
            }
        }

        if (!validKindFound)
        {
            if (appConfig.testVerbosity(VerbosityLevel::detailed))
                logMsg << cxxName << " - " << notice << "skipped" <<  /* normal << */  " due kind of name - " << marty::clang::helpers::DeclKindOfKind_toStdString(allNameKinds) << normal << endl;
            continue;
        }

        std::string regexStr;
        if (umba::regex_helpers::regexMatch(cxxName,regexes,&regexStr))
        {
            if (appConfig.testVerbosity(VerbosityLevel::detailed))
                logMsg << cxxName << " - " << notice << "skipped" <<  /* normal << */  " due '" << originalMasks[regexStr] << "' (" << regexStr << ")" << normal << endl;
            continue;
        }

        auto fileName = cppNameToFileName(cxxName);

        auto fullName = umba::filename::appendPath(appConfig.outputPath, fileName);

        auto canonicalFullName = umba::filename::makeCanonical(fullName);

        auto path = umba::filename::getPath(canonicalFullName);

        auto createDirectory = [](std::string p) -> bool
        {
            std::vector<std::string> pathList;
            pathList.reserve(8);
            while(p.size()>3)
            {
                pathList.push_back(p);
                p = umba::filename::getPath(p);
            }

            for(std::vector<std::string>::const_reverse_iterator it=pathList.rbegin(); it!=pathList.rend(); ++it)
            {
                std::error_code ec;
                // Хз, как проверять результат сэтого вызова - для существующего каталога возвращает false с кодом 0, хотя должен вроде бы true возвращать

                std::filesystem::create_directory(*it, ec); // Игнорим ошибки

                // if (!std::filesystem::create_directory(*it, ec) && ec.value()!=0) 
                // {
                //     LOG_ERR_OPT << "failed to create directory: " << *it 
                //                 << ", reason: " << ec.message() 
                //                 << ", ec.value: " << ec.value() 
                //                 //<< ", category: " << ec.category().message(ec.default_error_condition()) 
                //                 << endl;
                //     return false;
                // }
            
            }

            // return true;
            return std::filesystem::exists(p);
        };


        if (!createDirectory(path))
        {
            LOG_ERR_OPT << "failed to create directory: " << path << endl;
            continue;
        }

        //;

        std::ofstream hdrStream(canonicalFullName);
        if (!hdrStream)
        {
            LOG_ERR_OPT << "failed to create C++ header file: " << canonicalFullName /* fullName */  << endl;
            continue;
        }

        // Generate file content here

        hdrStream << "#pragma once\n\n";

        for(const auto& [fileName, locInfo] : info.locationFiles)
        {
            hdrStream << "// " << marty::clang::helpers::DeclKindOfKind_toStdString(marty::clang::helpers::declKind_toKindOfKind(locInfo.kind)) << "\n";

            std::string incName = locInfo.fullFilename;
            for(const auto &path : appConfig.scanPaths)
            {
                if (umba::filename::isSubPathName(path, incName, &incName, '/'))
                    break;
            }

            hdrStream << "#include " << appConfig.getQuotedName(incName) << "\n\n";

        }

        if (appConfig.testVerbosity(VerbosityLevel::detailed))
            logMsg << cxxName << " - " << canonicalFullName /* fullName */  << " - generated" << endl;

    }


    if (appConfig.testVerbosity(VerbosityLevel::normal))
    {
        printInfoLogSectionHeader(logMsg, "Job done");
        auto tickDiff = umba::time_service::getCurTimeMs() - startTick;
        logMsg << "Time elapsed: " << tickDiff << "ms" << "\n";
        //startTick = umba::time_service::getCurTimeMs();
    }


    return 0;
}


