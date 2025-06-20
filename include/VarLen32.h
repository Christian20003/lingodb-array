#include <cstdint>
#include <string>

class VarLen32 {
    private:
    uint32_t len;
    uint8_t* ptr;
    
    public:
    VarLen32(std::string value);

    std::string str();
};