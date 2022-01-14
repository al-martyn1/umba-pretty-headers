#pragma once

#include <string>
#include <vector>
#include <map>

#include "umba/program_location.h"




struct AppConfig
{

    static const unsigned                ofEmptyOptionFlags      = 0x0000;
    static const unsigned                ofQuet                  = 0x0001; // 
    static const unsigned                ofKeepGenerated         = 0x0002; // 
    static const unsigned                ofVerbose               = 0x0004; // 
    static const unsigned                ofSuperVerbose          = 0x0008; // 



    std::map<std::string,std::string>    macros;

    // bool                                 keepGeneratedFiles = false;
    std::vector<std::string>             clangCompileFlagsTxtFilename; // compile_flags.txt

    std::vector<std::string>             excludeFilesList;

    std::vector<std::string>             scanPaths;


    unsigned                             optionFlags = 0; // ofNormalizeFilenames; // ofEmptyOptionFlags;


    void ofSet  ( unsigned ofFlags )       { optionFlags |=  ofFlags; }
    void ofReset( unsigned ofFlags )       { optionFlags &= ~ofFlags; }
    bool ofGet  ( unsigned ofFlags ) const { return (optionFlags&ofFlags)==ofFlags; }
    void ofSet  ( unsigned ofFlags , bool setState )
    {
        if (setState) ofSet  (ofFlags);
        else          ofReset(ofFlags);
    }

    void setOptQuet( bool q ) { ofSet(ofQuet,q);      }
    bool getOptQuet( )  const { return ofGet(ofQuet); }

    void setOptVerbose( bool q ) { ofSet(ofVerbose,q);      }
    bool getOptVerbose( )  const { return ofGet(ofVerbose); }

    void setOptSuperVerbose( bool q ) { ofSet(ofSuperVerbose,q);      }
    bool getOptSuperVerbose( )  const { return ofGet(ofSuperVerbose); }

    void setOptKeepGenerated( bool q ) { ofSet(ofKeepGenerated,q);      }
    bool getOptKeepGenerated( )  const { return ofGet(ofKeepGenerated); }

    // void setOptNormalizeFilenames( bool q ) { ofSet(ofNormalizeFilenames,q);      }
    // bool getOptNormalizeFilenames( )  const { return ofGet(ofNormalizeFilenames); }


    template<typename StreamType>
    StreamType& print( StreamType &s ) const
    {
        s << "Keep generated files: " << (getOptKeepGenerated() ? "true" : "false") << "\n";
        for(auto inputFilename: clangCompileFlagsTxtFilename)
        {
            s << "CLang options file: " << inputFilename << "\n";
        }

        s << "---\n";

        for(auto scanPath : scanPaths)
        {
            s << "Scan Path          : " << scanPath << "\n";
        }

        s << "---\n";

        for(auto excludedFile : excludeFilesList)
        {
            s << "Excluded File Mask : " << excludedFile << "\n";
        }

        return s;
    }


    AppConfig getAdjustedConfig( const umba::program_location::ProgramLocation<std::string> &programLocation ) const
    {
        AppConfig appConfig;

        appConfig.macros             = macros;
        //appConfig.keepGeneratedFiles = keepGeneratedFiles;
        appConfig.scanPaths          = scanPaths;
        appConfig.optionFlags        = optionFlags;

        for(auto inputFilename: clangCompileFlagsTxtFilename)
        {
            appConfig.clangCompileFlagsTxtFilename.push_back( programLocation.makeAbsPath(inputFilename) );
        }

        for(auto excludedFile: excludeFilesList)
        {
            appConfig.excludeFilesList.push_back( umba::filename::normalizePathSeparators(excludedFile,'/') );
        }

        return appConfig;
    }

}; // struct AppConfig


template<typename StreamType> inline
StreamType& operator<<(StreamType &s, const AppConfig &cfg)
{
    return cfg.print( s );
}



