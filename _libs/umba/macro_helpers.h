#pragma once

#include <string>
#include <map>

#include "string_plus.h"
#include "env.h"


//----------------------------------------------------------------------------
// umba::macros::
namespace umba{
namespace macros{




//----------------------------------------------------------------------------
template<typename StringType>
bool getMacroTextFromMap(const std::map<StringType,StringType> &m, const StringType &name, StringType &text)
{
    std::map<StringType,StringType>::const_iterator it = m.find(name);
    if (it==m.end())
        return false;

    text = it->second;

    return true;
}

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
template<typename StringType>
struct MacroTextFromMap
{
    std::map<StringType,StringType> m;

    MacroTextFromMap(const std::map<StringType,StringType> &_m) : m(_m) {}

    bool operator()(const StringType &name, StringType &text) const
    {
        return getMacroTextFromMap(m, name, text);
    }

}; // struct MacroTextFromMap

//------------------------------
template<typename StringType>
struct MacroTextFromMapRef
{
    const std::map<StringType,StringType> &m;

    MacroTextFromMapRef(const std::map<StringType,StringType> &_m) : m(_m) {}

    bool operator()(const StringType &name, StringType &text) const
    {
        return getMacroTextFromMap(m, name, text);
    }

}; // struct MacroTextFromMapRef

//------------------------------
template<typename StringType>
struct MacroTextFromEnv
{
    MacroTextFromEnv() {}

    bool operator()(const StringType &name, StringType &text) const
    {
        return umba::env::getVar(name, text);
    }

}; // struct MacroTextFromEnv

//------------------------------
template<typename StringType>
struct MacroTextFromMapOrEnv
{
    std::map<StringType,StringType> m;

    MacroTextFromMapOrEnv(const std::map<StringType,StringType> &_m) : m(_m) {}

    bool operator()(const StringType &name, StringType &text) const
    {
        if (getMacroTextFromMap(m, name, text))
            return true;
        return umba::env::getVar(name, text);
    }

}; // struct MacroTextFromMapOrEnv

//------------------------------
template<typename StringType>
struct MacroTextFromMapOrEnvRef
{
    const std::map<StringType,StringType> &m;

    MacroTextFromMapOrEnvRef(const std::map<StringType,StringType> &_m) : m(_m) {}

    bool operator()(const StringType &name, StringType &text) const
    {
        if (getMacroTextFromMap(m, name, text))
            return true;
        return umba::env::getVar(name, text);
    }

}; // struct MacroTextFromMapOrEnvRef

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------

} // namespace macros
} // namespace umba

// umba::macros::



