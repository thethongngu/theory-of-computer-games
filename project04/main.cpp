#include <iostream>
#include "coordinator.h"
#include "test/test_coordinator.h"
#include "helper.h"

int main() {
    CoordinatorTest::run_tests();
    Helper::production = false;

    Coordinator coordinator;
    coordinator.start();
    return 0;
}