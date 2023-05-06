#pragma once


#include "umba/umba.h"
#include "umba/simple_formatter.h"

#include <string>
#include <iostream>
#include <filesystem>
#include <regex>
#include <sstream>
#include <exception>
#include <stdexcept>
#include <vector>
#include <map>
#include <set>

#include "umba/filename.h"
#include "regex_helpers.h"

#include "utils.h"
#include "app_config.h"
// #include "regexp.h"


//----------------------------------------------------------------------------
extern umba::SimpleFormatter logMsg;





//----------------------------------------------------------------------------
inline
void scanFolders( const AppConfig &appConfig
                , std::vector<std::string> &foundFiles
                , std::vector<std::string> &excludedFiles
                , std::set<std::string>    &foundExtentions
                )
{
    using namespace umba::omanip;

    std::list<std::string> scanPaths( appConfig.scanPaths.begin(), appConfig.scanPaths.end() );

    std::map<std::string,std::regex>  excludeRegexes;
    std::map<std::string,std::string> excludeOriginalMasks;

    for(auto excludeFileMask : appConfig.excludeFilesMaskList)
    {
        //auto normalizedName = StringType normalizePathSeparators( const StringType &fileName, typename StringType::value_type pathSep = getNativePathSep<typename StringType::value_type>() )
        auto regexStr = expandSimpleMaskToEcmaRegex(excludeFileMask);
        excludeRegexes      [regexStr] = std::regex(regexStr);
        excludeOriginalMasks[regexStr] = excludeFileMask;
    }

    std::map<std::string,std::regex>  includeRegexes;
    std::map<std::string,std::string> includeOriginalMasks;

    for(auto includeFileMask : appConfig.includeFilesMaskList)
    {
        //auto normalizedName = StringType normalizePathSeparators( const StringType &fileName, typename StringType::value_type pathSep = getNativePathSep<typename StringType::value_type>() )
        auto regexStr = expandSimpleMaskToEcmaRegex(includeFileMask);
        includeRegexes      [regexStr] = std::regex(regexStr);
        includeOriginalMasks[regexStr] = includeFileMask;
    }


    bool bFound = false;

    namespace fs = std::filesystem;


    // std::smatch m;
    //  
    // if ( !regex_search(text, m, std::regex(rExpr) ) )
    //     return false;
    //  
    //  
    // std::smatch m;
    // std::regex r = std::regex(rExpr);
    // if ( !regex_search(text, m, r ) )
    //     return false;
    //  
    // std::regex("meow", std::regex::ECMAScript|std::regex::icase).

    for( std::list<std::string>::const_iterator it=scanPaths.begin(); it!=scanPaths.end(); ++it )
    {
        fs::directory_iterator scanPathDirectoryIterator;
        try
        {
            scanPathDirectoryIterator = fs::directory_iterator(*it);
        }
        catch(...)
        {
            LOG_ERR_OPT << "invalid scan path: " << *it << "\n";
            continue;
        }


        for (const auto & entry : scanPathDirectoryIterator)
        {
            // https://en.cppreference.com/w/cpp/filesystem/directory_entry

            if (!entry.exists())
                continue;

            //------------------------------
            std::string entryName = entry.path().string();

            if (entry.is_directory())
            {
                scanPaths.push_back(entryName);
                // std::cout << entry.path() << "\n";
                continue;
            }

            //------------------------------
            if (!entry.is_regular_file())
            {
                continue; // Какая-то шляпа попалась
            }

            //------------------------------
            entryName = umba::filename::makeCanonical(entryName);

            if (!bFound)
            {
                bFound = true;

                if (appConfig.testVerbosity(VerbosityLevel::detailed))
                {
                    printInfoLogSectionHeader(logMsg, "Found Files");
                    // logMsg << "---------------------\nFound Files:" << endl << "------------" << endl;
                }
            }


            if (appConfig.testVerbosity(VerbosityLevel::detailed))
            {
                logMsg << entryName << " - ";
            }

            auto normalizedEntryName = umba::filename::normalizePathSeparators(entryName,'/');

            //TODO: !!! Нужно что-то решать с отсутствующим расширением

            //umba::filename::

            bool addThisFile = false;
            bool excludedByIncludeMask = false;

            std::string includeRegexStr;
            std::string excludeRegexStr;

            bool matchInclude = true;
            if (!includeRegexes.empty()) // матчим только если не пусто
            {
                matchInclude = umba::regex_helpers::regexMatch(normalizedEntryName,includeRegexes,&includeRegexStr);
            }

            if (!matchInclude)
            {
                // Не подходит под инклюзивную маску
                addThisFile = false;
                excludedByIncludeMask = true;
            }
            else
            {
                addThisFile = true; // Вроде подошло, надо проверить исключения

                if (umba::regex_helpers::regexMatch(normalizedEntryName,excludeRegexes,&excludeRegexStr))
                {
                    addThisFile = false;
                    excludedByIncludeMask = false;
                }
            }

            if (addThisFile)
            {
                foundFiles.push_back(entryName);

                auto ext = umba::filename::getExt(entryName);

                foundExtentions.insert(ext);

                if (appConfig.testVerbosity(VerbosityLevel::detailed))
                {
                    if (ext.empty())
                        ext = "<EMPTY>";
                    else
                        ext = std::string(".") + ext;

                    logMsg << good << "added" << normal;
                    logMsg << " (" << notice << ext << normal << ")";
                    if (!includeRegexStr.empty())
                    {
                        // orgMask = includeOriginalMasks[includeRegexStr]
                        logMsg << " due include mask '" << includeOriginalMasks[includeRegexStr] << "' (" << includeRegexStr << ")" << normal;
                    }

                    if (ext.empty())
                    {
                        logMsg << " - !note: empty extention: " << notice << entryName << normal << "";
                        //logMsg << "\n";
                    }

                    logMsg << "\n";

                }
            }
            else
            {
                excludedFiles.push_back(entryName);

                if (appConfig.testVerbosity(VerbosityLevel::detailed))
                {
                    if (excludedByIncludeMask)
                    {
                        logMsg << notice << "skipped" <<  /* normal << */  " due include masks" << normal << "\n";
                    }
                    else
                    {
                        logMsg << notice << "skipped" <<  /* normal << */  " due exclude mask '" << excludeOriginalMasks[excludeRegexStr] << "' (" << excludeRegexStr << ")" << normal << "\n";
                    }
                    // std::string regexStr = excludedByIncludeMask ? includeRegexStr : excludeRegexStr;
                    // std::string orgMask  = excludedByIncludeMask ? includeOriginalMasks[includeRegexStr] : excludeOriginalMasks[excludeRegexStr];
                    // std::string wichMask = excludedByIncludeMask ? "include" : "exclude";
                    // logMsg << notice << "skipped" <<  /* normal << */  " due " << wichMask << " mask '" << orgMask << "' (" << regexStr << ")" << normal << endl;
                }
            }


    // std::map<std::string,std::regex>  includeRegexes;
    // std::map<std::string,std::string> includeOriginalMasks;

            #if 0
            std::string regexStr;
            if (!umba::regex_helpers::regexMatch(normalizedEntryName,excludeRegexes,&regexStr))
            {
                foundFiles.push_back(entryName);

                auto ext = umba::filename::getExt(entryName);

                foundExtentions.insert(ext);

                if (appConfig.testVerbosity(VerbosityLevel::detailed))
                {
                    if (ext.empty())
                        ext = "<EMPTY>";
                    else
                        ext = std::string(".") + ext;

                    logMsg << good << "Added" << normal;
                    logMsg << " (" << notice << ext << normal << ")";
                    logMsg << endl;
                }
            }
            else
            {
                excludedFiles.push_back(entryName);

                if (appConfig.testVerbosity(VerbosityLevel::detailed))
                    logMsg << notice << "skipped" <<  /* normal << */  " due '" << excludeOriginalMasks[regexStr] << "' (" << regexStr << ")" << normal << endl;
            }
            #endif

// error
// warning
// notice
// normal
// good

        }

    }
}


