//
// Created by fish on 2021/6/30.
//

#ifndef HIPARSER_STRINGUTILS_H
#define HIPARSER_STRINGUTILS_H

#include <vector>
#include <string>

class StringUtils {
public:
    static std::vector<std::string> split(const std::string& input, const std::string& delim);
    static void trim(std::string& str);
};


#endif //HIPARSER_STRINGUTILS_H
