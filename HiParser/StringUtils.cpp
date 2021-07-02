//
// Created by fish on 2021/6/30.
//

#include "StringUtils.h"
#include <regex>

std::vector<std::string> StringUtils::split(const std::string &input, const std::string &delim) {
    std::regex re {delim};
    return std::vector<std::string> {
        std::sregex_token_iterator(input.begin(), input.end(), re, -1),
        std::sregex_token_iterator()
    };
}

void StringUtils::trim(std::string& s) {
    s.erase(0, s.find_first_not_of(" "));
    s.erase(s.find_first_not_of(" ") + 1);
}
