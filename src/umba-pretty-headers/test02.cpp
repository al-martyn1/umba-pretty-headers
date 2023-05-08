/*! \file
    \brief Проба CLang Tooling'а, потрогал RecursiveASTVisitor
*/

#include "umba/umba.h"
#include "umba/debug_helpers.h"

#include <iostream>
#include <iomanip>
#include <string>

#include "../common/clang.h"

#include "../common/marty_clang_helpers.h"


#define TEST02_SHOW_ONLY_EXPLICIT_SOURCE_DECLS

// Clang Tutorial: Finding Declarations
// https://xinhuang.github.io/posts/2014-10-19-clang-tutorial-finding-declarations.html

// https://clang.llvm.org/docs/RAVFrontendAction.html


class DeclVisitor : public clang::RecursiveASTVisitor<DeclVisitor>
{

      clang::SourceManager &SourceManager;

public:

      std::map< clang::Decl::Kind, unsigned > declUsageMap;
      std::map< clang::Decl::Kind, unsigned > declUsageMapUnknowns;

      DeclVisitor(clang::SourceManager &SourceManager)
      : SourceManager(SourceManager)
      , declUsageMap(marty::clang::helpers::makeClangDeclKindUsageMap())
      {}

      template< typename StreamType >
      StreamType& printDecl(StreamType &stream, clang::NamedDecl *NamedDecl, int extraLfCount = 1, std::string kindStr = "") const
      {
          // Habitat   - естественная среда
          // Hobbiton  - https://en.wikipedia.org/wiki/Hobbiton_Movie_Set
          //             https://ru.wikipedia.org/wiki/%D0%A5%D0%BE%D0%B1%D0%B1%D0%B8%D1%82%D0%BE%D0%BD_(%D0%B4%D0%BE%D1%81%D1%82%D0%BE%D0%BF%D1%80%D0%B8%D0%BC%D0%B5%D1%87%D0%B0%D1%82%D0%B5%D0%BB%D1%8C%D0%BD%D0%BE%D1%81%D1%82%D1%8C)

          // if (kindStr.isEmpty()) // Fucking Qt hobbiton/habitat
          if (kindStr.empty())
              kindStr = NamedDecl->getDeclKindName();

          stream << "Found " << NamedDecl->getQualifiedNameAsString() 
                 << " at "
                 //<< getDeclLocation(NamedDecl->getLocStart()) << "\n";
                 << getDeclLocation(NamedDecl->getBeginLoc()) << "\n" // https://clang.llvm.org/doxygen/classclang_1_1Decl.html
                 << "Kind                      : " << kindStr << "\n"
                 << "IdentifierNamespace       : " << NamedDecl->getIdentifierNamespace() << "\n"
                 << "IdentifierNamespaceForKind: " << NamedDecl->getIdentifierNamespaceForKind(NamedDecl->getKind()) << "\n"
                 << "\n"
                 //<< "\n"
                 ;

          for(int i=0; i<extraLfCount; ++i)
             stream<<"\n";

          return stream;
      }


      bool VisitNamedDecl(clang::NamedDecl *NamedDecl)
      {
          // !!! Интересно
          // IdentifierInfo* NamedDecl::getIdentifier () const
          // clang::IdentifierInfo Class Reference
          // https://clang.llvm.org/doxygen/classclang_1_1IdentifierInfo.html#a75a4d06f5f7cfc78f08d9580a29da877

          // Decl - https://clang.llvm.org/doxygen/classclang_1_1Decl.html

          // Kind getKind() const
          // clang/AST/DeclNodes.inc
          // const char * getDeclKindName() const
          // bool isInAnonymousNamespace() const
          // bool isInStdNamespace() const

          auto &stream = llvm::outs();

          auto kind        = NamedDecl->getKind();
          auto idns        = NamedDecl->getIdentifierNamespace();
          auto idnsForKind = NamedDecl->getIdentifierNamespaceForKind(kind);

          auto hz = clang::Decl::Kind::Namespace;
          // clang::Decl::Kind::NamespaceAlias;
          // clang::Decl::Kind::ObjCCompatibleAlias;
          // clang::Decl::Kind::;
          // clang::Decl::Kind::;
          // clang::Decl::Kind::;

          #define TEST02_SWITCH_KIND_CASE_IMPL(k) \
                      case k : declUsageMap[k]++; printDecl(stream, NamedDecl, 1, marty::clang::helpers::getClangDeclKindName(k) /* #k */  ); break

          #define TEST02_SWITCH_KIND_CASE_EX(k) \
                      TEST02_SWITCH_KIND_CASE_IMPL(k)

          #define TEST02_SWITCH_KIND_CASE(k) \
                      TEST02_SWITCH_KIND_CASE_IMPL(clang::Decl::Kind::k)

          // https://clang.llvm.org/doxygen/DeclBase_8cpp_source.html

          switch(kind)
          {
              TEST02_SWITCH_KIND_CASE(Function);
              TEST02_SWITCH_KIND_CASE(CXXDeductionGuide);
              TEST02_SWITCH_KIND_CASE(CXXMethod);
              TEST02_SWITCH_KIND_CASE(CXXConstructor);
              TEST02_SWITCH_KIND_CASE(ConstructorUsingShadow);
              TEST02_SWITCH_KIND_CASE(CXXDestructor);
              TEST02_SWITCH_KIND_CASE(CXXConversion);
              TEST02_SWITCH_KIND_CASE(EnumConstant);
              TEST02_SWITCH_KIND_CASE(Var);
              TEST02_SWITCH_KIND_CASE(ImplicitParam);
              TEST02_SWITCH_KIND_CASE(ParmVar);
              TEST02_SWITCH_KIND_CASE(ObjCMethod);
              TEST02_SWITCH_KIND_CASE(ObjCProperty);
              TEST02_SWITCH_KIND_CASE(MSProperty);
              TEST02_SWITCH_KIND_CASE(Label);
              TEST02_SWITCH_KIND_CASE(IndirectField);
              TEST02_SWITCH_KIND_CASE(Binding);
              TEST02_SWITCH_KIND_CASE(NonTypeTemplateParm);
              TEST02_SWITCH_KIND_CASE(VarTemplate);
              TEST02_SWITCH_KIND_CASE(Concept);
              TEST02_SWITCH_KIND_CASE(ObjCCompatibleAlias);
              TEST02_SWITCH_KIND_CASE(ObjCInterface);
              TEST02_SWITCH_KIND_CASE(Typedef);
              TEST02_SWITCH_KIND_CASE(TypeAlias);
              TEST02_SWITCH_KIND_CASE(TemplateTypeParm);
              TEST02_SWITCH_KIND_CASE(ObjCTypeParam);
              TEST02_SWITCH_KIND_CASE(UnresolvedUsingTypename);
              TEST02_SWITCH_KIND_CASE(UsingShadow);
              TEST02_SWITCH_KIND_CASE(UnresolvedUsingValue);
              TEST02_SWITCH_KIND_CASE(Using);
              TEST02_SWITCH_KIND_CASE(UsingPack);
              TEST02_SWITCH_KIND_CASE(UsingEnum);
              TEST02_SWITCH_KIND_CASE(ObjCProtocol);
              TEST02_SWITCH_KIND_CASE(Field);
              TEST02_SWITCH_KIND_CASE(ObjCAtDefsField);
              TEST02_SWITCH_KIND_CASE(ObjCIvar);
              TEST02_SWITCH_KIND_CASE(Record);
              TEST02_SWITCH_KIND_CASE(CXXRecord);
              TEST02_SWITCH_KIND_CASE(Enum);
              TEST02_SWITCH_KIND_CASE(Namespace);
              TEST02_SWITCH_KIND_CASE(NamespaceAlias);
              TEST02_SWITCH_KIND_CASE(FunctionTemplate);
              TEST02_SWITCH_KIND_CASE(ClassTemplate);
              TEST02_SWITCH_KIND_CASE(TemplateTemplateParm);
              TEST02_SWITCH_KIND_CASE(TypeAliasTemplate);
              TEST02_SWITCH_KIND_CASE(UnresolvedUsingIfExists);
              TEST02_SWITCH_KIND_CASE(OMPDeclareReduction);
              TEST02_SWITCH_KIND_CASE(OMPDeclareMapper);
              TEST02_SWITCH_KIND_CASE(Friend);
              TEST02_SWITCH_KIND_CASE(FriendTemplate);
              TEST02_SWITCH_KIND_CASE(AccessSpec);
              TEST02_SWITCH_KIND_CASE(LinkageSpec);
              TEST02_SWITCH_KIND_CASE(Export);
              TEST02_SWITCH_KIND_CASE(FileScopeAsm);
              TEST02_SWITCH_KIND_CASE(StaticAssert);
              TEST02_SWITCH_KIND_CASE(ObjCPropertyImpl);
              TEST02_SWITCH_KIND_CASE(PragmaComment);
              TEST02_SWITCH_KIND_CASE(PragmaDetectMismatch);
              TEST02_SWITCH_KIND_CASE(Block);
              TEST02_SWITCH_KIND_CASE(Captured);
              TEST02_SWITCH_KIND_CASE(TranslationUnit);
              TEST02_SWITCH_KIND_CASE(ExternCContext);
              TEST02_SWITCH_KIND_CASE(Decomposition);
              TEST02_SWITCH_KIND_CASE(MSGuid);
              TEST02_SWITCH_KIND_CASE(TemplateParamObject);
              TEST02_SWITCH_KIND_CASE(UsingDirective);
              TEST02_SWITCH_KIND_CASE(BuiltinTemplate);
              TEST02_SWITCH_KIND_CASE(ClassTemplateSpecialization);
              TEST02_SWITCH_KIND_CASE(ClassTemplatePartialSpecialization);
              TEST02_SWITCH_KIND_CASE(ClassScopeFunctionSpecialization);
              TEST02_SWITCH_KIND_CASE(VarTemplateSpecialization);
              TEST02_SWITCH_KIND_CASE(VarTemplatePartialSpecialization);
              TEST02_SWITCH_KIND_CASE(ObjCImplementation);
              TEST02_SWITCH_KIND_CASE(ObjCCategory);
              TEST02_SWITCH_KIND_CASE(ObjCCategoryImpl);
              TEST02_SWITCH_KIND_CASE(Import);
              TEST02_SWITCH_KIND_CASE(OMPThreadPrivate);
              TEST02_SWITCH_KIND_CASE(OMPAllocate);
              TEST02_SWITCH_KIND_CASE(OMPRequires);
              TEST02_SWITCH_KIND_CASE(OMPCapturedExpr);
              TEST02_SWITCH_KIND_CASE(Empty);
              TEST02_SWITCH_KIND_CASE(LifetimeExtendedTemporary);
              TEST02_SWITCH_KIND_CASE(RequiresExprBody);

              // DECL_RANGE(ObjCImpl, ObjCCategoryImpl, ObjCImplementation)
              //TEST02_SWITCH_KIND_CASE();

              default:
                   stream << "UnsignedKind: " << (unsigned)kind << "\n";
                   declUsageMapUnknowns[kind]++;
                   printDecl(stream, NamedDecl);
          };
          

          // llvm::outs() << "Found " << NamedDecl->getQualifiedNameAsString() 
          //              << " at "
          //              //<< getDeclLocation(NamedDecl->getLocStart()) << "\n";
          //              << getDeclLocation(NamedDecl->getBeginLoc()) << "\n" // https://clang.llvm.org/doxygen/classclang_1_1Decl.html
          //              << NamedDecl->getDeclKindName()
          //              << "\n"
          //              << "\n"
          //              ;

          // printDecl(stream, NamedDecl);
          // StreamType& printDecl(StreamType &stream, clang::NamedDecl *NamedDecl, int extraLfCount = 1, std::string kindStr = "") const

          return true;
      }

      clang::SourceManager & getSourceManager() const
      {
          return SourceManager;
      }

private:

      std::string getDeclLocation(clang::SourceLocation Loc) const
      {
          std::ostringstream OSS;
          OSS << SourceManager.getFilename(Loc).str() << ":"
              << SourceManager.getSpellingLineNumber(Loc) << ":"
              << SourceManager.getSpellingColumnNumber(Loc);
          return OSS.str();
      }

}; // class DeclVisitor


class DeclFinder : public clang::ASTConsumer
{
    DeclVisitor Visitor;

public:

    DeclFinder(clang::SourceManager &SM) : Visitor(SM) {}

    void HandleTranslationUnit(clang::ASTContext &Context) final
    {
        #if defined(TEST02_SHOW_ONLY_EXPLICIT_SOURCE_DECLS)

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

        #else

            // No glitch handling
            Visitor.TraverseDecl(Context.getTranslationUnitDecl());

        #endif
    }

}; // class DeclFinder


class DeclFindingAction : public clang::ASTFrontendAction // https://clang.llvm.org/doxygen/classclang_1_1ASTFrontendAction.html
{

public:

    std::unique_ptr<clang::ASTConsumer>
    CreateASTConsumer(clang::CompilerInstance &CI, clang::StringRef InFile) final // https://clang.llvm.org/doxygen/classclang_1_1FrontendAction.html#a9277d23d8eec19dad5a9aeef721cd6a3
    {
      return std::unique_ptr<clang::ASTConsumer>(
          new DeclFinder(CI.getSourceManager()) );
    }

}; // class DeclFindingAction


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

    auto pActionFactory = clang::tooling::newFrontendActionFactory<DeclFindingAction>(); // std::unique_ptr
    auto pActionFactoryRawPtr = pActionFactory.get();

    auto res = clangTool.run( pActionFactoryRawPtr ); // pass raw ptr


    // llvm::outs();
    // marty::clang::helpers::getClangDeclKindName(k)

    return res;
}

