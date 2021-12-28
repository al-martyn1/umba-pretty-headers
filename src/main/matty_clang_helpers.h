#pragma once


#include "clang.h"



// marty::clang::helpers
namespace marty{
namespace clang{
namespace helpers
{


template< typename Context, typename Object > inline
::clang::FullSourceLoc getFullSourceLoc( Context *pContext, Object *pObject )
{
    return Context->getFullLoc(pObject->getBeginLoc());
}


template< typename Stream, typename StringType > inline
Stream& printError( Stream &stream, const ::clang::FullSourceLoc &sourceLoc, StringType msg )
{
    auto fileName = (sourceLoc.isValid() && sourceLoc.getFileEntry() ? sourceLoc.getFileEntry()->getName() : "<FILE>");
    auto line     = (sourceLoc.isValid() ? sourceLoc.getSpellingLineNumber()   : 0);
    auto pos      = (sourceLoc.isValid() ? sourceLoc.getSpellingColumnNumber() : 0);

    stream << fileName << ":" << line << ":" << pos << ": error: " << msg << "\n";

    return stream;
}






} // namespace helpers
} // namespace clang
} // namespace marty


