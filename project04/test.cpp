//
// Created by thethongngu on 11/20/19.
//

#include "test.h"
#include "coordinator.h"

int test::test01() {
    std::string a = "test test";
    a.append(1, (char) 32);
    char raw_command[] = "test test\n   \n      dfdf        ";
    char new_command[] = "";
    Coordinator::preprocess()
}
