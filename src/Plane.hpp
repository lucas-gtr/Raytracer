#pragma once

#include "Object.h"

class Plane : public Object {
private:
  int m_axis;
  
public:
  Plane(const vec3& position, int axis, const vec3& scale, const Material& material) :
  Object(material, position, vec3(0.0f), scale), m_axis(axis) { }
  
  const RayHit RayIntersection(const vec3& origin, const vec3& dir) const;
};
