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
