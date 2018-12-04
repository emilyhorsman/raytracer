# Raytracer

## Recommended reading

- Doxygen output in `html/files.html` to see an overview of which sources files are responsible for various features.
- `README.pdf` for an overview of the features, sample images, instructions on compiling, running, and scene files.

## Spelunking this codebase

Q: Where do you handle the GLUT window? <br>
A: `main.cc`

Q: Where do you actually render an image into the GLUT window? <br>
A: `Utility.cc`

Q: Where do you render to a PPM image? <br>
A: `ImageFile.cc`

Q: Where does all the ray tracing begin? <br>
A: `Renderer.cc`, particularly `Renderer::render` and `RenderThread::trace`

Q: Where do you handle anti-aliasing? <br>
A: `Renderer.cc`, particularly `RenderThread::renderPixel` and `RenderThread::computeAntiAliasingSample`

Q: Where do you handle refraction? <br>
A: `Renderer.cc`, particularly `RenderThread::trace` and `computeRefractionDir`

Q: Where do you handle depth of field? <br>
A: `Renderer.cc` and `Camera.cc`, particularly `RenderThread::computePrimaryRay` and `Camera::computePrimaryRay`

Q: Where do you handle soft shadows? <br>
A: `PointLight.cc`, particularly `PointLight::direction`

Q: Where do you handle multi-threading? <br>
A: `Renderer.cc`, particularly `Renderer::render`, `Renderer::getWork`, and `RenderThread::run`

Q: Where do you parse scene files? <br>
A: `SceneFile.cc`, particularly `loadSceneFile`

Q: Where do you generate the checkerboard texture? <br>
A: `Material.cc`, particularly `CheckerboardMaterial::getColor`

Q: Where do you compute texture mapping for spheres? <br>
A: `Objects.cc`, particularly `Sphere::getColor`

Q: Where do you compute ray-object intersections? <br>
A: `Objects.cc`, every subclass of `SceneObject` has an `intersect` function
