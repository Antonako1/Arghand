#include <Arghand.h>
#include <iostream>

std::vector<std::string> CreateVector(int argc, char* argv[]) {
    std::vector<std::string> args;
    for (int i = 0; i < argc; ++i) {
        args.push_back(argv[i]);
    }
    return args;
}

void Arghand::SetParserOptions(uint64_t options) {
    this->parserOptions = options;
}

Arghand::Arghand() {
    this->SetParserOptions(DefaultParserOptions);
};
Arghand::~Arghand() = default;
Arghand::ParseResult Arghand::parse(int argc, char* argv[]) {
    this->args =  CreateVector(argc, argv);
    for (const auto& arg : this->args) {
        std::cout << "Argument: " << arg << std::endl;
    }
    return ParseResult::Success;
}