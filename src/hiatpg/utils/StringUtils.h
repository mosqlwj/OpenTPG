//
// Created by luolijun on 2020/7/6.
//

#ifndef HIATPG_STRINGUTILS_HPP
#define HIATPG_STRINGUTILS_HPP

#include <string>
#include <vector>


class StringUtils {
public:
    static  void trim(std::string& s);
    static  std::vector<std::string> split(const std::string& in, const std::string& delim);
};


#endif  // HIATPG_STRINGUTILS_HPP
