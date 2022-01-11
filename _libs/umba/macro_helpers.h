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

    const char* getName() const { return "MacroTextFromMap"; }

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

    const char* getName() const { return "MacroTextFromMapRef"; }

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

    const char* getName() const { return "MacroTextFromEnv"; }

}; // struct MacroTextFromEnv

//------------------------------
template<typename StringType>
struct MacroTextFromMapOrEnv
{
    std::map<StringType,StringType> m;
    bool                            envAllowed = true; // To allow control in runtime

    MacroTextFromMapOrEnv(const std::map<StringType,StringType> &_m, bool _envAllowed = true) : m(_m), envAllowed(_envAllowed) {}

    bool operator()(const StringType &name, StringType &text) const
    {
        if (getMacroTextFromMap(m, name, text))
            return true;

        if (!envAllowed)
            return false;

        return umba::env::getVar(name, text);
    }

    const char* getName() const { return "MacroTextFromMapOrEnv"; }

}; // struct MacroTextFromMapOrEnv

//------------------------------
template<typename StringType>
struct MacroTextFromMapOrEnvRef
{
    const std::map<StringType,StringType> &m;
    bool                            envAllowed = true; // To allow control in runtime

    MacroTextFromMapOrEnvRef(const std::map<StringType,StringType> &_m, bool _envAllowed = true) : m(_m), envAllowed(_envAllowed) {}

    bool operator()(const StringType &name, StringType &text) const
    {
        if (getMacroTextFromMap(m, name, text))
            return true;

        if (!envAllowed)
            return false;

        return umba::env::getVar(name, text);
    }

    const char* getName() const { return "MacroTextFromMapOrEnvRef"; }

}; // struct MacroTextFromMapOrEnvRef

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------

} // namespace macros
} // namespace umba

// umba::macros::



