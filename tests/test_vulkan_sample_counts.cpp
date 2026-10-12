#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>
#include <plume_render_interface_types.h>
#include <cassert>
#include <cstdio>
static VkFormat queriedFormat = VK_FORMAT_UNDEFINED;
static VkImageUsageFlags queriedUsage = 0;
static VkImageCreateFlags queriedFlags = 0;
static VkResult queryResult = VK_SUCCESS;
static bool rejectStorage = false;
static VkFormatFeatureFlags formatFeatures = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT |
    VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT;
void vkGetPhysicalDeviceFormatProperties(VkPhysicalDevice,VkFormat format,VkFormatProperties* properties) {
    assert(format == VK_FORMAT_R16G16B16A16_UNORM);
    properties->optimalTilingFeatures = formatFeatures;
}
VkResult vkGetPhysicalDeviceImageFormatProperties(VkPhysicalDevice,VkFormat format,VkImageType type,VkImageTiling tiling,VkImageUsageFlags usage,VkImageCreateFlags flags,VkImageFormatProperties* properties) {
    assert(type == VK_IMAGE_TYPE_2D && tiling == VK_IMAGE_TILING_OPTIMAL);
    queriedFormat = format; queriedUsage = usage; queriedFlags = flags;
    properties->sampleCounts = format == VK_FORMAT_R16G16B16A16_UNORM ? VK_SAMPLE_COUNT_1_BIT | VK_SAMPLE_COUNT_2_BIT : VK_SAMPLE_COUNT_1_BIT | VK_SAMPLE_COUNT_2_BIT | VK_SAMPLE_COUNT_4_BIT | VK_SAMPLE_COUNT_8_BIT;
    return rejectStorage && (usage & VK_IMAGE_USAGE_STORAGE_BIT) ? VK_ERROR_FORMAT_NOT_SUPPORTED : queryResult;
}
namespace plume {
#include "vulkan_format_conversion.inc"
#ifdef __ANDROID__
#include "high_precision_support.inc"
#endif
struct VulkanDevice {
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkPhysicalDeviceProperties physicalDeviceProperties = {};
    VkPhysicalDeviceSampleLocationsPropertiesEXT sampleLocationProperties = {};
    RenderDeviceCapabilities capabilities = {};
    RenderSampleCounts getSampleCountsSupported(RenderFormat format) const;
#ifdef __ANDROID__
    void configurePrecision(bool extendedFormats) {
        VkPhysicalDeviceFeatures2 deviceFeatures = {};
        deviceFeatures.features.shaderStorageImageExtendedFormats = extendedFormats;
#include "high_precision_capability.inc"
    }
#endif
};
#include "vulkan_sample_counts.inc"
}
int main() {
    using namespace plume;
    VulkanDevice device;
    device.physicalDeviceProperties.limits.framebufferColorSampleCounts = VK_SAMPLE_COUNT_1_BIT | VK_SAMPLE_COUNT_2_BIT | VK_SAMPLE_COUNT_4_BIT | VK_SAMPLE_COUNT_8_BIT;
    device.physicalDeviceProperties.limits.framebufferDepthSampleCounts = VK_SAMPLE_COUNT_1_BIT | VK_SAMPLE_COUNT_2_BIT | VK_SAMPLE_COUNT_4_BIT;
#ifdef __ANDROID__
    device.capabilities.sampleLocations = true;
    device.sampleLocationProperties.sampleLocationSampleCounts = VK_SAMPLE_COUNT_2_BIT | VK_SAMPLE_COUNT_4_BIT | VK_SAMPLE_COUNT_8_BIT;
    device.configurePrecision(true);
    assert(device.capabilities.highPrecisionColorSupported);
    assert(queriedUsage == (VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT));
    device.configurePrecision(false);
    assert(!device.capabilities.highPrecisionColorSupported && "Storage extended formats must be enabled");
    for (VkFormatFeatureFlags bit : {VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT, VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT,
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT, VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT, VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT}) {
        formatFeatures ^= bit;
        device.configurePrecision(true);
        assert(!device.capabilities.highPrecisionColorSupported && "Every required image format feature must be supported");
        formatFeatures ^= bit;
    }
    rejectStorage = true;
    device.configurePrecision(true);
    assert(!device.capabilities.highPrecisionColorSupported && "High precision must support downsampling storage images");
    rejectStorage = false;
    queryResult = VK_ERROR_FORMAT_NOT_SUPPORTED;
    device.configurePrecision(true);
    assert(!device.capabilities.highPrecisionColorSupported && "High precision must support sampled color targets");
    queryResult = VK_SUCCESS;
    assert(device.getSampleCountsSupported(RenderFormat::R16G16B16A16_UNORM) == (VK_SAMPLE_COUNT_1_BIT | VK_SAMPLE_COUNT_2_BIT) && "Use per-format MSAA limits instead of global color limits");
    assert(queriedFormat == VK_FORMAT_R16G16B16A16_UNORM);
    const VkImageUsageFlags common = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    assert(queriedUsage == (common | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT) && queriedFlags == 0);
    assert(device.getSampleCountsSupported(RenderFormat::R8G8B8A8_UNORM) == device.physicalDeviceProperties.limits.framebufferColorSampleCounts);
    device.capabilities.sampleLocations = true;
    assert(device.getSampleCountsSupported(RenderFormat::D32_FLOAT) == device.physicalDeviceProperties.limits.framebufferDepthSampleCounts);
    assert(queriedUsage == (common | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT));
    assert(queriedFlags == VK_IMAGE_CREATE_SAMPLE_LOCATIONS_COMPATIBLE_DEPTH_BIT_EXT);
    device.sampleLocationProperties.sampleLocationSampleCounts = VK_SAMPLE_COUNT_4_BIT;
    assert(device.getSampleCountsSupported(RenderFormat::R8G8B8A8_UNORM) == (VK_SAMPLE_COUNT_1_BIT | VK_SAMPLE_COUNT_4_BIT) && "Do not advertise sample counts without custom sample-location support");
    device.capabilities.sampleLocations = false;
    assert(device.getSampleCountsSupported(RenderFormat::R8G8B8A8_UNORM) == VK_SAMPLE_COUNT_1_BIT && "RT64 Android requires custom sample locations for MSAA");
    queryResult = VK_ERROR_FORMAT_NOT_SUPPORTED;
    assert(device.getSampleCountsSupported(RenderFormat::R16G16B16A16_UNORM) == 0 && "Unsupported image formats must not advertise MSAA");
#else
    assert(device.getSampleCountsSupported(RenderFormat::R16G16B16A16_UNORM) == device.physicalDeviceProperties.limits.framebufferColorSampleCounts);
    assert(device.getSampleCountsSupported(RenderFormat::D32_FLOAT) == device.physicalDeviceProperties.limits.framebufferDepthSampleCounts);
    assert(queriedFormat == VK_FORMAT_UNDEFINED && "Desktop sample-count policy stays unchanged");
#endif
    puts("Vulkan MSAA limits respect Android image formats/usages and desktop policy");
}
