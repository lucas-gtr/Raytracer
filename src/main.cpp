#include <limits>
#include <cmath>
#include <iostream>
#include <fstream>
#include <vector>
#include <optional>

#include "geometry.h"

#include "Object.h"
#include "Sphere.hpp"
#include "Plane.hpp"
#include "Light.hpp"
#include "Skybox.hpp"

const int IMAGE_WIDTH = 1024;
const int IMAGE_HEIGHT = 768;
int PIXEL_SUBDIVISION = 1;
float WEIGHT_PER_PIXEL = 1.0 / (PIXEL_SUBDIVISION * PIXEL_SUBDIVISION);

const int BOUNCE_RAY_MAX = 4;
const float FOV = 1.0f;

std::string ENVIRONMENT_PATH = "envmap.jpg";

vec3 reflect(const vec3& dir, const vec3& normal) {
  return dir - normal * 2.f * dot(dir, normal);
}

vec3 refract(const vec3& dir, const vec3& normal, const float refractive_index) {
  float cosi = - std::max(-1.f, std::min(1.f, dot(dir, normal)));
  float etai = 1, etat = refractive_index;
  vec3 n = normal;
  if (cosi < 0) {
    cosi = -cosi;
    std::swap(etai, etat); n = -normal;
  }
  float eta = etai / etat;
  float k = 1 - eta*eta*(1 - cosi*cosi);
  return k < 0 ? vec3(0,0,0) : dir*eta + n*(eta * cosi - sqrtf(k));
}

std::optional<RayHit> GetClosestIntersection(const vec3& origin, const vec3& dir, const std::vector<Object*>& object_list) {
  RayHit closest_ray;
  closest_ray.object_ptr = nullptr;
  closest_ray.distance = std::numeric_limits<float>::max();

  for (const auto& object : object_list) {
    RayHit temp_ray = object->RayIntersection(origin, dir);
    if (temp_ray.distance < closest_ray.distance) closest_ray = temp_ray;
  }
  if (closest_ray.object_ptr != nullptr) return closest_ray;
  
  return std::nullopt;
}

vec3 OffsetPoint(const vec3& position, const vec3& normal, const vec3& direction) {
  return position + normal * 1e-3 * (dot(direction, normal) < 0 ? -1.0f : 1.0f);
}

bool IsInShadow(const vec3& shadow_origin, const vec3& light_direction, const std::vector<Object*>& object_list, float light_distance) {
  auto bounce_light_info = GetClosestIntersection(shadow_origin, light_direction, object_list);
  
  return (bounce_light_info.has_value() && bounce_light_info->distance < light_distance);
}

vec3 GetRayColor(const vec3& origin, const vec3& dir, const std::vector<Object*>& object_list, const std::vector<Light*>& light_list, Skybox* skybox, int depth = 0) {
  vec3 background_color = skybox->GetDirectionColor(dir);
  
  if(depth >= BOUNCE_RAY_MAX) return background_color;
  
  const auto& intersection = GetClosestIntersection(origin, dir, object_list);
  
  if(!intersection.has_value()) return background_color;
    
  const Object* object_ptr = intersection->object_ptr;
  const vec3 position = intersection->position;
  const vec3 normal = intersection->normal;
  
  vec3 color(0.0f);
  
  vec3 diffuse_light(0.0f);
  vec3 specular_light(0.0f);
  
  vec3 reflect_color(0.0f);
  if(object_ptr->material().reflectivity() > 0.0f) {
    vec3 reflect_dir = reflect(dir, intersection->normal).normalized();
    vec3 reflect_origin = OffsetPoint(position, normal, reflect_dir);
    reflect_color = GetRayColor(reflect_origin, reflect_dir, object_list, light_list, skybox, depth+1);
  }
  
  vec3 refract_color(0.0f);
  if(object_ptr->material().refractivity() > 0.0f) {
    vec3 refract_dir = refract(dir, intersection->normal, object_ptr->material().refract_indice());
    vec3 refract_origin = OffsetPoint(position, normal, refract_dir);
    if(refract_dir.length() != 0.0) refract_color = GetRayColor(refract_origin, refract_dir, object_list, light_list, skybox, depth+1);
  }
  
  for(auto& light : light_list) {
    vec3 light_dir = (light->position() - position).normalized();
    
    vec3 shadow_origin = OffsetPoint(position, normal, light_dir);
    if(!IsInShadow(shadow_origin, light_dir, object_list, (light->position() - position).length())) {
      diffuse_light += light->color() * light->intensity() * std::max(0.f, dot(light_dir, intersection->normal));
      specular_light += light->color() * powf(std::max(0.f, dot(-reflect(-light_dir, intersection->normal), dir)), object_ptr->material().shininess()) * light->intensity();
    }
  }
  color += object_ptr->material().diffuse_color() * object_ptr->material().diffuse_component() * diffuse_light;
  color += vec3(1.0f) * specular_light * object_ptr->material().specular_component();
  color += reflect_color * object_ptr->material().reflectivity();
  color += refract_color * object_ptr->material().refractivity();

  return color;
}

void render(const std::vector<Object*>& object_list, const std::vector<Light*>& light_list, const vec3& camera_position, Skybox* skybox) {
  std::vector<vec3> framebuffer(IMAGE_WIDTH*IMAGE_HEIGHT);
    
  const int subpixel_height = IMAGE_HEIGHT*PIXEL_SUBDIVISION;
  const int subpixel_width = IMAGE_WIDTH*PIXEL_SUBDIVISION;

  #pragma omp parallel for
  for (size_t j = 0; j<subpixel_height; j++) {
    if (j % (subpixel_height / 10) == 0 && j != 0) {
      std::cout << (j * 100 / subpixel_height) + 1 << "%" << std::endl;
    }
    for (size_t i = 0; i<subpixel_width; i++) {
      float x = float(i)/PIXEL_SUBDIVISION;
      float y = float(j)/PIXEL_SUBDIVISION;
      
      float dir_x =  (2*(x + 0.5)/(float)IMAGE_WIDTH  - 1)*tan(FOV/2.)*IMAGE_WIDTH/(float)IMAGE_HEIGHT;
      float dir_y = -(2*(y + 0.5)/(float)IMAGE_HEIGHT - 1)*tan(FOV/2.);
      
      vec3 dir = vec3(dir_x, dir_y, -1).normalized();
      
      vec3 subpixel_color = GetRayColor(camera_position, dir, object_list, light_list, skybox);
      framebuffer[int(x)+int(y)*IMAGE_WIDTH] += subpixel_color * WEIGHT_PER_PIXEL;
    }
  }
  
  std::ofstream ofs;
  ofs.open("./out.ppm");
  ofs << "P6\n" << IMAGE_WIDTH << " " << IMAGE_HEIGHT << "\n255\n";
  for (size_t i = 0; i < IMAGE_HEIGHT*IMAGE_WIDTH; ++i) {
    vec3& color = framebuffer[i];
    float max_color = std::max(color.r, std::max(color.g, color.b));
    if (max_color > 1) color = color * (1. / max_color);
    
    ofs << (char)(255 * std::max(0.f, std::min(1.f, color.r)));
    ofs << (char)(255 * std::max(0.f, std::min(1.f, color.g)));
    ofs << (char)(255 * std::max(0.f, std::min(1.f, color.b)));
  }
  ofs.close();
}

int main() {
  std::vector<Object*> object_list;
  std::vector<Light*> light_list;
  
  Material      ivory(vec3(0.4, 0.4, 0.3), 0.6,  0.3, 50.0, 0.1, 0.0f, 1.0f);
  Material red_rubber(vec3(0.9,  0.1, 0.0), 0.3, 0.1, 10.0, 0.1, 0.0f, 1.0f);
  Material green_wall(vec3(0.25,  0.45, 0.2), 0.6, 0.2, 32.0, 0.2, 0.0f, 1.0f);
  Material ground(vec3(0.6,  0.4, 0.7), 0.5, 0.1, 10.0, 0.0, 0.0f, 1.0f);
  Material     mirror(vec3(1.0, 1.0, 1.0), 0.0, 10.0, 1425., 0.8, 0.0f, 1.0f);
  Material      glass(vec3(1.0, 1.0, 1.0), 0.0, 1.0, 1425., 0.1, 0.8f, 1.5f);
  
  object_list.push_back(new Sphere(vec3(-5,    -1.5,   -20), 2,      ivory));
  object_list.push_back(new Sphere(vec3(-1.0, -1.5, -12), 2,     glass));
  object_list.push_back(new Sphere(vec3( 2.5, -0.5, -22), 3, red_rubber));
  object_list.push_back(new Sphere(vec3( 7,    8,   -28), 4,     mirror));
  
  object_list.push_back(new Plane(vec3(0.0f, -3.5f, -20.0f), 1, vec3(20.0f, 1.0f, 20.0f), ground));
  object_list.push_back(new Plane(vec3(-10.0f, 3.5f, -20.0f), 0, vec3(1.0f, 20.0f, 20.0f), green_wall));

  light_list.push_back(new Light(vec3(-20.0f, 20.0f, 20.0f), vec3(1.0f, 1.0f, 1.0f), 1.5f));
  light_list.push_back(new Light(vec3(30.0f, 50.0f, -25.0f), vec3(1.0f, 1.0f, 1.0f), 1.8f));
  light_list.push_back(new Light(vec3(30.0f, 20.0f, 30.0f), vec3(1.0f, 1.0f, 1.0f), 1.7f));
  
  Skybox* skybox = new Skybox(ENVIRONMENT_PATH);

  vec3 camera_position = vec3(0.0f, 0.0f, 0.0f);
  
  render(object_list, light_list, camera_position, skybox);
  
  for (Object* object : object_list) delete object;
  for (Light* light : light_list) delete light;
  delete skybox;

  return 0;
}
