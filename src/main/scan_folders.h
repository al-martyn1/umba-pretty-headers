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

#include "utils.h"
#include "app_config.h"
// #include "regexp.h"


//----------------------------------------------------------------------------
extern umba::SimpleFormatter logMsg;




//----------------------------------------------------------------------------
//! Легким движением руки простая маска превращается в регэксп
/*! Символ ^ (крышечка, XOR) - если первый символ маски - привязывает маску к началу текста
                             - если последний символ маски - привязывает маску к концу текста
 */
inline
std::string expandSimpleMaskToEcmaRegex( std::string s )
{
    std::string res; res.reserve(s.size());


    // https://en.cppreference.com/w/cpp/regex/ecmascript

    // The assertion ^ (beginning of line) matches
    //  
    // 1) The position that immediately follows a LineTerminator character (this may not be supported) 
    //    (until C++17) (this is only guaranteed if std::regex_constants::multiline(C++ only) is enabled) (since C++17)
    // 2) The beginning of the input (unless std::regex_constants::match_not_bol(C++ only) is enabled)


    // The assertion $ (end of line) matches
    //  
    // 1) The position of a LineTerminator character (this may not be supported) (until C++17)(this is only guaranteed 
    //    if std::regex_constants::multiline(C++ only) is enabled) (since C++17)
    // 2) The end of the input (unless std::regex_constants::match_not_eol(C++ only) is enabled)


    // characters ^ $ \ . * + ? ( ) [ ] { } |

    // Single character matches The Atom '.' matches and consumes any one character 
    // from the input string except for LineTerminator (U+000D, U+000A, U+2029, or U+2028)
    // Quantifiers
    // { DecimalDigits } exact DecimalDigits
    // { DecimalDigits , } from value of DecimalDigits to inf

    bool glueBeginning = s.empty() ? false : s.front()=='^';
    bool glueEnding    = s.empty() ? false : s.back ()=='^';

    if (glueEnding)
        s.erase(s.size()-1,1);

    if (glueBeginning)
        s.erase(0,1);

    for(auto ch: s)
    {
        switch(ch)
        {
            // case '*' : res.append(".{1,}");
            case '*' : res.append(".*");
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


    }

    return ( glueBeginning ? std::string("^") : std::string() )
         + res
         + ( glueEnding    ? std::string("$") : std::string() )
         ;
}

//----------------------------------------------------------------------------
inline
bool regexMatch(const std::string &text, const std::regex &r)
{
    std::smatch m;
    if ( !regex_search(text, m, r ) )
        return false;

    return true;
}

//----------------------------------------------------------------------------
inline
bool regexMatch(const std::string &text, const std::string &r)
{
    return regexMatch(text,std::regex(r));
}

//----------------------------------------------------------------------------
inline
bool regexMatch( const std::string &text
               , const std::map<std::string,std::regex> &regexes
               , std::string *pMatchedRegexText
               )
{
    for(auto [key,val] : regexes)
    {
        if (regexMatch(text,val))
        {
            if (pMatchedRegexText)
               *pMatchedRegexText = key;
            return true;
        }
    }

    return false;
}

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

    std::map<std::string,std::regex>  regexes;
    std::map<std::string,std::string> originalMasks;

    for(auto excludeFileMask : appConfig.excludeFilesList)
    {
        //auto normalizedName = StringType normalizePathSeparators( const StringType &fileName, typename StringType::value_type pathSep = getNativePathSep<typename StringType::value_type>() )
        auto regexStr = expandSimpleMaskToEcmaRegex(excludeFileMask);
        regexes      [regexStr] = std::regex(regexStr);
        originalMasks[regexStr] = excludeFileMask;
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
            if (appConfig.getOptNormalizeFilenames())
            {
                entryName = umba::filename::makeCanonical(entryName);
            }

            if (!bFound)
            {
                bFound = true;

                if (!appConfig.getOptQuet())
                {
                    printInfoLogSectionHeader(logMsg, "Found Files");
                    // logMsg << "---------------------\nFound Files:" << endl << "------------" << endl;
                }
            }


            if (!appConfig.getOptQuet())
            {
                logMsg << entryName << " - ";
            }

            auto normalizedEntryName = umba::filename::normalizePathSeparators(entryName,'/');

            std::string regexStr;
            if (!regexMatch(normalizedEntryName,regexes,&regexStr))
            {
                foundFiles.push_back(entryName);

                auto ext = umba::filename::getExt(entryName);

                foundExtentions.insert(ext);

                if (!appConfig.getOptQuet())
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

                if (!appConfig.getOptQuet())
                    logMsg << notice << "Skipped" <<  /* normal << */  " due '" << originalMasks[regexStr] << "' (" << regexStr << ")" << normal << endl;
            }

// error
// warning
// notice
// normal
// good

        }

    }
}


