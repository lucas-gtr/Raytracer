#pragma once

#include "vector.h"

class Light {
private:
  vec3 m_position;
  vec3 m_color;
  float m_intensity;
  
public:
  Light(const vec3& pos, const vec3& color, const float& intensity) : m_position(pos), m_color(color), m_intensity(intensity) { }
  
  const vec3& position() const { return m_position; }
  const vec3& color() const { return m_color; }
  const float& intensity() const { return m_intensity; }
};
