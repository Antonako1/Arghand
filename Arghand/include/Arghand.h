/*+++
Arghand.h - Header file for the Arghand Argument Handler Library

This library provides a simple and efficient way to handle command-line arguments in C++ applications.
Documentation and usage instructions for the Arghand library can be found in the README.md file.

Licensed under the BSD 2-Clause License. See LICENSE file in the project/package root for details.

Author: Antonako1
---*/
#pragma once
#ifndef ARGHAND_H
#define ARGHAND_H
#ifdef _WIN32
#   ifdef ARGHAND_EXPORTS
#       define ARGHAND_API __declspec(dllexport)
#   else
#       define ARGHAND_API __declspec(dllimport)
#   endif
#else
#   define ARGHAND_API
#endif
#include <string>
#include <vector>
#include <cstdint>
#include <iostream>

#define QSTU64(x) static_cast<uint64_t>(x)

enum class CmdOptionFlags : uint64_t {
    None =              0x00000000,     // No special attributes
    IsValueRequired =   0x00000002,     // Option requires a value
    IsList =            0x00000004,     // Option can have multiple values
    IsHidden =          0x00000008,     // Option is hidden from help output
    IsDeprecated =      0x00000010,     // Option is deprecated
    ExpectPath =        0x00000020,     // Option expects a file path
    IsOptional =        0x00000040,     // Option is optional
    IsHelpOption =      0x00000080,     // Option is a help option. Parsing will be stopped and help will be printed
    IsVersionOption =   0x00000100,     // Option is a version. Parsing will be stopped and version will be printed
};

#define NoInputDefault QSTU64(CmdOptionFlags::None) // Default value for options that do not require input
#define InputDefault QSTU64(CmdOptionFlags::IsValueRequired) // Default value for options that require a value
#define ListInputDefault QSTU64(CmdOptionFlags::IsList) // Default value for options that can have multiple values

typedef struct _CMD_Option { // Represents a command-line option
    std::string short_name; // Short name of the option (e.g., '-o')
    std::string long_name; // Long name of the option (e.g., '--option')
    std::string name; // Combined name for the option (e.g., '-o,--option')
    uint64_t options; // Bitmask for option flags
    std::string DefaultValue; // Default value for the option, if applicable
    std::string description; // Description of the option for help output
} CmdOption, *PCmdOption;


#define CMD_OPTION(short_name, long_name, flags, defaultValue, description) \
    { \
        short_name, \
        long_name, \
        std::string(short_name) + "," + std::string(long_name), \
        flags, \
        defaultValue, \
        description \
    }

typedef struct ParsedOption { // Represents a parsed command-line option
    std::string name;
    std::vector<std::string> values;
} ParsedOption, *PParsedOption;

/*
allow args as follow:
Style UNIX=-, WINDOWS=/:

prefixes: -, --, /
Short args: -o
Long args: --option, /option
Values: :value, =value,  value
List values seperated by 'X' (default is '|')

Example:

-h,     --help
        --set-path=path/to/somewhere
        --wildcards=*.txt,*.cpp
*/  
class ARGHAND_API Arghand {
public:
    static enum class ParserOptions : uint64_t {
        IgnoreCase = 0x00000001,        // Ignore case when matching options
        StyleUnix = 0x00000002,         // Use Unix-style prefixes
        StyleWindows = 0x00000004,      // Use Windows-style prefixes
    };
    static constexpr inline uint64_t DefaultParserOptions = 
                                        QSTU64(ParserOptions::IgnoreCase) | 
                                        QSTU64(ParserOptions::StyleUnix);


    void SetParserOptions(uint64_t options);

    enum class ParseResult {
        Success,
        Error,
        HelpRequested
    };
    Arghand();
    ~Arghand();
    ParseResult parse(int argc, char* argv[]);

    // Turning arguments into different types
    static bool ToBoolean(std::string& value);
    static int ToInteger(std::string& value);
    static double ToDouble(std::string& value);
    static std::vector<std::string> ToList(std::string& value, char separator = '|');
    
    void SetSeparator(char separator) {ListSeparator = separator;}
    char GetSeparator() const { return ListSeparator; }

    void SetCmdOptions(const std::vector<CmdOption>& options) {
        copy(cmdOptions.begin(), cmdOptions.end(), std::back_inserter(options));
    }
    const std::vector<CmdOption>& GetCmdOptions() const {
        return cmdOptions;
    }

    bool operator[](const std::string& name) const; // Check if an option was parsed

    void PrintHelp() const;

    void SetHelpHeader(const std::string& header) {helpHeader = header;}
    const std::string& GetHelpHeader() const {return helpHeader;}
    void SetHelpFooter(const std::string& footer) {helpFooter = footer;}
    const std::string& GetHelpFooter() const {return helpFooter;}
    void SetLicense(const std::string& licenseText) {license = licenseText;}
    const std::string& GetLicense() const {return license;}
    void SetVersion(const std::string& versionInfo) {version = versionInfo;}
    const std::string& GetVersion() const {return version;}

private:
    char ListSeparator = '|';
    std::vector<std::string> args;
    std::vector<CmdOption> cmdOptions;
    std::vector<ParsedOption> parsedOptions;

    std::string helpHeader; // Header for help output
    std::string helpFooter; // Footer for help output
    std::string license; // License displayed in the help output
    std::string version; // Version information displayed in the help output
    
    uint64_t parserOptions;
};

#endif // ARGHAND_H