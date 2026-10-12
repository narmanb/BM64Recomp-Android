#include "ultramodern/config.hpp"
#include <cassert>
#include <cstdio>
namespace RT64 {
struct UserConfiguration {
    enum class Resolution { Manual, WindowIntegerScale };
    Resolution resolution = Resolution::Manual;
    double resolutionMultiplier = 2;
    int downsampleMultiplier = 1;
};
}
struct Application { RT64::UserConfiguration userConfig; };
void configure(Application* application, const ultramodern::renderer::GraphicsConfig& config) {
#include "resolution_selection.inc"
}
static void check(const char* mode, double scale, int savedDownsample, int expectedDownsample) {
    ultramodern::renderer::GraphicsConfig cfg{};
    cfg.res_option = nlohmann::json(mode).get<ultramodern::renderer::Resolution>();
    cfg.ds_option = savedDownsample;
    Application app;
    configure(&app, cfg);
    assert(app.userConfig.resolution == RT64::UserConfiguration::Resolution::Manual);
    assert(app.userConfig.resolutionMultiplier == scale && "Resolution preference must reach the actual RT64 render scale");
    assert(app.userConfig.downsampleMultiplier == expectedDownsample);
    assert(nlohmann::json(cfg.res_option) == mode && "Resolution must survive native JSON round trips");
}
int main() {
    check("Original", 1, 1, 1);
    check("Original2x", 2, 1, 1);
    check("Original", 4, 4, 4);
    check("Original2x", 8, 4, 4);
#ifdef __ANDROID__
    for (int ds : {1, 2, 4}) {
        check("Original3x", 3, ds, 1);
        check("Original4x", 4, ds, 1);
        check("Original6x", 6, ds, 1);
    }
#else
    static_assert(static_cast<int>(ultramodern::renderer::Resolution::OptionCount) == 3);
    assert(nlohmann::json("Original6x").get<ultramodern::renderer::Resolution>() == ultramodern::renderer::Resolution::Original);
#endif
    ultramodern::renderer::GraphicsConfig cfg{};
    cfg.res_option = ultramodern::renderer::Resolution::Auto;
    cfg.ds_option = 4;
    Application app;
    configure(&app, cfg);
    assert(app.userConfig.resolution == RT64::UserConfiguration::Resolution::WindowIntegerScale);
    assert(app.userConfig.downsampleMultiplier == 1);
    puts("Resolution JSON round trips, real RT64 scales and downsampling policies pass");
}
