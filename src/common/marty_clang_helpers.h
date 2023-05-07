#pragma once


#include "clang.h"

#include "umba/enum_helpers.h"
#include "umba/flag_helpers.h"

#define LLVM_ERROR(__Context, __Object, __File, __Message) \
do { \
    marty::clang::helpers::printError( llvm::errs(), marty::clang::helpers::getFullSourceLoc(__Context, __Object), __Message ); \
    /*__Object->dump();*/ \
} while (0)



// marty::clang::helpers::
namespace marty{
namespace clang{
namespace helpers{



//----------------------------------------------------------------------------
enum class DeclKindOfKind
{
    invalid        = -1,

    none           = 0,
                             // clang::Decl::Kind

    cxxClass       = 0x0001, // ClassTemplate, CXXRecord      - classes and structs?
    freeFunction   = 0x0002, // FunctionTemplate, Function    - non-member functions and function templates
    cTypedef       = 0x0004, // Typedef                       - Old good Toby (typedef)
    cxxTypeAlias   = 0x0008, // TypeAlias, TypeAliasTemplate  - C++ usings
    cxxVarTemplate = 0x0010, // VarTemplate                   - C++ usings for var templates
    cEnum          = 0x0020, // Enum                          - enums - Old good plain "C" and new C++ enum classes
    ppDefine       = 0x0040, // defines                       - preprocessor defines

    all            = cxxClass|freeFunction|cTypedef|cxxTypeAlias|cxxVarTemplate|cEnum,
    allWithDefine  = all|ppDefine,

    exactAll       = allWithDefine,

    type           = cxxClass | cTypedef | cxxTypeAlias | cEnum

};


inline
DeclKindOfKind declKind_toKindOfKind( ::clang::Decl::Kind k )
{
    switch(k)
    {
        case ::clang::Decl::Kind::ClassTemplate       : return DeclKindOfKind::cxxClass;
        case ::clang::Decl::Kind::CXXRecord           : return DeclKindOfKind::cxxClass;

        case ::clang::Decl::Kind::FunctionTemplate    : return DeclKindOfKind::freeFunction;
        case ::clang::Decl::Kind::Function            : return DeclKindOfKind::freeFunction;

        case ::clang::Decl::Kind::Typedef             : return DeclKindOfKind::cTypedef;

        case ::clang::Decl::Kind::TypeAlias           : return DeclKindOfKind::cxxTypeAlias;
        case ::clang::Decl::Kind::TypeAliasTemplate   : return DeclKindOfKind::cxxTypeAlias;

        case ::clang::Decl::Kind::VarTemplate         : return DeclKindOfKind::cxxVarTemplate;

        case ::clang::Decl::Kind::Enum                : return DeclKindOfKind::cEnum;

        default                                       : return DeclKindOfKind::invalid;

    }
}

UMBA_ENUM_CLASS_IMPLEMENT_FLAG_OPERATORS(DeclKindOfKind)

inline
std::string DeclKindOfKind_flagToStdString(DeclKindOfKind k)
{
    switch(k)
    {
        // case DeclKindOfKind::cxxClass      : return std::string("cxxClass"      );
        // case DeclKindOfKind::freeFunction  : return std::string("freeFunction"  );
        // case DeclKindOfKind::cTypedef      : return std::string("cTypedef"      );
        // case DeclKindOfKind::cxxTypeAlias  : return std::string("cxxTypeAlias"  );
        // case DeclKindOfKind::cxxVarTemplate: return std::string("cxxVarTemplate");
        // case DeclKindOfKind::cEnum         : return std::string("cEnum"         );
        // case DeclKindOfKind::ppDefine      : return std::string("ppDefine"      );
        case DeclKindOfKind::cxxClass      : return std::string("class"        );
        case DeclKindOfKind::freeFunction  : return std::string("function"     );
        case DeclKindOfKind::cTypedef      : return std::string("typedef"      );
        case DeclKindOfKind::cxxTypeAlias  : return std::string("type-alias"   );
        case DeclKindOfKind::cxxVarTemplate: return std::string("var-template" );
        case DeclKindOfKind::cEnum         : return std::string("enum"         );
        case DeclKindOfKind::ppDefine      : return std::string("pp-macro"     );
        default                            : return std::string("invalid"      );
    }
}

inline
std::map<DeclKindOfKind, std::string>
getAllDeclKindOfKindMap()
{
    std::map<DeclKindOfKind, std::string> res;

    res[DeclKindOfKind::cxxClass      ] = DeclKindOfKind_flagToStdString(DeclKindOfKind::cxxClass      );
    res[DeclKindOfKind::freeFunction  ] = DeclKindOfKind_flagToStdString(DeclKindOfKind::freeFunction  );
    res[DeclKindOfKind::cTypedef      ] = DeclKindOfKind_flagToStdString(DeclKindOfKind::cTypedef      );
    res[DeclKindOfKind::cxxTypeAlias  ] = DeclKindOfKind_flagToStdString(DeclKindOfKind::cxxTypeAlias  );
    res[DeclKindOfKind::cxxVarTemplate] = DeclKindOfKind_flagToStdString(DeclKindOfKind::cxxVarTemplate);
    res[DeclKindOfKind::cEnum         ] = DeclKindOfKind_flagToStdString(DeclKindOfKind::cEnum         );
    res[DeclKindOfKind::ppDefine      ] = DeclKindOfKind_flagToStdString(DeclKindOfKind::ppDefine      );

    return res;
}

inline
std::string DeclKindOfKind_toStdString(DeclKindOfKind kFlags)
{
    static auto m = getAllDeclKindOfKindMap();
    return umba::flag_helpers::util::toStringImpl(m,kFlags);
}

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
template< typename Context, typename Object > inline
::clang::FullSourceLoc getFullSourceLoc( Context *pContext, Object *pObject )
{
    return Context->getFullLoc(pObject->getBeginLoc());
}

template< typename Context, typename Object > inline
::clang::FullSourceLoc getFullSourceLoc( Context &context, Object *pObject )
{
    return context.getFullLoc(pObject->getBeginLoc());
}

inline
llvm::StringRef getSourceLocName( const ::clang::FullSourceLoc &loc )
{
    if (!loc.isValid())
        return llvm::StringRef();

    if (!loc.getFileEntry())
        return llvm::StringRef();

    return loc.getFileEntry()->getName();

//  llvm::StringRef fileName = (loc.isValid() && loc.getFileEntry() ? loc.getFileEntry()->getName() : "<INVALID_LOCATION>");

}


template< typename Stream, typename StringType > inline
Stream& printError( Stream &stream, StringType msg )
{
    stream << "error: " << msg << "\n";

    return stream;
}


template< typename Stream, typename StringType, typename FilenameStringType > inline
Stream& printError( Stream &stream, StringType msg, FilenameStringType fileName, unsigned line, unsigned pos )
{
    stream << fileName << ":" << line << ":" << pos << ": error: " << msg << "\n";

    return stream;
}




template< typename Stream, typename StringType > inline
Stream& printError( Stream &stream, const ::clang::FullSourceLoc &sourceLoc, StringType msg )
{
    auto fileName = (sourceLoc.isValid() && sourceLoc.getFileEntry() ? sourceLoc.getFileEntry()->getName() : "<FILE>");
    auto line     = (sourceLoc.isValid() ? sourceLoc.getSpellingLineNumber()   : 0);
    auto pos      = (sourceLoc.isValid() ? sourceLoc.getSpellingColumnNumber() : 0);

    //stream << fileName << ":" << line << ":" << pos << ": error: " << msg << "\n";

    //return stream;

    printError( stream, msg, fileName, line, pos );
}



inline
std::map< ::clang::Decl::Kind, unsigned > makeClangDeclKindUsageMap()
{
    static
    std::map< ::clang::Decl::Kind, unsigned > res;

    if (!res.empty())
        return res;

    res[::clang::Decl::Kind::Function] = 0;
    res[::clang::Decl::Kind::CXXDeductionGuide] = 0;
    res[::clang::Decl::Kind::CXXMethod] = 0;
    res[::clang::Decl::Kind::CXXConstructor] = 0;
    res[::clang::Decl::Kind::ConstructorUsingShadow] = 0;
    res[::clang::Decl::Kind::CXXDestructor] = 0;
    res[::clang::Decl::Kind::CXXConversion] = 0;
    res[::clang::Decl::Kind::EnumConstant] = 0;
    res[::clang::Decl::Kind::Var] = 0;
    res[::clang::Decl::Kind::ImplicitParam] = 0;
    res[::clang::Decl::Kind::ParmVar] = 0;
    res[::clang::Decl::Kind::ObjCMethod] = 0;
    res[::clang::Decl::Kind::ObjCProperty] = 0;
    res[::clang::Decl::Kind::MSProperty] = 0;
    res[::clang::Decl::Kind::Label] = 0;
    res[::clang::Decl::Kind::IndirectField] = 0;
    res[::clang::Decl::Kind::Binding] = 0;
    res[::clang::Decl::Kind::NonTypeTemplateParm] = 0;
    res[::clang::Decl::Kind::VarTemplate] = 0;
    res[::clang::Decl::Kind::Concept] = 0;
    res[::clang::Decl::Kind::ObjCCompatibleAlias] = 0;
    res[::clang::Decl::Kind::ObjCInterface] = 0;
    res[::clang::Decl::Kind::Typedef] = 0;
    res[::clang::Decl::Kind::TypeAlias] = 0;
    res[::clang::Decl::Kind::TemplateTypeParm] = 0;
    res[::clang::Decl::Kind::ObjCTypeParam] = 0;
    res[::clang::Decl::Kind::UnresolvedUsingTypename] = 0;
    res[::clang::Decl::Kind::UsingShadow] = 0;
    res[::clang::Decl::Kind::UnresolvedUsingValue] = 0;
    res[::clang::Decl::Kind::Using] = 0;
    res[::clang::Decl::Kind::UsingPack] = 0;
    res[::clang::Decl::Kind::UsingEnum] = 0;
    res[::clang::Decl::Kind::ObjCProtocol] = 0;
    res[::clang::Decl::Kind::Field] = 0;
    res[::clang::Decl::Kind::ObjCAtDefsField] = 0;
    res[::clang::Decl::Kind::ObjCIvar] = 0;
    res[::clang::Decl::Kind::Record] = 0;
    res[::clang::Decl::Kind::CXXRecord] = 0;
    res[::clang::Decl::Kind::Enum] = 0;
    res[::clang::Decl::Kind::Namespace] = 0;
    res[::clang::Decl::Kind::NamespaceAlias] = 0;
    res[::clang::Decl::Kind::FunctionTemplate] = 0;
    res[::clang::Decl::Kind::ClassTemplate] = 0;
    res[::clang::Decl::Kind::TemplateTemplateParm] = 0;
    res[::clang::Decl::Kind::TypeAliasTemplate] = 0;
    res[::clang::Decl::Kind::UnresolvedUsingIfExists] = 0;
    res[::clang::Decl::Kind::OMPDeclareReduction] = 0;
    res[::clang::Decl::Kind::OMPDeclareMapper] = 0;
    res[::clang::Decl::Kind::Friend] = 0;
    res[::clang::Decl::Kind::FriendTemplate] = 0;
    res[::clang::Decl::Kind::AccessSpec] = 0;
    res[::clang::Decl::Kind::LinkageSpec] = 0;
    res[::clang::Decl::Kind::Export] = 0;
    res[::clang::Decl::Kind::FileScopeAsm] = 0;
    res[::clang::Decl::Kind::StaticAssert] = 0;
    res[::clang::Decl::Kind::ObjCPropertyImpl] = 0;
    res[::clang::Decl::Kind::PragmaComment] = 0;
    res[::clang::Decl::Kind::PragmaDetectMismatch] = 0;
    res[::clang::Decl::Kind::Block] = 0;
    res[::clang::Decl::Kind::Captured] = 0;
    res[::clang::Decl::Kind::TranslationUnit] = 0;
    res[::clang::Decl::Kind::ExternCContext] = 0;
    res[::clang::Decl::Kind::Decomposition] = 0;
    res[::clang::Decl::Kind::MSGuid] = 0;
    res[::clang::Decl::Kind::TemplateParamObject] = 0;
    res[::clang::Decl::Kind::UsingDirective] = 0;
    res[::clang::Decl::Kind::BuiltinTemplate] = 0;
    res[::clang::Decl::Kind::ClassTemplateSpecialization] = 0;
    res[::clang::Decl::Kind::ClassTemplatePartialSpecialization] = 0;
    res[::clang::Decl::Kind::ClassScopeFunctionSpecialization] = 0;
    res[::clang::Decl::Kind::VarTemplateSpecialization] = 0;
    res[::clang::Decl::Kind::VarTemplatePartialSpecialization] = 0;
    res[::clang::Decl::Kind::ObjCImplementation] = 0;
    res[::clang::Decl::Kind::ObjCCategory] = 0;
    res[::clang::Decl::Kind::ObjCCategoryImpl] = 0;
    res[::clang::Decl::Kind::Import] = 0;
    res[::clang::Decl::Kind::OMPThreadPrivate] = 0;
    res[::clang::Decl::Kind::OMPAllocate] = 0;
    res[::clang::Decl::Kind::OMPRequires] = 0;
    res[::clang::Decl::Kind::OMPCapturedExpr] = 0;
    res[::clang::Decl::Kind::Empty] = 0;
    res[::clang::Decl::Kind::LifetimeExtendedTemporary] = 0;
    res[::clang::Decl::Kind::RequiresExprBody] = 0;

    return res;
}


inline
std::string getClangDeclKindName(::clang::Decl::Kind kind)
{
    #define MARTY_CLANG_HELPERS_GET_CLANG_DECL_KIND_NAME_ENTRY( k ) case ::clang::Decl::Kind::k: return std::string( #k )
    switch(kind)
    {
        MARTY_CLANG_HELPERS_GET_CLANG_DECL_KIND_NAME_ENTRY(Function);
        MARTY_CLANG_HELPERS_GET_CLANG_DECL_KIND_NAME_ENTRY(CXXDeductionGuide);
        MARTY_CLANG_HELPERS_GET_CLANG_DECL_KIND_NAME_ENTRY(CXXMethod);
        MARTY_CLANG_HELPERS_GET_CLANG_DECL_KIND_NAME_ENTRY(CXXConstructor);
        MARTY_CLANG_HELPERS_GET_CLANG_DECL_KIND_NAME_ENTRY(ConstructorUsingShadow);
        MARTY_CLANG_HELPERS_GET_CLANG_DECL_KIND_NAME_ENTRY(CXXDestructor);
        MARTY_CLANG_HELPERS_GET_CLANG_DECL_KIND_NAME_ENTRY(CXXConversion);
        MARTY_CLANG_HELPERS_GET_CLANG_DECL_KIND_NAME_ENTRY(EnumConstant);
        MARTY_CLANG_HELPERS_GET_CLANG_DECL_KIND_NAME_ENTRY(Var);
        MARTY_CLANG_HELPERS_GET_CLANG_DECL_KIND_NAME_ENTRY(ImplicitParam);
        MARTY_CLANG_HELPERS_GET_CLANG_DECL_KIND_NAME_ENTRY(ParmVar);
        MARTY_CLANG_HELPERS_GET_CLANG_DECL_KIND_NAME_ENTRY(ObjCMethod);
        MARTY_CLANG_HELPERS_GET_CLANG_DECL_KIND_NAME_ENTRY(ObjCProperty);
        MARTY_CLANG_HELPERS_GET_CLANG_DECL_KIND_NAME_ENTRY(MSProperty);
        MARTY_CLANG_HELPERS_GET_CLANG_DECL_KIND_NAME_ENTRY(Label);
        MARTY_CLANG_HELPERS_GET_CLANG_DECL_KIND_NAME_ENTRY(IndirectField);
        MARTY_CLANG_HELPERS_GET_CLANG_DECL_KIND_NAME_ENTRY(Binding);
        MARTY_CLANG_HELPERS_GET_CLANG_DECL_KIND_NAME_ENTRY(NonTypeTemplateParm);
        MARTY_CLANG_HELPERS_GET_CLANG_DECL_KIND_NAME_ENTRY(VarTemplate);
        MARTY_CLANG_HELPERS_GET_CLANG_DECL_KIND_NAME_ENTRY(Concept);
        MARTY_CLANG_HELPERS_GET_CLANG_DECL_KIND_NAME_ENTRY(ObjCCompatibleAlias);
        MARTY_CLANG_HELPERS_GET_CLANG_DECL_KIND_NAME_ENTRY(ObjCInterface);
        MARTY_CLANG_HELPERS_GET_CLANG_DECL_KIND_NAME_ENTRY(Typedef);
        MARTY_CLANG_HELPERS_GET_CLANG_DECL_KIND_NAME_ENTRY(TypeAlias);
        MARTY_CLANG_HELPERS_GET_CLANG_DECL_KIND_NAME_ENTRY(TemplateTypeParm);
        MARTY_CLANG_HELPERS_GET_CLANG_DECL_KIND_NAME_ENTRY(ObjCTypeParam);
        MARTY_CLANG_HELPERS_GET_CLANG_DECL_KIND_NAME_ENTRY(UnresolvedUsingTypename);
        MARTY_CLANG_HELPERS_GET_CLANG_DECL_KIND_NAME_ENTRY(UsingShadow);
        MARTY_CLANG_HELPERS_GET_CLANG_DECL_KIND_NAME_ENTRY(UnresolvedUsingValue);
        MARTY_CLANG_HELPERS_GET_CLANG_DECL_KIND_NAME_ENTRY(Using);
        MARTY_CLANG_HELPERS_GET_CLANG_DECL_KIND_NAME_ENTRY(UsingPack);
        MARTY_CLANG_HELPERS_GET_CLANG_DECL_KIND_NAME_ENTRY(UsingEnum);
        MARTY_CLANG_HELPERS_GET_CLANG_DECL_KIND_NAME_ENTRY(ObjCProtocol);
        MARTY_CLANG_HELPERS_GET_CLANG_DECL_KIND_NAME_ENTRY(Field);
        MARTY_CLANG_HELPERS_GET_CLANG_DECL_KIND_NAME_ENTRY(ObjCAtDefsField);
        MARTY_CLANG_HELPERS_GET_CLANG_DECL_KIND_NAME_ENTRY(ObjCIvar);
        MARTY_CLANG_HELPERS_GET_CLANG_DECL_KIND_NAME_ENTRY(Record);
        MARTY_CLANG_HELPERS_GET_CLANG_DECL_KIND_NAME_ENTRY(CXXRecord);
        MARTY_CLANG_HELPERS_GET_CLANG_DECL_KIND_NAME_ENTRY(Enum);
        MARTY_CLANG_HELPERS_GET_CLANG_DECL_KIND_NAME_ENTRY(Namespace);
        MARTY_CLANG_HELPERS_GET_CLANG_DECL_KIND_NAME_ENTRY(NamespaceAlias);
        MARTY_CLANG_HELPERS_GET_CLANG_DECL_KIND_NAME_ENTRY(FunctionTemplate);
        MARTY_CLANG_HELPERS_GET_CLANG_DECL_KIND_NAME_ENTRY(ClassTemplate);
        MARTY_CLANG_HELPERS_GET_CLANG_DECL_KIND_NAME_ENTRY(TemplateTemplateParm);
        MARTY_CLANG_HELPERS_GET_CLANG_DECL_KIND_NAME_ENTRY(TypeAliasTemplate);
        MARTY_CLANG_HELPERS_GET_CLANG_DECL_KIND_NAME_ENTRY(UnresolvedUsingIfExists);
        MARTY_CLANG_HELPERS_GET_CLANG_DECL_KIND_NAME_ENTRY(OMPDeclareReduction);
        MARTY_CLANG_HELPERS_GET_CLANG_DECL_KIND_NAME_ENTRY(OMPDeclareMapper);
        MARTY_CLANG_HELPERS_GET_CLANG_DECL_KIND_NAME_ENTRY(Friend);
        MARTY_CLANG_HELPERS_GET_CLANG_DECL_KIND_NAME_ENTRY(FriendTemplate);
        MARTY_CLANG_HELPERS_GET_CLANG_DECL_KIND_NAME_ENTRY(AccessSpec);
        MARTY_CLANG_HELPERS_GET_CLANG_DECL_KIND_NAME_ENTRY(LinkageSpec);
        MARTY_CLANG_HELPERS_GET_CLANG_DECL_KIND_NAME_ENTRY(Export);
        MARTY_CLANG_HELPERS_GET_CLANG_DECL_KIND_NAME_ENTRY(FileScopeAsm);
        MARTY_CLANG_HELPERS_GET_CLANG_DECL_KIND_NAME_ENTRY(StaticAssert);
        MARTY_CLANG_HELPERS_GET_CLANG_DECL_KIND_NAME_ENTRY(ObjCPropertyImpl);
        MARTY_CLANG_HELPERS_GET_CLANG_DECL_KIND_NAME_ENTRY(PragmaComment);
        MARTY_CLANG_HELPERS_GET_CLANG_DECL_KIND_NAME_ENTRY(PragmaDetectMismatch);
        MARTY_CLANG_HELPERS_GET_CLANG_DECL_KIND_NAME_ENTRY(Block);
        MARTY_CLANG_HELPERS_GET_CLANG_DECL_KIND_NAME_ENTRY(Captured);
        MARTY_CLANG_HELPERS_GET_CLANG_DECL_KIND_NAME_ENTRY(TranslationUnit);
        MARTY_CLANG_HELPERS_GET_CLANG_DECL_KIND_NAME_ENTRY(ExternCContext);
        MARTY_CLANG_HELPERS_GET_CLANG_DECL_KIND_NAME_ENTRY(Decomposition);
        MARTY_CLANG_HELPERS_GET_CLANG_DECL_KIND_NAME_ENTRY(MSGuid);
        MARTY_CLANG_HELPERS_GET_CLANG_DECL_KIND_NAME_ENTRY(TemplateParamObject);
        MARTY_CLANG_HELPERS_GET_CLANG_DECL_KIND_NAME_ENTRY(UsingDirective);
        MARTY_CLANG_HELPERS_GET_CLANG_DECL_KIND_NAME_ENTRY(BuiltinTemplate);
        MARTY_CLANG_HELPERS_GET_CLANG_DECL_KIND_NAME_ENTRY(ClassTemplateSpecialization);
        MARTY_CLANG_HELPERS_GET_CLANG_DECL_KIND_NAME_ENTRY(ClassTemplatePartialSpecialization);
        MARTY_CLANG_HELPERS_GET_CLANG_DECL_KIND_NAME_ENTRY(ClassScopeFunctionSpecialization);
        MARTY_CLANG_HELPERS_GET_CLANG_DECL_KIND_NAME_ENTRY(VarTemplateSpecialization);
        MARTY_CLANG_HELPERS_GET_CLANG_DECL_KIND_NAME_ENTRY(VarTemplatePartialSpecialization);
        MARTY_CLANG_HELPERS_GET_CLANG_DECL_KIND_NAME_ENTRY(ObjCImplementation);
        MARTY_CLANG_HELPERS_GET_CLANG_DECL_KIND_NAME_ENTRY(ObjCCategory);
        MARTY_CLANG_HELPERS_GET_CLANG_DECL_KIND_NAME_ENTRY(ObjCCategoryImpl);
        MARTY_CLANG_HELPERS_GET_CLANG_DECL_KIND_NAME_ENTRY(Import);
        MARTY_CLANG_HELPERS_GET_CLANG_DECL_KIND_NAME_ENTRY(OMPThreadPrivate);
        MARTY_CLANG_HELPERS_GET_CLANG_DECL_KIND_NAME_ENTRY(OMPAllocate);
        MARTY_CLANG_HELPERS_GET_CLANG_DECL_KIND_NAME_ENTRY(OMPRequires);
        MARTY_CLANG_HELPERS_GET_CLANG_DECL_KIND_NAME_ENTRY(OMPCapturedExpr);
        MARTY_CLANG_HELPERS_GET_CLANG_DECL_KIND_NAME_ENTRY(Empty);
        MARTY_CLANG_HELPERS_GET_CLANG_DECL_KIND_NAME_ENTRY(LifetimeExtendedTemporary);
        MARTY_CLANG_HELPERS_GET_CLANG_DECL_KIND_NAME_ENTRY(RequiresExprBody);

        default:
            {
                std::ostringstream oss; 
                oss << "clang::Decl::Kind::UNKNOWN_" << (unsigned)kind;
                return oss.str();
            }
    }
}





} // namespace helpers
} // namespace clang
} // namespace marty

// marty::clang::helpers::
