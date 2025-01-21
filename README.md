# Ray Tracing Rendering Engine

This project implements a basic ray tracing algorithm to render a 3D scene. The engine supports reflection, refraction, shadows, and multiple light sources. It uses a simple ray-object intersection test to calculate pixel colors and outputs the result in a PPM image format.

## Features

- **Ray Tracing**: Calculates the color of each pixel by casting rays through a 3D scene.
- **Reflection and Refraction**: Simulates reflective and refractive materials, such as mirrors and glass.
- **Shadows**: Checks if a point is in shadow by testing for intersections with objects between the light source and the point.
- **Multiple Light Sources**: Supports multiple light sources in the scene with configurable intensity.
- **Skybox**: Uses an environment map to simulate distant background elements.

## Configuration

- **IMAGE_WIDTH** and **IMAGE_HEIGHT**: Dimensions of the output image (default: 1024x768).
- **PIXEL_SUBDIVISION**: Number of subdivision per pixel for anti-aliasing (default: 3).
- **BOUNCE_RAY_MAX**: Maximum number of ray bounces for reflections/refractions (default: 4).
- **FOV**: Field of view for the camera (default: 1.0f).
- **ENVIRONMENT_PATH**: Path to the environment map image (default: `"envmap.jpg"`).

## File Output

The rendered image is saved in the `out.ppm` file in the PPM format.

## Future Improvements

- **Light attenuation**: Implement light attenuation to simulate the decrease in light intensity over distance.
- **Add support for different light types**: Extend the lighting system to include various light types such as point lights, spotlights, and directional lights.
- **Add support for 3D models (OBJ files)**: Implement functionality to load and render 3D models from OBJ files.
- **Add texture mapping**: Introduce texture support, including diffuse, roughness, normal, and metal textures, to enhance material realism.

## License

This project is licensed under the MIT License - see the `LICENSE` file for details.
