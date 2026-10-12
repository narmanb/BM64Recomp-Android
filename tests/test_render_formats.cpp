#include <plume_render_interface_types.h>
#include <cassert>
#include <cstdio>
using namespace plume;
namespace RT64 {
#include "swapchain_format.inc"
}
using namespace RT64;
struct CapturedTexture { RenderFormat format; };
struct CaptureDevice {
    RenderFormat sampled = RenderFormat::UNKNOWN;
    RenderSampleCounts getSampleCountsSupported(RenderFormat format) {
        sampled = format;return RenderSampleCount::COUNT_8;
    }
    std::unique_ptr<CapturedTexture> createTexture(const RenderTextureDesc& desc) {
        return std::make_unique<CapturedTexture>(CapturedTexture{desc.format});
    }
};
struct ActualFormatSelections {
#include "ui_swapchain_format.inc"
    CaptureDevice device;
    CaptureDevice* device_ = &device;
    RenderSwapChainDesc swapChainDesc;
    RenderGraphicsPipelineDesc pipeline_desc;
    std::unique_ptr<CapturedTexture> screen_texture_,screen_texture_ms_;
    RenderMultisampling multisampling_;
    void configure() {
        const int image_width=1280,image_height=720;
#include "render_format_selections.inc"
    }
};
int main() {
#ifdef __ANDROID__
    const auto expected=RenderFormat::R8G8B8A8_UNORM;
#else
    const auto expected=RenderFormat::B8G8R8A8_UNORM;
#endif
    ActualFormatSelections s;s.configure();
    assert(RT64::GetSwapChainFormat()==expected);
    assert(s.swapChainDesc.format==expected && "Swapchain must match RT64's final rendering pipeline");
    assert(s.pipeline_desc.renderTargetFormat[0]==expected && "UI pipeline must match the swapchain render pass");
    assert(s.device.sampled==expected);
    assert(s.screen_texture_->format==expected && s.screen_texture_ms_->format==expected);
    puts("Swapchain, UI pipeline and offscreen texture formats match");
}
