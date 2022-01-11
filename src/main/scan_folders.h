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


#include "app_config.h"
// #include "regexp.h"


extern umba::SimpleFormatter logMsg;



// https://en.cppreference.com/w/cpp/regex/ecmascript
inline
std::string expandSimpleMaskToEcmaRegex( const std::string &s )
{
    std::string res; res.reserve(s.size());

    for(auto ch: s)
    {
        switch(ch)
        {
            case '*' : res.append(".{1,}");
                       break;
            case '?' : res.append(".{1}");
                       break;
            case '^' : res.append("\\^");
                       break;
            case '$' : res.append("\\$");
                       break;
            case '\\': res.append("\\\\");
                       break;
            case '.' : res.append("\\.");
                       break;
            // case '*' : res.append("\\*");
            //            break;
            // case '?' : res.append("\\?");
            //            break;
            case '+' : res.append("\\+");
                       break;
            case '(' : res.append("\\(");
                       break;
            case ')' : res.append("\\)");
                       break;
            case '[' : res.append("\\[");
                       break;
            case ']' : res.append("\\]");
                       break;
            case '{' : res.append("\\{");
                       break;
            case '}' : res.append("\\}");
                       break;
            case '|' : res.append("\\|");
                       break;
            default:
                       res.append(1, ch);
        }


        // characters ^ $ \ . * + ? ( ) [ ] { } |

        // Single character matches The Atom '.' matches and consumes any one character 
        // from the input string except for LineTerminator (U+000D, U+000A, U+2029, or U+2028)
        // Quantifiers
        // { DecimalDigits } exact DecimalDigits
        // { DecimalDigits , } from value of DecimalDigits to inf

    }

    return res;
}


inline
void scanFolders(const AppConfig &appConfig, std::vector<std::string> &foundFiles)
{
    std::list<std::string> scanPaths( appConfig.scanPaths.begin(), appConfig.scanPaths.end() );

    std::map<std::string,std::regex>  regexes;
    std::map<std::string,std::string> originalMasks;

    for(auto excludeFileMask : appConfig.excludeFilesList)
    {
        //auto normalizedName = StringType normalizePathSeparators( const StringType &fileName, typename StringType::value_type pathSep = getNativePathSep<typename StringType::value_type>() )
        auto regexStr = expandSimpleMaskToEcmaRegex(excludeFileMask);
        regexes      [regexStr] = std::regex(regexStr);
        originalMasks[regexStr] = excludeFileMask;
    }

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
        // std::string excludedByMask;
        // std::string excludedByRegex;

        for (const auto & entry : fs::directory_iterator(*it))
        {
            // https://en.cppreference.com/w/cpp/filesystem/directory_entry

            if (!entry.exists())
                continue;

            if (entry.is_directory())
            {
                scanPaths.push_back(entry.path().string());
                std::cout << entry.path() << "\n";
                continue;
            }

            if (!entry.is_regular_file())
            {
                continue; // Какая-то шляпа попалась
            }

            // exists
            // is_directory
            // is_regular_file
            std::cout << entry.path() << "\n";
        }

    }
// error
// warning
// notice
}


