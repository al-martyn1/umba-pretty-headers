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
#include "umba/info_log.h"
#include "../common/scan_for_pp.h"

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



int main(int argc, char* argv[])
{

    //std::string scanFilename = "F:\\_github\\umba_pretty_headers\\_libs\\umba\\zz_detect_environment.h";
    std::string scanFilename = "E:\\_github\\umba_pretty_headers\\_libs\\umba\\zz_detect_environment.h";
    
    std::map<std::string, std::map<std::string,std::string> >   foundDefinitions;
    std::map<std::string, std::map<std::string,std::string> >   foundUsages;

    scanForPp( scanFilename, foundDefinitions, foundUsages );

    //------------------------------
    umba::info_log::printSectionHeader( umbaLogStreamMsg, "Found Defined Macros" );

    std::map<std::string, std::map<std::string,std::string> >::const_iterator 
    it = foundDefinitions.begin();
    
    for(; it!=foundDefinitions.end(); ++it)
    {
        umbaLogStreamMsg << it->first << "\n";

        const std::map<std::string,std::string> &defs = it->second;

        for( const auto &[d,l] : defs )
        {
            umbaLogStreamMsg << "    " << d << " - [" << l << "]\n";
        }
    }

    //------------------------------
    umba::info_log::printSectionHeader( umbaLogStreamMsg, "Found Macro Usage" );

    it = foundUsages.begin();
    
    for(; it!=foundUsages.end(); ++it)
    {
        umbaLogStreamMsg << it->first << "\n";

        const std::map<std::string,std::string> &defs = it->second;

        for( const auto &[d,l] : defs )
        {
            umbaLogStreamMsg << "    " << d << " - [" << l << "]\n";
        }
    }

    //------------------------------

    return 0;
}

