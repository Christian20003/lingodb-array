#include "../include/VarLen32.h"
#include "cstring"

using lingodb::runtime::VarLen32;

VarLen32::VarLen32(uint8_t *ptr, uint32_t len) {
    this->ptr = ptr;
    this->len = len;
}

VarLen32::~VarLen32() {
    //delete[] this->ptr;
}

VarLen32 VarLen32::fromString(std::string value) {
    auto *ptr = new uint8_t[value.size()];
    memcpy(ptr, value.data(), value.size());
    return VarLen32(ptr, value.size());
}

std::string VarLen32::str() {
    return std::string((char *) this->ptr, this->len);
}