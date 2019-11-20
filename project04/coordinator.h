//
// Created by thethongngu on 11/20/19.
//

#ifndef PROJECT04_COORDINATOR_H
#define PROJECT04_COORDINATOR_H

#include <string>


class Coordinator {
public:
    Coordinator();

    static void preprocess(char* raw_command, char* new_command);
    void run(char* raw_command);
};


#endif //PROJECT04_COORDINATOR_H
