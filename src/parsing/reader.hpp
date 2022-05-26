#pragma once

#include <string>
#include <istream>
#include <array>
#include <fstream>

#include <cstdio>

#include <thread>
#include <mutex>
#include <condition_variable>


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


template<size_t N>
struct DoubleBufferedReader {

    DoubleBufferedReader(std::ifstream fstream) : 
        file(std::move(fstream))
    {
        loading_thread = std::thread([&] () { do_loading(); });
    }

    bool getline(std::string& line) {
        if (active_buffer->size() > 0) {
            //std::cout << "line present\n";
            line = std::move(active_buffer->back());
            active_buffer->pop_back();
            return true;
        }

        if (eof_reached) {
            return false;
        }

        {
            std::unique_lock lock(mutex);

            //std::cout << "waiting for lines\n";
            loading_done.wait(lock, [&] () { return buffer_ready; });
            std::swap(active_buffer, waiting_buffer);
            buffer_ready = false;
            can_load = true;
            //std::cout << active_buffer << " " << waiting_buffer << "\n";
        }

        processing_done.notify_one();

        if (active_buffer->size() > 0) {
            line = std::move(active_buffer->back());
            active_buffer->pop_back();
            return true;
        }
        
        eof_reached = true;

        return false;
    }

    ~DoubleBufferedReader() {
        loading_thread.join();
    }

private:
    void do_loading() {
        while (true) {
            //std::cout << "loading lines\n";
            waiting_buffer->clear();
            for (int j = 0; j < N; ++j) {
                std::string line;
                if (!std::getline(file, line)) {
                    //std::cout << "bad file\n";
                    break;
                }
                waiting_buffer->push_back(std::move(line));
            }
            //std::cout << "loading done " << waiting_buffer->size() << "\n";

            bool should_quit = waiting_buffer->empty();

            if (should_quit) {
                //std::cout << "gonna quit\n";
            }

            {
                std::unique_lock lock(mutex);
                if (buffer_ready) {
                    //std::cout << "IT WAS READY!!!!!!!\n";
                }
                buffer_ready = true;
            }

            loading_done.notify_one();

            if (should_quit) {
                return;
            }

            {
                std::unique_lock lock(mutex);
                processing_done.wait(lock, [&] () { return can_load; });
                can_load = false;
            }
        }
    }


private:
    std::ifstream file;

    std::vector<std::string> buffer1;
    std::vector<std::string> buffer2;

    std::vector<std::string>* active_buffer = &buffer1;
    std::vector<std::string>* waiting_buffer = &buffer2;

    bool eof_reached = false;

    std::mutex mutex;
    bool buffer_ready = false;
    bool can_load = false;
    std::condition_variable processing_done;
    std::condition_variable loading_done;

    std::thread loading_thread;
};
