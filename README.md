# Ray Tracer

## Key Concepts
  - Interactive, Real-time* Path Tracing
  - CPU based rendering with multithreading (Thread-pool based)
  - Static scenes defined by triangle meshes
  - Physically Based Rendering
  
 
</br>

## Project Modules

### Scene 
  - Basic OBJ file parsing for vertex-normal-uv data
  - Pre-computed Bounding Volume Hierarchy as Acceleration Structure (AABB-based, Surface Area Heuristic model)
  - Per-object materials
  - Material index table
  - Solid color/HDRI Skyboxes
  
### Material
  - Solid/Texture sampled diffuse color 
  - PBR: Roughness, Specular, Metalic...
  - Soft edges using normal interpolation
  
### Renderer (Raytracer)
  - Monte Carlo Integration with Importance Sampling 
  - Importance Sampling using [Next Event Estimation]
  - Denoising and Antialiasing using jitter accumulation


</br>

## Dependencies
  - GLFW
  - Vulkan
  - Dear ImGui
  - glm
  
  
 </br> 
 
 ## Refs
  - [Physically Based Rendering]
  - [Ray Tracing Gems]
  - [GPU Ray Tracing in Unity]
  
 
 
[Next Event Estimation]: https://www.cg.tuwien.ac.at/sites/default/files/course/4411/attachments/08_next%20event%20estimation.pdf
[Physically Based Rendering]: http://wiki.cgt3d.cn/mediawiki/images/a/a0/Physically_Based_Rendering%28PBRT%29_3rd_Ed%282016%29.pdf
[Ray Tracing Gems]: https://www.realtimerendering.com/raytracinggems/unofficial_RayTracingGems_v1.9.pdf
[GPU Ray Tracing in Unity]: http://three-eyed-games.com/2018/05/03/gpu-ray-tracing-in-unity-part-1/
