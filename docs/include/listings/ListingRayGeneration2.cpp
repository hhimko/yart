const glm::vec3* Camera::GetRayDirections(uint32_t width, uint32_t height)
{
    if (width != m_cacheWidth || height != m_cacheHeight) {
        m_rayDirectionsCache.resize(width * height);
        RecalculateCache(m_rayDirectionsCache.data(), width, height);

        m_cacheWidth = width;
        m_cacheHeight = height;
    }

    return m_rayDirectionsCache.data();
}
