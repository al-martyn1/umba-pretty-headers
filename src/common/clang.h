#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <set>
#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>
#include <exception>
#include <stdexcept>
#include <iterator>


#include "clang/AST/ASTConsumer.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/AST/DeclBase.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Basic/SourceLocation.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/CompilationDatabase.h"
#include "clang/Tooling/JSONCompilationDatabase.h"

#include "llvm/Support/raw_ostream.h"


#if defined(_MSC_VER)

#pragma comment(lib, "clangAnalysis")
#pragma comment(lib, "clangAPINotes")
#pragma comment(lib, "clangApplyReplacements")
#pragma comment(lib, "clangARCMigrate")
#pragma comment(lib, "clangAST")
#pragma comment(lib, "clangASTMatchers")
#pragma comment(lib, "clangBasic")
#pragma comment(lib, "clangChangeNamespace")
#pragma comment(lib, "clangCodeGen")
#pragma comment(lib, "clangCrossTU")
#pragma comment(lib, "clangDaemon")
#pragma comment(lib, "clangDaemonTweaks")
#pragma comment(lib, "clangDependencyScanning")
#pragma comment(lib, "clangDirectoryWatcher")
#pragma comment(lib, "clangDoc")
#pragma comment(lib, "clangdRemoteIndex")
#pragma comment(lib, "clangDriver")
#pragma comment(lib, "clangdSupport")
#pragma comment(lib, "clangDynamicASTMatchers")
#pragma comment(lib, "clangEdit")
#pragma comment(lib, "clangFormat")
#pragma comment(lib, "clangFrontend")
#pragma comment(lib, "clangFrontendTool")
#pragma comment(lib, "clangHandleCXX")
#pragma comment(lib, "clangHandleLLVM")
#pragma comment(lib, "clangIncludeFixer")
#pragma comment(lib, "clangIncludeFixerPlugin")
#pragma comment(lib, "clangIndex")
#pragma comment(lib, "clangIndexSerialization")
#pragma comment(lib, "clangInterpreter")
#pragma comment(lib, "clangLex")
#pragma comment(lib, "clangMove")
#pragma comment(lib, "clangParse")
#pragma comment(lib, "clangQuery")
#pragma comment(lib, "clangReorderFields")
#pragma comment(lib, "clangRewrite")
#pragma comment(lib, "clangRewriteFrontend")
#pragma comment(lib, "clangSema")
#pragma comment(lib, "clangSerialization")
#pragma comment(lib, "clangStaticAnalyzerCheckers")
#pragma comment(lib, "clangStaticAnalyzerCore")
#pragma comment(lib, "clangStaticAnalyzerFrontend")
#pragma comment(lib, "clangTesting")
#pragma comment(lib, "clangTidy")
#pragma comment(lib, "clangTidyAbseilModule")
#pragma comment(lib, "clangTidyAlteraModule")
#pragma comment(lib, "clangTidyAndroidModule")
#pragma comment(lib, "clangTidyBoostModule")
#pragma comment(lib, "clangTidyBugproneModule")
#pragma comment(lib, "clangTidyCERTModule")
#pragma comment(lib, "clangTidyConcurrencyModule")
#pragma comment(lib, "clangTidyCppCoreGuidelinesModule")
#pragma comment(lib, "clangTidyDarwinModule")
#pragma comment(lib, "clangTidyFuchsiaModule")
#pragma comment(lib, "clangTidyGoogleModule")
#pragma comment(lib, "clangTidyHICPPModule")
#pragma comment(lib, "clangTidyLinuxKernelModule")
#pragma comment(lib, "clangTidyLLVMLibcModule")
#pragma comment(lib, "clangTidyLLVMModule")
#pragma comment(lib, "clangTidyMain")
#pragma comment(lib, "clangTidyMiscModule")
#pragma comment(lib, "clangTidyModernizeModule")
#pragma comment(lib, "clangTidyMPIModule")
#pragma comment(lib, "clangTidyObjCModule")
#pragma comment(lib, "clangTidyOpenMPModule")
#pragma comment(lib, "clangTidyPerformanceModule")
#pragma comment(lib, "clangTidyPlugin")
#pragma comment(lib, "clangTidyPortabilityModule")
#pragma comment(lib, "clangTidyReadabilityModule")
#pragma comment(lib, "clangTidyUtils")
#pragma comment(lib, "clangTidyZirconModule")
#pragma comment(lib, "clangTooling")
#pragma comment(lib, "clangToolingASTDiff")
#pragma comment(lib, "clangToolingCore")
#pragma comment(lib, "clangToolingInclusions")
#pragma comment(lib, "clangToolingRefactoring")
#pragma comment(lib, "clangToolingSyntax")
#pragma comment(lib, "clangTransformer")
#pragma comment(lib, "DynamicLibraryLib")
#pragma comment(lib, "ExampleIRTransforms")
#pragma comment(lib, "findAllSymbols")
#pragma comment(lib, "gtest")
#pragma comment(lib, "gtest_main")
#pragma comment(lib, "Kaleidoscope-Ch4")
#pragma comment(lib, "Kaleidoscope-Ch5")
#pragma comment(lib, "Kaleidoscope-Ch6")
#pragma comment(lib, "Kaleidoscope-Ch7")
#pragma comment(lib, "Kaleidoscope-Ch8")
#pragma comment(lib, "Kaleidoscope-Ch9")
#pragma comment(lib, "libclang")
#pragma comment(lib, "LLVM-C")
#pragma comment(lib, "LLVMAArch64AsmParser")
#pragma comment(lib, "LLVMAArch64CodeGen")
#pragma comment(lib, "LLVMAArch64Desc")
#pragma comment(lib, "LLVMAArch64Disassembler")
#pragma comment(lib, "LLVMAArch64Info")
#pragma comment(lib, "LLVMAArch64Utils")
#pragma comment(lib, "LLVMAggressiveInstCombine")
#pragma comment(lib, "LLVMAMDGPUAsmParser")
#pragma comment(lib, "LLVMAMDGPUCodeGen")
#pragma comment(lib, "LLVMAMDGPUDesc")
#pragma comment(lib, "LLVMAMDGPUDisassembler")
#pragma comment(lib, "LLVMAMDGPUInfo")
#pragma comment(lib, "LLVMAMDGPUUtils")
#pragma comment(lib, "LLVMAnalysis")
#pragma comment(lib, "LLVMARMAsmParser")
#pragma comment(lib, "LLVMARMCodeGen")
#pragma comment(lib, "LLVMARMDesc")
#pragma comment(lib, "LLVMARMDisassembler")
#pragma comment(lib, "LLVMARMInfo")
#pragma comment(lib, "LLVMARMUtils")
#pragma comment(lib, "LLVMAsmParser")
#pragma comment(lib, "LLVMAsmPrinter")
#pragma comment(lib, "LLVMAVRAsmParser")
#pragma comment(lib, "LLVMAVRCodeGen")
#pragma comment(lib, "LLVMAVRDesc")
#pragma comment(lib, "LLVMAVRDisassembler")
#pragma comment(lib, "LLVMAVRInfo")
#pragma comment(lib, "LLVMBinaryFormat")
#pragma comment(lib, "LLVMBitReader")
#pragma comment(lib, "LLVMBitstreamReader")
#pragma comment(lib, "LLVMBitWriter")
#pragma comment(lib, "LLVMBPFAsmParser")
#pragma comment(lib, "LLVMBPFCodeGen")
#pragma comment(lib, "LLVMBPFDesc")
#pragma comment(lib, "LLVMBPFDisassembler")
#pragma comment(lib, "LLVMBPFInfo")
#pragma comment(lib, "LLVMCFGuard")
#pragma comment(lib, "LLVMCFIVerify")
#pragma comment(lib, "LLVMCodeGen")
#pragma comment(lib, "LLVMCore")
#pragma comment(lib, "LLVMCoroutines")
#pragma comment(lib, "LLVMCoverage")
#pragma comment(lib, "LLVMDebugInfoCodeView")
#pragma comment(lib, "LLVMDebugInfoDWARF")
#pragma comment(lib, "LLVMDebugInfoGSYM")
#pragma comment(lib, "LLVMDebugInfoMSF")
#pragma comment(lib, "LLVMDebugInfoPDB")
#pragma comment(lib, "LLVMDemangle")
#pragma comment(lib, "LLVMDlltoolDriver")
#pragma comment(lib, "LLVMDWARFLinker")
#pragma comment(lib, "LLVMDWP")
#pragma comment(lib, "LLVMExecutionEngine")
#pragma comment(lib, "LLVMExegesis")
#pragma comment(lib, "LLVMExegesisAArch64")
#pragma comment(lib, "LLVMExegesisMips")
#pragma comment(lib, "LLVMExegesisPowerPC")
#pragma comment(lib, "LLVMExegesisX86")
#pragma comment(lib, "LLVMExtensions")
#pragma comment(lib, "LLVMFileCheck")
#pragma comment(lib, "LLVMFrontendOpenACC")
#pragma comment(lib, "LLVMFrontendOpenMP")
#pragma comment(lib, "LLVMFuzzMutate")
#pragma comment(lib, "LLVMGlobalISel")
#pragma comment(lib, "LLVMHexagonAsmParser")
#pragma comment(lib, "LLVMHexagonCodeGen")
#pragma comment(lib, "LLVMHexagonDesc")
#pragma comment(lib, "LLVMHexagonDisassembler")
#pragma comment(lib, "LLVMHexagonInfo")
#pragma comment(lib, "LLVMInstCombine")
#pragma comment(lib, "LLVMInstrumentation")
#pragma comment(lib, "LLVMInterfaceStub")
#pragma comment(lib, "LLVMInterpreter")
#pragma comment(lib, "LLVMipo")
#pragma comment(lib, "LLVMIRReader")
#pragma comment(lib, "LLVMJITLink")
#pragma comment(lib, "LLVMLanaiAsmParser")
#pragma comment(lib, "LLVMLanaiCodeGen")
#pragma comment(lib, "LLVMLanaiDesc")
#pragma comment(lib, "LLVMLanaiDisassembler")
#pragma comment(lib, "LLVMLanaiInfo")
#pragma comment(lib, "LLVMLibDriver")
#pragma comment(lib, "LLVMLineEditor")
#pragma comment(lib, "LLVMLinker")
#pragma comment(lib, "LLVMLTO")
#pragma comment(lib, "LLVMMC")
#pragma comment(lib, "LLVMMCA")
#pragma comment(lib, "LLVMMCACustomBehaviourAMDGPU")
#pragma comment(lib, "LLVMMCDisassembler")
#pragma comment(lib, "LLVMMCJIT")
#pragma comment(lib, "LLVMMCParser")
#pragma comment(lib, "LLVMMipsAsmParser")
#pragma comment(lib, "LLVMMipsCodeGen")
#pragma comment(lib, "LLVMMipsDesc")
#pragma comment(lib, "LLVMMipsDisassembler")
#pragma comment(lib, "LLVMMipsInfo")
#pragma comment(lib, "LLVMMIRParser")
#pragma comment(lib, "LLVMMSP430AsmParser")
#pragma comment(lib, "LLVMMSP430CodeGen")
#pragma comment(lib, "LLVMMSP430Desc")
#pragma comment(lib, "LLVMMSP430Disassembler")
#pragma comment(lib, "LLVMMSP430Info")
#pragma comment(lib, "LLVMNVPTXCodeGen")
#pragma comment(lib, "LLVMNVPTXDesc")
#pragma comment(lib, "LLVMNVPTXInfo")
#pragma comment(lib, "LLVMObjCARCOpts")
#pragma comment(lib, "LLVMObject")
#pragma comment(lib, "LLVMObjectYAML")
#pragma comment(lib, "LLVMOption")
#pragma comment(lib, "LLVMOrcJIT")
#pragma comment(lib, "LLVMOrcShared")
#pragma comment(lib, "LLVMOrcTargetProcess")
#pragma comment(lib, "LLVMPasses")
#pragma comment(lib, "LLVMPowerPCAsmParser")
#pragma comment(lib, "LLVMPowerPCCodeGen")
#pragma comment(lib, "LLVMPowerPCDesc")
#pragma comment(lib, "LLVMPowerPCDisassembler")
#pragma comment(lib, "LLVMPowerPCInfo")
#pragma comment(lib, "LLVMProfileData")
#pragma comment(lib, "LLVMRemarks")
#pragma comment(lib, "LLVMRISCVAsmParser")
#pragma comment(lib, "LLVMRISCVCodeGen")
#pragma comment(lib, "LLVMRISCVDesc")
#pragma comment(lib, "LLVMRISCVDisassembler")
#pragma comment(lib, "LLVMRISCVInfo")
#pragma comment(lib, "LLVMRuntimeDyld")
#pragma comment(lib, "LLVMScalarOpts")
#pragma comment(lib, "LLVMSelectionDAG")
#pragma comment(lib, "LLVMSparcAsmParser")
#pragma comment(lib, "LLVMSparcCodeGen")
#pragma comment(lib, "LLVMSparcDesc")
#pragma comment(lib, "LLVMSparcDisassembler")
#pragma comment(lib, "LLVMSparcInfo")
#pragma comment(lib, "LLVMSupport")
#pragma comment(lib, "LLVMSymbolize")
#pragma comment(lib, "LLVMSystemZAsmParser")
#pragma comment(lib, "LLVMSystemZCodeGen")
#pragma comment(lib, "LLVMSystemZDesc")
#pragma comment(lib, "LLVMSystemZDisassembler")
#pragma comment(lib, "LLVMSystemZInfo")
#pragma comment(lib, "LLVMTableGen")
#pragma comment(lib, "LLVMTableGenGlobalISel")
#pragma comment(lib, "LLVMTarget")
#pragma comment(lib, "LLVMTestingSupport")
#pragma comment(lib, "LLVMTextAPI")
#pragma comment(lib, "LLVMTransformUtils")
#pragma comment(lib, "LLVMVectorize")
#pragma comment(lib, "LLVMWebAssemblyAsmParser")
#pragma comment(lib, "LLVMWebAssemblyCodeGen")
#pragma comment(lib, "LLVMWebAssemblyDesc")
#pragma comment(lib, "LLVMWebAssemblyDisassembler")
#pragma comment(lib, "LLVMWebAssemblyInfo")
#pragma comment(lib, "LLVMWebAssemblyUtils")
#pragma comment(lib, "LLVMWindowsManifest")
#pragma comment(lib, "LLVMX86AsmParser")
#pragma comment(lib, "LLVMX86CodeGen")
#pragma comment(lib, "LLVMX86Desc")
#pragma comment(lib, "LLVMX86Disassembler")
#pragma comment(lib, "LLVMX86Info")
#pragma comment(lib, "LLVMXCoreCodeGen")
#pragma comment(lib, "LLVMXCoreDesc")
#pragma comment(lib, "LLVMXCoreDisassembler")
#pragma comment(lib, "LLVMXCoreInfo")
#pragma comment(lib, "LLVMXRay")
#pragma comment(lib, "LTO")
#pragma comment(lib, "Remarks")


#if defined(WIN32) || defined(_WIN32)

// https://stackoverflow.com/questions/7028304/error-lnk2019-when-using-getfileversioninfosize
#pragma comment(lib, "Version")
#pragma comment(lib, "advapi32")
#pragma comment(lib, "shell32")

#endif

#endif
