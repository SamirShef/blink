#pragma once
#include <cstdint>
#include <string>

enum SubsystemType {
    LEXER,
    PARSER,
    SEMANTIC,
    CODEGEN,
};

void throw_error(std::string file_name, bool& file_name_in_error_printed, SubsystemType subsystem_type, std::string message, int line, std::uint8_t error_code = 1);