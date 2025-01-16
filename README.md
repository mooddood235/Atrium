# Atrium
Atrium is a GPU accelerated path tracer written in GLSL.

![Camera](https://github.com/mooddood235/Atrium/assets/62807754/17e96540-e867-493f-92f1-31be59419331)

# Features
- Unidirectional path tracer
- SAH BVH
- Dielectric BSDF, Conductor BRDF, Lambertian BRDF, Uber BSDF
- BSDF importance sampling
- Image based lighting
- Cumalitive rendering
- PBR metallic-roughness workflow
- Texture support
  - Albedo, metallic-roughness, emission, normal
- GLTF 2.0 support
  - KHR_materials_ior
  - KHR_materials_emissive_strength
  - KHR_materials_transmission
  - Meshes
  - Cameras
- Interactive mode
- PNG image output
- Terminal interface

# Interface 
`./Atrium [ScenePath] [EnvMapPath] [MaxDepth] [Samples] [Width] [Height] | Optional Flags`

Optional flags:

 -  `-i` for interactive mode
 -  `-o` followed by image out path (only png supported). No image output in interactive mode.
  
Press 's' to show progress in non-interactive mode.

# Gallery
![out](https://github.com/user-attachments/assets/49ccff42-0034-4dbc-b467-8fce3cce2239)
![Dragon](https://github.com/mooddood235/Atrium/assets/62807754/27bb77d2-aa7d-4556-aed3-3b83dfd99470)
![Clendintine](https://github.com/mooddood235/Atrium/assets/62807754/f69eba2e-00cb-4ed3-a765-fbb59e1f73c7)
![Chess](https://github.com/mooddood235/Atrium/assets/62807754/9220260e-4bda-4e71-9172-a84ff2507838)
![Gun](https://github.com/mooddood235/Atrium/assets/62807754/0617baa8-8c7d-452a-8b04-f57d980c4ec2)
![Bathroom](https://github.com/mooddood235/Atrium/assets/62807754/6506b029-5107-4045-ad1b-a7421d825bbd)
![plant3](https://github.com/mooddood235/Atrium/assets/62807754/375ceb78-53f5-4a02-9ca7-0d98b2238f62)
![PolyDenoised](https://github.com/mooddood235/Atrium/assets/62807754/1576cc34-b7f7-4ca6-ad61-4a0bc96b5d27)



