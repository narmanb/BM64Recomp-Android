#include <cassert>
#include <cstdio>
struct UserConfig {
    enum class InternalColorFormat { Automatic, Standard, High };
    InternalColorFormat internalColorFormat = InternalColorFormat::Automatic;
};
using UserConfiguration = UserConfig;
struct Device {
    struct Caps {bool preferHDR; bool highPrecisionColorSupported;} caps{true, false};
    const Caps& getCapabilities() { return caps; }
};
bool selectPrecision(UserConfig userConfig, Device* device) {
#include "color_precision_selection.inc"
    return usesHDR;
}
int main() {
    UserConfig cfg; Device device;
#ifdef __ANDROID__
    assert(!selectPrecision(cfg, &device) && "Automatic precision must fall back when high color format features are unsupported");
    cfg.internalColorFormat = UserConfig::InternalColorFormat::High;
    assert(!selectPrecision(cfg, &device) && "Forced high precision must fall back before shader/target creation");
    device.caps.highPrecisionColorSupported = true;
#endif
    cfg.internalColorFormat = UserConfig::InternalColorFormat::High;
    assert(selectPrecision(cfg, &device));
    cfg.internalColorFormat = UserConfig::InternalColorFormat::Standard;
    assert(!selectPrecision(cfg, &device));
    cfg.internalColorFormat = UserConfig::InternalColorFormat::Automatic;
    device.caps.preferHDR = false;
    assert(!selectPrecision(cfg, &device));
    puts("Color precision selection respects Android format support and desktop policy");
}
