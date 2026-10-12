#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <fstream>
#include <mutex>
#include <string>
#include <vector>
#include <sys/wait.h>
#include <unistd.h>
#if __has_include("plume_android_failure.h")
#include "plume_android_failure.h"
#endif
static VkResult submitResult=VK_SUCCESS,waitResult=VK_SUCCESS;
static int resetCalls=0;
VkResult vkQueueSubmit(VkQueue,uint32_t,const VkSubmitInfo*,VkFence) {return submitResult;}
VkResult vkWaitForFences(VkDevice,uint32_t,const VkFence*,VkBool32,uint64_t) {return waitResult;}
VkResult vkResetFences(VkDevice,uint32_t,const VkFence*) {++resetCalls;return VK_SUCCESS;}
namespace plume {
struct RenderCommandList {};
struct RenderCommandSemaphore {};
struct RenderCommandFence {};
struct VulkanCommandList : RenderCommandList {VkCommandBuffer vk=VK_NULL_HANDLE;};
struct VulkanCommandSemaphore : RenderCommandSemaphore {VkSemaphore vk=VK_NULL_HANDLE;};
struct VulkanCommandFence : RenderCommandFence {VkFence vk=VK_NULL_HANDLE;};
struct VulkanDevice {VkDevice vk=VK_NULL_HANDLE;};
struct VulkanQueue {VkQueue vk=VK_NULL_HANDLE;std::mutex* mutex;};
enum class RenderCommandListType {UNKNOWN,DIRECT,COMPUTE,COPY};
struct VulkanCommandQueue {
    VulkanQueue* queue;VulkanDevice* device;RenderCommandListType type=RenderCommandListType::DIRECT;uint32_t familyIndex=0;
    void executeCommandLists(const RenderCommandList**,uint32_t,RenderCommandSemaphore**,uint32_t,RenderCommandSemaphore**,uint32_t,RenderCommandFence*);
    void waitForCommandFence(RenderCommandFence*);
};
#include "vulkan_failure_methods.inc"
}
static void runCase(VkResult submit,VkResult wait,int exitCode,const char* operation,const std::string& dir) {
    const auto child=fork();assert(child>=0);
    if(child==0) {
        setenv("APP_FOLDER_PATH",dir.c_str(),1);
        submitResult=submit;waitResult=wait;
        std::mutex mutex;plume::VulkanQueue queue{VK_NULL_HANDLE,&mutex};plume::VulkanDevice device;
        plume::VulkanCommandQueue commands{&queue,&device};plume::VulkanCommandList list;plume::VulkanCommandFence fence;
        const plume::RenderCommandList* lists[]={&list};
        commands.executeCommandLists(lists,1,nullptr,0,nullptr,0,&fence);
        commands.waitForCommandFence(&fence);
        assert(resetCalls==1 && "Only successfully completed fences may be reset");
        _Exit(0);
    }
    int status=0;assert(waitpid(child,&status,0)==child);
    assert(WIFEXITED(status) && WEXITSTATUS(status)==exitCode && "Fatal Vulkan failure must stop before waiting/presenting another frame");
    if(exitCode!=0) {
        std::ifstream file(dir+"/graphics-failure.txt");
        std::string text((std::istreambuf_iterator<char>(file)),{});
        assert(text.find(operation)!=std::string::npos && "The first Vulkan failure must be saved for the launcher");
        remove((dir+"/graphics-failure.txt").c_str());
    }
}
int main() {
    char path[]="/tmp/bm64-vulkan-test-XXXXXX";assert(mkdtemp(path));std::string dir=path;
    runCase(VK_ERROR_DEVICE_LOST,VK_SUCCESS,EXIT_FAILURE,"vkQueueSubmit",dir);
    runCase(VK_SUCCESS,VK_ERROR_DEVICE_LOST,EXIT_FAILURE,"vkWaitForFences",dir);
    runCase(VK_SUCCESS,VK_TIMEOUT,EXIT_FAILURE,"vkWaitForFences",dir);
    runCase(VK_SUCCESS,VK_SUCCESS,0,"",dir);
    rmdir(path);
    puts("Fatal Vulkan failures stop without invalid fence reuse; successful submissions continue");
}
