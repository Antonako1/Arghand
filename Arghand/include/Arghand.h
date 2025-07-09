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

#ifndef __cplusplus
#error "This header file requires a C++ compiler."
#endif // __cplusplus

#include <string>
#include <vector>
#include <cstdint>
#include <iostream>
#include <algorithm>
#include <stdexcept>

/// Converts enum class to uint64_t
template<typename E>
constexpr uint64_t QSTU64(E e) {
    return static_cast<uint64_t>(e);
}

/// Command option flags.
/// Combine using bitwise OR. See defines below for common flags.
enum class CmdOptionFlags : uint64_t {
    None =              0x00000000,     // No input required
    IsValueRequired =   0x00000002,     // Option requires a value
    IsList =            0x00000004,     // Option is a list (multiple values allowed, separated by a user defined character)
    IsHelpOption =      0x00000080,     // Option is a help option. Logs help information and returns from parser with a custom status.
    IsVersionOption =   0x00000100,     // Option is a version option. Logs version information and returns from parser with a custom status.
    IsRequired =       0x00000200,      // Option is required. Will help with help message auto generation and parsing.
};

// Convenience defines for common command option flags
#define HelpOptionDefault QSTU64(CmdOptionFlags::IsHelpOption)
#define VersionOptionDefault QSTU64(CmdOptionFlags::IsVersionOption)
#define NoInputDefault QSTU64(CmdOptionFlags::None)
#define InputDefault QSTU64(CmdOptionFlags::IsValueRequired) | QSTU64(CmdOptionFlags::IsRequired)
#define ListInputDefault QSTU64(CmdOptionFlags::IsList) | QSTU64(CmdOptionFlags::IsRequired)

// Command option structure
typedef struct _CMD_Option {
    std::string short_name; // Short name of the option (e.g., "-h")
    std::string long_name;  // Long name of the option (e.g., "--help")
    std::string name;       // Combined name for easier matching (e.g., "-h,--help")
    uint64_t options;       // Flags for the option (e.g., IsValueRequired, IsList, IsHelpOption)
    std::string DefaultValue; // Default value for the option if not provided
    std::string description;  // Description of the option for help messages
} CmdOption, *PCmdOption;

// Macro to define a command option with short and long names, flags, default value, and description
// Usage: CMD_OPTION("h", "help", HelpOptionDefault, "", "Display help
#define CMD_OPTION(short_name, long_name, flags, defaultValue, description) \
    { short_name, long_name, std::string(short_name) + "," + std::string(long_name), flags, defaultValue, description }

// Parsed option structure
typedef struct ParsedOption {
    std::string short_name;
    std::string long_name;
    std::vector<std::string> values;
} ParsedOption, *PParsedOption;


/// Parser options for the Arghand library.
/// These options control the behavior of the argument parser and the whole argument handler.
enum class ParserOptions : uint64_t {
    IgnoreCase = 0x00000001,    // Ignore case when matching options
    StyleUnix = 0x00000002,     // Use Unix-style options (e.g., --option) instead of Windows-style (/option)
    StyleWindows = 0x00000004,  // Use Windows-style options (e.g., /option) instead of Unix-style (--option)
    HelpDisplayAppName = 0x00000008,    // Display application name in help output
    HelpDisplayVersion = 0x00000010,    // Display version information in help output
    HelpDisplayLicense = 0x00000020,    // Display license information in help output
    HelpDisplayHeader = 0x00000040,     // Display header in help output
    HelpDisplayFooter = 0x00000080,     // Display footer in help output
    VersionDisplayFooter = 0x00000100,  // Display footer in version output
    HelpAutoGenerateArgumentUsageText = 0x00000200, // Automatically generate argument usage text in help output

    // Display all help information
    HelpDisplayAll = QSTU64(HelpDisplayLicense) | QSTU64(HelpDisplayHeader) | QSTU64(HelpDisplayFooter) | QSTU64(HelpAutoGenerateArgumentUsageText),

    // Default options for the parser
    DefaultOptions = QSTU64(StyleUnix) | QSTU64(HelpDisplayAll),
};

// Helper function to convert enum class to underlying type
template<typename Enum>
constexpr auto to_underlying(Enum e) noexcept {
    return static_cast<std::underlying_type_t<Enum>>(e);
}

// Overloaded operators for ParserOptions to allow bitwise operations
inline ParserOptions operator|(ParserOptions lhs, ParserOptions rhs) {
    return static_cast<ParserOptions>(to_underlying(lhs) | to_underlying(rhs));
}
inline ParserOptions operator&(ParserOptions lhs, ParserOptions rhs) {
    return static_cast<ParserOptions>(to_underlying(lhs) & to_underlying(rhs));
}
inline ParserOptions operator~(ParserOptions e) {
    return static_cast<ParserOptions>(~to_underlying(e));
}

/// Arghand class - Main class for handling command-line arguments
class Arghand {
public:
    /// Default constructor
    Arghand() {
        SetParserOptions(ParserOptions::DefaultOptions);
        SetSeparator(',');
        this->args.clear();
        this->cmdOptions.clear();
        this->parsedOptions.clear();
    }
    /// Destructor...
    ~Arghand() {}

    /// Enum class for parse results
    enum class ParseResult {
        Success,            // Parsing was successful
        Error,              // An error occurred during parsing
        MissingValue,       // A required value was missing for an option
        SuccessWithHelp,    // Parsing was successful and help was displayed
        SuccessWithVersion  // Parsing was successful and version was displayed
    };

    /// Parses command-line arguments
    /// @param argc Number of command-line arguments
    /// @param argv Array of command-line arguments
    /// @return ParseResult indicating the result of the parsing operation
    ParseResult parse(int argc, char* argv[]) {
        bool use_unix_style = (QSTU64(parserOptions) & QSTU64(ParserOptions::StyleUnix)) != 0;
        bool ignore_case = (QSTU64(parserOptions) & QSTU64(ParserOptions::IgnoreCase)) != 0;

        std::string prefix_lng = use_unix_style ? "--" : "/";
        std::string prefix_sht = use_unix_style ? "-" : "/";

        parsedOptions.clear();
        args = CreateVector(argc, argv);

        for (size_t i = 1; i < args.size(); ++i) {
            std::string arg = args[i];
            std::string match_arg = arg;

            if (ignore_case) {
                std::transform(match_arg.begin(), match_arg.end(), match_arg.begin(),
                            [](unsigned char c) { return std::tolower(c); });
            }

            bool matched = false;
            for (const auto& option : cmdOptions) {
                std::string long_name = option.long_name;
                std::string short_name = option.short_name;

                if (ignore_case) {
                    std::transform(long_name.begin(), long_name.end(), long_name.begin(),
                                [](unsigned char c) { return std::tolower(c); });
                    std::transform(short_name.begin(), short_name.end(), short_name.begin(),
                                [](unsigned char c) { return std::tolower(c); });
                }

                if (match_arg == prefix_lng + long_name || match_arg == prefix_sht + short_name) {
                    matched = true;

                    // Handle special options
                    if (option.options & QSTU64(CmdOptionFlags::IsHelpOption)) {
                        PrintHelp();
                        return ParseResult::SuccessWithHelp;
                    }

                    if (option.options & QSTU64(CmdOptionFlags::IsVersionOption)) {
                        PrintVersion(true);
                        return ParseResult::SuccessWithVersion;
                    }

                    ParsedOption parsed;
                    parsed.short_name = option.short_name;
                    parsed.long_name = option.long_name;

                    // Handle required value
                    if (option.options & QSTU64(CmdOptionFlags::IsValueRequired)) {
                        if (i + 1 < args.size()) {
                            std::string next = args[i + 1];
                            bool next_is_option = next.rfind(prefix_lng, 0) == 0 || next.rfind(prefix_sht, 0) == 0;

                            if (!next_is_option) {
                                parsed.values.push_back(next);
                                ++i; // Skip the value
                            }
                            else {
                                if (!option.DefaultValue.empty()) {
                                    parsed.values.push_back(option.DefaultValue);
                                }
                                else {
                                    std::cerr << "Missing value for required option: " << arg << std::endl;
                                    return ParseResult::MissingValue;
                                }
                            }
                        }
                        else {
                            if (!option.DefaultValue.empty()) {
                                parsed.values.push_back(option.DefaultValue);
                            }
                            else {
                                std::cerr << "Missing value for required option: " << arg << std::endl;
                                return ParseResult::MissingValue;
                            }
                        }
                    }
                    // Handle list
                    else if (option.options & QSTU64(CmdOptionFlags::IsList)) {
                        if (i + 1 < args.size()) {
                            std::string next = args[i + 1];
                            bool next_is_option = next.rfind(prefix_lng, 0) == 0 || next.rfind(prefix_sht, 0) == 0;

                            if (!next_is_option) {
                                parsed.values = ToList(next, ListSeparator);
                                ++i; // Skip the value
                            }
                            else {
                                if (!option.DefaultValue.empty()) {
                                    parsed.values = ToList(option.DefaultValue, ListSeparator);
                                }
                                else {
                                    std::cerr << "Missing list value for option: " << arg << std::endl;
                                    return ParseResult::MissingValue;
                                }
                            }
                        }
                        else {
                            if (!option.DefaultValue.empty()) {
                                parsed.values = ToList(option.DefaultValue, ListSeparator);
                            }
                            else {
                                std::cerr << "Missing list value for option: " << arg << std::endl;
                                return ParseResult::MissingValue;
                            }
                        }
                    }
                    // No value needed
                    else {
                        if (option.options & QSTU64(CmdOptionFlags::IsRequired)) {
                            if (!option.DefaultValue.empty()) {
                                parsed.values.push_back(option.DefaultValue);
                            }
                            else {
                                std::cerr << "Missing value for required option: " << arg << std::endl;
                                return ParseResult::MissingValue;
                            }
                        }
                        else {
                            if (!option.DefaultValue.empty())
                                parsed.values.push_back(option.DefaultValue);
                            else
                                parsed.values.push_back(""); // Fallback to empty string
                        }
                    }


                    parsedOptions.push_back(parsed);
                    break;
                }
            }

            // If unmatched and looks like an option
            if (!matched && (arg.rfind(prefix_lng, 0) == 0 || arg.rfind(prefix_sht, 0) == 0)) {
                std::cerr << "Unknown option: " << arg << std::endl;
                return ParseResult::Error;
            }
        }

        return ParseResult::Success;
    }

    /// Converts a string to a boolean value.
    /// @param value The string value to convert
    /// @return True if the string represents a true value, false otherwise.
    /// Recognized true values: "true", "1", "yes", "on" (case-insensitive)
    static bool ToBoolean(std::string& value) {
        std::string buf = value;
        std::transform(buf.begin(), buf.end(), buf.begin(),
                       [](unsigned char c) { return std::tolower(c); });
        return (buf == "true" || buf == "1" || buf == "yes" || buf == "on");
    }

    /// Converts a string to an integer value.
    /// @param value The string value to convert
    /// @return The integer value if conversion is successful, 0 otherwise.
    static int ToInteger(std::string& value) {
        try {
            return std::stoi(value);
        } catch (const std::invalid_argument&) {
            std::cerr << "Invalid integer value: " << value << std::endl;
            return 0;
        } catch (const std::out_of_range&) {
            std::cerr << "Integer value out of range: " << value << std::endl;
            return 0;
        }
    }

    /// Converts a string to a double value.
    /// @param value The string value to convert
    /// @return The double value if conversion is successful, 0.0 otherwise.
    static double ToDouble(std::string& value) {
        try {
            return std::stod(value);
        } catch (const std::invalid_argument&) {
            std::cerr << "Invalid double value: " << value << std::endl;
            return 0.0;
        } catch (const std::out_of_range&) {
            std::cerr << "Double value out of range: " << value << std::endl;
            return 0.0;
        }
    }

    /// Converts a string to a list of strings, separated by a specified character.
    /// @param value The string value to convert
    /// @param separator The character used to separate values in the string (default is '|')
    /// @return A vector of strings containing the separated values.
    /// If the string is empty, returns an empty vector.
    static std::vector<std::string> ToList(const std::string& value, char separator = '|') {
        std::vector<std::string> list;
        size_t start = 0;
        size_t end = value.find(separator);
        while (end != std::string::npos) {
            list.push_back(value.substr(start, end - start));
            start = end + 1;
            end = value.find(separator, start);
        }
        list.push_back(value.substr(start));
        return list;
    }

    /// Sets the separator character for list options.
    /// @param separator The character to use as a separator for list options (default is '|')
    /// This character is used to split list values in options that are marked as lists.
    void SetSeparator(char separator) { ListSeparator = separator; }
    /// Gets the current separator character for list options.
    /// @return The character used as a separator for list options.
    char GetSeparator() const { return ListSeparator; }

    /// Sets the command options for the argument handler.
    void SetCmdOptions(const std::vector<CmdOption>& options) {cmdOptions = options;}
    /// Gets the command options currently set in the argument handler.
    const std::vector<CmdOption>& GetCmdOptions() const {return cmdOptions;}

    /// Checks if an option with the given name exists in the parsed options.
    /// @param name The name of the option to check (can be short or long name)
    /// @return True if the option exists, false otherwise.
    bool operator[](const std::string& name) const {
        for (const auto& option : parsedOptions) {
            if (option.short_name == name || option.long_name == name) {
                return true;
            }
        }
        return false;
    }
    /// Gets the value of an option by its name.
    /// @param name The name of the option to get the value for (can be short or long name)
    /// @return The value of the option if it exists, or an empty string if not
    std::string GetValue(const std::string& name) const {
        for (const auto& option : parsedOptions) {
            if (option.short_name == name || option.long_name == name) {
                if (!option.values.empty()) {
                    return option.values[0];
                }
            }
        }
        
        for (const auto& option : cmdOptions) {
            if (option.short_name == name || option.long_name == name) {
                return option.DefaultValue;
            }
        }
        return "";
    }

    /// Gets the values of an option by its name.
    /// @param name The name of the option to get the values for (can be short or long name)
    /// @return A vector of strings containing the values of the option if it exists, or an empty vector if not
    std::vector<std::string> GetValues(const std::string& name) {
        for (auto& option : parsedOptions) {
            if (option.short_name == name || option.long_name == name) {
                return option.values;
            }
        }

        for (const auto& option : cmdOptions) {
            if (option.short_name == name || option.long_name == name) {
                if (option.options & QSTU64(CmdOptionFlags::IsList)) {
                    std::vector<std::string> list = ToList(option.DefaultValue, ListSeparator);
                    return list;
                } else {
                    static std::vector<std::string> single_value = { option.DefaultValue };
                    return single_value;
                }
            }
        }

        static std::vector<std::string> empty;
        return empty;
    }

// Macro to check if a specific parser option exists
#define ParserOptionsExist(x) ((QSTU64(parserOptions) & QSTU64(x)) != 0)

    /// @brief Prints the help information for the command-line options.
    /// This function generates and displays the help text based on the command options set in the handler
    void PrintHelp() const {
        bool use_unix_style = (QSTU64(parserOptions) & QSTU64(ParserOptions::StyleUnix)) != 0;
        std::string prefix_lng = use_unix_style ? "--" : "/";
        std::string prefix_sht = use_unix_style ? "-" : "/";
        if(ParserOptionsExist(ParserOptions::HelpDisplayHeader)) {
            std::cout << helpHeader << std::endl;
        }
        if(ParserOptionsExist(ParserOptions::HelpDisplayAppName)) {
            std::cout << applicationName << std::endl;
        }
        if(ParserOptionsExist(ParserOptions::HelpDisplayVersion)) {
            PrintVersion(false);
        }

        if(ParserOptionsExist(ParserOptions::HelpAutoGenerateArgumentUsageText)) {
            std::cout << "Usage: \n\t" << applicationName << " ";
            for(const auto& option : cmdOptions) {
                bool exists_long_name = !option.long_name.empty();
                bool exists_short_name = !option.short_name.empty();
                char br_o = ' ';
                char br_c = ' ';
                if (option.options & QSTU64(CmdOptionFlags::IsRequired)) {
                    br_o = '{';
                    br_c = '}';
                } else {
                    br_o = '[';
                    br_c = ']';
                }
                if(option.options & QSTU64(CmdOptionFlags::IsValueRequired) || option.options & QSTU64(CmdOptionFlags::IsList)) std::cout << br_o;
                std::cout << br_o << (exists_short_name ? prefix_sht + option.short_name : "")
                            << (exists_short_name && exists_long_name ? " | " : "")
                            << (exists_long_name ? prefix_lng + option.long_name : "") << br_c;
                if(option.options & QSTU64(CmdOptionFlags::IsValueRequired)) {
                    std::cout << " <value>";
                } else if(option.options & QSTU64(CmdOptionFlags::IsList)) {
                    std::cout << " <value1" << ListSeparator << "value2" << ">";
                }
                if(option.options & QSTU64(CmdOptionFlags::IsValueRequired) || option.options & QSTU64(CmdOptionFlags::IsList)) std::cout << br_c;
                std::cout <<" ";
            }
            std::cout << "\n\n";
        }
        for (const auto& option : cmdOptions) {
            bool exists_long_name = !option.long_name.empty();
            bool exists_short_name = !option.short_name.empty();

            std::cout << (exists_short_name ? prefix_sht + option.short_name : "    ")
                      << (exists_short_name && exists_long_name ? ", " : "")
                      << (exists_long_name ? prefix_lng + option.long_name : "\t")
                      << "\t\t\t"
                      << option.description
                      << std::endl;
        }
        if(ParserOptionsExist(ParserOptions::HelpDisplayFooter)) {
            std::cout << helpFooter << std::endl;
        }
        if(ParserOptionsExist(ParserOptions::HelpDisplayLicense))
            PrintLicense();
    }

    /// Prints the version information of the application.
    /// @param prt_lcs If true, prints the license information after the version.
    /// If the version is not set, it will print an error message.
    void PrintVersion(bool prt_lcs) const {
        if (version.empty()) {
            std::cerr << "Version information is not set." << std::endl;
        } else {
            // if no application name is set
            if (applicationName.empty()) {
                std::cout << "Version " << version << std::endl;
            }
            else {
                std::cout << applicationName << " version " << version << std::endl;
            }
        }
        
        if (prt_lcs && ParserOptionsExist(ParserOptions::VersionDisplayFooter)) {
            std::cout << versionFooter << std::endl;
        }
        if (prt_lcs) {
            PrintLicense();
        }

    }

    /// Prints the license information of the application.
    void PrintLicense() const {
        std::cout << license << std::endl;
    }

    /// Sets the help header text.
    void SetHelpHeader(const std::string& header) { helpHeader = header; }
    /// Gets the help header text.
    const std::string& GetHelpHeader() const { return helpHeader; }

    /// Sets the help footer text.
    void SetHelpFooter(const std::string& footer) { helpFooter = footer; }
    /// Gets the help footer text.
    const std::string& GetHelpFooter() const { return helpFooter; }

    /// Sets the license text.
    void SetLicense(const std::string& licenseText) { license = licenseText; }
    /// Gets the license text.
    const std::string& GetLicense() const { return license; }

    /// Converts version numbers to a string format.
    std::string VersionNumToString(int major, int minor, int patch) const { return std::to_string(major) + "." + std::to_string(minor) + "." + std::to_string(patch); }

    /// Sets the version information.
    void SetVersion(const std::string& versionInfo) { version = versionInfo; }
    /// Gets the version information.
    const std::string& GetVersion() const { return version; }
    /// Sets the parser options for the argument handler.
    void SetParserOptions(ParserOptions options) { parserOptions = options; }

    /// Gets the current parser options.
    void SetApplicationName(const std::string& name) { applicationName = name; }
    /// Gets the application name.
    const std::string& GetApplicationName() const { return applicationName; }

    /// Gets the current parser options.
    void SetVersionFooter(const std::string& footer) {
        versionFooter = footer;
    }
    /// Gets the version footer text.
    const std::string& GetVersionFooter() const {
        return versionFooter;
    }
    
private:
    /// Creates a vector of strings from command-line arguments.
    std::vector<std::string> CreateVector(int argc, char* argv[]) {
        std::vector<std::string> args;
        for (int i = 0; i < argc; ++i) {
            args.push_back(argv[i]);
        }
        return args;
    }

    char ListSeparator; // Character used to separate list values in options
    std::vector<std::string> args;  // Vector to store command-line arguments
    std::vector<CmdOption> cmdOptions;  // Vector to store command options
    std::vector<ParsedOption> parsedOptions;    // Vector to store parsed options

    std::string applicationName;    // Name of the application for help and version output
    std::string helpHeader;     // Header text for help output
    std::string helpFooter;     // Footer text for help output
    std::string license;        // License text for the application
    std::string version;        // Version information for the application
    std::string versionFooter;  // Footer text for version output

    ParserOptions parserOptions;    // Options for the argument parser, controlling its behavior
};


#endif // ARGHAND_H
