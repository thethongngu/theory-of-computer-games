#include <iostream>
#include <cassert>
#include "coordinator.h"
#include "test.h"

int main() {
    assert(Test::test01());
    assert(Test::test02());
    assert(Test::test03());

    Coordinator coordinator;
    coordinator.start();
    return 0;
}