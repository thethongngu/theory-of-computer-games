//
// Created by thethongngu on 11/20/19.
//

#include <iostream>
#include <cassert>
#include "test_coordinator.h"
#include "../coordinator.h"

int CoordinatorTest::preprocess01() {
    std::string a = "production production";
    a.append(1, (char) 13);
    a.append("  production ");
    a.append(1, (char) 9);
    a.append(1, (char) 13);
    a.append(1, (char) 3);
    a.append("production");
    auto command = Coordinator::preprocess(a);
    return command == "production production  production  production";
}

int CoordinatorTest::preprocess02() {
    std::string a = "production # production 33 ";
    a.append(1, (char) 13);
    a.append(1, (char) 9);
    a.append(1, (char) 13);
    a.append(1, (char) 3);
    auto command = Coordinator::preprocess(a);
    return command == "production ";
}

int CoordinatorTest::preprocess03() {
    std::string a = "production # production 33 ";
    a.append(1, (char) 10);
    a.append(1, (char) 9);
    a.append(1, (char) 13);
    a.append(1, (char) 3);
    a.append("aaa");
    auto command = Coordinator::preprocess(a);
    return command == "production \n aaa";
}

int CoordinatorTest::preprocess04() {
    auto command = Coordinator::preprocess("# thong thong");
    return command == "";
}

int CoordinatorTest::preprocess05() {
    auto command = Coordinator::preprocess("");
    return command == "";
}

int CoordinatorTest::parse_command01() {
    auto command = Coordinator::parse_command("");
    return command.id == -1 && command.arguments.empty() && command.command.empty();
}

int CoordinatorTest::parse_command02() {
    auto command = Coordinator::parse_command("      ");
    return command.id == -1 && command.arguments.empty() && command.command.empty();
}

int CoordinatorTest::parse_command03() {
    auto command = Coordinator::parse_command("-1");
    return command.id == -1 && command.arguments.empty() && command.command == "-1";
}

int CoordinatorTest::parse_command04() {
    auto command = Coordinator::parse_command("abc");
    return command.id == -1 && command.arguments.empty() && command.command == "abc";
}

int CoordinatorTest::parse_command05() {
    auto command = Coordinator::parse_command("     abc  ");
    return command.id == -1 && command.arguments.empty() && command.command == "abc";
}

int CoordinatorTest::parse_command06() {
    auto command = Coordinator::parse_command("10 abc");
    return command.id == 10 && command.arguments.empty() && command.command == "abc";
}

int CoordinatorTest::parse_command07() {
    auto command = Coordinator::parse_command("  10    abc  ");
    return command.id == 10 && command.arguments.empty() && command.command == "abc";
}

int CoordinatorTest::parse_command08() {
    auto command = Coordinator::parse_command("  -10  abc ");
    return command.id == -1 && command.arguments.size() == 1 && command.arguments[0] == "abc"
            && command.command == "-10";
}

int CoordinatorTest::parse_command09() {
    auto command = Coordinator::parse_command("10 abc aa bb   cc  ");
    return command.id == 10 && command.arguments.size() == 3 && command.command == "abc"
            && command.arguments[0] == "aa" && command.arguments[1] == "bb" && command.arguments[2] == "cc";
}

void CoordinatorTest::run_tests() {
    assert(preprocess01());
    assert(preprocess02());
    assert(preprocess03());
    assert(preprocess04());
    assert(preprocess05());
    assert(parse_command01());
    assert(parse_command02());
    assert(parse_command03());
    assert(parse_command04());
    assert(parse_command05());
    assert(parse_command06());
    assert(parse_command07());
    assert(parse_command08());
    assert(parse_command09());
    std::cout << "Passed all tests!" << std::endl;
}
