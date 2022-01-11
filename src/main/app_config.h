#pragma once

#include <string>
#include <vector>

#include "umba/program_location.h"



struct AppConfig
{
    std::map<std::string,std::string>    macros;

    bool                                 keepCompileFlags = false;
    std::vector<std::string>             clangCompileFlagsTxtFilename; // compile_flags.txt

    std::vector<std::string>             excludeFilesList;

    std::vector<std::string>             scanPaths;


    template<typename StreamType>
    StreamType& print( StreamType &s ) const
    {
        s << "Keep CLang option files: " << (keepCompileFlags ? "true" : "false") << "\n";
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

        appConfig.macros           = macros;
        appConfig.keepCompileFlags = keepCompileFlags;
        appConfig.scanPaths        = scanPaths;

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



