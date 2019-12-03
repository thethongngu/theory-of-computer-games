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
    static int get_random_number(int module);
    static void print(const std::string& s);

    static std::random_device rand_dev;
    static std::mt19937 generator;
    static std::uniform_int_distribution<int> dist;
};


#endif //PROJECT04_HELPER_H
