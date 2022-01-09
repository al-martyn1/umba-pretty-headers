std::string appFullName   = "Umba Pretty Headers";
std::string appVersion    = "0.1";
std::string appCommitHash;  //UNDONE
std::string appBuildDate  = __DATE__;
std::string appBuildTime  = __TIME__;

const char *appHomeUrl    = "";
const char *appistrPath   = "";

#if defined(WIN32) || defined(_WIN32)
    const char *appSubPath    = "bin/umba-pretty-headers.exe";
#else
    const char *appSubPath    = "bin/umba-pretty-headers";
#endif

