//
// Created by thethongngu on 11/23/19.
//

#include <string>
#include "helper.h"

std::string Helper::to_lowercase(const std::string& s) {
    std::string res;
    for(auto x: s) {
        if (x >= 'A' && x <= 'Z') res.append(1, x + 32);
    }
    return res;
}
