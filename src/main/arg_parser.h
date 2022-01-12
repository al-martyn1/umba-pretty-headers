#pragma once

#include <stack>

#include "app_config.h"
#include "umba/cmd_line.h"


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
    //const GeneratorOptions &gopts = generatorOptions; // stub for log

    // umba::string_plus::trim(a);
    //  
    // if (a.empty())
    // {
    //     LOG_ERR_OPT<<"invalid (empty) argument\n";
    //     return -1;
    // }
    //  
    // umba::command_line::CommandLineOption opt(a, pCol);

    // pCol->setCollectMode( opt.isHelpOption() );

    if (opt.isOption())
    {
        std::string errMsg;
        int intVal;

        if (opt.name.empty())
        {
            LOG_ERR_OPT<<"invalid (empty) option name\n";
            return -1;
        }

       if (opt.isOption("quet") || opt.isOption('q') || opt.setDescription("Operate quetly"))
        {
            argsParser.quet = true;
            appConfig.setOptQuet(true);
        }
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

        else if (opt.isBuiltinsDisableOptionMain  () || opt.setDescription("Don't parse predefined options from main distribution options file '" + argsParser.getBuiltinsOptFileName(umba::program_location::BuiltinOptionsLocationFlag::appGlobal   ) + "'"))
        { } // simple skip - обработка уже сделана

        else if (opt.isBuiltinsDisableOptionCustom() || opt.setDescription("Don't parse predefined options from custom global options file '"     + argsParser.getBuiltinsOptFileName(umba::program_location::BuiltinOptionsLocationFlag::customGlobal) + "'"))
        { } // simple skip - обработка уже сделана

        else if (opt.isBuiltinsDisableOptionUser  () || opt.setDescription("Don't parse predefined options from user local options file '"        + argsParser.getBuiltinsOptFileName(umba::program_location::BuiltinOptionsLocationFlag::userLocal   ) + "'"))
        { } // simple skip - обработка уже сделана

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

        else if ( opt.isOption("keep-compile-flags") || opt.isOption('K') 
               // || opt.setParam("VAL",true)
               || opt.setDescription("Keep generated 'compile_flags_*.txt'"))
        {
            if (argsParser.hasHelpOption) return 0;
            appConfig.keepCompileFlags = true;
            return 0;
        }

        else if ( opt.isOption("exclude-files") || opt.isOption('X') || opt.setParam("MASK")
               || opt.setDescription("Exclude files from parsing. The 'MASK' parameter is a simple file mask, where '*' means any number of any chars, and '?' means exact one any char. In addition, symbol '^' in front and/or back of mask means that mask will be bound to beginning/ending of the tested file name"))
        {
            if (argsParser.hasHelpOption) return 0;
            
            if (!opt.hasArg())
            {
                LOG_ERR_OPT<<"exclude files mask not taken (--exclude-files)\n";
                return -1;
            }

            std::vector< std::string > lst = umba::string_plus::split(opt.optArg, ',');
            appConfig.excludeFilesList.insert(appConfig.excludeFilesList.end(), lst.begin(), lst.end());


            return 0;
        }

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

        else if ( opt.isOption("path") || opt.isOption('P') || opt.setParam("PATH")
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

        else if (opt.isOption("autocomplete-install") || opt.setDescription("Install autocompletion to bash/clink(cmd)"))
        {
            if (argsParser.hasHelpOption) return 0;

            //return autocomplete(opt, true);
            return umba::command_line::autocompletionInstaller( pCol, opt, pCol->getPrintHelpStyle(), true, [&]( bool bErr ) -> decltype(auto) { return bErr ? LOG_ERR_OPT : LOG_MSG_OPT; } );
        }
        else if (opt.isOption("autocomplete-uninstall") || opt.setDescription("Remove autocompletion from bash/clink(cmd)"))
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



