#pragma once

#include <stack>

#include "app_config.h"
#include "umba/cmd_line.h"
#include "marty_clang_helpers.h"


AppConfig    appConfig;


struct ArgParser
{

std::stack<std::string> optFiles;


std::string makeAbsPath( std::string p )
{
    std::string basePath;

    if (optFiles.empty())
        basePath = umba::filesys::getCurrentDirectory<std::string>();
    else
        basePath = umba::filename::getPath(optFiles.top());


    return umba::filename::makeAbsPath( p, basePath );

}



// 0 - ok, 1 normal stop, -1 - error
template<typename ArgsParser>
int operator()( const std::string                               &a           //!< строка - текущий аргумент
              , umba::command_line::CommandLineOption           &opt         //!< Объект-опция, содержит разобранный аргумент и умеет отвечать на некоторые вопросы
              , ArgsParser                                      &argsParser  //!< Класс, который нас вызывает, содержит некоторый контекст
              , umba::command_line::ICommandLineOptionCollector *pCol        //!< Коллектор опций - собирает инфу по всем опциям и готов вывести справку
              , bool fBuiltin
              , bool ignoreInfos
              )
{
    //using namespace marty::clang::helpers;

    std::string dppof = "Don't parse predefined options from ";

    if (opt.isOption())
    {
        std::string errMsg;
        int intVal;

        if (opt.name.empty())
        {
            LOG_ERR_OPT<<"invalid (empty) option name\n";
            return -1;
        }

       if (opt.isOption("quet") || opt.isOption('q') || opt.setDescription("Operate quetly. Short alias for '--verbose=quet'"))
        {
            argsParser.quet = true;
            appConfig.setOptQuet(true);
        }

        else if (opt.setParam("LEVEL", 1, "0/quet/no/q|" 
                                          "1/normal/n|" 
                                          "2/config/c|" 
                                          "3/detailed/detail/d|" 
                                          "4/extra/high/e" 
                             )
              || opt.setInitial(1) || opt.isOption("verbose") || opt.isOption('V')
              || opt.setDescription("Set verbosity level. LEVEL parameter can be one of the next values:\n"
                                    "quet - maximum quet mode (same as --quet).\n"
                                    "normal - print common details.\n"
                                    "config - print common details and app config.\n"
                                    "detailed - print common details, app config and all declarations, which are found in user files.\n"
                                    "extra - print common details, app config and all found declarations (from all files)." // "\n"
                                   )
              )
        {
            if (argsParser.hasHelpOption) return 0;

            auto mapper = [](int i) -> VerbosityLevel
                          {
                              //return AppConfig::VerbosityLevel_fromStdString((VerbosityLevel)i);
                              switch(i)
                              {
                                  case  0: case  1: case  2: case  3: case  4: return (VerbosityLevel)i;
                                  default: return VerbosityLevel::begin;
                              }
                          };

            VerbosityLevel lvl;
            if (!opt.getParamValue( lvl, errMsg, mapper ) )
            {
                LOG_ERR_OPT<<errMsg<<"\n";
                return -1;
            }

            appConfig.setVerbosityLevel(lvl);
            if (lvl==VerbosityLevel::quet)
                argsParser.quet = true;
        }

        else if ( opt.isBuiltinsDisableOptionMain  () 
               || opt.setDescription( dppof + "main distribution options file '" + argsParser.getBuiltinsOptFileName(umba::program_location::BuiltinOptionsLocationFlag::appGlobal   ) + "'"))
        { } // simple skip - обработка уже сделана

        else if ( opt.isBuiltinsDisableOptionCustom() 
               || opt.setDescription( dppof + "custom global options file '"     + argsParser.getBuiltinsOptFileName(umba::program_location::BuiltinOptionsLocationFlag::customGlobal) + "'"))
        { } // simple skip - обработка уже сделана

        else if ( opt.isBuiltinsDisableOptionUser  () 
               || opt.setDescription( dppof + "user local options file '"        + argsParser.getBuiltinsOptFileName(umba::program_location::BuiltinOptionsLocationFlag::userLocal   ) + "'"))
        { } // simple skip - обработка уже сделана

        else if (opt.isOption("version") || opt.isOption('v') || opt.setDescription("Show version info"))
        {
            if (argsParser.hasHelpOption) return 0;

            if (!ignoreInfos)
            {
                printOnlyVersion();
                return 1;
            }
        }

        else if (opt.isOption("where") || opt.setDescription("Show where the executable file is"))
        {
            if (argsParser.hasHelpOption) return 0;

            LOG_MSG_OPT << programLocationInfo.exeFullName << "\n";
            return 0;
        }

        else if (opt.setParam("CLR", 0, "no/none/file|" 
                                        "ansi/term|" 
                                        #if defined(WIN32) || defined(_WIN32)
                                        "win32/win/windows/cmd/console"
                                        #endif
                             )
              || opt.setInitial(-1) || opt.isOption("color") 
              || opt.setDescription("Force set console output coloring")
              /* ", can be:\nno, none, file - disable coloring\nansi, term - set ansi terminal coloring\nwin32, win, windows, cmd, console - windows console specific coloring method" */
              )
        {
            if (argsParser.hasHelpOption) return 0;

            umba::term::ConsoleType res;
            auto mapper = [](int i) -> umba::term::ConsoleType
                          {
                              switch(i)
                              {
                                  case 0 : return umba::term::ConsoleType::file;
                                  case 1 : return umba::term::ConsoleType::ansi_terminal;
                                  case 2 : return umba::term::ConsoleType::windows_console;
                                  default: return umba::term::ConsoleType::file;
                              };
                          };
            if (!opt.getParamValue( res, errMsg, mapper ) )
            {
                LOG_ERR_OPT<<errMsg<<"\n";
                return -1;
            }

            coutWriter.forceSetConsoleType(res);
            cerrWriter.forceSetConsoleType(res);
        }

        //------------

        else if ( opt.isOption("keep-generated-files") || opt.isOption('K') 
               // || opt.setParam("VAL",true)
               || opt.setDescription("Do not delete generated files"))
        {
            if (argsParser.hasHelpOption) return 0;
            appConfig.setOptKeepGenerated(true);
            return 0;
        }

        else if ( opt.isOption("quoted-include") || opt.isOption('U')
               || opt.setDescription("By default, the '#include' directive in generated files uses angle brackets '<>'. This option turns on generating quoted includes with '\"\"' quotation")
                )
        {
            if (argsParser.hasHelpOption) return 0;
            appConfig.setOptQuotedIncludes(true);
            return 0;
        }

        else if ( opt.isOption("exclude-names") || opt.isOption('N') || opt.setParam("MASK,...")
               || opt.setDescription("Exclude C/C++ names from output. For details about 'MASK' parameter see '--exclude-files' option description.")
                )
        {
            if (argsParser.hasHelpOption) return 0;
            
            if (!opt.hasArg())
            {
                LOG_ERR_OPT<<"exclude names mask not taken (--exclude-names)\n";
                return -1;
            }

            std::vector< std::string > lst = umba::string_plus::split(opt.optArg, ',');
            appConfig.excludeNamesMaskList.insert(appConfig.excludeNamesMaskList.end(), lst.begin(), lst.end());

            return 0;
        }

        else if ( opt.isOption("output-path") || opt.isOption("output-root") ||  /* opt.isOption("output") ||  */ opt.isOption('O') || opt.setParam("PATH")
               || opt.setDescription("Set output root path")
                )
        {
            if (argsParser.hasHelpOption) return 0;
            
            if (!opt.hasArg())
            {
                LOG_ERR_OPT<<"output path not taken (--output-path)\n";
                return -1;
            }

            auto optArg = umba::macros::substMacros(opt.optArg,umba::macros::MacroTextFromMapOrEnv<std::string>(appConfig.macros),umba::macros::keepUnknownVars);
            appConfig.outputPath = makeAbsPath(optArg);
            return 0;
        }

        else if ( opt.isOption("exclude-files") || opt.isOption('X') || opt.setParam("MASK,...")
               || opt.setDescription("Exclude files from parsing. The 'MASK' parameter is a simple file mask, where '*' "
                                     "means any number of any chars, and '?' means exact one of any char. In addition, "
                                     "symbol '^' in front and/or back of the mask means that the mask will be bound to beginning/ending "
                                     "of the tested file name.\n"
                                     "Also, regular expresion syntax allowed in form '" + 
                                     umba::regex_helpers::getRawEcmaRegexPrefix<std::string>() + "YOURREGEX'. The regular expresions supports\n"
                                     "See also: C++ Modified ECMA Script regular expression grammar - https://en.cppreference.com/w/cpp/regex/ecmascript"
                                    )
                )
        {
            if (argsParser.hasHelpOption) return 0;
            
            if (!opt.hasArg())
            {
                LOG_ERR_OPT<<"exclude files mask not taken (--exclude-files)\n";
                return -1;
            }

            std::vector< std::string > lst = umba::string_plus::split(opt.optArg, ',');
            appConfig.excludeFilesMaskList.insert(appConfig.excludeFilesMaskList.end(), lst.begin(), lst.end());

            return 0;
        }

        #if 0
            #define UMBA_PRETTY_HEADERS_HANDLE_KIND_ARG(flag)                       \
                                                                                    {}

        #else
            #define UMBA_PRETTY_HEADERS_HANDLE_KIND_ARG(flag)                       \
                                                                                    \
                                {                                                   \
                                    if (argsParser.hasHelpOption) return 0;         \
                                    appConfig.addDeclKind(marty::clang::helpers :: DeclKindOfKind :: flag);  \
                                    return 0;                                       \
                                }
        #endif

        else if (opt.isOption("class") || opt.isOption('C')        || opt.setDescription("Allow to generate class/struct includes"))
            UMBA_PRETTY_HEADERS_HANDLE_KIND_ARG(cxxClass)

        else if (opt.isOption("function") || opt.isOption('F')     || opt.setDescription("Allow to generate free function includes"))
            UMBA_PRETTY_HEADERS_HANDLE_KIND_ARG(freeFunction)

        else if (opt.isOption("typedef") || opt.isOption('T')      || opt.setDescription("Allow to generate typedef includes"))
            UMBA_PRETTY_HEADERS_HANDLE_KIND_ARG(cTypedef)

        else if (opt.isOption("type-alias") || opt.isOption('L')   || opt.setDescription("Allow to generate type alias includes"))
            UMBA_PRETTY_HEADERS_HANDLE_KIND_ARG(cxxTypeAlias)

        else if (opt.isOption("var-template") || opt.isOption('M') || opt.setDescription("Allow to generate variable template includes"))
            UMBA_PRETTY_HEADERS_HANDLE_KIND_ARG(cxxVarTemplate)

        else if (opt.isOption("enum") || opt.isOption('E')         || opt.setDescription("Allow to generate enum includes (both enum and enum class)"))
            UMBA_PRETTY_HEADERS_HANDLE_KIND_ARG(cEnum)

        else if (opt.isOption("define") || opt.isOption('D')       || opt.setDescription("Allow to generate preprocessor define includes"))
            UMBA_PRETTY_HEADERS_HANDLE_KIND_ARG(ppDefine)

        else if (opt.isOption("type") || opt.isOption('Y')         || opt.setDescription("Allow to generate all type includes (equivalent to --class --typedef --type-alias --enum)"))
            UMBA_PRETTY_HEADERS_HANDLE_KIND_ARG(type)

        else if (opt.isOption("all") || opt.isOption('A')          || opt.setDescription("Allow to generate includes for all kinds of declaration, except to --define option.\n--all is the default option."))
            UMBA_PRETTY_HEADERS_HANDLE_KIND_ARG(all)


        else if (opt.isOption("generate-clear-script") || opt.isOption('R') || opt.setDescription("Generate clear script - .bat/.sh."))
            {
                if (argsParser.hasHelpOption) return 0;
                appConfig.setOptGenerateClearScript(true);
                return 0;
            }

        else if (opt.isOption("used-macros")    || opt.setDescription("Write all found (in PP conditions) macros to file '$(OutputRoot)/__used_macros.txt'."))
            {
                if (argsParser.hasHelpOption) return 0;
                appConfig.setOptUsedMacros(true);
                return 0;
            }

        else if (opt.isOption("defined-macros") || opt.setDescription("Write all defined macros to file '$(OutputRoot)/__defined_macros.txt'."))
            {
                if (argsParser.hasHelpOption) return 0;
                appConfig.setOptDefinedMacros(true);
                return 0;
            }

        else if (opt.isOption("no-output") || opt.isOption("dry-run") || opt.setDescription("Do not actually write output files. Simulation mode. Behave normally, but do not copy/creater/update any files."))
            {
                if (argsParser.hasHelpOption) return 0;
                appConfig.setOptNoOutput(true);
                return 0;
            }

        // else if (opt.isOption("XXX") || opt.setDescription("Allow to generate XXX includes"))
        //     UMBA_PRETTY_HEADERS_HANDLE_KIND_ARG()

        else if ( opt.isOption("set") || opt.isOption('S') || opt.setParam("NAME:VALUE")
               || opt.setDescription("Set up macro in form: 'NAME:VALUE'"))
        {
            if (argsParser.hasHelpOption) return 0;
            
            if (!opt.hasArg())
            {
                LOG_ERR_OPT<<"Set up macro requires at least macro name (--set)\n";
                return -1;
            }

            std::string name, val;
            umba::string_plus::split_to_pair( opt.optArg, name, val, ':' );
            appConfig.macros[name] = val;

            return 0;
        }

        else if ( opt.isOption("path") || opt.isOption("scan") || opt.isOption('P') || opt.setParam("PATH")
               || opt.setDescription("Add path to scan path list"))
        {
            if (argsParser.hasHelpOption) return 0;
            
            if (!opt.hasArg())
            {
                LOG_ERR_OPT<<"Add path to scan path list requires argument (--path)\n";
                return -1;
            }

            auto optArg = umba::macros::substMacros(opt.optArg,umba::macros::MacroTextFromMapOrEnv<std::string>(appConfig.macros),umba::macros::keepUnknownVars);
            appConfig.scanPaths.push_back(makeAbsPath(optArg));

            return 0;
        }


        //------------

        else if ( opt.isOption("autocomplete-install") 
               || opt.setDescription("Install autocompletion to bash"
                                     #if defined(WIN32) || defined(_WIN32)
                                         "/clink(cmd)"
                                     #endif
                                    )
               )
        {
            if (argsParser.hasHelpOption) return 0;

            //return autocomplete(opt, true);
            return umba::command_line::autocompletionInstaller( pCol, opt, pCol->getPrintHelpStyle(), true, [&]( bool bErr ) -> decltype(auto) { return bErr ? LOG_ERR_OPT : LOG_MSG_OPT; } );
        }
        else if ( opt.isOption("autocomplete-uninstall") 
               || opt.setDescription("Remove autocompletion from bash"
                                     #if defined(WIN32) || defined(_WIN32)
                                         "/clink(cmd)"
                                     #endif
                                    )
                )
        {
            if (argsParser.hasHelpOption) return 0;

            //return autocomplete(opt, false);
            return umba::command_line::autocompletionInstaller( pCol, opt, pCol->getPrintHelpStyle(), false, [&]( bool bErr ) -> decltype(auto) { return bErr ? LOG_ERR_OPT : LOG_MSG_OPT; } );
        }

        else if (opt.isHelpStyleOption())
        {
            // Job is done in isHelpStyleOption
        }
        else if (opt.isHelpOption()) // if (opt.infoIgnore() || opt.isOption("help") || opt.isOption('h') || opt.isOption('?') || opt.setDescription(""))
        {
            if (!ignoreInfos)
            {
                if (pCol && !pCol->isNormalPrintHelpStyle())
                    argsParser.quet = true;
                //printNameVersion();
                if (!argsParser.quet)
                {
                    printBuildDateTime();
                    printCommitHash();
                    std::cout<<"\n";
                //printHelp();
                }

                if (pCol && pCol->isNormalPrintHelpStyle() && argsParser.argsNeedHelp.empty())
                {
                    auto helpText = opt.getHelpOptionsString();
                    std::cout<<"Usage: " << programLocationInfo.exeFullName << " [OPTIONS] input_file [output_file]\n\nOptions:\n\n"<<helpText;
                }
                
                if (pCol) // argsNeedHelp
                    std::cout<<pCol->makeText( 78, &argsParser.argsNeedHelp );

                return 1;

            }

            return 0; // simple skip then parse builtins
        }
        else
        {
            LOG_ERR_OPT<<"unknown option: "<<opt.argOrg<<"\n";
            return -1;
        }

        return 0;

    } // if (opt.isOption())
    else if (opt.isResponseFile())
    {
        std::string optFileName = makeAbsPath(opt.name);

        optFiles.push(optFileName);

        auto parseRes = argsParser.parseOptionsFile( optFileName );

        optFiles.pop();

        if (!parseRes)
            return -1;

        if (argsParser.mustExit)
            return 1;

        /*
        std::ifstream optFile(opt.name.c_str());
        if (!optFile)
        {
            LOG_ERR_OPT<<"failed to read response file '"<<opt.name<<"'\n";
            return -1;
        }
        else
        {
            std::string optLine;
            while( std::getline( optFile, optLine) )
            {
                umba::string_plus::trim(optLine);
                if (optLine.empty())
                    continue;

                if (umba::command_line::isComment( optLine ))
                    continue;

                int paRes = parseArg( optLine, pCol, false, true );
                if (paRes)
                {
                   return paRes;
                }
            }
        }
        */

        return 0;
    
    }

    appConfig.clangCompileFlagsTxtFilename.push_back(makeAbsPath(a));

/*    
    if (inputFilename.empty())
        inputFilename = a;
    else
        outputFilename = a;
*/

    return 0;

}

}; // struct ArgParser



class CommandLineOptionCollector : public umba::command_line::CommandLineOptionCollectorImplBase
{
protected:
    virtual void onOptionDup( const std::string &opt ) override
    {
        LOG_ERR_OPT<<"Duplicated option key - '"<<opt<<"'\n";
        throw std::runtime_error("Duplicated option key");
    }

};



