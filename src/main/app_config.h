#pragma once

#include <string>
#include <vector>
#include <map>

#include "umba/program_location.h"
#include "umba/enum_helpers.h"
#include "umba/flag_helpers.h"

#include "regex_helpers.h"
#include "marty_clang_helpers.h"

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
enum class VerbosityLevel
{
     invalid      = -1,
     begin        = 0,

     quet         = 0,   // quet 
     normal       = 1,   // normal - print common details
     config       = 2,   // print common details and app config
     detailed     = 3,   // print common details, app config and all declarations, found in user files
     extra        = 4,    // print common details, app config and all found declarations

     end          = extra

};

UMBA_ENUM_CLASS_IMPLEMENT_RELATION_OPERATORS(VerbosityLevel)

//----------------------------------------------------------------------------





//----------------------------------------------------------------------------
struct AppConfig
{

    static const unsigned                    ofEmptyOptionFlags      = 0x0000;
    static const unsigned                    ofKeepGenerated         = 0x0001; // 
    static const unsigned                    ofQuotedIncludes        = 0x0002; // 


    std::map<std::string,std::string>        macros;

    std::vector<std::string>                 clangCompileFlagsTxtFilename; // compile_flags.txt

    std::vector<std::string>                 excludeFilesMaskList;
    std::vector<std::string>                 excludeNamesMaskList;

    std::vector<std::string>                 scanPaths;
    std::string                              outputPath;

    unsigned                                 optionFlags = 0; // ofNormalizeFilenames; // ofEmptyOptionFlags;

    VerbosityLevel                           verbosityLevel = VerbosityLevel::normal;

    marty::clang::helpers::DeclKindOfKind    allowedKinds = marty::clang::helpers::DeclKindOfKind::none;


    typedef std::string StdString;
    UMBA_ENUM_CLASS_IMPLEMENT_STRING_CONVERTERS_MEMBER( StdString , VerbosityLevel, "quet", "normal", "config", "verbose", "extra" )


    void setVerbosityLevel(VerbosityLevel lvl) { verbosityLevel = lvl; }

    //! ��������� ������� lvl �� ������� ������������ ����������� ������� � ��� ��� ������� ������.
    /*! ������� ����������� lvl ������ ���� ������ ��� ����� ��������� � �������.
     */
    bool testVerbosity(VerbosityLevel lvl) const
    {
        return (verbosityLevel==VerbosityLevel::invalid)
             ? false
             : lvl<=verbosityLevel
             ;
    }

    std::string testVerbosityStringRes(VerbosityLevel lvl) const
    {
        return testVerbosity(lvl) ? "true" : "false";
    }

    void ofSet  ( unsigned ofFlags )       { optionFlags |=  ofFlags; }
    void ofReset( unsigned ofFlags )       { optionFlags &= ~ofFlags; }
    bool ofGet  ( unsigned ofFlags ) const { return (optionFlags&ofFlags)==ofFlags; }
    void ofSet  ( unsigned ofFlags , bool setState )
    {
        if (setState) ofSet  (ofFlags);
        else          ofReset(ofFlags);
    }

    static std::string getOptValAsString(unsigned opt)    { return opt ? "Yes" : "No"; }

    static std::string getOptNameString(unsigned ofFlag)
    {
        switch(ofFlag)
        {
            case ofKeepGenerated         : return "Keep Generated Files";
            case ofQuotedIncludes        : return "Quoted Includes";
            default                      : return "Multiple flags taken!!!";
        }
    }

    void setOptKeepGenerated( bool q ) { ofSet(ofKeepGenerated,q);      }
    bool getOptKeepGenerated( )  const { return ofGet(ofKeepGenerated); }

    void setOptQuotedIncludes( bool q ) { ofSet(ofQuotedIncludes,q);      }
    bool getOptQuotedIncludes( )  const { return ofGet(ofQuotedIncludes); }


    void setOptQuet( bool q ) { setVerbosityLevel(VerbosityLevel::quet);  }
    //bool getOptQuet( )  const { return testVerbosity(VerbosityLevel::quet); }

    bool getOptShowConfig( )  const { return testVerbosity(VerbosityLevel::config); }

    void addDeclKind( marty::clang::helpers::DeclKindOfKind k )
    {
        allowedKinds |= k;
    }

    bool isDeclKindAllowed( marty::clang::helpers::DeclKindOfKind k ) const
    {
        return (allowedKinds & k) != 0;
    }

    bool declKindAllowed( marty::clang::helpers::DeclKindOfKind k ) const { return isDeclKindAllowed(k); }

    std::string getQuotedName(const std::string &n) const
    {
        std::string res;
        res.reserve(n.size()+2);
        res.append(1,getOptQuotedIncludes()?'\"':'<');
        res.append(n);
        res.append(1,getOptQuotedIncludes()?'\"':'>');
        return res;
    }
     


    template<typename StreamType>
    StreamType& printVerbosity( StreamType &s ) const
    {
        s << "Verbosity      : " << VerbosityLevel_toStdString(verbosityLevel) << "\n";
        return s;
    }

    template<typename StreamType>
    StreamType& printVerbosityTest( StreamType &s, VerbosityLevel lvl ) const
    {
        s << VerbosityLevel_toStdString(lvl) << ": " << testVerbosityStringRes(lvl) << "\n";
        return s;
    }

    template<typename StreamType>
    StreamType& printVerbosityTests( StreamType &s ) const
    {
        printVerbosityTest( s, VerbosityLevel::invalid   );
        printVerbosityTest( s, VerbosityLevel::quet      );
        printVerbosityTest( s, VerbosityLevel::normal    );
        printVerbosityTest( s, VerbosityLevel::config    );
        printVerbosityTest( s, VerbosityLevel::detailed  );
        printVerbosityTest( s, VerbosityLevel::extra     );
        return s;
    }

    template<typename StreamType>
    StreamType& print( StreamType &s ) const
    {
        s << "\n";
        printVerbosity(s) << "\n";

        //------------------------------

        s << "Output Path    : " << outputPath << "\n"; // endl;

        s << "\n";

        s << "Output Kinds   : " << marty::clang::helpers::DeclKindOfKind_toStdString(allowedKinds) << "\n"; // endl;

        s << "\n";

        s << "Option Flags   :\n";
        s << "    " << getOptNameString(ofKeepGenerated)  << ": " << getOptValAsString(optionFlags&ofKeepGenerated) << "\n";
        s << "    " << getOptNameString(ofQuotedIncludes) << ": " << getOptValAsString(optionFlags&ofQuotedIncludes) << "\n";

        s << "\n";

        //------------------------------

        s << "CLang options files:\n";
        for(auto inputFilename: clangCompileFlagsTxtFilename)
        {
            s << "    " << inputFilename << "\n";
        }

        //------------------------------

        s << "\n";
        s << "Scan Paths:\n";
        for(auto scanPath : scanPaths)
        {
            s << "    " << scanPath << " (" << umba::filename::makeCanonical(scanPath) << ")\n";
        }

        s << "\n";

        //------------------------------

        if (macros.empty())
            s << "Macros : <EMPTY>";
        else
        {
            s << "Macros:\n";
            for(auto [key,val] : macros)
            {
                s << "    '" << key << "' : '" << val << "'\n";
            }
        }

        s << "\n";
        
        //------------------------------

        s << "Exclude File Masks:\n";
        for(auto excludeFileMask : excludeFilesMaskList)
	    {
            auto regexStr = expandSimpleMaskToEcmaRegex(excludeFileMask);
            s << "    '" << excludeFileMask;

            bool isRaw = false;
            if (umba::string_plus::starts_with<std::string>(excludeFileMask,umba::regex_helpers::getRawEcmaRegexPrefix<std::string>()))
                isRaw = true;

            if (regexStr==excludeFileMask || isRaw)
                s << "'\n";
            else
            {
                s << "', corresponding mECMA regexp: '"
                  << regexStr
                  << "'\n";
            }
        }

        s << "\n";
        
        //------------------------------

        s << "Exclude File Masks:\n";
        for(auto excludeNameMask : excludeNamesMaskList)
	    {
            auto regexStr = expandSimpleMaskToEcmaRegex(excludeNameMask);
            s << "    '" << excludeNameMask;

            bool isRaw = false;
            if (umba::string_plus::starts_with<std::string>(excludeNameMask,umba::regex_helpers::getRawEcmaRegexPrefix<std::string>()))
                isRaw = true;

            if (regexStr==excludeNameMask || isRaw)
                s << "'\n";
            else
            {
                s << "', corresponding mECMA regexp: '"
                  << regexStr
                  << "'\n";
            }
        }

        s << "\n";
        
        //------------------------------

        

        return s;
    }


    AppConfig getAdjustedConfig( const umba::program_location::ProgramLocation<std::string> &programLocation ) const
    {
        AppConfig appConfig;

        appConfig.macros             = macros;
        //appConfig.keepGeneratedFiles = keepGeneratedFiles;
        appConfig.scanPaths          = scanPaths;
        appConfig.outputPath         = outputPath;
        appConfig.optionFlags        = optionFlags;
        appConfig.verbosityLevel     = verbosityLevel;

        appConfig.excludeNamesMaskList = excludeNamesMaskList;


        appConfig.allowedKinds       = allowedKinds;
        if (appConfig.allowedKinds==marty::clang::helpers::DeclKindOfKind::none)
            appConfig.allowedKinds = marty::clang::helpers::DeclKindOfKind::all;


        for(auto inputFilename: clangCompileFlagsTxtFilename)
        {
            appConfig.clangCompileFlagsTxtFilename.push_back( programLocation.makeAbsPath(inputFilename) );
        }

        for(auto excludeFileMask: excludeFilesMaskList)
        {
            if (umba::string_plus::starts_with(excludeFileMask,umba::regex_helpers::getRawEcmaRegexPrefix<std::string>()))
                appConfig.excludeFilesMaskList.push_back(excludeFileMask); // keep regex as is
            else
                appConfig.excludeFilesMaskList.push_back( umba::filename::normalizePathSeparators(excludeFileMask,'/') );
        }

        return appConfig;
    }

}; // struct AppConfig


template<typename StreamType> inline
StreamType& operator<<(StreamType &s, const AppConfig &cfg)
{
    return cfg.print( s );
}



