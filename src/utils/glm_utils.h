#pragma once

#include <glm/glm.hpp>


namespace yart 
{
    namespace utils 
    {
        /// @brief Create a world space to camera space transformation matrix, otherwise known as the view matrix
        /// @param position Camera position in world space
        /// @param look_dir Camera look direction vector. The vector is assumed to be relative to camera position and normalized
        /// @param up Normalized up vector
        /// @tparam T A floating-point scalar type
        /// @tparam Q A value from `glm::qualifier` enum
        /// @return The view matrix
        /// @note This method is based on `glm::lookAt`, which was transformed to match YART's requirements
        template<typename T, glm::qualifier Q>
        GLM_FUNC_QUALIFIER glm::mat<4, 4, T, Q> CreateViewMatrix(glm::vec<3, T, Q> const& position, glm::vec<3, T, Q> const& look_dir, glm::vec<3, T, Q> const& up)
        {
            // 'look_dir' vector is equivalent to camera's forward direction (positive z)
            glm::vec<3, T, Q> const u(-glm::normalize(glm::cross(look_dir, up))); // Camera's 'left' direction (positive x)  
            glm::vec<3, T, Q> const v(glm::cross(-u, look_dir)); // Camera's 'up' direction (positive y)

            glm::mat<4, 4, T, Q> view_matrix(0);
            view_matrix[0][0] = u.x;
            view_matrix[1][0] = u.y;
            view_matrix[2][0] = u.z;
            view_matrix[0][1] = v.x;
            view_matrix[1][1] = v.y;
            view_matrix[2][1] = v.z;
            view_matrix[0][2] = look_dir.x;
            view_matrix[1][2] = look_dir.y;
            view_matrix[2][2] = look_dir.z;
            view_matrix[3][3] = static_cast<T>(1.0);

            return view_matrix;
        }
        
    } // namespace utils
} // namespace yart
