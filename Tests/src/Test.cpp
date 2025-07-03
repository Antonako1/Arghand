#include <Arghand.h>

int main(int argc, char* argv[]) {
    Arghand handler;
    std::vector<CmdOption> options = {
        CMD_OPTION("", "help",     HelpOptionDefault,      "",           "Display help information"),
        CMD_OPTION("v", "",  VersionOptionDefault,   "",           "Display version information"),
        CMD_OPTION("o", "output",   InputDefault,           "output.txt", "Specify output file"),
        CMD_OPTION("l", "list",     ListInputDefault,       "a,b",           "Specify a list of values (comma-separated)"),
    };
    handler.SetCmdOptions(options);
    handler.SetSeparator(',');
    handler.SetParserOptions(
        (ParserOptions::DefaultOptions |
        ParserOptions::VersionDisplayFooter ) & 
        ~(ParserOptions::HelpDisplayAppName | 
        ParserOptions::HelpDisplayVersion) 
    );

    handler.SetApplicationName("Arghand");
    handler.SetHelpHeader("Usage: \n\tArghand test application [options]\n");
    handler.SetHelpFooter("\nMaintained at https://github.com/Antonako1/Arghand.");
    handler.SetLicense("Licensed under the BSD-2-Clause License.");
    handler.SetVersion(handler.VersionNumToString(1, 0, 0));
    handler.SetVersionFooter("Maintained at https://github.com/Antonako1/Arghand.");
    
    Arghand::ParseResult res = handler.parse(argc, argv);
    if(res == Arghand::ParseResult::Error){
        std::cerr << "Error parsing command line arguments." << std::endl;
        return 1;
    }
    
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