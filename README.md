# Raytracer

![A scene demoing many rendering features of this ray tracer, including refraction and soft shadows.](./Sample.png)

More sample renderings available in [README.pdf](./README.pdf).

## Features

- CPU-based ray tracer
- Scene files for configuring rendering and scene parameters, materials, lights, and objects
- Support for planes, spheres, and disks
- Color and checkered material with proper texture mapping for spheres
- Materials specify coefficients for: ambient, diffuse, and specular light; transmission; index of refraction
- Multi-threaded rendering with a configurable number of threads
- Rudimentary refraction (no Fresnel effect)
- Soft shadows achieved by 'jittering' point lights and averaging multiple renders
- Anti-aliasing with both regular (uniform) and random sampling techniques
- Adjustable depth-of field and camera field of view
