#include "Helpers.h"

#include <sstream>

std::string repeat(const std::string &str, int times) {
    std::stringstream ss;
    for (int i = 0; i<times; i++) {
        ss << str;
    }
    return ss.str();
}
