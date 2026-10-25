#pragma once

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <format>
#include <print>
#include <iostream>

struct ProgamOptions {
    std::string_view program_path;
    std::string_view pattern;
    std::string_view file_name;
    bool valid{false};
    std::string err;

    ProgamOptions(int argc, char** argv) {
        program_path = argv[0];

        if (argc != 3) {
            err = std::format("Expected 2 arguments (<pattern> <file_name>), got: {}", argc - 1);
            return;
        }

        pattern = argv[1];
        file_name = argv[2];

        if (!std::filesystem::exists(file_name)) {
            err = std::format("File doesn't exist: {}", file_name);
            return;
        }

        valid = true;
    }
};


[[nodiscard]] inline ProgamOptions parse_args(int argc, char** argv)  {
    ProgamOptions opts(argc, argv);
    if (not opts.valid) {
        std::println(std::cerr, "[error]: {}", opts.err);
        std::exit(1);
    }

    return opts;
}

[[nodiscard]] inline std::string get_text(ProgamOptions opts) {
    std::ifstream in(opts.file_name.data());

    if (not in) {
        throw std::runtime_error(std::format("Couldn't open file: {}", opts.file_name));
    }

    std::stringstream ss;
    ss << in.rdbuf();
    return ss.str();
}
