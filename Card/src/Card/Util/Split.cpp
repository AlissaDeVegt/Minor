#include "Split.h"

namespace Card {

    std::vector<std::string> Card::Split::split(std::string line, char delimeter)
    {
        std::vector<std::string> data;

        int start, end;
        start = end = 0;

        //split the string
        while ((start = line.find_first_not_of(delimeter, end)) != std::string::npos) {

            end = line.find(delimeter, start);

            data.push_back(line.substr(start, end - start));
        }

        return data;
    }
}

