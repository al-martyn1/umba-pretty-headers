/*! \file
    \brief Проверка сканера макросов (по сишным сорцам)
*/

#include "umba/umba.h"
#include "umba/simple_formatter.h"
#include "umba/char_writers.h"

#include <iostream>
#include <iomanip>
#include <string>

#include "umba/macros.h"
#include "umba/macro_helpers.h"
#include "umba/enum_helpers.h"
#include "umba/flag_helpers.h"
#include "scan_for_pp.h"
#include "utils.h"

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
#include "utils.h"



int main(int argc, char* argv[])
{

    //std::string scanFilename = "F:\\_github\\umba_pretty_headers\\_libs\\umba\\zz_detect_environment.h";
    std::string scanFilename = "E:\\_github\\umba_pretty_headers\\_libs\\umba\\zz_detect_environment.h";
    
    std::map<std::string, std::map<std::string,std::string> >   foundDefinitions;
    std::map<std::string, std::map<std::string,std::string> >   foundUsages;

    scanForPp( scanFilename, foundDefinitions, foundUsages );

    //------------------------------
    printInfoLogSectionHeader( logMsg, "Found Defined Macros" );

    std::map<std::string, std::map<std::string,std::string> >::const_iterator 
    it = foundDefinitions.begin();
    
    for(; it!=foundDefinitions.end(); ++it)
    {
        logMsg << it->first << "\n";

        const std::map<std::string,std::string> &defs = it->second;

        for( const auto &[d,l] : defs )
        {
            logMsg << "    " << d << " - [" << l << "]\n";
        }
    }

    //------------------------------
    printInfoLogSectionHeader( logMsg, "Found Macro Usage" );

    it = foundUsages.begin();
    
    for(; it!=foundUsages.end(); ++it)
    {
        logMsg << it->first << "\n";

        const std::map<std::string,std::string> &defs = it->second;

        for( const auto &[d,l] : defs )
        {
            logMsg << "    " << d << " - [" << l << "]\n";
        }
    }

    //------------------------------

    return 0;
}

