/// @brief Sample and return the sky/background color for a given ray
/// @param ray Traced ray 
/// @return Color of the sky at a given ray direction
glm::vec3 MissShader([[maybe_unused]] const Ray& ray)
{
    // Solid color sky shader
    static constexpr glm::vec3 sky_color = { 0.131f, 0.241f, 0.500f };
    return sky_color;
}
