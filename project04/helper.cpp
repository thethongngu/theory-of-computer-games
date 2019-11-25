//
// Created by thethongngu on 11/23/19.
//

#include <string>
#include <cstdlib>
#include "helper.h"

std::string Helper::to_lowercase(const std::string& s) {
    std::string res;
    for(auto x: s) {
        if (x >= 'A' && x <= 'Z') res.append(1, x + 32);
    }
    return res;
}

/** Check whether a string is int
 *  Return -1 if not an int, otherwise value of int
 *  Note: 'int' in here follow GPT 2.0
 */
int Helper::get_int(const std::string &s) {
    if (s.find_first_not_of("01234566789") != std::string::npos) return -1;
    return strtol(s.c_str(), nullptr, 0);
}