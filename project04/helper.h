//
// Created by thethongngu on 11/23/19.
//

#ifndef PROJECT04_HELPER_H
#define PROJECT04_HELPER_H


class Helper {
public:
    static bool production;

    static std::string to_lowercase(const std::string& s);
    static int get_int(const std::string& s);
    static void print(const std::string& s);
};


#endif //PROJECT04_HELPER_H
