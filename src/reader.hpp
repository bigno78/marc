#pragma once

#include <string>
#include <istream>

struct StreamReader {

    StreamReader(std::istream& in) : in(in) { };

    template<typename DataCollector>
    void operator()(DataCollector& collecter) {

    } 

private:
    std::istream& in;

};

template<typename DataCollector>
void read_data_from_stream(std::istream& in, DataCollector& collector) {
    std::string line;

    while (std::getline(in, line)) {
        std::stringstream line_stream(line);
        
        line_stream >> std::ws;

        if (line_stream.eof()) {
            continue;
        }

        size_t row, col;
        line_stream >> row >> col;

        if (!line_stream) {
            return;
        }

        collector.on_entry(row, col);
    }
}
