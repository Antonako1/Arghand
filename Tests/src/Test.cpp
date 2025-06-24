#include <Arghand.h>
#include <iostream>

int main(int argc, char* argv[]) {
    Arghand handler;
    std::vector<CmdOption> options = {
        CMD_OPTION("h", "help",     NoInputDefault, "",           "Display help information"),
        CMD_OPTION("v", "version",  NoInputDefault, "",           "Display version information"),
        CMD_OPTION("o", "output",   InputDefault,   "output.txt", "Specify output file"),
    };
    handler.SetCmdOptions(options);
    handler.SetParserOptions(handler.DefaultParserOptions);
    handler.SetHelpHeader("This will be displayed at the top of the help output.");
    handler.SetHelpFooter("This will be displayed at the bottom of the help output.");
    handler.SetLicense("This is the license text that will be displayed in the help output.");
    handler.SetVersion("Version 1.0.0");
    handler.parse(argc, argv);
    if(handler["help"]){
        handler.PrintHelp();
    }
    return 0;
}