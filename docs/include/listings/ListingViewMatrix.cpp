/// @brief Create a world space to camera space transformation matrix
/// @param look_dir Camera's normalized look-at vector
/// @return The view matrix
glm::mat4x4 CreateViewMatrix(glm::vec3 const& look_dir)
{
    glm::vec3 right = -glm::normalize(glm::cross(look_dir, up));
    glm::vec3 up = glm::cross(-u, look_dir);

    glm::mat4x4 view_matrix(0);
    view_matrix[0][0] = right.x;
    view_matrix[1][0] = right.y;
    view_matrix[2][0] = right.z;
    view_matrix[0][1] = -up.x;
    view_matrix[1][1] = -up.y;
    view_matrix[2][1] = -up.z;
    view_matrix[0][2] = look_dir.x;
    view_matrix[1][2] = look_dir.y;
    view_matrix[2][2] = look_dir.z;
    view_matrix[3][3] = 1.0f;

    return view_matrix;
}
