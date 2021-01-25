#include <vector>
#include <string>
#include <sstream>

#ifndef UTILS_H
#define UTILS_H
std::vector<std::string> splitStringAtWhitespace(std::string text) {
    std::vector<std::string> parts;
    std::istringstream iss(text);
    for(std::string s; iss >> s; )
        parts.push_back(s);
    return parts;
}
#endif
