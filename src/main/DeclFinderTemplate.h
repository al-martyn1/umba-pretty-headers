#pragma once


// marty::clang::helpers
namespace marty{
namespace clang{
namespace helpers
{

// marty::clang::helpers::DeclFinderTemplate
template< typename DeclVisitorType, bool handleExplicitSourcesOnly >
class DeclFinderTemplate : public ::clang::ASTConsumer
{
    DeclVisitorType Visitor;

public:

    DeclFinderTemplate(::clang::SourceManager &SM) : Visitor(SM) {}

    void HandleTranslationUnit(::clang::ASTContext &Context) final
    {
        if (handleExplicitSourcesOnly)
        {
            // Handle the Glitch

            auto &SourceManager = Visitor.getSourceManager();
           
            auto Decls = Context.getTranslationUnitDecl()->decls();
            for (auto &Decl : Decls)
            {
                const auto& FileID = SourceManager.getFileID(Decl->getLocation());
                if (FileID != SourceManager.getMainFileID())
                    continue;
                Visitor.TraverseDecl(Decl);
            }

        }
        else
        {
            // No glitch handling
            Visitor.TraverseDecl(Context.getTranslationUnitDecl());
        }

    }

}; // class DeclFinder



} // namespace helpers
} // namespace clang
} // namespace marty


