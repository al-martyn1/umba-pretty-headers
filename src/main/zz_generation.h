    std::set<std::string> allGeneratedFiles;
    auto compileFlagFilesAutoDeleter = umba::makeLeaveScopeExec
                                       (
                                           [&]()
                                           {
                                               if (appConfig.getOptKeepGenerated()) return;

                                               for( auto f: allGeneratedFiles)
                                               {
                                                   // std::remove(f.c_str());
                                                   std::filesystem::remove(f);
                                               }
                                           }
                                       );
    

    std::vector<std::string> generatedCompileFlagsTxtFiles;
    std::map< std::string, std::vector<std::string> > generatedCompileFlagsIncPaths;

    for(auto compileFlagsTxt : appConfig.clangCompileFlagsTxtFilename)
    {
        std::map<std::string, std::vector<std::string> > cflags;
        std::vector<std::string>                         commonLines;

        if (!parseCompileFlags(compileFlagsTxt, cflags, commonLines))
        {
            return 1;
        }

        std::vector<std::string> tmpGeneratedCompileFlagsTxtFiles;
        std::map< std::string, std::vector<std::string> > tmpIncludePaths;

        generateCompileFlags(appConfig, compileFlagsTxt, cflags, commonLines, tmpGeneratedCompileFlagsTxtFiles, tmpIncludePaths);

        generatedCompileFlagsTxtFiles.insert( generatedCompileFlagsTxtFiles.end(), tmpGeneratedCompileFlagsTxtFiles.begin(), tmpGeneratedCompileFlagsTxtFiles.end() );

        generatedCompileFlagsIncPaths.insert( tmpIncludePaths.begin(), tmpIncludePaths.end() );
    }

    allGeneratedFiles.insert(generatedCompileFlagsTxtFiles.begin(), generatedCompileFlagsTxtFiles.end());


    if (!appConfig.getOptQuet())
    {
        if (!generatedCompileFlagsTxtFiles.empty())
        {
            //printInfoLogSectionHeader(logMsg, "Generated 'CompileFlags' Files");
            printInfoLogSectionHeader(logMsg, "Generated 'compile_flags.txt' Files");
        }

        for(auto & name : generatedCompileFlagsTxtFiles)
        {
            logMsg << endl;
            logMsg << name << endl;

            logMsg << "Include paths: ";

            std::map< std::string, std::vector<std::string> >::const_iterator pit = generatedCompileFlagsIncPaths.find(name);

            if (pit==generatedCompileFlagsIncPaths.end())
            {
                logMsg << warning << "Not found" << normal << endl;
            }
            else
            {
                logMsg << endl;
                const auto &paths = pit->second;
                for( const auto &p : paths)
                {
                    logMsg << "  " << p; // << endl;
                    auto cp = umba::filename::makeCanonical(p,'/');
                    logMsg << " - " << cp << endl;
                }
            }
        }

        printInfoLogSectionHeader(logMsg, "Initialization completed");
        auto tickDiff = umba::time_service::getCurTimeMs() - startTick;
        logMsg << "Time elapsed: " << tickDiff << "ms" << "\n";
        startTick = umba::time_service::getCurTimeMs();
    }

    // Make canonical include paths
    {
        std::map< std::string, std::vector<std::string> >::iterator pit = generatedCompileFlagsIncPaths.begin();
        for(; pit!=generatedCompileFlagsIncPaths.end(); ++pit)
        {
            auto &paths = pit->second;
            for( auto &p : paths)
            {
                p = umba::filename::makeCanonical(p,'/');
            }
        }
    }


