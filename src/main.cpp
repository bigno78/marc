#include <iostream>
#include <sstream>
#include <fstream>

#include <string>

struct {

};

void exit_error(const std::string& msg)
{
    std::cerr << msg << "\n";
    std::exit(1);
}

bool try_read_word(std::stringstream& sstream, std::string& word)
{
    sstream >> word;
    return static_cast<bool>(sstream);
}



int main(int argc, char** argv)
{
    if (argc < 2)
    {
        std::cerr << "Filename needs to be specified.\n";
        return 1;
    }

    std::string filename = argv[1];
    std::ifstream file(filename);

    std::string line;

    if (!std::getline(file, line))
    {
        std::cerr << "The header is missing.\n";
        return 1;
    }

    std::string word, declaration, format, type, symmetry;
    std::stringstream line_stream(line);

    line_stream >> word;

    if (!try_read_word(line_stream, word) || !str_eq(word, "%%MatrixMarket"))
    {
        exit_error("Missing '%%MatrixMarket' declaration.");
    }

    if (!try_read_word(line_stream, word))
    {
        exit_error("Missing object declaration.");
    }

    if (!str_eq(word, "matrix"))
    {
        exit_error("This MM file does not encode a matrix.");
    }

    if (!try_read_word(line_stream, word))
    {
        exit_error("Missing type declaration.");
    }

    if (!str_eq(word, "real") &&
        !str_eq(word, "integer") &&
        !str_eq(word, "complex"))
    {

    }

    line_stream >> declaration >> format >> type >> symmetry;

    if (!line_stream) 
    {
        std::cerr << "The header is incomplete.";
    }

}