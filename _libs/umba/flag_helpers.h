#pragma once

#include "umba/umba.h"
#include "umba/preprocessor.h"
#include "stl.h"
#include "umba/string_plus.h"

#include <vector>



//----------------------------------------------------------------------------
// umba::flag::
namespace umba{
namespace flag{



//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
// umba::flag::util::
namespace util{




//----------------------------------------------------------------------------
template< typename StringType, typename FlagType > inline
FlagType getAllFlags( const std::map<FlagType,StringType> &flagsMap )
{
    FlagType res = (FlagType)0;
    for(auto [key,val] : flagsMap)
    {
        res |= key;
    }

    return res;
}

//----------------------------------------------------------------------------
template< typename StringType, typename FlagType > inline
StringType toStringImpl( const std::map<FlagType,StringType> &flagsMap, FlagType flagsVal )
{
    typedef typename std::underlying_type< FlagType >::type    IntType;


    auto intFlagsVal = (IntType)flagsVal;

    //FlagType allFlags = getAllFlags(flagsMap);
    IntType intAllFlags = (IntType)getAllFlags(flagsMap);

    // Special case - no flags set - looking up for this case
    // Or - if ORing all flags result is zero
    if (intFlagsVal==0 || intAllFlags==0)
    {
        std::map<FlagType,StringType>::const_iterator it = flagsMap.find(flagsVal);
        if (it==flagsMap.end())
            return umba::string_plus::make_string<StringType>("0");
        else
            return it->second;
    }

    std::vector<StringType> resStrVec;

    for(auto [flag,flagName] : flagsMap)
    {
        IntType iFlags = (IntType)flag;
        if ((intAllFlags&iFlags)==iFlags)
        {
            resStrVec.push_back(flagName);
            intAllFlags &= ~iFlags;
        }

        if (intAllFlags==0)
            break;
    }

    return umba::string_plus::make_string<StringType>(resStrVec, umba::string_plus::make_string<StringType>("|") );

}


} // namespace util

// typedef typename std::underlying_type< EnumType >::type    EnumUnderlyingType;


//----------------------------------------------------------------------------


} // namespace flag
} // namespace umba

