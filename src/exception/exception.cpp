#include "../../include/exception/exception.hpp"
#include <iostream>

std::string subsystem_type_to_string(SubsystemType type) {
    switch (type) {
        case LEXER:
            return "lexer";
        case PARSER:
            return "parser";
        case SEMANTIC:
            return "semantic";
        case CODEGEN:
            return "codegen";
    }
}

void throw_error(std::string file_name, bool& file_name_in_error_printed, SubsystemType subsystem_type, std::string message, int line, std::uint8_t error_code) {
    if (!file_name_in_error_printed) {
        std::cerr << "In file: " << file_name << ':' << line << ':' << '\n';
    }

    std::cerr << subsystem_type_to_string(subsystem_type) << ": " << message;
    exit(error_code);
}