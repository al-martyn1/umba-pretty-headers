# Umba Pretty Headers
### Advanced Includes Generator for C++ types.

This tool is based on the part of the LLVM/CLang project and uses the CLang Tooling Library.

This tool allows You to generate separate headers for each declaration from Your code, including:

- Classes/structs and it's templates
- Functions and it's templates
- C-style typedefs
- Modern C++ type alias
- Modern C++ variable templates
- Old good plain "C" enum's and new C++ enum classes
- Preprocessor definitions

### How It Works

Thsi tool parses Your C/C++ header files and generates a separate files for each found declaration, creating the folders structure according to the Your code C++ namespaces.

### Usage Example

```
umba-pretty-headers --quet --type --function --path==./_libs --path=./src --output-root=./_libs/_pretty-headers --exclude-files=*.h^ --exclude-names=*_H^ --exclude-names=util::*
```

### Another More Simple Tool

There is another simpliest tool, which generates headers using text configuration files - https://github.com/al-martyn1/umba-make-headers

### Important Command Line Options, which controls which types of declaration will be handled

**-C**,
**--class** - Allow to generate class/struct includes.

**-F**,
**--function** - Allow to generate free function includes.

**-T**,
**--typedef** - Allow to generate typedef includes.

**-L**,
**--type-alias** - Allow to generate type alias includes.

**-M**,
**--var-template** - Allow to generate variable template includes.

**-E**,
**--enum** - Allow to generate enum includes (both enum and enum class).

**-D**,
**--define** - Allow to generate preprocessor define includes.

**-Y**,
**--type** - Allow to generate all type includes (equivalent to --class --typedef --type-alias --enum).

**-A**,
**--all** - Allow to generate includes for all kinds of declaration, except to --define option.
--all is the default option.


### All Command Line Options

**-q**,
**--quet** - Operate quetly. Short alias for '--verbose=quet'.

**--home** - Open homepage.

**-V=LEVEL**,
**--verbose=LEVEL** - Set verbosity level. LEVEL parameter can be one of the next values:
quet - maximum quet mode (same as --quet).
normal - print common details.
config - print common details and app config.
detailed - print common details, app config and all declarations, which are found in user files.
extra - print common details, app config and all found declarations (from all files).
Allowed values: '0' / 'no' / 'q' / 'quet', '1' / 'n' / 'normal', '2' / 'c' / 'config', '3' / 'd' / 'detail' / 'detailed' or '4' / 'e' / 'extra' / 'high', initial value: 'normal'.

**--no-builtin-options** - Don't parse predefined options from main distribution options file '$(AppRoot)/conf/umba-pretty-headers.options'.

**--no-custom-builtin-options** - Don't parse predefined options from custom global options file '$(AppRoot)/conf/umba-pretty-headers.custom.options'.

**--no-user-builtin-options** - Don't parse predefined options from user local options file '$(Home)/.umba-pretty-headers.options'.

**-v**,
**--version** - Show version info.

**--where** - Show where the executable file is.

**--color=CLR** - Force set console output coloring.
Allowed values: 'file' / 'no' / 'none', 'ansi' / 'term' or 'cmd' / 'console' / 'win' / 'win32' / 'windows', initial value: <AUTO>.

**-K**,
**--keep-generated-files** - Do not delete generated files.

**-U**,
**--quoted-include** - By default, the '#include' directive in generated files uses angle brackets '<>'. This option turns on generating quoted includes with '""' quotation.

**-N=MASK,...**,
**--exclude-names=MASK,...** - Exclude C/C++ names from output. For details about 'MASK' parameter see '--exclude-files' option description.

**-O=PATH**,
**--output-path=PATH**,
**--output-root=PATH** - Set output root path.

**-X=MASK,...**,
**--exclude-files=MASK,...** - Exclude files from parsing. The 'MASK' parameter is a simple file mask, where '*' means any number of any chars, and '?' means exact one of any char. In addition, symbol '^' in front and/or back of the mask means that the mask will be bound to beginning/ending of the tested file name.
Also, regular expresion syntax allowed in form '{*?regex?*}YOURREGEX'. The regular expresions supports
See also: C++ Modified ECMA Script regular expression grammar - https://en.cppreference.com/w/cpp/regex/ecmascript.

**-C**,
**--class** - Allow to generate class/struct includes.

**-F**,
**--function** - Allow to generate free function includes.

**-T**,
**--typedef** - Allow to generate typedef includes.

**-L**,
**--type-alias** - Allow to generate type alias includes.

**-M**,
**--var-template** - Allow to generate variable template includes.

**-E**,
**--enum** - Allow to generate enum includes (both enum and enum class).

**-D**,
**--define** - Allow to generate preprocessor define includes.

**-Y**,
**--type** - Allow to generate all type includes (equivalent to --class --typedef --type-alias --enum).

**-A**,
**--all** - Allow to generate includes for all kinds of declaration, except to --define option.
--all is the default option.

**-R**,
**--generate-clear-script** - Generate clear script - .bat/.sh.

**--used-macros** - Write all found (in PP conditions) macros to file '$(OutputRoot)/__used_macros.txt'.

**--defined-macros** - Write all defined macros to file '$(OutputRoot)/__defined_macros.txt'.

**--dry-run**,
**--no-output** - Do not actually write output files. Simulation mode. Behave normally, but do not copy/creater/update any files.

**-S=NAME:VALUE**,
**--set=NAME:VALUE** - Set up macro in form: 'NAME:VALUE'.

**-P=PATH**,
**--path=PATH**,
**--scan=PATH** - Add path to scan path list.

**--autocomplete-install** - Install autocompletion to bash/clink(cmd).

**--autocomplete-uninstall** - Remove autocompletion from bash/clink(cmd).
