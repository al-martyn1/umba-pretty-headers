#pragma once


//----------------------------------------------------------------------------
// marty::clang::helpers
namespace marty{
namespace clang{
namespace helpers
{
//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
std::set<std::string> explicitProcessedHeaders;



//----------------------------------------------------------------------------
// marty::clang::helpers::DeclFinderMode
enum class DeclFinderMode
{
    handleAll,
    handleExplicitTakenOnly,
    handleExplicitHeadersOnly,
    printSourceFilename

}; // enum class DeclFinderMode

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
// marty::clang::helpers::DeclFinderTemplate
template< typename DeclVisitorType, DeclFinderMode handleMode >
class DeclFinderTemplate : public ::clang::ASTConsumer
{
    DeclVisitorType Visitor;

public:

    DeclFinderTemplate(::clang::SourceManager &SM) : Visitor(SM) {}

    void HandleTranslationUnit(::clang::ASTContext &Context) final
    {
        using namespace umba::omanip;

        auto &SourceManager = Visitor.getSourceManager();
        
        auto Decls = Context.getTranslationUnitDecl();


        if (handleMode==DeclFinderMode::handleAll)
        {
            // No glitch handling

            Visitor.TraverseDecl(Decls);
        }

        else if (handleMode==DeclFinderMode::handleExplicitTakenOnly)
        {
            // Handle the Glitch

            auto decls = Decls->decls();

            for (auto &Decl : decls)
            {
                const auto& FileID = SourceManager.getFileID(Decl->getLocation());
                if (FileID != SourceManager.getMainFileID())
                    continue;
                Visitor.TraverseDecl(Decl);
            }

        }

        else if (handleMode==DeclFinderMode::handleExplicitHeadersOnly)
        {
            auto decls = Decls->decls();
            ::clang::DeclContext::decl_iterator it = decls.begin();
            for(; it!=decls.end(); ++it)
            {
                ::clang::Decl * pd = *it;

                auto loc = Context.getFullLoc( pd->getBeginLoc() );

                llvm::StringRef fileName = (loc.isValid() && loc.getFileEntry() ? loc.getFileEntry()->getName() : "<INVALID_LOCATION>");

                std::string strFileName = fileName.str();

                // if () continue;

                //Visitor.TraverseDecl(Decl);
                Visitor.TraverseDecl(pd);
            }
        }

        else if (handleMode==DeclFinderMode::printSourceFilename)
        {

            // ::clang::FullSourceLoc loc = getFullSourceLoc( Context *pContext, Object *pObject )

            auto decls = Decls->decls();

            //for (auto &Decl : decls)
            ::clang::DeclContext::decl_iterator it = decls.begin();
            for(; it!=decls.end(); ++it)
            {
                // auto d = *it;

                ::clang::Decl * pd = *it;

                // ::clang::FullSourceLoc loc = marty::clang::helpers::getFullSourceLoc( Context, pd );

                auto loc = Context.getFullLoc( pd->getBeginLoc() );

                //auto 
                llvm::StringRef fileName = (loc.isValid() && loc.getFileEntry() ? loc.getFileEntry()->getName() : "<INVALID_LOCATION>");

                std::string strFileName = fileName.str();

                //logMsg << strFileName << endl;
                std::cout << strFileName << std::endl;

                // https://clang.llvm.org/doxygen/classclang_1_1SourceManager.html#aecbed88c199b46a1cfcf2a2da61a7f52
                // StringRef getFilename(SourceLocation SpellingLoc) 

                // ::clang::SourceLocation loc = Decl.getLocation();

                // const auto& FileID = SourceManager.getFileID(Decl->getLocation());
                // const auto& fileManager = SourceManager.getFileManager();
                //  
                // if (FileID != SourceManager.getMainFileID())
                //     continue;
                // Visitor.TraverseDecl(Decl);
            }

        }

    }

}; // class DeclFinder



} // namespace helpers
} // namespace clang
} // namespace marty


