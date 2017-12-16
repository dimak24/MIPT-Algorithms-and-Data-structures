#include "tests.h"

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    int x = RUN_ALL_TESTS();
}