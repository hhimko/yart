////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Backend module public API for GPU communication and platform dependent operations
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once


#include <functional>

#include <imgui.h>

#include "yart/common/utils/yart_utils.h"


namespace yart
{
    namespace Backend
    {
        /// @brief Backend event handler callback type 
        using event_callback_t = std::function<void()>;


        /// @brief Image formats supported by the Backend module 
        enum class ImageFormat : uint8_t {
            R32G32B32A32_FLOAT = 0,
            COUNT
        };

        /// @brief Image samplers supported by the Backend module 
        enum class ImageSampler : uint8_t {
            NEAREST = 0,
            BILINEAR,
            BICUBIC,
            COUNT
        };


        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Abstract Image base class, for managing and uploading 2D textures to the GPU  
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        class Image {
        public:     
            virtual ~Image() = default;

            /// @brief Upload and bind pixel data to the image  
            /// @param data Pointer to pixel array to upload and bind. The size of the array must be equal to `(width * height * channels)`,
            ///     where `channels` is the number of channels in the image, based on the image format used
            virtual void BindData(const void* data) = 0;

            /// @brief Rebuild the image for a new given size
            /// @param width New image width in texels
            /// @param height New image height in texels
            /// @warning Rebuilding essentially leads to the loss of previously bound image data, so Image::BindData() has to be called after the resize
            virtual void Resize(uint32_t width, uint32_t height) = 0;

            /// @brief Set the image sampler type used for interpolation
            /// @param sampler New sampler type 
            virtual void SetSampler(ImageSampler sampler) = 0;

            /// @brief Get the image ID, used by Dear ImGui for differentiating GPU textures
            /// @return Dear ImGui's `ImTextureID` for this image
            virtual ImTextureID GetImTextureID() const = 0;

            /// @brief Get the current size of the image in pixels
            /// @return Image size in pixels
            ImVec2 GetSize() const 
            {
                ImVec2 size = { static_cast<float>(m_width), static_cast<float>(m_height) };
                return size;
            }

            /// @brief Get the sampler type currently used by the image 
            /// @return Current sampler type
            ImageSampler GetSampler() const 
            {
                return m_sampler;
            }

            /// @brief Get this image's format size in bytes
            /// @return Size of a single pixel in the given format in bytes
            constexpr uint8_t GetFormatSize()
            {
                switch (m_format) {
                case ImageFormat::R32G32B32A32_FLOAT: return 4 * 4;
                case ImageFormat::COUNT:
                default:
                    YART_ABORT("Unknown ImageFormat value passed to `Backend::Image::GetFormatSize`\n");
                }

                YART_UNREACHABLE();
                return 0;
            }

            /// @brief  Get this image's format channel count in bytes
            /// @return Number of channels in the given format. (3 == RGB, 4 == RGBA)
            constexpr uint8_t GetFormatChannelsCount()
            {
                switch (m_format) {
                case ImageFormat::R32G32B32A32_FLOAT: return 4;
                case ImageFormat::COUNT:
                default:
                    YART_ABORT("Unknown ImageFormat value passed to `Backend::Image::GetFormatChannelsCount`\n");
                }

                YART_UNREACHABLE();
                return 0;
            }

        protected:
            /// @brief Image base class constructor
            /// @param width Initial width of the image in texels
            /// @param height Initial height of the image in texels
            /// @param format Format of the image
            /// @param sampler Initial sampler type used by the image
            Image(uint32_t width, uint32_t height, ImageFormat format, ImageSampler sampler)
                : m_width(width), m_height(height), m_format(format), m_sampler(sampler)
            { };

        protected:
            /// @brief Width of the image in texels
            uint32_t m_width;
            /// @brief Height of the image in texels
            uint32_t m_height; 
            /// @brief Image data format
            ImageFormat m_format;
            /// @brief Sampler type used for interpolation
            ImageSampler m_sampler;

        };


        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// Backend module public API interface functions 
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        /// @brief Initialize and open a backend window
        /// @param window_title Initial window title
        /// @param window_width Initial width of the window in pixels
        /// @param window_height Initial height of the window in pixels
        /// @return Whether the window has been successfully created 
        bool Init(const char* window_title, uint32_t window_width, uint32_t window_height);

        /// @brief Set a custom callback function to be called when initializing Dear ImGui for custom setup
        /// @param callback A callback function, or `nullptr` to remove the current callback 
        void SetDearImGuiSetupCallback(event_callback_t callback);

        /// @brief Set a custom callback function to be called when the platform window is closed by the user
        /// @param callback A callback function, or `nullptr` to remove the current callback 
        void SetWindowCloseCallback(event_callback_t callback);

        /// @brief Get the mouse cursor position as returned by the platform backend
        /// @return Mouse position in screen-space coordinates
        ImVec2 GetMousePos();

        /// @brief Set the mouse cursor position directly to the platform backend
        /// @param pos New mouse position in screen-space coordinates
        void SetMousePos(const ImVec2& pos);

        /// @brief Process all pending backend events
        /// @note This function should be called at the beginning of each frame 
        void PollEvents();

        /// @brief Begin recording draw commands for a new frame
        /// @details Should be called once every frame and finalized with Backend::Render()
        void NewFrame();

        /// @brief Submit and present the next frame to the backend window
        /// @note Backend::NewFrame() has to always be called before calling this method
        void Render();

        /// @brief Terminate the backend window and perform a resource cleanup
        /// @note This method is safe to be called even if the window has not been initialized successfully
        void Close();


        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// Image class factory functions
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        /// @brief Allocate a new image without uploading and binding pixel data 
        /// @param width Initial image width in texels
        /// @param height Initial image height in texels
        /// @param format Image format from the `ImageFormat` enum
        /// @param sampler Image sampler used for interpolation from the `ImageSampler` enum
        Image* CreateImage(uint32_t width, uint32_t height, ImageFormat format, ImageSampler sampler = ImageSampler::NEAREST);

        /// @brief Allocate a new image and upload and bind initial pixel data 
        /// @param width Initial image width in texels
        /// @param height Initial image height in texels
        /// @param data Pointer to pixel array to upload and bind. The size of the array must be equal to `(width * height * channels)`,
        ///     where `channels` is the number of channels in the image, based on the image format used
        /// @param format Image format from the `ImageFormat` enum
        /// @param sampler Image sampler used for interpolation from the `ImageSampler` enum
        Image* CreateImage(uint32_t width, uint32_t height, const void* data, ImageFormat format, ImageSampler sampler = ImageSampler::NEAREST);

        /// @brief Destroy a previously allocated image
        /// @param image Pointer to the image
        /// @note Backend images get automatically deallocated when the module gets shut down. 
        ///     Use this method to free images manually during the runtime
        void DestroyImage(Image* image);

    } // namespace Backend
} // namespace yart
