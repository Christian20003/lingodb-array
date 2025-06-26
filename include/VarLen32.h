#include <cstdint>
#include <string>

namespace lingodb::runtime {

class VarLen32 {
    private:
    uint32_t len;
    uint8_t* ptr;
    
    public:
    VarLen32(uint8_t *ptr, uint32_t len);
    ~VarLen32();

    static VarLen32 fromString(std::string value);

    std::string str();
};

}