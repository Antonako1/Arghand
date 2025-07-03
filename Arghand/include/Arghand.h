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
enum class CmdOptionFlags : uint64_t {
    None =              0x00000000,
    IsValueRequired =   0x00000002,
    IsList =            0x00000004,
    IsHelpOption =      0x00000080,
    IsVersionOption =   0x00000100,
};

#define HelpOptionDefault QSTU64(CmdOptionFlags::IsHelpOption)
#define VersionOptionDefault QSTU64(CmdOptionFlags::IsVersionOption)
#define NoInputDefault QSTU64(CmdOptionFlags::None)
#define InputDefault QSTU64(CmdOptionFlags::IsValueRequired)
#define ListInputDefault QSTU64(CmdOptionFlags::IsList)

typedef struct _CMD_Option {
    std::string short_name;
    std::string long_name;
    std::string name;
    uint64_t options;
    std::string DefaultValue;
    std::string description;
} CmdOption, *PCmdOption;

#define CMD_OPTION(short_name, long_name, flags, defaultValue, description) \
    { short_name, long_name, std::string(short_name) + "," + std::string(long_name), flags, defaultValue, description }

typedef struct ParsedOption {
    std::string short_name;
    std::string long_name;
    std::vector<std::string> values;
} ParsedOption, *PParsedOption;



enum class ParserOptions : uint64_t {
    IgnoreCase = 0x00000001,
    StyleUnix = 0x00000002,
    StyleWindows = 0x00000004,
    HelpDisplayAppName = 0x00000008,
    HelpDisplayVersion = 0x00000010,
    HelpDisplayLicense = 0x00000020,
    HelpDisplayHeader = 0x00000040,
    HelpDisplayFooter = 0x00000080,
    HelpDisplayAll = QSTU64(HelpDisplayAppName) | QSTU64(HelpDisplayVersion) | QSTU64(HelpDisplayLicense) | QSTU64(HelpDisplayHeader) | QSTU64(HelpDisplayFooter),
    VersionDisplayFooter = 0x00000100,
    DefaultOptions = QSTU64(StyleUnix) | QSTU64(HelpDisplayAll)
};
// Define bitwise OR operator for ParserOptions if not already defined
template<typename Enum>
constexpr auto to_underlying(Enum e) noexcept {
    return static_cast<std::underlying_type_t<Enum>>(e);
}

inline ParserOptions operator|(ParserOptions lhs, ParserOptions rhs) {
    return static_cast<ParserOptions>(to_underlying(lhs) | to_underlying(rhs));
}
inline ParserOptions operator&(ParserOptions lhs, ParserOptions rhs) {
    return static_cast<ParserOptions>(to_underlying(lhs) & to_underlying(rhs));
}
inline ParserOptions operator~(ParserOptions e) {
    return static_cast<ParserOptions>(~to_underlying(e));
}


class Arghand {
public:
    Arghand() {
        SetParserOptions(ParserOptions::DefaultOptions);
        SetSeparator(',');
        this->args.clear();
        this->cmdOptions.clear();
        this->parsedOptions.clear();
    }

    ~Arghand() {}

    enum class ParseResult {
        Success,
        Error,
        MissingValue,
        SuccessWithHelp,
        SuccessWithVersion
    };

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
                            parsed.values.push_back(args[i + 1]);
                            ++i; // Skip value in next loop
                        } else {
                            if (option.DefaultValue.empty()) {
                                std::cerr << "Missing value for option: " << arg << std::endl;
                                return ParseResult::MissingValue;
                            }
                            parsed.values.push_back(option.DefaultValue);
                        }
                    }
                    // Handle list
                    else if (option.options & QSTU64(CmdOptionFlags::IsList)) {
                        if (i + 1 < args.size()) {
                            parsed.values = ToList(args[i + 1], ListSeparator);
                            ++i; // Skip value in next loop
                        } else {
                            if (option.DefaultValue.empty()) {
                                std::cerr << "Missing list value for option: " << arg << std::endl;
                                return ParseResult::MissingValue;
                            }
                            parsed.values = ToList(option.DefaultValue, ListSeparator);
                        }
                    }
                    // No value needed
                    else {
                        parsed.values.push_back(option.DefaultValue);
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


    static bool ToBoolean(std::string& value) {
        std::string buf = value;
        std::transform(buf.begin(), buf.end(), buf.begin(),
                       [](unsigned char c) { return std::tolower(c); });
        return (buf == "true" || buf == "1" || buf == "yes" || buf == "on");
    }

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

    void SetSeparator(char separator) { ListSeparator = separator; }
    char GetSeparator() const { return ListSeparator; }

    void SetCmdOptions(const std::vector<CmdOption>& options) {cmdOptions = options;}

    const std::vector<CmdOption>& GetCmdOptions() const {return cmdOptions;}

    bool operator[](const std::string& name) const {
        for (const auto& option : parsedOptions) {
            if (option.short_name == name || option.long_name == name) {
                return true;
            }
        }
        return false;
    }

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
#define ParserOptionsExist(x) ((QSTU64(parserOptions) & QSTU64(x)) != 0)

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
    void PrintLicense() const {
        std::cout << license << std::endl;
    }

    void SetHelpHeader(const std::string& header) { helpHeader = header; }
    const std::string& GetHelpHeader() const { return helpHeader; }

    void SetHelpFooter(const std::string& footer) { helpFooter = footer; }
    const std::string& GetHelpFooter() const { return helpFooter; }

    void SetLicense(const std::string& licenseText) { license = licenseText; }
    const std::string& GetLicense() const { return license; }

    std::string VersionNumToString(int major, int minor, int patch) const { return std::to_string(major) + "." + std::to_string(minor) + "." + std::to_string(patch); }

    void SetVersion(const std::string& versionInfo) { version = versionInfo; }
    const std::string& GetVersion() const { return version; }

    void SetParserOptions(ParserOptions options) { parserOptions = options; }

    void SetApplicationName(const std::string& name) { applicationName = name; }
    const std::string& GetApplicationName() const { return applicationName; }

    void SetVersionFooter(const std::string& footer) {
        versionFooter = footer;
    }
    const std::string& GetVersionFooter() const {
        return versionFooter;
    }
    
private:
    std::vector<std::string> CreateVector(int argc, char* argv[]) {
        std::vector<std::string> args;
        for (int i = 0; i < argc; ++i) {
            args.push_back(argv[i]);
        }
        return args;
    }

    char ListSeparator;
    std::vector<std::string> args;
    std::vector<CmdOption> cmdOptions;
    std::vector<ParsedOption> parsedOptions;

    std::string applicationName;
    std::string helpHeader;
    std::string helpFooter;
    std::string license;
    std::string version;
    std::string versionFooter;

    ParserOptions parserOptions;
};


#endif // ARGHAND_H
