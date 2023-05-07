/*! \file
    \brief Проба CLang Tooling'а, получаем AST
*/

#include "umba/umba.h"
#include "umba/debug_helpers.h"

#include <iostream>
#include <iomanip>
#include <string>

#include "../common/clang.h"

#include "../common/marty_clang_helpers.h"
#include "../common/utils.h"





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



// ..\src\main\test01.cpp test01.cpp.txt

int main(int argc, char* argv[])
{
    // llvm::errs() << "Argc: " << argc << "\n";

    std::string inputFilename ;
    std::string outputFilename;



    #if defined(WIN32) || defined(_WIN32)

        if (IsDebuggerPresent())
        {
            inputFilename  = "..\\src\\main\\test01.cpp";
            outputFilename = "test01.cpp.txt";
        }
        else
        {
            if (argc < argsMin)
            {
                llvm::errs() << "Usage: " << argv[appFilenameArgIdx] << " input.cpp output" << "\n";
                return -1;
            }
           
            inputFilename = argv[inputFilenameArgIdx];
        }

    #else

        if (argc < argsMin)
        {
            llvm::errs() << "Usage: " << argv[appFilenameArgIdx] << " input.cpp output" << "\n";
            return -1;
        }

        inputFilename = argv[inputFilenameArgIdx];

    #endif



    std::ifstream inputStream(inputFilename);
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
    for (int argN = optionsStartArgIdx; argN < argc; ++argN)
    {
        args.push_back(argv[argN]);
    }

    //if (inputActionArgIdx!=0)
    //{
        auto ast = clang::tooling::buildASTFromCode( cxxCode, inputFilename );

        //auto Action = new GenerateKernelsFrontendAction(argv[inputActionArgIdx]);
        //tooling::runToolOnCodeWithArgs(Action, CXXCode, Args, argv[1]);
        //delete Action;
    //}
    
    inputStream.close();
    
    return 0;
}

