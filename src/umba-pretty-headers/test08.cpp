/*! \file
    \brief Проверка подстановки макросов (umba::macros::substMacros, вида $(Macro) ) и хелперов для регэкспов.
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
#include "umba/scanners.h"


// template<typename GetterType>
// std::string
//     testMacro( "$(ALLUSERSPROFILE)", MacroTextFromEnv<std::string>(), substFlagsDefault );
//     testMacro( "$(ALLUSERSPROFILE)", MacroTextFromMap<std::string>(macroTexts), substFlagsDefault );


inline
const std::map<int,std::string>& getUmbaMacrosFlagsStringMap()
{
    using namespace umba::macros;

    static std::map<int,std::string> flags;

    if (!flags.empty())
       return flags;

    flags[argsAllowed                   ] = "argsAllowed"                   ;
    flags[conditionAllowed              ] = "conditionAllowed"              ;
    flags[appendVarValueAllowed         ] = "appendVarValueAllowed"         ;
    flags[setVarValueSubstitutionAllowed] = "setVarValueSubstitutionAllowed";
    flags[changeDot                     ] = "changeDot"                     ;
    flags[changeSlash                   ] = "changeSlash"                   ;
    flags[uppercaseNames                ] = "uppercaseNames"                ;
    flags[lowercaseNames                ] = "lowercaseNames"                ;
    flags[disableRecursion              ] = "disableRecursion"              ;
    flags[keepUnknownVars               ] = "keepUnknownVars"               ;

    return flags;
}


template<typename MacroGetter>
void testMacro( const std::string &text, const MacroGetter &getter, int flags )
{
    std::cout << "Text   : [" << text << "], "
              << "getter: " << getter.getName() << ", "
              << "flags: " << umba::flag_helpers::util::toStringImpl(getUmbaMacrosFlagsStringMap(), flags ) //<< ""
              << "\n";
    std::cout << "Substed: [" << umba::macros::substMacros(text,getter,flags) << "]\n";
    std::cout << "---\n\n";
}


void testMask(const std::string &testRegexText, const std::string &testMaskStr, bool useAnchors)
{
    // std::string testRegexStr      = expandSimpleMaskToEcmaRegex( testMaskStr /* , useAnchors */  );
    std::string testRegexStr      = umba::regex_helpers::expandSimpleMaskToEcmaRegex(testMaskStr, true /* useAnchoring */, true /* allowRawRegexes */);
    std::string testRegexTextNorm = umba::filename::normalizePathSeparators(testRegexText,'/');

    std::cout << "Regex test\n";
    std::cout << "Mask       : " << testMaskStr       << "\n";
    std::cout << "Use Anchors: " << (useAnchors?"yes":"no") << "\n";
    std::cout << "Regex      : " << testRegexStr      << "\n";
    std::cout << "Text       : " << testRegexText     << "\n";
    std::cout << "TextNorm   : " << testRegexTextNorm << "\n";
    std::cout << "Result     : " << (umba::regex_helpers::regexMatch(testRegexTextNorm, testRegexStr) ? "true" : "false") << "\n";
    std::cout << "-------\n";
}




int main(int argc, char* argv[])
{
    using namespace umba::macros;

    std::string testMaskStr       = "*.cpp";
    std::string testRegexText     = "F:\\_github\\umba_pretty_headers\\_libs\\sfmt\\simple_formatter.cpp";

    testMask("F:\\_github\\umba_pretty_headers\\_libs\\sfmt\\simple_formatter.cpp", "*.cpp"   , false);
    testMask("F:\\_github\\umba_pretty_headers\\_libs\\sfmt\\simple_formatter.cpp", "^*.cpp^" , false);
    testMask("F:\\_github\\umba_pretty_headers\\_libs\\sfmt\\simple_formatter.cpp", "^*.cpp^" , true );
    

    std::cout << "Flags test\n";
    auto flagsNameMap = getUmbaMacrosFlagsStringMap();
    auto flagsStr = umba::flag_helpers::util::toStringImpl(flagsNameMap, argsAllowed|changeDot );
    std::cout << flagsStr << "\n";
    std::cout << "-------\n";

    std::map<std::string,std::string>  macroTexts;

    macroTexts["TestMacro" ] = "<test>, calling $(OtherMacro)";
    macroTexts["OtherMacro"] = "other test macro";
    macroTexts["MacroWithArgs"] = "Args - $(%0), $(%1) - $(%2)!";


    testMacro( "$(MacroWithArgs:$(OtherMacro):$(TestMacro))", MacroTextFromMapOrEnv<std::string>(macroTexts), keepUnknownVars|argsAllowed );

    testMacro( "$(TestMacro) - $(OtherMacro)", MacroTextFromEnv     <std::string>(), substFlagsDefault );
    testMacro( "$(TestMacro) - $(OtherMacro)", MacroTextFromMap     <std::string>(macroTexts), substFlagsDefault );
    testMacro( "$(TestMacro) - $(OtherMacro)", MacroTextFromMap     <std::string>(macroTexts), keepUnknownVars );
    testMacro( "$(TestMacro) - $(OtherMacro)", MacroTextFromMapOrEnv<std::string>(macroTexts), keepUnknownVars );

    testMacro( "$(ALLUSERSPROFILE)", MacroTextFromEnv     <std::string>(), substFlagsDefault );
    testMacro( "$(ALLUSERSPROFILE)", MacroTextFromMap     <std::string>(macroTexts), substFlagsDefault );
    testMacro( "$(ALLUSERSPROFILE)", MacroTextFromMap     <std::string>(macroTexts), keepUnknownVars );
    testMacro( "$(ALLUSERSPROFILE)", MacroTextFromMapOrEnv<std::string>(macroTexts), keepUnknownVars );

    testMacro( "$(HOMEDRIVE)$(HOMEPATH)/$(TestMacro)", MacroTextFromEnv     <std::string>(), substFlagsDefault );
    testMacro( "$(HOMEDRIVE)$(HOMEPATH)/$(TestMacro)", MacroTextFromMap     <std::string>(macroTexts), substFlagsDefault );
    testMacro( "$(HOMEDRIVE)$(HOMEPATH)/$(TestMacro)", MacroTextFromMap     <std::string>(macroTexts), keepUnknownVars );
    testMacro( "$(HOMEDRIVE)$(HOMEPATH)/$(TestMacro)", MacroTextFromMapOrEnv<std::string>(macroTexts), keepUnknownVars );


    /*
    MacroTextFromMap
    MacroTextFromMapRef
    MacroTextFromEnv
    MacroTextFromMapOrEnv
    MacroTextFromMapOrEnvRef

    argsAllowed
    conditionAllowed
    appendVarValueAllowed
    setVarValueSubstitutionAllowed
    changeDot
    changeSlash
    uppercaseNames
    lowercaseNames
    disableRecursion
    keepUnknownVars

    */



    /*

    Переменные среды, которые есть везде

    ALLUSERSPROFILE=C:\ProgramData
    CommonProgramFiles=C:\Program Files\Common Files
    CommonProgramFiles(x86)=C:\Program Files (x86)\Common Files
    CommonProgramW6432=C:\Program Files\Common Files
    COMPUTERNAME=MYCOMP-PC
    ComSpec=C:\WINDOWS\system32\cmd.exe
    HOMEDRIVE=C:
    HOMEPATH=\Users\
    JAVA_HOME=C:\Program Files\Java\jdk1.7.0_67
    JDK_HOME=C:\Program Files\Java\jdk1.7.0_67
    LOCALAPPDATA=C:\Users\Local_User_Name\AppData\Local
    LOGONSERVER=\\MYCOMP-PC
    PROCESSOR_ARCHITECTURE=AMD64
    PROCESSOR_IDENTIFIER=Intel64 Family 6 Model 146 Stepping 8, GenuineIntel
    PROCESSOR_LEVEL=6
    PROCESSOR_REVISION=9e0a
    ProgramData=C:\ProgramData
    ProgramFiles=C:\Program Files
    ProgramFiles(x86)=C:\Program Files (x86)
    ProgramW6432=C:\Program Files
    PROMPT=$P$G
    TEMP=C:\Users\Local_User_Name\AppData\Local\Temp
    USERDOMAIN=mycomp-pc
    USERDOMAIN_ROAMINGPROFILE=mycomp-pc
    USERNAME=Local_User_Name
    USERPROFILE=C:\Users\Local_User_Name
 
    */



    using namespace umba::omanip;

    umba::info_log::printSectionHeader(umbaLogStreamMsg, "Log Colors");

    umbaLogStreamMsg << emergency << "emergency " << normal << endl;
    umbaLogStreamMsg << alert     << "alert     " << normal << endl;
    umbaLogStreamMsg << critical  << "critical  " << normal << endl;
    umbaLogStreamMsg << error     << "error     " << normal << endl;
    umbaLogStreamMsg << warning   << "warning   " << normal << endl;
    umbaLogStreamMsg << notice    << "notice    " << normal << endl;
    umbaLogStreamMsg << info      << "info      " << normal << endl;
    umbaLogStreamMsg << debug     << "debug     " << normal << endl;
    umbaLogStreamMsg << good      << "good      " << normal << endl;
    umbaLogStreamMsg << caption   << "caption   " << normal << endl;
    umbaLogStreamMsg << normal    << "normal    " << normal << endl;



    return 0;
}

