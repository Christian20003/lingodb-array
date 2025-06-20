#include "../include/VarLen32.h"
#include "cstring"

VarLen32::VarLen32(std::string value) {
    auto *ptr = new uint8_t[value.size()];
    memcpy(ptr, value.data(), value.size());
    this->ptr = ptr;
    this->len = value.size();
}

std::string VarLen32::str() {
    return std::string((char *) this->ptr, this->len);
}