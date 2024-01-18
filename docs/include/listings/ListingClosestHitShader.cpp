/// @brief 
/// @param ray 
/// @param payload 
/// @param world 
/// @return 
glm::vec3 ClosestHitShader(const Ray& ray, const HitPayload& payload, const Scene& scene)
{
    float diffuse = 0.0f, specular = 0.0f;
    for (size_t i = 0; i < scene.numLights; ++i) {
        const glm::vec3 light_pos = scene.lights[i].position;
        const float light_intensity = scene.lights[i].intensity;

        // Compute the distance and direction from intersection to the light
        const float dist = glm::distance(payload.hitPosition, light_pos);
        const glm::vec3 dir = glm::normalize(light_pos - payload.hitPosition);
        
        // Compute the diffuse term
        float k_d = payload.hitObject.materialDiffuseIntensity;
        float i_d = glm::max(0.0f, glm::dot(payload.hitNormal, dir));
        diffuse += k_d * light_intensity * i_d;

        // Compute the specular term
        glm::vec3 h = glm::normalize(-ray.direction + dir);
        float n = payload.hitObject.materialSpecularFalloff;
        float k_s = payload.hitObject.materialSpecularIntensity;
        float i = glm::pow(glm::max(0.0f, glm::dot(payload.hitNormal, h)), n);
        specular += k_s * light_intensity * i_s;
    }

    const glm::vec3 i_a = scene.worldAmbientIntensity * scene.worldAmbientColor;
    return i_a + payload.hitObject.materialColor * diffuse + specular
}
