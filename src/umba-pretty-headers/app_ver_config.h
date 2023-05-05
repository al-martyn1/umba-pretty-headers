std::string appFullName   = "Umba Pretty Headers";
std::string appVersion    = "1.0";
std::string appCommitHash;  //UNDONE
std::string appBuildDate  = __DATE__;
std::string appBuildTime  = __TIME__;

const char *appHomeUrl    = "https://github.com/al-martyn1/umba_pretty_headers";
const char *appistrPath   = "";

#if defined(WIN32) || defined(_WIN32)
    const char *appSubPath    = "bin/umba-pretty-headers.exe";
#else
    const char *appSubPath    = "bin/umba-pretty-headers";
#endif

