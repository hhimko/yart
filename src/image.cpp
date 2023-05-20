#include "image.h"

#include "utils/core_utils.h"
#include "utils/vk_utils.h"


namespace yart
{
    Image::Image(VkDevice device, uint32_t width, uint32_t height, const void *data)
        : m_vkDevice(device)
    {
        YART_ASSERT(m_vkDevice != VK_NULL_HANDLE);
    }

    Image::~Image()
    {
        Release();
    }

    void Image::BindData(const void *data)
    {

    }

    void Image::Resize(uint32_t width, uint32_t height, const void *data)
    {
        Release();
    }

    void Image::CreateDescriptorSet(uint32_t width, uint32_t height)
    {
        static VkFormat FORMAT = VK_FORMAT_R32G32B32_SFLOAT;
        
    }

    void Image::Release()
    {
        YART_ASSERT(m_vkDevice != VK_NULL_HANDLE);

        VkResult res = vkDeviceWaitIdle(m_vkDevice);
        CHECK_VK_RESULT_ABORT(res);


    }
} // namespace yart
