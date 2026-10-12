#include <plume_render_interface_types.h>
#include <cassert>
#include <cstdio>
using namespace plume;
namespace RT64 {
struct UserConfiguration {
    enum class Antialiasing { None, MSAA2X, MSAA4X, MSAA8X };
};
struct RenderTarget {
    static RenderFormat colorBufferFormat(bool usesHDR);
    static RenderFormat depthBufferFormat();
};
#include "render_target_formats.inc"
}
#include "max_supported_aa.inc"
static uint32_t device_msaa_counts = 1;
static RT64::UserConfiguration::Antialiasing device_max_msaa = RT64::UserConfiguration::Antialiasing::None;
namespace zelda64::renderer {
    uint32_t RT64MSAACounts();
    RT64::UserConfiguration::Antialiasing RT64MaxMSAA();
}
#include "msaa_capability_accessors.inc"
struct Menu {
    bool msaa2x_supported = false, msaa4x_supported = false, msaa8x_supported = false;
    void update() {
#include "msaa_ui_flags.inc"
    }
};
struct Device {
    bool positions = true;
    RenderFormat queriedColor = RenderFormat::UNKNOWN;
    RenderSampleCounts mask = RenderSampleCount::COUNT_2 | RenderSampleCount::COUNT_4 | RenderSampleCount::COUNT_8;
    struct Capabilities { bool sampleLocations; };
    Capabilities getCapabilities() { return {positions}; }
    RenderSampleCounts getSampleCountsSupported(RenderFormat format) {
        if (format == RenderFormat::D32_FLOAT) return mask;
        queriedColor = format;
        if (format == RenderFormat::R16G16B16A16_UNORM) return RenderSampleCount::COUNT_2 & mask;
        return mask;
    }
};
struct Shaders { bool usesHDR = false; };
struct App { Device* device; Shaders* shaderLibrary; };
struct Context {
    App* app;
    RT64::UserConfiguration::Antialiasing device_max_msaa = RT64::UserConfiguration::Antialiasing::None;
    bool sample_positions_supported = false;
    uint32_t device_msaa_counts = 1;
    void configure() {
#include "msaa_capability_selection.inc"
    }
};
int main() {
    using AA = RT64::UserConfiguration::Antialiasing;
    Device device; Shaders shaders; App app{&device, &shaders}; Context ctx{&app};
    ctx.configure();
    assert(device.queriedColor == RT64::RenderTarget::colorBufferFormat(false));
    assert(ctx.device_max_msaa == AA::MSAA8X);
    shaders.usesHDR = true;
    ctx.configure();
#ifdef __ANDROID__
    assert(device.queriedColor == RT64::RenderTarget::colorBufferFormat(true) && "MSAA support must use the active high-precision color format");
    assert(ctx.device_max_msaa == AA::MSAA2X && "Do not expose unsupported MSAA modes for high precision");
    shaders.usesHDR = false;
    device.mask = RenderSampleCount::COUNT_4;
    ctx.configure();
    assert(ctx.device_msaa_counts == 4 && "Preserve exact sparse sample counts for native settings");
    device_msaa_counts = ctx.device_msaa_counts;
    device_max_msaa = ctx.device_max_msaa;
    Menu menu; menu.update();
    assert(!menu.msaa2x_supported && menu.msaa4x_supported && !menu.msaa8x_supported && "Only enable supported MSAA buttons even for sparse masks");
#else
    assert(device.queriedColor == RenderFormat::R8G8B8A8_UNORM);
    assert(ctx.device_max_msaa == AA::MSAA8X);
#endif
    device.positions = false;
    ctx.configure();
    assert(ctx.device_max_msaa == AA::None && !ctx.sample_positions_supported);
#ifdef __ANDROID__
    assert(ctx.device_msaa_counts == 1);
#endif
    puts("MSAA capabilities match active color/depth formats and sample-position support");
}
