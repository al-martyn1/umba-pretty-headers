/*! \file
    \brief Проба CLang Tooling'а, потрогал DeclarationMatcher
*/

#include "umba/umba.h"
#include "umba/debug_helpers.h"

#include <iostream>
#include <iomanip>
#include <string>

#include "clang.h"

#include "marty_clang_helpers.h"
#include "utils.h"



// Clang Tutorial: The AST Matcher - https://xinhuang.github.io/posts/2015-02-08-clang-tutorial-the-ast-matcher.html
// Matching the Clang AST - https://clang.llvm.org/docs/LibASTMatchers.html
// AST Matcher Reference  - https://clang.llvm.org/docs/LibASTMatchersReference.html
// https://clang.llvm.org/doxygen/namespaceclang_1_1ast__matchers.html#a22b3e62e1b83cda2c7a5d9fc0f14f5cf

class MatchCallback : public ::clang::ast_matchers::MatchFinder::MatchCallback
{
public:

    // virtual ~MatchCallback () override
 
    // https://clang.llvm.org/doxygen/structclang_1_1ast__matchers_1_1MatchFinder_1_1MatchResult.html
    virtual void run(const ::clang::ast_matchers::MatchFinder::MatchResult &Result) override
    {
        // Result.Nodes.getNodeAs(StringRef ID)
        // const IDToNodeMap & Result.Nodes.getMap()

        // ::clang::ASTContext *const Context
        // ::clang::SourceManager *const SourceManager;

        auto nodesMap = Result.Nodes.getMap();
        //for( auto )

        for ( const auto& [key, val]: nodesMap )
        {
            auto keyCopy = key;
            auto valCopy = val;

        }
    }

    virtual void onStartOfTranslationUnit() override
    {
        // Called at the start of each translation unit.
    }

    virtual void onEndOfTranslationUnit() override
    {
        // Called at the end of each translation unit.
    }
 	
    // virtual StringRef getID() const override
    //virtual llvm::Optional<TraversalKind> getCheckTraversalKind() const override

};


// ..\src\main\test01.cpp test01.cpp.txt

int main(int argc, char* argv[])
{
    // llvm::errs() << "Argc: " << argc << "\n";

    std::string                cdbFilename;
    std::vector< std::string > inputFiles;

    //std::string inputFilename ;
    //std::string outputFilename;

    // https://clang.llvm.org/docs/JSONCompilationDatabase.html

    #if defined(WIN32) || defined(_WIN32)

        if (IsDebuggerPresent())
        {
            cdbFilename = "..\\tests\\data\\compile_flags.txt";
            inputFiles.push_back("..\\tests\\data\\test_data_02.cpp");
            //outputFilename = "test01.cpp.txt";
        }
        else
        {
            if (argc < 3)
            {
                llvm::errs() << "Usage: " << argv[0] << " dbname input.cpp" << "\n";
                return -1;
            }
           
            cdbFilename = argv[1];
            inputFiles.push_back(argv[2]);
        }

    #else

        if (argc < 2)
        {
            llvm::errs() << "Usage: " << argv[0] << " dbname input.cpp" << "\n";
            return -1;
        }

        cdbFilename = argv[1];
        inputFiles.push_back(argv[2]);

    #endif


    if (!inputFiles.empty())
    {
        std::vector< std::string >::const_iterator b = inputFiles.begin();
        auto sb = *b;
    }

    std::string errRecipientStr;

    // https://clang.llvm.org/doxygen/classclang_1_1tooling_1_1JSONCompilationDatabase.html
    // https://clang.llvm.org/doxygen/namespaceclang_1_1tooling.html#a4ffae085d7853c7cffc94f6df464ee08
    // clang::tooling::JSONCommandLineSyntax val;

    // Не работает
    // auto cdb = clang::tooling::JSONCompilationDatabase::loadFromFile(cdbFilename, errRecipientStr, clang::tooling::JSONCommandLineSyntax::AutoDetect );

    // See \llvm-project\clang\lib\Tooling\CompilationDatabase.cpp
    auto pcdb = clang::tooling::FixedCompilationDatabase::loadFromFile(cdbFilename, errRecipientStr);
     
    if (pcdb==0 || !errRecipientStr.empty())
    {
        marty::clang::helpers::printError( llvm::errs(), errRecipientStr );
        return -1;
    }


    clang::tooling::ClangTool clangTool(*pcdb, inputFiles);

    //  
    //  
    // auto pActionFactory = clang::tooling::newFrontendActionFactory
    //                           < marty::clang::helpers::DeclFindingActionTemplate
    //                               < marty::clang::helpers::DeclFinderTemplate
    //                                   < DeclVisitor
    //                                   , true // handleExplicitSourcesOnly
    //                                   >
    //                               > 
    //                           >(); // std::unique_ptr
    //  
    // auto pActionFactoryRawPtr = pActionFactory.get();
    //  
    // auto res = clangTool.run( pActionFactoryRawPtr ); // pass raw ptr

    ::clang::ast_matchers::DeclarationMatcher declarationMatcher(::clang::ast_matchers::anything());
    //::clang::ast_matchers::DeclarationMatcher declarationMatcher;

    MatchCallback myMatchCallback;

    clang::ast_matchers::MatchFinder finder;
    finder.addMatcher(declarationMatcher, &myMatchCallback);

    auto res = clangTool.run(::clang::tooling::newFrontendActionFactory(&finder).get());

    //auto res = 0;


    // llvm::outs();
    // marty::clang::helpers::getClangDeclKindName(k)

    return res;
}

