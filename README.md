# Ray Tracer

## 1. Key Concepts
  - Interactive, Real-time* Path Tracing
  - CPU based rendering with multithreading (Thread-pool based)
  - Static scenes defined by triangle meshes
  - Physically Based Rendering
  
 
</br>

## 2. Project Modules

### Scene 
  - Basic OBJ file parsing for vertex-normal-uv data
  - Pre-computed Bounding Volume Hierarchy as Acceleration Structure (AABB-based, Surface Area Heuristic model)
  - Per-object materials
  - Material index table
  - Solid color/HDRI Skyboxes
  - Ambient illumination from the skybox
  - Point and Area lights
  
### Material
  - Solid/Texture sampled diffuse color 
  - PBR: Roughness, Specular, Metalic...
  - Soft edges using normal interpolation
  - BRDF/Direct lighting illumination
  
### Renderer (Raytracer)
  - Monte Carlo Integration with Importance Sampling 
  - [Next Event Estimation] light sampling
  - Distribution Ray Tracing for antialiasing, soft shadows, DoF, etc.
  - Triangle-Ray intersection with Möller-Trumbore
  - Hit/Miss "shader" pipeline
  - Denoising and Antialiasing using jitter accumulation
  - Rendering to GPU texture


</br>

## 3. Dependencies
  - GLFW
  - Vulkan
  - Dear ImGui
  - glm
  
  
</br>

## 4. Development Waypoints

  - 1. Project Initialization 
    - [ ] Basic GLFW + Vulkan window
    - [ ] GLM setup
    - [ ] ImGui debugging UI setup
    - [ ] Pixel buffer to GPU texture rendering
    
  - 2. Triangle Ray Tracing
    - [ ] Camera+Ray definition
    - [ ] Triangle-Ray intersection
    - [ ] Sample UV Hit-shader
    - [ ] Thread-pool parallelization 
    
  - 3. Scene definition
    - [ ] Interactive camera transforms 
    - [ ] OBJ file parsing
    - [ ] HDRI loading
    - [ ] Miss-shader for skyboxes
    
  - 4. Light definition
    - [ ] Point lights
    - [ ] Direct lighting illumination/shadows
    - [ ] Soft shadows with Distribution Ray Tracing
    
  - 5. Material rendering
    - [ ] Diffuse color
    - [ ] Global illumination
    - [ ] Camera jitter+accumulation AA
    - [ ] Diffuse texture materials
    
  - 6. Optimization
    - [ ] SAH-BVH Acceleration Structure
    - [ ] Next Event Estimation
 
  - 7. ...
    - [ ] Camera redefinition for DoF
    - [ ] PBR materials
    - [ ] Roughness, bump-map texures 
  
  
 </br> 
 
 ## 5. Refs
  - [Physically Based Rendering]
  - [Ray Tracing Gems]
  - [GPU Ray Tracing in Unity]
  - [Ray Tracing in One Weekend]
  
  
 
 
[Next Event Estimation]: https://www.cg.tuwien.ac.at/sites/default/files/course/4411/attachments/08_next%20event%20estimation.pdf
[Physically Based Rendering]: http://wiki.cgt3d.cn/mediawiki/images/a/a0/Physically_Based_Rendering%28PBRT%29_3rd_Ed%282016%29.pdf
[Ray Tracing Gems]: https://www.realtimerendering.com/raytracinggems/unofficial_RayTracingGems_v1.9.pdf
[GPU Ray Tracing in Unity]: http://three-eyed-games.com/2018/05/03/gpu-ray-tracing-in-unity-part-1/
[Ray Tracing in One Weekend]: https://raytracing.github.io/books/RayTracingInOneWeekend.html#overview
