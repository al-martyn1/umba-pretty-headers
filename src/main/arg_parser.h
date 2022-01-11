#pragma once

#include <stack>

#include "app_config.h"



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
int operator()( std::string a, ArgsParser &argsParser, umba::command_line::ICommandLineOptionCollector *pCol, bool fBuiltin, bool ignoreInfos)
{
    //const GeneratorOptions &gopts = generatorOptions; // stub for log

    umba::string_plus::trim(a);

    if (a.empty())
    {
        LOG_ERR_OPT<<"invalid (empty) argument\n";
        return -1;
    }

    umba::command_line::CommandLineOption opt(a, pCol);

    pCol->setCollectMode( opt.isHelpOption() );

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
        else if (opt.isOption("no-builtin-options") || opt.setDescription("Don't parse predefined options from command line options file 'conf/umba-pretty-headers.options' and 'conf/umba-pretty-headers.user'"))
        {
            // simple skip
        }

        //------------

        else if (opt.isOption("keep-compile-flags") || opt.isOption('K') || opt.setDescription("Keep generated compile_flags_*.txt"))
        {
            if (argsParser.hasHelpOption) return 0;
            appConfig.keepCompileFlags = true;
            return 0;
        }

        else if (opt.isOption("exclude-files") || opt.isOption('X') || opt.setDescription("Exclude files from parsing"))
        {
            if (argsParser.hasHelpOption) return 0;
            
            if (!opt.hasArg())
            {
                LOG_ERR_OPT<<"exclude files list not taken (--exclude-files)\n";
                return -1;
            }

            std::vector< std::string > lst = umba::string_plus::split(opt.optArg, ',');
            appConfig.excludeFilesList.insert(appConfig.excludeFilesList.end(), lst.begin(), lst.end());


            return 0;
        }

        else if (opt.isOption("set") || opt.isOption('S') || opt.setDescription("Set up macro in form: 'NAME:VALUE'"))
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

        else if (opt.isOption("path") || opt.isOption('P') || opt.setDescription("Add path to scan path list"))
        {
            if (argsParser.hasHelpOption) return 0;
            
            if (!opt.hasArg())
            {
                LOG_ERR_OPT<<"Add path to scan path list requires argument (--path)\n";
                return -1;
            }

            appConfig.scanPaths.push_back(makeAbsPath(opt.optArg));

            return 0;
        }


        //------------

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
                    std::cout<<"Usage: " << programLocationInfo.exeFullName << " [OPTIONS] input_file [output_file]\n\nOptions:\n\n"<<opt.getHelpOptionsString();
                
                if (pCol) // argsNeedHelp
                    std::cout<<pCol->makeText( 64, &argsParser.argsNeedHelp );

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

        auto parseRes = argsParser.parseBuiltinsFile( optFileName );

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



