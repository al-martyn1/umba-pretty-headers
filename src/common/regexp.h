#pragma once

#include <regex>
#include <sstream>
#include <exception>
#include <stdexcept>

#include "rdlc-core/macros.h"



/*
    Что нужно от regexp'ов?

    1) Проверить соответствие, и сгенерировать новую строку на базе проверяемой.

    2) Проверить соответствие элементов вектора строк, и добавить новые, сформированные на базе проверяемых
*/


// ECMAScript regex pattern syntax used, http://www.cplusplus.com/reference/regex/ECMAScript/

//-----------------------------------------------------------------------------
inline
bool regexpSimpleMatch( const std::string &str, const std::string &expr )
{
    try
    {
        return std::regex_match( str, std::regex(expr) );
    }
    catch(...)
    {}

    return false;
}

inline
bool regexpSimpleMatchUnsafe( const std::string &str, const std::string &expr )
{
    return std::regex_match( str, std::regex(expr) );
}

inline
bool regexpSimpleMatchUnsafe( const std::set<std::string> &strs, const std::string &expr )
{
    std::regex r = std::regex(expr);
    for( const auto& s : strs )
    {
        if (std::regex_match(s, r))
            return true;
    }
    return false;
}



//-----------------------------------------------------------------------------
inline
void smatchFillVars( const std::smatch &m, const std::string &text, std::map< std::string, std::string> &vars )
{
    vars["{SOURCE}"] = text;
    vars["{R}"]      = text;

    vars["{MATCH}"]  = m.str();
    vars["{M}"]      = m.str();

    vars["{PREFIX}"] = m.prefix();
    vars["{P}"]      = m.prefix();

    vars["{S}"]      = m.suffix();
    vars["{SUFFIX}"] = m.prefix();


    for(size_t i = 1; i<m.size(); ++i)
	{
        std::ostringstream oss;
        oss<<"{"<<i<<"}";
        vars[oss.str()] = m.str(i);
    }
}

//-----------------------------------------------------------------------------
inline
bool regexpMatchFillVars( const std::string &rExpr, const std::string &text, std::map< std::string, std::string> &vars )
{
    std::smatch m;
    std::regex r = std::regex(rExpr);
    if ( !regex_search(text, m, r ) )
        return false;
    smatchFillVars( m, text, vars );
    return true;
}

//-----------------------------------------------------------------------------
inline
bool regexpMatchFillVars( const std::string &rExpr, const std::set<std::string> &textSet, std::map< std::string, std::string> &vars )
{
    
    std::regex r = std::regex(rExpr);

    for( const auto &text : textSet )
    {
        std::smatch m;
        if ( regex_search(text, m, r ) )
        {
            smatchFillVars( m, text, vars );
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
inline
bool regexpEvalString( std::string &res
                     , const std::string &rExpr
                     , const std::string &targetTpl
                     , const std::string &text
                     , std::map< std::string, std::string> vars
                     )
{
    std::smatch m;

    if ( !regex_search(text, m, std::regex(rExpr) ) )
        return false;

    if (m.empty())
    {
        //res.clear();
        return false;
    }

    smatchFillVars( m, text, vars );
    res = macroExpandString( targetTpl, vars, RegexMacroNameSymbolsValidator() );

    return true;
}

//-----------------------------------------------------------------------------
template< typename TplIterType, typename InsertIterType >
inline
bool regexpEvalStrings( InsertIterType inserter
                      , const std::string &rExpr
                      , TplIterType b, TplIterType e
                      , const std::string &text
                      , std::map< std::string, std::string> vars
                      )
{
    std::smatch m;

    if ( !regex_search(text, m, std::regex(rExpr) ) )
        return false;

    if (m.empty())
    {
        //res.clear();
        return false;
    }

    smatchFillVars( m, text, vars );

    for( ; b!=e; ++b )
        *inserter = macroExpandString( *b, vars, RegexMacroNameSymbolsValidator() );

    return true;
}

//-----------------------------------------------------------------------------
inline
bool regexCheck( const std::string &expr, std::string &err )
{
    try
    {
        std::regex_match( "***", std::regex(expr) );
        return true;
    }
    catch(const std::exception &e)
    {
        err = e.what();
    }
    catch(...)
    {
        err = "Unknown error";
    }

    return false;
}

//-----------------------------------------------------------------------------
inline
std::string regexpAddAssertionBOL( const std::string &s )
{
    if (s.empty())
        return s;

    if (s.front()=='^')
        return s;

    return std::string("^") + s;
}

//-----------------------------------------------------------------------------
inline
std::string regexpAddAssertionEOL( const std::string &s )
{
    if (s.empty())
        return s;

    if (s.back()=='$')
        return s;

    return s + std::string("$");
}

//-----------------------------------------------------------------------------




