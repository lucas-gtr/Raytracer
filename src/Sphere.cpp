#include "Sphere.hpp"

const RayHit Sphere::RayIntersection(const vec3 &origin, const vec3 &dir) const {
  RayHit ray_hit;
  ray_hit.object_ptr = nullptr;
  ray_hit.distance = std::numeric_limits<float>::max();
  
  float radius = m_scale.x / 2.0f;
  
  constexpr float epsilon = 1e-6f;
  if (std::abs(dir.length() - 1.0f) > epsilon) {
    std::cerr << "The direction vector 'dir' must be normalized." << '\n';
    return ray_hit;
  }
  
  vec3 to_sphere = m_position - origin;

  float projection_length = dot(to_sphere, dir);
  
  if(projection_length <= 0) return ray_hit;
  
  float squared_distance_to_ray = dot(to_sphere, to_sphere) - projection_length * projection_length;
  float squared_radius = radius * radius;
  
  if (squared_distance_to_ray > squared_radius) return ray_hit;
  
  float offset = sqrtf(squared_radius - squared_distance_to_ray);
  float t0 = projection_length - offset;
  float t1 = projection_length + offset;
  
  ray_hit.distance = (t0 >= 0) ? t0 : t1;
  if (ray_hit.distance < 0) return ray_hit;
  
  ray_hit.object_ptr = this;
  ray_hit.position = origin + dir * ray_hit.distance;
  ray_hit.normal = (ray_hit.position - m_position).normalized();
  
  return ray_hit;
}
