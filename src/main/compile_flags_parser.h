#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>

#include "umba/string_plus.h"
#include "umba/filename.h"


/*

-I..\_libs\mem_pools\inc
-I
..\_libs
-I..\_src
-I..\_libs\sfmt\inc

## WIN32
-DWIN32

##MCU_STM32_F1xx
-DSTM32F103RB

##MCU_STM32_F3xx
-DSTM32F303RB

##MCU_STM32_F4xx
-DSTM32F405VG

*/


//----------------------------------------------------------------------------
inline
bool parseCompileFlags( const std::string &curFile, std::ifstream &optFile, std::map<std::string, std::vector<std::string> > &cflags, std::vector<std::string> &commonLines )
{
    bool appendToPrevLine = false;

    std::vector<std::string> curLines;
    std::string              curConfig;

    unsigned lineNo = 0;

    std::string optLine;
    auto startNewSection = [&]()
        {
            if (curConfig.empty())
                commonLines       = curLines;
            else
                cflags[curConfig] = curLines;

            appendToPrevLine = false;
            curConfig        = optLine;
            curLines.clear();
         
        };


    while( std::getline( optFile, optLine ) )
    {
        ++lineNo;
        umba::string_plus::trim(optLine);

        if (optLine.empty())
            continue;

        if (umba::string_plus::starts_with_and_strip( optLine, "##" ))
        {
            umba::string_plus::trim(optLine);
            startNewSection();
            continue;
        }

        if (umba::string_plus::starts_with( optLine, "#" )) // skip comment lines
            continue;

        if (appendToPrevLine)
        {
            if (umba::string_plus::starts_with( optLine, "-" ))
            {
                LOG_ERR << "missing option value for option in previous line\n";
                return false;
            }

            if (curLines.empty())
                curLines.push_back(optLine);
            else
                curLines.back().append(optLine);

            appendToPrevLine = false;
            continue;
        }

        curLines.push_back(optLine);

        if (optLine=="-I" || optLine=="-D") // Known options wich can be splitted to separate lines
            appendToPrevLine = true;

    }

    startNewSection();

    return true;
}

//----------------------------------------------------------------------------
inline
bool parseCompileFlags( const std::string &fileName, std::map<std::string, std::vector<std::string> > &cflags, std::vector<std::string> &commonLines )
{
    std::ifstream optFile(fileName.c_str());
    if (!optFile)
        return false; // resVec;

    return parseCompileFlags( fileName, optFile, cflags, commonLines );
}

//----------------------------------------------------------------------------
inline
std::string filterFilenameForbiddenChars( std::string s )
{
    /*
    https://stackoverflow.com/questions/1976007/what-characters-are-forbidden-in-windows-and-linux-directory-names
    < (less than)
    > (greater than)
    : (colon - sometimes works, but is actually NTFS Alternate Data Streams)
    " (double quote)
    / (forward slash)
    \ (backslash)
    | (vertical bar or pipe)
    ? (question mark)
    * (asterisk)
   
    also add ';', '&', '%', '{', '}', '!', '$' and ' ' - to forbidden set
   
    */

    std::string forbiddenChars = " <>:;\"\'\\/|?*&{}%!$";

    for(auto &ch: s)
    {
        if (forbiddenChars.find(ch)!=forbiddenChars.npos)
            ch = '_';
    }

    return s;
}

//----------------------------------------------------------------------------
inline
bool generateCompileFlags( const std::string &baseFileName
                         , const std::map<std::string, std::vector<std::string> > &cflags
                         , const std::vector<std::string> &commonLines
                         , std::vector<std::string> &generatedFiles
                         )
{
    std::string basePath = umba::filename::getPath(baseFileName);
    std::string baseName = umba::filename::getName(baseFileName);
    std::string baseExt  = umba::filename::getExt(baseFileName);

    // https://en.cppreference.com/w/cpp/language/range-for
    for(auto [key,val] : cflags)
    {
        auto configName = filterFilenameForbiddenChars(key);

        auto fileName = umba::filename::appendPath(basePath, umba::filename::appendExt( baseName+std::string("_")+configName, baseExt));

        auto lines = commonLines;

        lines.insert(lines.end(), val.begin(), val.end());

        std::ofstream optFile(fileName.c_str());
        if (!optFile)
            return false; // resVec;

        generatedFiles.push_back(fileName);

        auto text = umba::string_plus::merge<std::string>(lines,"\n");

        optFile << text << "\n";
    }

    return true;

}


