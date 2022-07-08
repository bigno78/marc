#include "header.hpp"
#include "../utils.hpp"

#include <vector>
#include <sstream>
#include <iostream>
#include <array>


#define CHECK_STATUS(expr)  \
    do {                    \
        auto status = expr; \
        if (!status) {      \
            return status;  \
        }                   \
    } while(false)


struct Token {
    std::string word;
    size_t start_col;
};


struct LineTokenizer {

    LineTokenizer(const std::string& line) : line_stream(line), line_length(line.size()) {
        line_stream >> std::ws;
    }

    size_t current_pos() {
        return has_next() ? static_cast<size_t>(line_stream.tellg()) : line_length;
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
    size_t line_length;
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
        return Status::error("Missing %%MatrixMarket declaration.", 1, 1);
    }

    Token token = tokenizer.next();

    if (token.start_col != 0 || token.word != "%%MatrixMarket") {
        return Status::error("Missing %%MatrixMarket declaration.", 1, 1);
    }

    if (!tokenizer.has_next()) {
        return Status::error("Missing 'matrix' declaration.", 1, tokenizer.current_pos() + 1);
    }

    token = tokenizer.next();

    if (!case_insensitive_eq(token.word, "matrix")) {
        std::string error_msg = "Invalid object declaration. Expected 'matrix' but '" + token.word + "' found.";
        return Status::error(error_msg, 1, token.start_col + 1);
    }

    return Status::success();
}


template<typename Iterable>
std::string stringify(const Iterable& options) {
    std::stringstream result_stream;
    const char* sep = "";
    for (const auto& item : options) {
        result_stream << sep << "'" << item << "'";
        sep = ", ";
    }
    return result_stream.str();
}


template<typename Iterable, typename Func>
Status parse_required_param(LineTokenizer& tokenizer, const Iterable& options, Func on_success) {
    if (!tokenizer.has_next()) {
        std::string error_msg = "Missing header declaration. Expected one of: " + stringify(options) + "."; 
        return Status::error(error_msg, 1, tokenizer.current_pos() + 1);
    }

    Token token = tokenizer.next();
    size_t i = index_of(token.word, options);

    if (i == options.size()) {
        std::string error_msg = "Unexpected header declaration '" + token.word 
                              + "'. Expected one of: " + stringify(options) + ".";
        return Status::error(error_msg, 1, token.start_col + 1);
    }

    on_success(i);

    return Status::success();
}


Status parse_format(LineTokenizer& tokenizer, Header& header) {
    std::array<std::string, 2> valid_formats = { "coordinate", "array" };
    std::array<Format, 2> enum_values = { Format::coordinate, Format::array };

    return parse_required_param(tokenizer, valid_formats, [&] (size_t i) { header.format = enum_values[i]; });
}


Status parse_type(LineTokenizer& tokenizer, Header& header) {
    std::array<const char*, 4> valid_types = { "integer", "real", "complex", "pattern" };
    std::array<Type, 4> enum_values = { Type::integer, Type::real, Type::complex, Type::pattern };

    return parse_required_param(tokenizer, valid_types, [&] (size_t i) { header.type = enum_values[i]; });
}


Status parse_symmetry(LineTokenizer& tokenizer, Header& header) {
    std::array<const char*, 4> valid_symmetries = { "general", "symmetric", "skew-symmetric", "hermitian" };
    std::array<Symmetry, 4> enum_values = { 
        Symmetry::general, Symmetry::symmetric, Symmetry::skew_symmetric, Symmetry::hermitian 
    };

    return parse_required_param(tokenizer, valid_symmetries, [&] (size_t i) { header.symmetry = enum_values[i]; });
}


Status parse_header_line(std::istream& in, Header& header) {
    std::string header_line;

    if (!std::getline(in, header_line)) {
        return Status::error("Missing header.", 1, 1);
    }

    LineTokenizer tokenizer(header_line);

    CHECK_STATUS( validate_magic_text(tokenizer) );
    CHECK_STATUS( parse_format(tokenizer, header) );
    CHECK_STATUS( parse_type(tokenizer, header) );
    CHECK_STATUS( parse_symmetry(tokenizer, header) );

    if (tokenizer.has_next()) {
        return Status::error("Unexpected characters on the header line.", 1, tokenizer.current_pos() + 1);
    }

    return Status::success();
}


Status parse_dimensions(std::istream& in, Header& header) {
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
                return Status::error("Invalid matrix dimensions.", line_no + 1, col + 1);
            }

            header.size = line_no;

            return Status::success();
        }

        line_no++;
    }

    return Status::error("Missing matrix dimensions.", line_no, 0);
}


Status parse_header(std::istream& in, Header& header)
{
    CHECK_STATUS( parse_header_line(in, header) );
    return parse_dimensions(in, header);
}
