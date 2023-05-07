/*! \file
    \brief Собрал всё в одну кучу, практически готовое приложение. Использовал clang::RecursiveASTVisitor и запихиваю всё собранное в глобальные переменные
*/

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
#include "umba/cli_tool_helpers.h"


#include "umba/time_service.h"


#include "../common/utils.h"
#include "../common/clang.h"
#include "../common/marty_clang_helpers.h"
#include "../common/DeclFindingActionTemplate.h"
#include "../common/DeclFinderTemplate.h"


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
#include "../common/utils.h"
#include "umba/scanners.h"

#include "app_config.h"

#include "umba/cmd_line.h"
#include "app_ver_config.h"
#include "arg_parser.h"


AppConfig    appConfig;
umba::program_location::ProgramLocation<std::string>   programLocationInfo;

const AppConfig           *pAppConfig = 0;
std::set<std::string>      curProcessedFiles;
std::string                currentSourceFullName;
std::string                currentSourcePath;


#include "../common/scan_sources.h"
#include "../common/scan_for_pp.h"



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


    appConfig = appConfig.getAdjustedConfig(programLocationInfo);
    pAppConfig = &appConfig;

    if (appConfig.testVerbosity(VerbosityLevel::config))
    {
        umba::info_log::printSectionHeader(umbaLogStreamMsg, "Actual Config");
        // umbaLogStreamMsg << appConfig;
        appConfig.print(umbaLogStreamMsg) << "\n";
    }

    if (appConfig.outputPath.empty())
    {
        LOG_ERR_OPT << "output path not taken (--output-path)" << endl;
        return 1;
    }



    #include "zz_generation.h"



    //unsigned totalFiles = 0;
    unsigned totalFilesCreated = 0;

    std::vector<std::string> foundFiles, excludedFiles;
    std::set<std::string>    foundExtentions;
    umba::scanners::scanFolders(appConfig, umbaLogStreamMsg, foundFiles, excludedFiles, foundExtentions);



    if (appConfig.testVerbosity(VerbosityLevel::normal))
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

    if (appConfig.testVerbosity(VerbosityLevel::normal))
    {
        umba::info_log::printSectionHeader(umbaLogStreamMsg, "Scaning for input files completed");
        auto tickDiff = umba::time_service::getCurTimeMs() - startTick;
        umbaLogStreamMsg << "Time elapsed: " << tickDiff << "ms" << "\n";
        //startTick = umba::time_service::getCurTimeMs();
    }

    if (foundFiles.empty())
    {
        LOG_WARN_OPT("input-empty") << "no files for processing";
    }


    std::vector<std::string>  createdFiles;
    std::vector<std::string>  createdFolders;


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
            umba::info_log::printSectionHeader(umbaLogStreamMsg, "Generating C++ source");
            // umbaLogStreamMsg << endl << "Generating C++ source: " << srcFullName << endl << endl;
            umbaLogStreamMsg << "File: " << srcFullName << endl << endl;
            umbaLogStreamMsg << "Processing..." << endl << endl;
        }
        

        // if (!appConfig.getOptQuet())
        // {
        //     umbaLogStreamMsg << endl << "Generating C++ source: " << srcFullName << endl << endl;
        //     //printInfoLogSectionHeader(umbaLogStreamMsg, "Generating C++ source");
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


    if (appConfig.testVerbosity(VerbosityLevel::normal))
    {
        umba::info_log::printSectionHeader(umbaLogStreamMsg, "Scaning C++ headers completed");
        auto tickDiff = umba::time_service::getCurTimeMs() - startTick;
        umbaLogStreamMsg << "Time elapsed: " << tickDiff << "ms" << "\n";
        //startTick = umba::time_service::getCurTimeMs();
    }



    auto createDirectory = [&](std::string p) -> bool
    {
        std::vector<std::string> pathList;
        pathList.reserve(8);
        while(p.size()>3)
        {
            pathList.push_back(p);
            p = umba::filename::getPath(p);
            createdFolders.push_back(p);
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


    auto printDeclStat = [&]( const std::map< clang::Decl::Kind, unsigned > &m, const char *title )
    {
        //if (!appConfig.getOptVerbose())
        if (!appConfig.testVerbosity(VerbosityLevel::normal))
            return;

        if (m.empty())
            return;

        using namespace umba::omanip;

        umba::info_log::printSectionHeader(umbaLogStreamMsg, title);

        for( auto [key,val] : m )
        {
            umbaLogStreamMsg << width(36) << marty::clang::helpers::getClangDeclKindName(key) << ": " << val <<endl;
        }

    };

    printDeclStat( declUsageMapHandled  , "Handled Declaration Types"   );
    printDeclStat( declUsageMapSkipped  , "Skipped Declaration Types"   );
    printDeclStat( declUsageMapUnhandled, "Unhandled Declaration Types" );
    printDeclStat( declUsageMapUnknowns , "Unknown Declaration Types"   );


    std::map<std::string,std::regex>  regexes;
    std::map<std::string,std::string> originalMasks;

    for(auto excludeNameMask : appConfig.excludeNamesMaskList)
    {
        auto regexStr = expandSimpleMaskToEcmaRegex(excludeNameMask);
        regexes      [regexStr] = std::regex(regexStr);
        originalMasks[regexStr] = excludeNameMask;
    }



    std::map<std::string, std::map<std::string,std::string> >   foundDefinitions;
    std::map<std::string, std::map<std::string,std::string> >   foundUsages;

    if (appConfig.isDeclKindAllowed(marty::clang::helpers::DeclKindOfKind::ppDefine))
    {
        for(auto file : foundFiles)
        {
            scanForPp( file, foundDefinitions, foundUsages );
        }
    }


    if (!foundDefinitions.empty())
    {
        if (appConfig.testVerbosity(VerbosityLevel::detailed))
            umba::info_log::printSectionHeader( umbaLogStreamMsg, "Found Defined Macros" );
       
        std::map<std::string, std::map<std::string,std::string> >::const_iterator 
        it = foundDefinitions.begin();


        auto definedMacrosTxtFullName = umba::filename::makeCanonical(umba::filename::appendPath(appConfig.outputPath, std::string("__defined_macros.txt")));
        std::ofstream definedMacrosTxtStream;
        if (appConfig.getOptDefinedMacros())
        {
            auto path = umba::filename::getPath(definedMacrosTxtFullName);

            if (!createDirectory(path))
                LOG_WARN_OPT("create-dir-failed") << "failed to create directory: " << path << endl;
            else
            {
                definedMacrosTxtStream.open( definedMacrosTxtFullName, std::ios_base::out | std::ios_base::trunc );
                if (!definedMacrosTxtStream)
                    LOG_WARN_OPT("create-file-failed") << "failed to create '__defined_macros.txt' file: " << definedMacrosTxtFullName << endl;
                else
                {
                    createdFiles.push_back(definedMacrosTxtFullName);
                    totalFilesCreated++;
                }
            }
        }

            //std::string incName = appConfig.getIncludeName(locInfo.fullFilename);
            //hdrStream << "#include " << appConfig.getQuotedName(incName) << "\n\n";
        
        for(; it!=foundDefinitions.end(); ++it)
        {
            if (appConfig.testVerbosity(VerbosityLevel::detailed))
                umbaLogStreamMsg << "File: " << it->first << "\n";

            auto incName = appConfig.getIncludeName(it->first);

            definedMacrosTxtStream << "File: " << it->first << "\n";
       
            const std::map<std::string,std::string> &defs = it->second;
       
            for( const auto &[d,l] : defs )
            {
                if (appConfig.testVerbosity(VerbosityLevel::detailed))
                    umbaLogStreamMsg << "    " << d ;

                definedMacrosTxtStream << "    " << d ;

                std::string regexStr;
                if (umba::regex_helpers::regexMatch(d,regexes,&regexStr))
                {
                    if (appConfig.testVerbosity(VerbosityLevel::detailed))
                        umbaLogStreamMsg << " - " << notice << "skipped" <<  /* normal << */  " due '" << originalMasks[regexStr] << "' (" << regexStr << ")" 
                                        << normal 
                                        //<< " - [" << l << "]"
                                        << endl;

                    definedMacrosTxtStream << " - skipped due '" << originalMasks[regexStr] << "' (" << regexStr << ")" 
                                        //<< " - [" << l << "]"
                                        << "\n";

                    continue;
                }


                auto definedMacroFullFileName = umba::filename::makeCanonical(umba::filename::appendPath(appConfig.outputPath, d));
                std::ofstream definedMacroStream;

                if (!appConfig.getOptNoOutput())
                {
                    auto path = umba::filename::getPath(definedMacroFullFileName);
               
                    if (!createDirectory(path))
                        LOG_WARN_OPT("create-dir-failed") << "failed to create directory: " << path << endl;
                    else
                    {
                        definedMacroStream.open( definedMacroFullFileName, std::ios_base::out | std::ios_base::trunc );
                        if (!definedMacroStream)
                            LOG_WARN_OPT("create-file-failed") << "failed to create file: " << definedMacroFullFileName << endl;
                        else
                        {
                            createdFiles.push_back(definedMacroFullFileName);
                            totalFilesCreated++;
                        }
                    }
                }

                definedMacroStream << "#pragma once\n\n";
                definedMacroStream << "// pp-macro\n\n";
                definedMacroStream << "// " << d << "\n\n";
                definedMacroStream << "#include " << appConfig.getQuotedName(incName) << "\n\n";

                if (appConfig.testVerbosity(VerbosityLevel::detailed))
                    umbaLogStreamMsg // << " - [" << l << "]" 
                           << endl;

                definedMacrosTxtStream << "\n";

                //!!! Write includes here
            }
        }
    }


    if (!foundUsages.empty())
    {
        if (appConfig.testVerbosity(VerbosityLevel::detailed))
            umba::info_log::printSectionHeader( umbaLogStreamMsg, "Found Macro Usage" );

        auto usedMacrosTxtFullName = umba::filename::makeCanonical(umba::filename::appendPath(appConfig.outputPath, std::string("__used_macros.txt")));
        std::ofstream usedMacrosTxtStream;
        if (appConfig.getOptUsedMacros())
        {
            auto path = umba::filename::getPath(usedMacrosTxtFullName);

            if (!createDirectory(path))
                LOG_WARN_OPT("create-dir-failed") << "failed to create directory: " << path << endl;
            else
            {
                usedMacrosTxtStream.open( usedMacrosTxtFullName, std::ios_base::out | std::ios_base::trunc );
                if (!usedMacrosTxtStream)
                    LOG_WARN_OPT("create-file-failed") << "failed to create '__used_macros.txt' file: " << usedMacrosTxtFullName << endl;
                else
                {
                    createdFiles.push_back(usedMacrosTxtFullName);
                    totalFilesCreated++;
                }
            }
        }

        std::map<std::string, std::map<std::string,std::string> >::const_iterator     
        it = foundUsages.begin();
        
        for(; it!=foundUsages.end(); ++it)
        {
            if (appConfig.testVerbosity(VerbosityLevel::detailed))
                umbaLogStreamMsg << it->first << "\n";

            usedMacrosTxtStream << it->first << "\n";
       
            const std::map<std::string,std::string> &defs = it->second;
       
            for( const auto &[d,l] : defs )
            {
                if (appConfig.testVerbosity(VerbosityLevel::detailed))
                    umbaLogStreamMsg << "    " << d << " - [" << l << "]\n";
                usedMacrosTxtStream << "    " << d << " - [" << l << "]\n";
            }
        }
    }


    if (!foundDeclarations.empty() && appConfig.testVerbosity(VerbosityLevel::detailed))
    {
        {
            std::ostringstream oss;
            oss << "Found User Declarations (Total: " << foundDeclarations.size() << ")";
            umba::info_log::printSectionHeader(umbaLogStreamMsg, oss.str());
        }

        for(const auto& [cppName, info] : foundDeclarations)
        {
            umbaLogStreamMsg << endl;

            auto fileNameForCppName = cppNameToFileName(cppName);
            umbaLogStreamMsg << cppName <<  " - (" << fileNameForCppName << ")"; // << endl;
            if (isCppSpecialName(cppName))
                umbaLogStreamMsg << " - C++ special name";

            std::string regexStr;
            if (umba::regex_helpers::regexMatch(cppName,regexes,&regexStr))
            {
                if (appConfig.testVerbosity(VerbosityLevel::detailed))
                    umbaLogStreamMsg << " - " << notice << "skipped" <<  /* normal << */  " due '" << originalMasks[regexStr] << "' (" << regexStr << ")" << normal << endl;
                continue;
            }


            umbaLogStreamMsg << endl;

            umbaLogStreamMsg << "Kinds:" << endl;
            for( auto kind : info.nameKinds )
            {
                umbaLogStreamMsg << "    " << marty::clang::helpers::getClangDeclKindName(kind) 
                       << " - "  << marty::clang::helpers::DeclKindOfKind_toStdString(marty::clang::helpers::declKind_toKindOfKind(kind))
                       << endl;
            }

            umbaLogStreamMsg << "Files:" << endl;
            for( const auto & [filenameKey, locFileInfo]: info.locationFiles )
            {
                umbaLogStreamMsg << "    " << marty::clang::helpers::getClangDeclKindName(locFileInfo.kind)
                       <<   ": " << locFileInfo.locationFilename
                       <<  " - " << locFileInfo.fullFilename
                       << endl;
            }

        }
        
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
                umbaLogStreamMsg << cxxName << " - " << notice << "skipped" <<  /* normal << */  " due kind of name - " << marty::clang::helpers::DeclKindOfKind_toStdString(allNameKinds) << normal << endl;
            continue;
        }

        std::string regexStr;
        if (umba::regex_helpers::regexMatch(cxxName,regexes,&regexStr))
        {
            if (appConfig.testVerbosity(VerbosityLevel::detailed))
                umbaLogStreamMsg << cxxName << " - " << notice << "skipped" <<  /* normal << */  " due '" << originalMasks[regexStr] << "' (" << regexStr << ")" << normal << endl;
            continue;
        }

        auto fileName = cppNameToFileName(cxxName);

        auto fullName = umba::filename::appendPath(appConfig.outputPath, fileName);

        auto canonicalFullName = umba::filename::makeCanonical(fullName);

        auto path = umba::filename::getPath(canonicalFullName);


        std::ofstream hdrStream;
        if (!appConfig.getOptNoOutput())
        {

            if (!createDirectory(path))
            {
                LOG_WARN_OPT("create-dir-failed") << "failed to create directory: " << path << endl;
                continue;
            }

            hdrStream.open( canonicalFullName, std::ios_base::out | std::ios_base::trunc );
            if (!hdrStream)
            {
                LOG_WARN_OPT("create-file-failed") << "failed to create C++ header file: " << canonicalFullName << endl;
                continue;
            }
            else
            {
                createdFiles.push_back(canonicalFullName);
                totalFilesCreated++;
            }
        }


        // Generate file content here

        hdrStream << "#pragma once\n\n";

        for(const auto& [fileName, locInfo] : info.locationFiles)
        {
            hdrStream << "// " << marty::clang::helpers::DeclKindOfKind_toStdString(marty::clang::helpers::declKind_toKindOfKind(locInfo.kind)) << "\n";

            std::string incName = appConfig.getIncludeName(locInfo.fullFilename);
            hdrStream << "// " << cxxName << "\n\n";
            hdrStream << "#include " << appConfig.getQuotedName(incName) << "\n\n";

        }

        if (appConfig.testVerbosity(VerbosityLevel::detailed))
            umbaLogStreamMsg << cxxName << " - " << canonicalFullName /* fullName */  << " - generated" << endl;

    }



        // std::vector<std::string>  createdFiles;
        // std::vector<std::string>  createdFolders;

    
    #if defined(WIN32) || defined(_WIN32)
        std::string clearScriptSimpleName = "clear.bat";
    #else
        std::string clearScriptSimpleName = "clear.sh";
    #endif

    auto clearScriptFileName = appConfig.getOutputPath(clearScriptSimpleName);

    std::ofstream clearScriptStream;
    if (!appConfig.getOptNoOutput() && appConfig.getOptGenerateClearScript())
    {
        clearScriptStream.open( clearScriptFileName, std::ios_base::out | std::ios_base::trunc );
        if (!clearScriptStream)
            LOG_WARN_OPT("create-file-failed") << "failed to create '" << clearScriptSimpleName << "' file: " << clearScriptFileName << endl;
    }


    std::set<std::string> createdSubFoldersSet;
    for( auto path : createdFolders )
    {
        path = appConfig.getOutputRelativePath(path);
        if (path.empty())
           continue;

        auto upperPath = umba::filename::getPath(path);
        while(!upperPath.empty())
        {
            path = upperPath;
            upperPath = umba::filename::getPath(path);
        }
        createdSubFoldersSet.insert(path);
    }

    std::set<std::string> createdFilesSet;
    for( auto name : createdFiles )
    {
        name = appConfig.getOutputRelativePath(name);
        if (name.empty())
           continue;

        if (umba::filename::getPath(name).empty())
            createdFilesSet.insert(name);
    }

    for( auto fileName : createdFilesSet )
    {
        #if defined(WIN32) || defined(_WIN32)

            clearScriptStream << "@del " << fileName << "\n";

        #else
            
        #endif
    }

    for( auto folderName : createdSubFoldersSet )
    {
        #if defined(WIN32) || defined(_WIN32)

            clearScriptStream << "@rd /Q /S " << folderName << "\n";

        #else
            
        #endif
    }




    if (appConfig.testVerbosity(VerbosityLevel::normal))
    {
        umba::info_log::printSectionHeader(umbaLogStreamMsg, "Job done");

        umbaLogStreamMsg << "Total files created: " << totalFilesCreated << endl;

        auto tickDiff = umba::time_service::getCurTimeMs() - startTick;
        umbaLogStreamMsg << "Time elapsed: " << tickDiff << "ms" << "\n";
        //startTick = umba::time_service::getCurTimeMs();
    }


    return 0;
}


