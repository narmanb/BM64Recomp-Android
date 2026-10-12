"""Compile production renderer decisions in host harnesses without a GPU or ROM."""
from pathlib import Path
import re
import sys
out = Path(sys.argv[1])
(out / 'android').mkdir(exist_ok=True)
(out / 'android/native_window.h').write_text('typedef struct ANativeWindow ANativeWindow;\n')
hooks = Path('lib/rt64/src/rhi/rt64_render_hooks.cpp').read_text()
start = hooks.index('    RenderFormat GetSwapChainFormat()')
end = hooks.index('\n    }', start) + 6
(out / 'swapchain_format.inc').write_text(hooks[start:end])
ui = Path('src/ui/ui_renderer.cpp').read_text()
(out / 'ui_swapchain_format.inc').write_text(next(l for l in ui.splitlines() if 'static' in l and 'SwapChainFormat' in l) + '\n')
app = Path('lib/rt64/src/hle/rt64_application.cpp').read_text()
selections = [next(l for l in app.splitlines() if 'swapChainDesc.format =' in l)]
selections.append('device_->getSampleCountsSupported(' + re.search(r'getSampleCountsSupported\((SwapChainFormat(?:\(\))?)\)', ui).group(1) + ');')
selections.extend(l for l in ui.splitlines() if 'pipeline_desc.renderTargetFormat[0] =' in l or 'screen_texture_ = device_->createTexture' in l or 'screen_texture_ms_ = device_->createTexture' in l)
assert len(selections) == 5
(out / 'render_format_selections.inc').write_text('\n'.join(selections))
vulkan = Path('lib/rt64/src/contrib/plume/plume_vulkan.cpp').read_text()
start = vulkan.index('    static uint32_t toFamilyIndex(')
end = vulkan.index('\n    }', start) + 6
(out / 'queue_type_index.inc').write_text(vulkan[start:end])
start = vulkan.index('        auto pickFamilyQueue = ')
end = vulkan.index('        // Create the struct to store the virtual queues.', start)
(out / 'queue_family_selection.inc').write_text(vulkan[start:end])

start = vulkan.index('    void VulkanCommandQueue::executeCommandLists(')
end = vulkan.index('    // VulkanPool', start)
(out / 'vulkan_failure_methods.inc').write_text(vulkan[start:end])

context = Path('src/main/rt64_render_context.cpp').read_text()
start = context.index('RT64::UserConfiguration::Antialiasing compute_max_supported_aa(')
end = context.index('\n}\n', start) + 3
(out / 'max_supported_aa.inc').write_text(context[start:end])
start = context.index('    if (app->device->getCapabilities().sampleLocations)')
end = context.index('    high_precision_fb_enabled = ', start)
(out / 'msaa_capability_selection.inc').write_text(context[start:end])
accessors = []
for declaration in ['RT64::UserConfiguration::Antialiasing zelda64::renderer::RT64MaxMSAA()', 'uint32_t zelda64::renderer::RT64MSAACounts()']:
    start = context.index(declaration)
    end = context.index('\n}\n', start) + 3
    accessors.append(context[start:end])
(out / 'msaa_capability_accessors.inc').write_text('\n'.join(accessors))
ui_config = Path('src/ui/ui_config.cpp').read_text()
start = ui_config.index('void recompui::update_supported_options() {') + len('void recompui::update_supported_options() {')
end = ui_config.index('    sample_positions_supported = ', start)
(out / 'msaa_ui_flags.inc').write_text(ui_config[start:end])
target = Path('lib/rt64/src/render/rt64_render_target.cpp').read_text()
formats = []
for name in ['colorBufferFormat', 'depthBufferFormat']:
    start = target.index('    RenderFormat RenderTarget::' + name + '(')
    end = target.index('\n    }', start) + 6
    formats.append(target[start:end])
(out / 'render_target_formats.inc').write_text('\n'.join(formats))
start = vulkan.index('    VkFormat toVk(RenderFormat format)')
end = vulkan.index('\n    }', start) + 6
(out / 'vulkan_format_conversion.inc').write_text(vulkan[start:end])
start = vulkan.index('    RenderSampleCounts VulkanDevice::getSampleCountsSupported(')
end = vulkan.index('\n    }', start) + 6
(out / 'vulkan_sample_counts.inc').write_text(vulkan[start:end])
start = app.index('        bool usesHDR;')
end = app.index('        // Call the init hook', start)
(out / 'color_precision_selection.inc').write_text(app[start:end])
start = vulkan.index('    static bool supportsHighPrecisionColor(VkPhysicalDevice physicalDevice, bool storageExtendedFormats) {')
end = vulkan.index('\n    }', start) + 6
(out / 'high_precision_support.inc').write_text(vulkan[start:end])
(out / 'high_precision_capability.inc').write_text(next(l for l in vulkan.splitlines() if 'capabilities.highPrecisionColorSupported =' in l))
