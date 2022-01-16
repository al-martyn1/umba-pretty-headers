#pragma once

#include "umba/regex_helpers.h"


template< typename StringType > inline
StringType expandSimpleMaskToEcmaRegex( StringType s )
{
    return umba::regex_helpers::expandSimpleMaskToEcmaRegex( s, true /* useAnchoring */, true /* allowRawRegexes */ );
}