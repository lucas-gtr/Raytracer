#pragma once

#include "Object.h"

class Sphere : public Object {
public:
  Sphere(const vec3& center, const float radius, const Material& material) :
  Object(material, center, vec3(0.0f), vec3(radius * 2.0f)) { }
  
  const RayHit RayIntersection(const vec3& origin, const vec3& dir) const;
};
