#pragma once


// marty::clang::helpers
namespace marty{
namespace clang{
namespace helpers
{


// marty::clang::helpers::DeclFindingActionTemplate
template< typename DeclFinderType >
class DeclFindingActionTemplate : public ::clang::ASTFrontendAction // https://clang.llvm.org/doxygen/classclang_1_1ASTFrontendAction.html
{

public:

    std::unique_ptr<::clang::ASTConsumer>
    CreateASTConsumer(::clang::CompilerInstance &CI, ::clang::StringRef InFile) final // https://clang.llvm.org/doxygen/classclang_1_1FrontendAction.html#a9277d23d8eec19dad5a9aeef721cd6a3
    {
      return std::unique_ptr<::clang::ASTConsumer>(
             new DeclFinderType(CI.getSourceManager())
                                                );
    }

}; // class DeclFindingActionTemplate



} // namespace helpers
} // namespace clang
} // namespace marty


