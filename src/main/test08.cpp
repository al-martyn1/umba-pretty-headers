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


template<typename MacroGetter>
void testMacro( const std::string text, const MacroGetter &getter, int flags )
{
    std::cout << "Text   : " << text << "\n";
    std::cout << "Substed: " << substMacros(text,getter,flags) << "\n";
    std::cout << "---\n\n";

}

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




int main(int argc, char* argv[])
{
    using namespace umba::macros;

    std::cout << "Flags test\n";
    std::cout << umba::flag::util::toStringImpl(getUmbaMacrosFlagsStringMap(), argsAllowed|changeDot ) << "\n";
    std::cout << "-------\n";

    std::map<std::string,std::string>  macroTexts;

    macroTexts["TestMacro" ] = "test";
    macroTexts["OtherMacro"] = "other test macro";

    testMacro( "$(ALLUSERSPROFILE)", MacroTextFromEnv<std::string>(), substFlagsDefault );

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


    return 0;
}

