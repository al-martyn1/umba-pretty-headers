#pragma once

#include <fstream>
#include <vector>
#include <map>
#include <set>
#include <string>

#include "umba/filename.h"
#include "utils.h"

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
inline
bool isValidMacroName( const std::string &name )
{
    if (name.empty())
        return false;

    if (isCppSpecialName(name))
        return false;

    if (name[0]>='0' && name[0]<='9')
        return false;

    return true;
}

//----------------------------------------------------------------------------
inline
std::string replaceInvalidCharsInCppName( std::string s, char replaceCh = ' ')
{
    for( auto &ch : s )
    {
        if (ch>='A' && ch<='Z')
            continue;
        if (ch>='0' && ch<='9')
            continue;
        if (ch>='a' && ch<='z')
            continue;
        if (ch=='_' || ch=='$')
            continue;
    
        ch = replaceCh;
    }

    return s;
}

//----------------------------------------------------------------------------
inline
std::string removeComments( std::string s )
{
    auto startPos = s.find("/*");
    auto endPos   = startPos;
    if (endPos!=s.npos)
        endPos = s.find("*/", endPos+1);

    while(endPos!=s.npos)
    {
        s.erase(startPos,endPos-startPos+2);
        startPos = s.find("/*");
        endPos   = startPos;
        if (endPos!=s.npos)
            endPos = s.find("*/", endPos+1);
    }

    return s;
}

//----------------------------------------------------------------------------
inline
void scanForPp( const std::string                                           &fileName
              , std::map<std::string, std::map<std::string,std::string> >   &foundDefinitions
              , std::map<std::string, std::map<std::string,std::string> >   &foundUsages
              )
{
    auto canonicalName = umba::filename::makeCanonical(fileName);
    auto keyName       = umba::filename::makeCanonical(fileName, '/');


    std::ifstream cxxStream(canonicalName);
    if (!cxxStream)
        return;

    std::vector<std::string> lines;
    lines.reserve(2048);

    
    for( std::string line; std::getline(cxxStream, line); )
    {
        lines.push_back(line);
    }

    const std::string inlineComment = "/*LF*/";
    using namespace umba::string_plus;

    {
        std::vector<std::string> gluedLines;
        gluedLines.reserve(lines.size());
       
        for( auto line : lines )
        {
            if (ends_with_and_strip(line,"\\"))
            {
                // Добавляем комент, чтобы обозначить, что тут было проболжение строки
                // Для того, чтобы можно было отбросить define'ы вида

                #define \
                        UMBA_PRETTY_HEADERS_SCAN_FOR_PP_EXAMPLE_MACRO

                // Такие дефайны мы не будем включать в результат - это скорее всего сделано как раз для этого,
                // потому, что в здравом уме никто так не пишет

                if (gluedLines.empty())
                    gluedLines.push_back(inlineComment+line);
                else
                    gluedLines.back().append(inlineComment+line);
            }
            else
            {
                gluedLines.push_back(line);
            }
        }

        gluedLines.swap(lines);
    }

    // С продолжениями строк разобрались

    for( auto line : lines )
    {
        std::string orgLine = line;
        trim(line);

        if (!starts_with_and_strip(line,"#"))
            continue; // Не препроцессор - пропускаем

        ltrim(line);

        if (starts_with_and_strip(line,"define "))
        {
            ltrim(line);

            auto pos = line.find_first_of( "<=>?!~[]&|+-/*%.(){} " );
            if (pos!=line.npos)
                line.erase(pos); // Удаляем хвост, включая найденную позицию

            trim(line);

            if (isValidMacroName(line))
                foundDefinitions[keyName][line] = orgLine;

            continue;

        }
        else if (starts_with_and_strip(line,"ifndef") || starts_with_and_strip(line,"ifdef") || starts_with_and_strip(line,"if"))
        {
            line = removeComments(line);
            line = replaceInvalidCharsInCppName( line, ' ' /* replaceCh */);
            trim(line);
            std::vector<std::string> words = umba::string_plus::split(line, ' ', false);
            umba::string_plus::cont_trim(words);

            for( const auto& w : words )
            {
                if (isValidMacroName(w) && w!="defined")
                    foundUsages[keyName][w] = orgLine;
            }
        }
    }
    
}


