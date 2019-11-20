//
// Created by thethongngu on 11/20/19.
//

#ifndef PROJECT04_COORDINATOR_H
#define PROJECT04_COORDINATOR_H

#include <string>


class Coordinator {
public:
    Coordinator();

    static std::string preprocess(const std::string& raw_command);
    void run(const std::string& command);
};


#endif //PROJECT04_COORDINATOR_H
