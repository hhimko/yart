#include "glm_utils.h"


namespace yart
{
    namespace utils
    {
        glm::vec3 SphericalToCartesianUnitVector(float yaw, float pitch)
        {
            // https://en.wikipedia.org/wiki/Spherical_coordinate_system

            return glm::vec3 {
                glm::cos(yaw) * glm::cos(pitch),
                glm::sin(pitch), 
                glm::sin(yaw) * glm::cos(pitch)
            };
        }

    } // namespace utils
} // namespace yart

