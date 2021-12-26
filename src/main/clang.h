#pragma once

#include <iostream>
#include <fstream>


#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"


#pragma comment(lib, "LLVMBitstreamReader")
#pragma comment(lib, "LLVMBitReader")
#pragma comment(lib, "LLVMBitWriter")
#pragma comment(lib, "LLVMRemarks")
#pragma comment(lib, "LLVMSupport")
#pragma comment(lib, "LLVMAnalysis")
#pragma comment(lib, "LLVMCore")
#pragma comment(lib, "LLVMFrontendOpenMP")
#pragma comment(lib, "LLVMDebugInfoDWARF")
#pragma comment(lib, "LLVMAsmPrinter")
#pragma comment(lib, "LLVMBinaryFormat")

#pragma comment(lib, "clangSerialization")
#pragma comment(lib, "clangLex")
#pragma comment(lib, "clangParse")
#pragma comment(lib, "clangSema")
#pragma comment(lib, "clangTidyOpenMPModule")
#pragma comment(lib, "clangTooling")
#pragma comment(lib, "clangFrontend")
#pragma comment(lib, "clangASTMatchers")
#pragma comment(lib, "clangAST")
#pragma comment(lib, "clangBasic")




