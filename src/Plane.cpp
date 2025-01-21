#include "Plane.hpp"

const RayHit Plane::RayIntersection(const vec3 &origin, const vec3 &dir) const {
  RayHit ray_hit;
  ray_hit.object_ptr = nullptr;
  ray_hit.distance = std::numeric_limits<float>::max();
  
  constexpr float epsilon = 1e-6f;
  if (std::abs(dir.length() - 1.0f) > epsilon) {
    throw std::invalid_argument("The direction vector 'dir' must be normalized.");
  }
  
  vec3 normal(0.0f);
  normal[m_axis] = 1.0f;
  
  float denom = dot(normal, dir);
  if (std::abs(denom) < epsilon) return ray_hit;
  
  float t = dot(m_position - origin, normal) / denom;
  if (t < 0) return ray_hit;
  
  vec3 intersection_point = origin + dir * t;
  vec3 local_point = intersection_point - m_position;
  
  if (std::abs(local_point.x) > m_scale.x * 0.5f ||
      std::abs(local_point.y) > m_scale.y * 0.5f ||
      std::abs(local_point.z) > m_scale.z * 0.5f) {
    return ray_hit;
  }
  
  if (dot(dir, normal) > 0.0f) {
    normal = -normal;
  }
  
  ray_hit.object_ptr = this;
  ray_hit.distance = t;
  ray_hit.position = intersection_point;
  ray_hit.normal = normal;
  
  return ray_hit;
}
