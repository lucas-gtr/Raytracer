#pragma once

#include "Material.hpp"

#include "geometry.h"

class Object;

struct RayHit {
  const Object* object_ptr;
  float distance;
  vec3 position;
  vec3 normal;
};

class Object {
private:
  Material m_material;
  
protected:
  vec3 m_position;
  vec3 m_rotation;
  vec3 m_scale;
  
public:
  Object(const Material& material, const vec3& position = vec3(0.0), const vec3& rotation = vec3(0.0), const vec3& scale = vec3(1.0)) : m_material(material), m_position(position), m_rotation(rotation), m_scale(scale) { }
  
  const Material& material() const { return m_material; }
  
  virtual const RayHit RayIntersection(const vec3& origin, const vec3& dir) const = 0;
  
  virtual ~Object() = default;
};
