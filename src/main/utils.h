#pragma once

#include "matty_clang_helpers.h"


#define LLVM_ERROR(__Context, __Object, __File, __Message) \
do { \
    marty::clang::helpers::printError( llvm::errs(), marty::clang::helpers::getFullSourceLoc(__Context, __Object), __Message ); \
    /*__Object->dump();*/ \
} while (0)
