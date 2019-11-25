#include <iostream>
#include "coordinator.h"
#include "test/test_coordinator.h"

int main() {
    CoordinatorTest::run_tests();

    Coordinator coordinator;
    coordinator.start();
    return 0;
}