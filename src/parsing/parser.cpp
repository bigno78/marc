#include "parser.hpp"
#include "../utils.hpp"

#include <vector>
#include <sstream>
#include <iostream>
#include <array>


struct Token {
    std::string word;
    size_t start_col;
};


struct LineTokenizer {

    LineTokenizer(const std::string& line) : line_stream(line) {
        line_stream >> std::ws;
    }

    size_t current_pos() {
        return line_stream.tellg();
    }

    bool has_next() const {
        return !line_stream.eof();
    }

    Token next() {
        size_t start = current_pos();

        std::string word;
        line_stream >> word >> std::ws;

        return { word, start };
    }

private:
    std::stringstream line_stream;
};


template<typename Iterable>
size_t index_of(const std::string& str, const Iterable& options) {
    size_t i = 0;
    for (const auto& opt : options) {
        if (case_insensitive_eq(str, opt)) {
            return i;
        }
        ++i;
    }
    return i;
}


Status validate_magic_text(LineTokenizer& tokenizer) {
    if (!tokenizer.has_next()) {
        return Status::error("Missing %%MatrixMarket declaration.", 0, 0);
    }

    Token token = tokenizer.next();

    if (token.start_col != 0 || token.word != "%%MatrixMarket") {
        return Status::error("Missing %%MatrixMarket declaration.", 0, 0);
    }

    if (!tokenizer.has_next()) {
        return Status::error("Missing 'matrix' declaration.", 0, tokenizer.current_pos());
    }

    token = tokenizer.next();

    if (!case_insensitive_eq(token.word, "matrix")) {
        std::stringstream error_stream;
        error_stream << "Ivalid object declaration. Expected 'matrix' but '" << token.word << "' found.";
        return Status::error(error_stream.str(), 0, token.start_col);
    }

    return Status::success();
}


Status parse_format(LineTokenizer& tokenizer, Header header) {
    if (!tokenizer.has_next()) {
        return Status::error("Missing format declaration.", 0, tokenizer.current_pos());
    }

    std::array<const char*, 2> valid_formats = { "coordinate", "array" };
    std::array<Format, 2> enum_values = { Format::coordinate, Format::array };

    Token token = tokenizer.next();

    size_t i = index_of(token.word, valid_formats);

    if (i == valid_formats.size()) {
        std::stringstream error_stream;
        error_stream << "Ivalid format '" << token.word << "'.";
        return Status::error(error_stream.str(), 0, token.start_col);
    }

    header.format = enum_values[i];

    return Status::success();
}


Status parse_type(LineTokenizer& tokenizer, Header header) {
    if (!tokenizer.has_next()) {
        return Status::error("Missing type declaration.", 0, tokenizer.current_pos());
    }

    std::array<const char*, 4> valid_types = { "integer", "real", "complex", "pattern" };
    std::array<Type, 4> enum_values = { Type::integer, Type::real, Type::complex, Type::pattern };

    Token token = tokenizer.next();

    size_t i = index_of(token.word, valid_types);

    if (i == valid_types.size()) {
        std::stringstream error_stream;
        error_stream << "Ivalid type '" << token.word << "'.";
        return Status::error(error_stream.str(), 0, token.start_col);
    }

    header.type = enum_values[i];

    return Status::success();
}


Status parse_symmetry(LineTokenizer& tokenizer, Header& header) {
    if (!tokenizer.has_next()) {
        return Status::error("Missing symmetry declaration.", 0, tokenizer.current_pos());
    }

    std::array<const char*, 4> valid_options = { "general", "symmetric", "skew-symmetric", "hermitian" };
    std::array<Symmetry, 4> enum_values = { 
        Symmetry::general, Symmetry::symmetric, Symmetry::skew_symmetric, Symmetry::hermitian 
    };

    Token token = tokenizer.next();

    size_t i = index_of(token.word, valid_options);

    if (i == valid_options.size()) {
        std::stringstream error_stream;
        error_stream << "Ivalid symmetry '" << token.word << "'.";
        return Status::error(error_stream.str(), 0, token.start_col);
    }

    header.symmetry = enum_values[i];

    return Status::success();
}


Status parse_header_line(std::ifstream& in, Header& header) {
    std::string header_line;

    if (!std::getline(in, header_line)) {
        return Status::error("Missing header.", 0, 0);
    }

    LineTokenizer tokenizer(header_line);

    Status status = validate_magic_text(tokenizer);
    if (!status) {
        return status;
    }

    status = parse_format(tokenizer, header);
    if (!status) {
        return status;
    }

    status = parse_type(tokenizer, header);
    if (!status) {
        return status;
    }

    status = parse_symmetry(tokenizer, header);
    if (!status) {
        return status;
    }

    if (tokenizer.has_next()) {
        return Status::error("Unexpected characters", 0, tokenizer.current_pos());
    }

    return Status::success();
}


Status parse_dimensions(std::ifstream& in, Header& header) {
    size_t line_no = 1;
    std::string line;

    while (std::getline(in, line)) {
        if (line.size() > 0 && line[0] == '%') {
            line_no++;
            continue;
        }

        std::stringstream line_stream(line);
        line_stream >> std::ws;

        if (!line_stream.eof()) {
            size_t col = line_stream.tellg();
            line_stream >> header.rows >> header.cols >> header.entries;
            
            if (!line_stream) {
                return Status::error("Invalid matrix dimensions", line_no, col);
            }

            return Status::success();
        }

        line_no++;
    }

    return Status::error("Missing matrix dimensions", line_no, 0);
}

Status parse_header(std::ifstream& in, Header& header)
{
    auto status = parse_header_line(in, header);
    if (!status) {
        return status;
    }

    return parse_dimensions(in, header);
}
