#pragma once

#include <string>
#include <vector>

#include "umba/program_location.h"



struct AppConfig
{
    bool                        keepCompileFlags = false;
    std::vector<std::string>    clangCompileFlagsTxtFilename; // compile_flags.txt

    std::vector<std::string>    excludeFilesList;


    template<typename StreamType>
    StreamType& print( StreamType &s ) const
    {
        s << "Keep CLang option files: " << (keepCompileFlags ? "true" : "false") << "\n";
        for(auto inputFilename: clangCompileFlagsTxtFilename)
        {
            s << "CLang options file: " << inputFilename << "\n";
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

        appConfig.keepCompileFlags = keepCompileFlags;

        for(auto inputFilename: clangCompileFlagsTxtFilename)
        {
            appConfig.clangCompileFlagsTxtFilename.push_back( programLocation.makeAbsPath(inputFilename) );
        }

        for(auto excludedFile: excludeFilesList)
        {
            appConfig.excludeFilesList.push_back( umba::filename::normalizePathSeparators(excludedFile) );
        }

        return appConfig;
    }

}; // struct AppConfig


template<typename StreamType> inline
StreamType& operator<<(StreamType &s, const AppConfig &cfg)
{
    return cfg.print( s );
}



