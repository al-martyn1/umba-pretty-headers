#pragma once

#include "umba/umba.h"
#include "umba/simple_formatter.h"

#include <string>

static const char *infoLogSectionSeparator = // "--------------------------------------"; // 38
"------------------------------------------------------------------------------"; // 78


template<typename StreamType> inline
StreamType& printInfoLogSectionHeader( StreamType &s, std::string secCaption )
{
    using namespace umba::omanip;

    secCaption += ":";

    s << "\n";
    s << infoLogSectionSeparator << "\n";
    s << warning << secCaption << normal << "\n";
    s << std::string(secCaption.size(), '-');
    s << "\n";

    return s;
}

template<typename StreamType> inline
StreamType& printInfoLogSectionHeader( StreamType &s, const char* secCaption )
{
    return printInfoLogSectionHeader( s, std::string(secCaption) );
}

