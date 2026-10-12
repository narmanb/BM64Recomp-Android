#include <plume_render_interface_types.h>
#include <vulkan/vulkan.h>
#include <array>
#include <bit>
#include <algorithm>
#include <cassert>
#include <cstdio>
using namespace plume;
#include "queue_type_index.inc"
unsigned numberOfSetBits(uint32_t x) {return std::popcount(x);}
struct Selection {
    std::array<uint32_t,3> queueFamilyIndices{};
    std::vector<bool> queueFamilyUsed;
    void configure(std::vector<VkQueueFamilyProperties> queueFamilyProperties) {
        const auto queueFamilyCount=static_cast<uint32_t>(queueFamilyProperties.size());
        queueFamilyUsed.assign(queueFamilyCount,false);
#include "queue_family_selection.inc"
    }
};
int main() {
    VkQueueFamilyProperties graphics{},compute{},transfer{};
    graphics.queueFlags=VK_QUEUE_GRAPHICS_BIT|VK_QUEUE_COMPUTE_BIT|VK_QUEUE_TRANSFER_BIT;graphics.queueCount=2;
    compute.queueFlags=VK_QUEUE_COMPUTE_BIT|VK_QUEUE_TRANSFER_BIT;compute.queueCount=1;
    transfer.queueFlags=VK_QUEUE_TRANSFER_BIT;transfer.queueCount=1;
    Selection selected;selected.configure({graphics,compute,transfer});
#ifdef __ANDROID__
    assert(selected.queueFamilyIndices[0]==selected.queueFamilyIndices[1] && "Exclusive shared resources require one Android queue family");
    assert(selected.queueFamilyIndices[0]==selected.queueFamilyIndices[2] && "Texture copies must remain in the present queue family");
    assert(selected.queueFamilyUsed[0] && !selected.queueFamilyUsed[1] && !selected.queueFamilyUsed[2]);
#else
    assert(selected.queueFamilyIndices[0]==0 && selected.queueFamilyIndices[1]==1 && selected.queueFamilyIndices[2]==2);
#endif
    selected.configure({graphics});
    assert(selected.queueFamilyIndices[0]==0 && selected.queueFamilyIndices[1]==0 && selected.queueFamilyIndices[2]==0);
    puts("Queue selection respects Android exclusive resource ownership and desktop policy");
}
