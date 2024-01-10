/// @brief Create a screen-space to camera-space transformation matrix
/// @param width Width of the screen in pixels
/// @param height Height of the screen in pixels
/// @param fov Horizontal camera field of view in radians
/// @param near_clip Near clipping plane distance
/// @return The inverse projection matrix
glm::mat4x4 CreateInverseProjectionMatrix(uint32_t width, uint32_t height,
    float fov, float near_clip)
{
    float aspect_ratio = static_cast<float>(width) / height;
    float u = near_clip * glm::tan(fov / 2.0f);
    float v = u / aspect_ratio;

    glm::mat4x4 ip_matrix(0);
    ip_matrix[0][0] = 2.0f * u / width;
    ip_matrix[1][1] = 2.0f * v / height;
    ip_matrix[2][2] = near_clip;
    ip_matrix[2][0] = -u;
    ip_matrix[2][1] = -v;
    ip_matrix[3][3] = 1.0f;

    return ip_matrix;
}
