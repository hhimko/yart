/// @brief Camera class for defining a viewing context for rendering 
class Camera {
public:
    /// @brief Get an array of ray directions for a specified screen size
    /// @param width Rendered image width in pixels
    /// @param height Rendered image height in pixels
    /// @return Ray directions array of size `width * height`
    const glm::vec3* GetRayDirections(uint32_t width, uint32_t height);

private:
    /// @brief Recalculate the camera ray directions cache for a specified screen size
    /// @param dirs Output array of size `width * height`
    /// @param width Rendered image width in pixels
    /// @param height Rendered image height in pixels
    void RecalculateCache(glm::vec3* dirs, uint32_t width, uint32_t height);

private:
    /// @brief World space position 
    glm::vec3 m_position = { 0.0f, 0.0f, -5.0f };

    /// @brief Normalized look-at vector
    glm::vec3 m_lookDirection = { .0f, .0f, 1.0f };

    /// @brief Horizontal camera FOV in degrees
    float m_fieldOfView = 60.0f;

    /// @brief Cached ray directions for a given output image size
    std::vector<glm::vec3> m_rayDirectionsCache;

    /// @brief Ray direction cache dimensions
    uint32_t m_cacheWidth, m_cacheHeight;
};
