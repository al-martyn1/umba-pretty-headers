
#include "clang.h"



#define HAS_ACTION_ARG



enum ArgNumbers
{
    argsMin = 2,
    appFilenameArgIdx    = 0,
    inputFilenameArgIdx  = 1,

    #if defined(HAS_ACTION_ARG)
        inputActionArgIdx     = 2,
        optionsStartArgIdx    = 3,
    #else
        inputActionArgIdx     = 0,
        optionsStartArgIdx    = 2,
    #endif

    dummy


};


int main(int argc, char* argv[])
{
    if (argc < argsMin)
    {
        llvm::errs() << "Usage: " << argv[appFilenameArgIdx] << " input.cpp output" << "\n";
        return -1;
    }

    std::ifstream inputStream(argv[inputFilenameArgIdx]);
    if (!inputStream.is_open())
    {
        llvm::errs() << "Cannot open " << argv[1] << "\n";
        return -1;
    }

    std::string cxxCode((std::istreambuf_iterator<char>(inputStream)), std::istreambuf_iterator<char>());

    //---------
    llvm::outs() << "File content:\n" << cxxCode << "\n";
    //---------

    std::vector<std::string> args;
    for (int arg = optionsStartArgIdx; arg < argc; ++arg)
    {
        args.push_back(argv[arg]);
    }

    if (inputActionArgIdx!=0)
    {
        //auto Action = new GenerateKernelsFrontendAction(argv[inputActionArgIdx]);
        //tooling::runToolOnCodeWithArgs(Action, CXXCode, Args, argv[1]);
        //delete Action;
    }
    

    inputStream.close();

    
    return 0;
}

