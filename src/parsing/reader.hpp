#pragma once

#include <string>
#include <istream>
#include <array>
#include <fstream>

#include <cstdio>

#include <sys/stat.h>


template<size_t N>
struct FileReaderC {

    FileReaderC(const std::string& filepath) : 
        file(std::fopen(filepath.c_str(), "r"))
    {
    }

    bool error() const {
        return file == nullptr || std::ferror(file);
    }

    bool eof() const {
        return i >= size && std::feof(file);
    }

    void seek(int pos) {
        std::fseek(file, pos, SEEK_SET);
    }

    char get() {
        if (i < size) {
            return buffer[i++];
        }

        size = std::fread(buffer.data(), 1, N, file);
        i = 0;

        return i < size ? buffer[i++] : EOF;
    }

    ~FileReaderC() {
        if (file) {
            std::fclose(file);
        }
    }

private:
    FILE* file;

    std::array<char, N> buffer;
    std::size_t size = 0;
    std::size_t i = 0;
};


template<size_t N>
struct FileReaderStream {

    FileReaderStream(const std::string& filepath) : 
        file(filepath)
    {
    }

    bool error() const {
        return (bool)file;
    }

    bool eof() const {
        return file.eof();
    }

    void seek(int pos) {
        file.seekg(pos);
    }

    char get() {
        if (i < size) {
            return buffer[i++];
        }

        file.read(buffer.data(), N);
        size = file.gcount();
        i = 0;

        return i < size ? buffer[i++] : EOF;
    }

private:
    std::ifstream file;

    std::array<char, N> buffer;
    std::size_t size = 0;
    std::size_t i = 0;
};

template<size_t N>
struct FileReaderLine {

    FileReaderLine(const std::string& filepath) : 
        file(filepath)
    {
    }

    FileReaderLine(std::ifstream fstream) : 
        file(std::move(fstream))
    {
    }

    bool error() const {
        return (bool)file;
    }

    bool eof() const {
        return file.eof();
    }

    void seek(int pos) {
        file.seekg(pos);
    }

    bool getline(std::string& line) {
        if (i < size) {
            line = std::move(lines[i++]);
            return true;
        }

        size = 0;
        for (int j = 0; j < N; ++j) {
            if (!std::getline(file, lines[j])) {
                break;
            }
            size++;
        }
        i = 0;

        if (size > 0) {
            line = std::move(lines[i++]);
            return true;
        }

        return false;
    }

private:
    std::ifstream file;

    std::array<std::string, N> lines;
    size_t size = 0;
    size_t i = 0;
};
