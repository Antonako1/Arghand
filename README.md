# Arghand - Argument handling single header library made with C++11

## Usage and documentation

### Example
```cpp
#include <Arghand.h>

int main(int argc, char* argv[]) {
    Arghand handler;
    // Create list of arguments
    std::vector<CmdOption> options = {
        CMD_OPTION("h", "help",     HelpOptionDefault,      "",           "Display help information"),
        CMD_OPTION("v", "version",  VersionOptionDefault,   "",           "Display version information"),
        CMD_OPTION("o", "output",   InputDefault,           "output.txt", "Specify output file"),
        CMD_OPTION("l", "list",     ListInputDefault,       "",           "Specify a list of values (comma-separated)"),
    };
    handler.SetCmdOptions(options); // Set options
    handler.SetParserOptions(DefaultParserOptions); // Set parsing styles
    handler.SetApplicationName("Test Application"); // Set the application name
    handler.SetSeparator(','); // Set list value separator
    handler.SetHelpHeader("This will be displayed at the top of the help output."); // Set help message header
    handler.SetHelpFooter("This will be displayed at the bottom of the help output."); // Set help message footer
    handler.SetLicense("This is the license text"); // Set license text
    handler.SetVersion(handler.VersionNumToString(1, 0, 0)); // Set version number
    Arghand::ParseResult res = handler.parse(argc, argv); // Parse arguments
    if(res == Arghand::ParseResult::Error){ // Check for errors
        std::cerr << "Error parsing command line arguments." << std::endl;
        return 1;
    }
    // Check if arguments were used
    if(handler["o"]){ 
        std::cout << "Output file specified: " << handler.GetValue("o") << std::endl;
    } else if(handler["list"]){
        std::vector<std::string> listValues = handler.GetValues("l");
        std::cout << "List values specified: ";
        for (const auto& value : listValues) {
            std::cout << value << ", ";
        }
        std::cout << std::endl;
    }
    return 0;
}

```

### Documentation

#### Argument syntax

```

```