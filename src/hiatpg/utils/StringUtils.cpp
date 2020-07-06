//
// Created by luolijun on 2020/7/6.
//

#include "StringUtils.hpp"

#include <regex>

std::vector<std::string> StringUtils::split(const std::string& in, const std::string& delim) {
    std::regex re{delim};
    return std::vector<std::string>{std::sregex_token_iterator(in.begin(), in.end(), re, -1),
                                    std::sregex_token_iterator()};
}

void StringUtils::trim(std::string& s) {
    int index = 0;
    if (!s.empty()) {
        while ((index = s.find(' ', index)) != std::string::npos) {
            s.erase(index, 1);
        }
    }
}