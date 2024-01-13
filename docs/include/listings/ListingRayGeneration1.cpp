void Camera::GetRayDirections(glm::vec3* dirs, uint32_t width, uint32_t height)
{
    // Calculate the view matrix inverse
    const glm::mat4x4 view_matrix = CreateViewMatrix(m_lookDirection);
    const glm::mat4x4 view_matrix_inverse = glm::inverse(view_matrix);

    // Calculate the projection matrix inverse
    const float fov = glm::radians(m_fieldOfView);
    static constexpr float near_clip = 0.01f;
    const glm::mat4x4 projection_matrix_inverse = 
        CreateInverseProjectionMatrix(fov, width, height, near_clip);
    
    // Combine matrices into a view projection matrix inverse 
    const glm::mat4x4 inverse_view_projection_matrix = 
        view_matrix_inverse * projection_matrix_inverse;

    // Calculate ray directions for each pixel
    for (size_t y = 0; y < height; ++y) {
        for (size_t x = 0; x < width; ++x) {
            const glm::vec4 pos = glm::vec4(x + 0.5f, y + 0.5f, 1.0f, 1.0f);
            const glm::vec4 dir = inverse_view_projection_matrix * pos;
            dirs[y * width + x] = glm::normalize(glm::vec3(dir));
        }
    }
}
