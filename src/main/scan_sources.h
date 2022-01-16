#pragma once


#include "DeclFindingActionTemplate.h"
#include "DeclFinderTemplate.h"


#include "app_config.h"


struct LocationFileInfo
{
    clang::Decl::Kind    kind;
    std::string          locationFilename;
    std::string          fullFilename;

};

struct DeclarationLocationFileInfo
{
    std::set<clang::Decl::Kind>             nameKinds;
    std::string                             name;

    std::map<std::string, LocationFileInfo> locationFiles;

    void addLocationFileInfo(clang::Decl::Kind kind, std::string locationFilename, const std::string &fullFilename )
    {
        locationFilename                = umba::filename::normalizePathSeparators(locationFilename,'/');
        locationFiles[locationFilename] = LocationFileInfo{kind, locationFilename, fullFilename};
    }

};





const AppConfig           *pAppConfig = 0;
std::set<std::string>      curProcessedFiles;
std::string                currentSourceFullName;
std::string                currentSourcePath;


//std::map< clang::Decl::Kind, unsigned > declUsageMap        ; // = marty::clang::helpers::makeClangDeclKindUsageMap();
std::map< clang::Decl::Kind, unsigned > declUsageMapHandled;
std::map< clang::Decl::Kind, unsigned > declUsageMapUnhandled;
std::map< clang::Decl::Kind, unsigned > declUsageMapSkipped;
std::map< clang::Decl::Kind, unsigned > declUsageMapUnknowns;


std::map< std::string, DeclarationLocationFileInfo> foundDeclarations;






class DeclVisitor : public clang::RecursiveASTVisitor<DeclVisitor>
{

      clang::SourceManager &SourceManager;

public:


      DeclVisitor(clang::SourceManager &SourceManager)
      : SourceManager(SourceManager)
      {}

      #define UMBA_PRETTY_HEADERS_DECL_VISITOR_DECLARE_AND_GET_LOCATION(NamedDecl)                                                                \
                  ::clang::FullSourceLoc loc         = marty::clang::helpers::getFullSourceLoc( (NamedDecl)->getASTContext(), (NamedDecl) );      \
                  llvm::StringRef        fileNameRef = marty::clang::helpers::getSourceLocName(loc);                                              \
                                                                                                                                                  \
                  std::string strLocFileName = fileNameRef.str();                                                                                 \
                  std::string strFullNameCmp = strLocFileName; /* for compare */                                                                  \
                                                                                                                                                  \
                  if (!strFullNameCmp.empty())                                                                                                    \
                      strFullNameCmp = umba::filename::makeCanonicalForCompare(umba::filename::makeAbsPath(strFullNameCmp,currentSourcePath),'/')



protected:

      template< typename StreamType >
      StreamType& printDeclarationInfo(StreamType &stream, clang::NamedDecl *NamedDecl, int extraLfCount = 1, std::string kindStr = "") const
      {
          using namespace umba::filename;
          using namespace marty::clang::helpers;

          // if (!pAppConfig || !pAppConfig->testVerbosity(VerbosityLevel::detailed)) 
          //     return stream;

          // if (!pAppConfig || !pAppConfig->getOptSuperVerbose()) 
          //     return stream;
          if (kindStr.empty())
              kindStr = NamedDecl->getDeclKindName();


          UMBA_PRETTY_HEADERS_DECL_VISITOR_DECLARE_AND_GET_LOCATION(NamedDecl); // makes strLocFileName and strFullName

          stream << "Found '" << NamedDecl->getQualifiedNameAsString()  << "'"
                 << " at "
                 //<< getDeclLocation(NamedDecl->getLocStart()) << "\n";
                 << getDeclLocation(NamedDecl->getBeginLoc()) << "\n" // https://clang.llvm.org/doxygen/classclang_1_1Decl.html
                 << "In File (taken loc)       : " << strLocFileName << "\n"
                 << "In File (full name)       : " << strFullNameCmp << "\n"
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


      bool isUserFile(const std::string &strFullNameCmp) const
      {
          if (curProcessedFiles.find(strFullNameCmp)==curProcessedFiles.end())
              return false; // not an user file
          return true;
      }

      bool isUserFile(clang::NamedDecl *NamedDecl) const
      {
          UMBA_PRETTY_HEADERS_DECL_VISITOR_DECLARE_AND_GET_LOCATION(NamedDecl); // makes strLocFileName and strFullName
          return isUserFile(strFullNameCmp);
      }

      bool isPrintDeclarationInfoAllowed(const std::string &strFullNameCmp) const
      {
          using namespace umba::omanip;

          
          if (!pAppConfig)
          {
              //logMsg << "CP1: return NotAllowed" << endl;
              return false; // If pAppConfig not set no other checks allowed. Helps to simplify next conditions
          }

          //pAppConfig->printVerbosity(logMsg);
          //pAppConfig->printVerbosityTests(logMsg);

          if (!pAppConfig->testVerbosity(VerbosityLevel::detailed)) 
          {
              //logMsg << "CP2: return NotAllowed" << endl;
              return false; // No any details allowed at all - no user, no system
          }
          // Detailed verbosity, user files decls allowed


          // if (strFullNameCmp=="D:/TC/Include/stddef.h")
          // {
          //     logMsg << "Found '" << "D:/TC/Include/stddef.h" << "' (for break only)" << endl;
          // }
          // if (strFullNameCmp=="C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Tools/MSVC/14.29.30133/include/vadefs.h")
          // {
          //     logMsg << "Found '" << "D:/TC/Include/stddef.h" << "' (for break only)" << endl;
          // }

          // Here allowed user and system files - --verbosity=detaled
          if (isUserFile(strFullNameCmp))
          {
              //logMsg << "CP3: return Allowed" << endl;
              return true;
          }

          if (!pAppConfig->testVerbosity(VerbosityLevel::extra))
          {
              //logMsg << "CP4: return NotAllowed" << endl;
              return false;
          }

          //logMsg << "CP5: return Allowed" << endl;
          return true;
      }


      bool isPrintDeclarationInfoAllowed(clang::NamedDecl *NamedDecl) const
      {
          UMBA_PRETTY_HEADERS_DECL_VISITOR_DECLARE_AND_GET_LOCATION(NamedDecl); // makes strLocFileName and strFullName
          return isPrintDeclarationInfoAllowed(strFullNameCmp);
      }

      void skipDeclaration(clang::NamedDecl *NamedDecl, std::string kindStr = "") const
      {
          //auto &stream = llvm::outs();
          auto &stream = logMsg;

          // if (!pAppConfig || !pAppConfig->getOptSuperVerbose()) 
          //     return;

          UMBA_PRETTY_HEADERS_DECL_VISITOR_DECLARE_AND_GET_LOCATION(NamedDecl); // makes strLocFileName and strFullName

          if (!isPrintDeclarationInfoAllowed(strFullNameCmp))
              return;

          stream << "***SKIPPED\n";
          printDeclarationInfo(stream, NamedDecl, 1 /* extraLfCount */ , kindStr);
      }

      void unhadledDeclaration(clang::NamedDecl *NamedDecl, std::string kindStr = "") const
      {
          //auto &stream = llvm::outs();
          auto &stream = logMsg;

          // if (!pAppConfig || !pAppConfig->getOptSuperVerbose()) 
          //     return;

          // UMBA_PRETTY_HEADERS_DECL_VISITOR_DECLARE_AND_GET_LOCATION(NamedDecl); // makes strLocFileName and strFullName

          if (!isPrintDeclarationInfoAllowed(NamedDecl))
              return;

          stream << "!!!UNHANDLED\n";
          printDeclarationInfo(stream, NamedDecl, 1 /* extraLfCount */ , kindStr);
      }

      void processDeclaration(clang::NamedDecl *NamedDecl, std::string kindStr = "") const
      {
          //auto &stream = llvm::outs();
          auto &stream = logMsg;

          UMBA_PRETTY_HEADERS_DECL_VISITOR_DECLARE_AND_GET_LOCATION(NamedDecl); // makes strLocFileName and strFullName

          //if (curProcessedFiles.find(strFullNameCmp)==curProcessedFiles.end())
          if (!isUserFile(strFullNameCmp))
          {
              // not an our file
              //if (!pAppConfig || !pAppConfig->testVerbosity(VerbosityLevel::extra)) 
              if (isPrintDeclarationInfoAllowed(strFullNameCmp))
                  printDeclarationInfo(stream, NamedDecl, 1 /* extraLfCount */ , kindStr);
              return;
          }

          auto declName     = NamedDecl->getQualifiedNameAsString();
          auto declKind     = NamedDecl->getKind();

          auto &declInfo    = foundDeclarations[declName];
          declInfo.name     = declName;
          declInfo.nameKinds.insert(declKind);
          declInfo.addLocationFileInfo( declKind, strLocFileName, strFullNameCmp );

          if (isPrintDeclarationInfoAllowed(strFullNameCmp))
          {
              printDeclarationInfo(stream, NamedDecl, 1 /* extraLfCount */ , kindStr);
          }
          
      }


      
public:

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

          //auto &stream = llvm::outs();
          auto &stream = logMsg;

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
                      case k : declUsageMapHandled[k]++; processDeclaration(NamedDecl, marty::clang::helpers::getClangDeclKindName(k) /* #k */  ); break

          #define HANDLE_DECL_EX(k) \
                      HANDLE_DECL_IMPL(k)

          #define HANDLE_DECL(k) \
                      HANDLE_DECL_IMPL(clang::Decl::Kind::k)


          #define UNHANDLED_DECL_IMPL(k) \
                      case k : declUsageMapUnhandled[k]++; unhadledDeclaration(NamedDecl, marty::clang::helpers::getClangDeclKindName(k) /* #k */  ); break

          #define UNHANDLED_DECL_EX(k) \
                      UNHANDLED_DECL_IMPL(k)

          #define UNHANDLED_DECL(k) \
                      UNHANDLED_DECL_IMPL(clang::Decl::Kind::k)


          #define SKIP_DECL_IMPL(k) \
                      case k : declUsageMapSkipped[k]++; skipDeclaration(NamedDecl, marty::clang::helpers::getClangDeclKindName(k) /* #k */  ); break

          #define SKIP_DECL_EX(k) \
                      SKIP_DECL_IMPL(k)

          #define SKIP_DECL(k) \
                      SKIP_DECL_IMPL(clang::Decl::Kind::k)

          // https://clang.llvm.org/doxygen/DeclBase_8cpp_source.html

          switch(kind)
          {
              HANDLE_DECL    (Function);
              HANDLE_DECL    (Typedef);
              HANDLE_DECL    (TypeAlias);
              HANDLE_DECL    (TypeAliasTemplate);
              HANDLE_DECL    (VarTemplate);
              HANDLE_DECL    (CXXRecord);
              HANDLE_DECL    (Enum);
              HANDLE_DECL    (FunctionTemplate);
              HANDLE_DECL    (ClassTemplate);
              SKIP_DECL      (CXXDeductionGuide);
              SKIP_DECL      (CXXMethod);
              SKIP_DECL      (CXXConstructor);
              SKIP_DECL      (CXXDestructor);
              SKIP_DECL      (CXXConversion);
              SKIP_DECL      (Var);
              SKIP_DECL      (ParmVar);
              SKIP_DECL      (NonTypeTemplateParm);
              SKIP_DECL      (TemplateTypeParm);
              SKIP_DECL      (UnresolvedUsingTypename);
              SKIP_DECL      (UnresolvedUsingValue);
              SKIP_DECL      (Using);
              SKIP_DECL      (Field);
              SKIP_DECL      (EnumConstant);
              SKIP_DECL      (Namespace);
              SKIP_DECL      (NamespaceAlias);
              SKIP_DECL      (TemplateTemplateParm);
              SKIP_DECL      (UsingDirective);
              SKIP_DECL      (ClassTemplateSpecialization);
              SKIP_DECL      (ClassTemplatePartialSpecialization);
              SKIP_DECL      (ClassScopeFunctionSpecialization);
              SKIP_DECL      (VarTemplateSpecialization);
              SKIP_DECL      (VarTemplatePartialSpecialization);
              SKIP_DECL      (ObjCAtDefsField);
              SKIP_DECL      (ObjCIvar);
              SKIP_DECL      (ObjCTypeParam);
              SKIP_DECL      (ObjCCompatibleAlias);
              SKIP_DECL      (ObjCInterface);
              SKIP_DECL      (ObjCMethod);
              SKIP_DECL      (ObjCProperty);
              SKIP_DECL      (ObjCProtocol);
              SKIP_DECL      (ObjCPropertyImpl);
              SKIP_DECL      (ObjCImplementation);
              SKIP_DECL      (ObjCCategory);
              SKIP_DECL      (ObjCCategoryImpl);
              UNHANDLED_DECL (ConstructorUsingShadow);
              UNHANDLED_DECL (ImplicitParam);
              UNHANDLED_DECL (MSProperty);
              UNHANDLED_DECL (Label);
              UNHANDLED_DECL (IndirectField);
              UNHANDLED_DECL (Binding);
              UNHANDLED_DECL (Concept);
              UNHANDLED_DECL (UsingShadow);
              UNHANDLED_DECL (UsingPack);
              UNHANDLED_DECL (UsingEnum);
              UNHANDLED_DECL (Record);
              UNHANDLED_DECL (UnresolvedUsingIfExists);
              UNHANDLED_DECL (OMPDeclareReduction);
              UNHANDLED_DECL (OMPDeclareMapper);
              UNHANDLED_DECL (Friend);
              UNHANDLED_DECL (FriendTemplate);
              UNHANDLED_DECL (AccessSpec);
              UNHANDLED_DECL (LinkageSpec);
              UNHANDLED_DECL (Export);
              UNHANDLED_DECL (FileScopeAsm);
              UNHANDLED_DECL (StaticAssert);
              UNHANDLED_DECL (PragmaComment);
              UNHANDLED_DECL (PragmaDetectMismatch);
              UNHANDLED_DECL (Block);
              UNHANDLED_DECL (Captured);
              UNHANDLED_DECL (TranslationUnit);
              UNHANDLED_DECL (ExternCContext);
              UNHANDLED_DECL (Decomposition);
              UNHANDLED_DECL (MSGuid);
              UNHANDLED_DECL (TemplateParamObject);
              UNHANDLED_DECL (BuiltinTemplate);
              UNHANDLED_DECL (Import);
              UNHANDLED_DECL (OMPThreadPrivate);
              UNHANDLED_DECL (OMPAllocate);
              UNHANDLED_DECL (OMPRequires);
              UNHANDLED_DECL (OMPCapturedExpr);
              UNHANDLED_DECL (Empty);
              UNHANDLED_DECL (LifetimeExtendedTemporary);
              UNHANDLED_DECL (RequiresExprBody);

              // DECL_RANGE(ObjCImpl, ObjCCategoryImpl, ObjCImplementation)
              //UNHANDLED_DECL();

              default:
                   stream << "UnsignedKind: " << (unsigned)kind << "\n";
                   declUsageMapUnknowns[kind]++;
                   printDeclarationInfo(stream, NamedDecl);
          };
          

          // llvm::outs() << "Found " << NamedDecl->getQualifiedNameAsString() 
          //              << " at "
          //              //<< getDeclLocation(NamedDecl->getLocStart()) << "\n";
          //              << getDeclLocation(NamedDecl->getBeginLoc()) << "\n" // https://clang.llvm.org/doxygen/classclang_1_1Decl.html
          //              << NamedDecl->getDeclKindName()
          //              << "\n"
          //              << "\n"
          //              ;

          // processDeclaration(stream, NamedDecl);
          // StreamType& processDeclaration(StreamType &stream, clang::NamedDecl *NamedDecl, int extraLfCount = 1, std::string kindStr = "") const

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
