////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Definitions of GLM helper utility functions
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once


#include <glm/glm.hpp>


namespace yart 
{
    namespace utils 
    {
        /// @brief Mathematical PI constant
        static constexpr float PI = 3.14159274f;

        /// @brief Helper epsilon constant
        static constexpr float EPSILON = 0.0001f; 

        /// @brief Degrees to radians conversion constant
        static constexpr float DEG_TO_RAD = PI / 180.0f;


        /// @brief Create a world space to camera space transformation matrix, otherwise known as the view matrix
        /// @param look_dir Camera look direction vector. The vector is assumed to be relative to camera position and normalized
        /// @param up Normalized up vector
        /// @tparam T A floating-point scalar type
        /// @tparam Q A value from `glm::qualifier` enum
        /// @return The view matrix
        /// @note This method is based on `glm::lookAt`, which was transformed to match YART's requirements
        template<typename T, glm::qualifier Q>
        GLM_FUNC_QUALIFIER glm::mat<4, 4, T, Q> CreateViewMatrix(glm::vec<3, T, Q> const& look_dir, glm::vec<3, T, Q> const& up)
        {
            // 'look_dir' vector is equivalent to camera's forward direction (positive z)
            glm::vec<3, T, Q> const u(-glm::normalize(glm::cross(look_dir, up))); // Camera view 'right' direction (positive x)  
            glm::vec<3, T, Q> const v(glm::cross(-u, look_dir)); // Camera view 'up' direction (positive y)

            glm::mat<4, 4, T, Q> view_matrix(0);
            view_matrix[0][0] = u.x;
            view_matrix[1][0] = u.y;
            view_matrix[2][0] = u.z;
            view_matrix[0][1] = -v.x; // | Output image pixel coordinates are flipped on the y-axis in relation to the camera rays
            view_matrix[1][1] = -v.y; // |
            view_matrix[2][1] = -v.z; // |
            view_matrix[0][2] = look_dir.x;
            view_matrix[1][2] = look_dir.y;
            view_matrix[2][2] = look_dir.z;
            view_matrix[3][3] = static_cast<T>(1.0);

            return view_matrix;
        }
        
        /// @brief Create a modified inverse camera projection matrix, which transforms raw (not normalized) screen coordinates into camera space
        /// @param fov Horizontal camera field of view 
        /// @param width Width of the screen in pixels
        /// @param height Height of the screen in pixels
        /// @param near_clip Near clipping plane distance
        /// @tparam T A floating-point scalar type
        /// @return The inverse projection matrix
        template<typename T>
        GLM_FUNC_QUALIFIER glm::mat<4, 4, T, glm::defaultp> CreateInverseProjectionMatrix(T fov, T width, T height, T near_clip)
        {
            T aspect_ratio = width / height;
            T u = near_clip * glm::tan(fov / static_cast<T>(2.0));
            T v = u / aspect_ratio;

            glm::mat<4, 4, T, glm::defaultp> inverse_projection_matrix(0);
            inverse_projection_matrix[0][0] = static_cast<T>(2.0) / width * u;  // | screen coordinate normalization + rescaling to match the aspect ratio
            inverse_projection_matrix[1][1] = static_cast<T>(2.0) / height * v; // | 
            inverse_projection_matrix[2][2] = near_clip; // z translation to the near clipping plane
            inverse_projection_matrix[2][0] = -u; // | x,y translation to lower-left camera view corner
            inverse_projection_matrix[2][1] = -v; // |
            inverse_projection_matrix[3][3] = static_cast<T>(1.0);

            return inverse_projection_matrix;
        }

        /// @brief Compute a linearly interpolated gradient from an equally spaced array of values
        /// @param values Array of gradient sampling points 
        /// @param size Size of the `values` array
        /// @param t Interpolation t parameter
        /// @tparam L Integer between 1 and 4 included that qualify the dimension of the vector
        /// @tparam T Floating-point scalar types
        /// @tparam Q Value from qualifier enum
        /// @return Linearly interpolated gradient
        template<glm::length_t L, typename T, glm::qualifier Q>
	    GLM_FUNC_QUALIFIER glm::vec<L, T, Q> LinearGradient(glm::vec<L, T, Q> const* values, size_t size, float t)
        {
            const glm::vec<L, T, Q>& v1 = values[static_cast<size_t>(glm::floor(t * (size - 1)))];
            const glm::vec<L, T, Q>& v2 = values[static_cast<size_t>(glm::ceil(t * (size - 1)))];
            float i = glm::fract(t * (size - 1));
            return v1 * (1.0f - i) + v2 * i;
        }

        /// @brief Compute a linearly interpolated gradient from an arbitrarily spaced array of values
        /// @param values Array of gradient sampling point color values
        /// @param locations Array of gradient sampling point locations 
        ///     Size of the array is expected to be equal to the `values` array size, and each value should be in the [0..1] range
        /// @param size Size of the `values` and `locations` arrays
        /// @param t Interpolation t parameter
        /// @tparam L Integer between 1 and 4 included that qualify the dimension of the vector
        /// @tparam T Floating-point scalar types
        /// @tparam Q Value from qualifier enum
        /// @return Linearly interpolated gradient
        template<glm::length_t L, typename T, glm::qualifier Q>
	    GLM_FUNC_QUALIFIER glm::vec<L, T, Q> LinearGradient(glm::vec<L, T, Q> const* values, float const* locations, size_t size, float t)
        {
            size_t k = size;
            for (size_t i = 0; i < size; ++i) {
                if (t <= locations[i]) {
                    k = i;
                    break;
                }
            }
            
            if (k == 0)
                return values[0];
            if (k == size)
                return values[size - 1];

            const glm::vec<L, T, Q>& v1 = values[k - 1];
            const glm::vec<L, T, Q>& v2 = values[k];
            float i = (t - locations[k - 1]) / (locations[k] - locations[k - 1]);
            return v1 * (1.0f - i) + v2 * i;
        }

        /// @brief Convert spherical coordinates `(r, θ, φ)` to cartesian coordinates, where the radial distance `r` is set to `1`
        /// @param yaw Angle (`φ`) of rotation around the `y` axis in radians
        /// @param pitch Angle (`θ`) of rotation around the `x` axis in radians
        /// @return Unit vector based on `yaw` and `pitch` rotations
        glm::vec3 SphericalToCartesianUnitVector(float yaw, float pitch);

    } // namespace utils
} // namespace yart
