#include <Arghand.h>

int main(int argc, char* argv[]) {
    Arghand handler;
    std::vector<CmdOption> options = {
        CMD_OPTION("h", "help",     HelpOptionDefault,      "",           "Display help information"),
        CMD_OPTION("v", "version",  VersionOptionDefault,   "",           "Display version information"),
        CMD_OPTION("o", "output",   InputDefault,           "output.txt", "Specify output file"),
        CMD_OPTION("l", "list",     ListInputDefault,       "",           "Specify a list of values (comma-separated)"),
    };
    handler.SetCmdOptions(options);
    handler.SetParserOptions(DefaultParserOptions);
    handler.SetApplicationName("Test Application");
    handler.SetSeparator(',');
    handler.SetHelpHeader("This will be displayed at the top of the help output.");
    handler.SetHelpFooter("This will be displayed at the bottom of the help output.");
    handler.SetLicense("This is the license text");
    handler.SetVersion(handler.VersionNumToString(1, 0, 0));
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