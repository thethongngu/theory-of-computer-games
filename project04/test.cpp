//
// Created by thethongngu on 11/20/19.
//

#include <iostream>
#include "test.h"
#include "coordinator.h"

int Test::test01() {
    std::string a = "test test";
    a.append(1, (char) 13);
    a.append("  test ");
    a.append(1, (char) 9);
    a.append(1, (char) 13);
    a.append(1, (char) 3);
    a.append("test");
    auto command = Coordinator::preprocess(a);
    return(command == "test test  test  test");
}

int Test::test02() {
    std::string a = "test # test 33 ";
    a.append(1, (char) 13);
    a.append(1, (char) 9);
    a.append(1, (char) 13);
    a.append(1, (char) 3);
    auto command = Coordinator::preprocess(a);
    return(command == "test ");
}

int Test::test03() {
    std::string a = "test # test 33 ";
    a.append(1, (char) 10);
    a.append(1, (char) 9);
    a.append(1, (char) 13);
    a.append(1, (char) 3);
    a.append("aaa");
    auto command = Coordinator::preprocess(a);
    return(command == "test \n aaa");
}
