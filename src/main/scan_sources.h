#pragma once


#include "DeclFindingActionTemplate.h"
#include "DeclFinderTemplate.h"


std::map< clang::Decl::Kind, unsigned > declUsageMap         = marty::clang::helpers::makeClangDeclKindUsageMap();
std::map< clang::Decl::Kind, unsigned > declUsageMapUnknowns;


class DeclVisitor : public clang::RecursiveASTVisitor<DeclVisitor>
{

      clang::SourceManager &SourceManager;

public:


      DeclVisitor(clang::SourceManager &SourceManager)
      : SourceManager(SourceManager)
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

          // stream << "Found " << NamedDecl->getQualifiedNameAsString() 
          //        << " at "
          //        //<< getDeclLocation(NamedDecl->getLocStart()) << "\n";
          //        << getDeclLocation(NamedDecl->getBeginLoc()) << "\n" // https://clang.llvm.org/doxygen/classclang_1_1Decl.html
          //        << "Kind                      : " << kindStr << "\n"
          //        << "IdentifierNamespace       : " << NamedDecl->getIdentifierNamespace() << "\n"
          //        << "IdentifierNamespaceForKind: " << NamedDecl->getIdentifierNamespaceForKind(NamedDecl->getKind()) << "\n"
          //        << "\n"
          //        //<< "\n"
          //        ;
          //  
          // for(int i=0; i<extraLfCount; ++i)
          //    stream<<"\n";

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

          #define HANDLE_DECL_IMPL(k) \
                      case k : declUsageMap[k]++; printDecl(stream, NamedDecl, 1, marty::clang::helpers::getClangDeclKindName(k) /* #k */  ); break

          #define HANDLE_DECL_EX(k) \
                      HANDLE_DECL_IMPL(k)

          #define HANDLE_DECL(k) \
                      HANDLE_DECL_IMPL(clang::Decl::Kind::k)

          // https://clang.llvm.org/doxygen/DeclBase_8cpp_source.html

          switch(kind)
          {
              HANDLE_DECL(Function);
              HANDLE_DECL(CXXDeductionGuide);
              HANDLE_DECL(CXXMethod);
              HANDLE_DECL(CXXConstructor);
              HANDLE_DECL(ConstructorUsingShadow);
              HANDLE_DECL(CXXDestructor);
              HANDLE_DECL(CXXConversion);
              HANDLE_DECL(EnumConstant);
              HANDLE_DECL(Var);
              HANDLE_DECL(ImplicitParam);
              HANDLE_DECL(ParmVar);
              HANDLE_DECL(ObjCMethod);
              HANDLE_DECL(ObjCProperty);
              HANDLE_DECL(MSProperty);
              HANDLE_DECL(Label);
              HANDLE_DECL(IndirectField);
              HANDLE_DECL(Binding);
              HANDLE_DECL(NonTypeTemplateParm);
              HANDLE_DECL(VarTemplate);
              HANDLE_DECL(Concept);
              HANDLE_DECL(ObjCCompatibleAlias);
              HANDLE_DECL(ObjCInterface);
              HANDLE_DECL(Typedef);
              HANDLE_DECL(TypeAlias);
              HANDLE_DECL(TemplateTypeParm);
              HANDLE_DECL(ObjCTypeParam);
              HANDLE_DECL(UnresolvedUsingTypename);
              HANDLE_DECL(UsingShadow);
              HANDLE_DECL(UnresolvedUsingValue);
              HANDLE_DECL(Using);
              HANDLE_DECL(UsingPack);
              HANDLE_DECL(UsingEnum);
              HANDLE_DECL(ObjCProtocol);
              HANDLE_DECL(Field);
              HANDLE_DECL(ObjCAtDefsField);
              HANDLE_DECL(ObjCIvar);
              HANDLE_DECL(Record);
              HANDLE_DECL(CXXRecord);
              HANDLE_DECL(Enum);
              HANDLE_DECL(Namespace);
              HANDLE_DECL(NamespaceAlias);
              HANDLE_DECL(FunctionTemplate);
              HANDLE_DECL(ClassTemplate);
              HANDLE_DECL(TemplateTemplateParm);
              HANDLE_DECL(TypeAliasTemplate);
              HANDLE_DECL(UnresolvedUsingIfExists);
              HANDLE_DECL(OMPDeclareReduction);
              HANDLE_DECL(OMPDeclareMapper);
              HANDLE_DECL(Friend);
              HANDLE_DECL(FriendTemplate);
              HANDLE_DECL(AccessSpec);
              HANDLE_DECL(LinkageSpec);
              HANDLE_DECL(Export);
              HANDLE_DECL(FileScopeAsm);
              HANDLE_DECL(StaticAssert);
              HANDLE_DECL(ObjCPropertyImpl);
              HANDLE_DECL(PragmaComment);
              HANDLE_DECL(PragmaDetectMismatch);
              HANDLE_DECL(Block);
              HANDLE_DECL(Captured);
              HANDLE_DECL(TranslationUnit);
              HANDLE_DECL(ExternCContext);
              HANDLE_DECL(Decomposition);
              HANDLE_DECL(MSGuid);
              HANDLE_DECL(TemplateParamObject);
              HANDLE_DECL(UsingDirective);
              HANDLE_DECL(BuiltinTemplate);
              HANDLE_DECL(ClassTemplateSpecialization);
              HANDLE_DECL(ClassTemplatePartialSpecialization);
              HANDLE_DECL(ClassScopeFunctionSpecialization);
              HANDLE_DECL(VarTemplateSpecialization);
              HANDLE_DECL(VarTemplatePartialSpecialization);
              HANDLE_DECL(ObjCImplementation);
              HANDLE_DECL(ObjCCategory);
              HANDLE_DECL(ObjCCategoryImpl);
              HANDLE_DECL(Import);
              HANDLE_DECL(OMPThreadPrivate);
              HANDLE_DECL(OMPAllocate);
              HANDLE_DECL(OMPRequires);
              HANDLE_DECL(OMPCapturedExpr);
              HANDLE_DECL(Empty);
              HANDLE_DECL(LifetimeExtendedTemporary);
              HANDLE_DECL(RequiresExprBody);

              // DECL_RANGE(ObjCImpl, ObjCCategoryImpl, ObjCImplementation)
              //HANDLE_DECL();

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
