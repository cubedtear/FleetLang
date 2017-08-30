#include <iostream>
#include <stdint.h>

extern "C" {
    float sign(int32_t x);
    int factorial(int n);

    void print(int32_t x) {
        std::cout << x << std::endl;
    }

    void printd(double v) {
        std::cout << v << std::endl;
    }
}