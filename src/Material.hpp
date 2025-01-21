#pragma once

#include "vector.h"

class Material {
private:
  vec3 m_diffuse_color;
  float m_diffuse_component;
  float m_specular_component;
  float m_reflectivity;
  float m_shininess;
  float m_refractivity;
  float m_refract_indice;
  
public:
  Material(const vec3& diffuse_color, float diffuse_component, float specular_component, float shininess, float reflectivity, float refractivity, float refract_indice)
  : m_diffuse_color(diffuse_color),
  m_diffuse_component(diffuse_component),
  m_specular_component(specular_component),
  m_reflectivity(reflectivity),
  m_shininess(shininess),
  m_refractivity(refractivity),
  m_refract_indice(refract_indice){ }
  
  Material()
  : m_diffuse_color(1.0f),
  m_diffuse_component(1.0f),
  m_specular_component(0.5f),
  m_reflectivity(0.0f),
  m_shininess(32.0f),
  m_refractivity(0.0f),
  m_refract_indice(1.0f) { }
  
  const vec3& diffuse_color() const { return m_diffuse_color; }
  float diffuse_component() const { return m_diffuse_component; }
  float specular_component() const { return m_specular_component; }
  float reflectivity() const { return m_reflectivity; }
  float shininess() const { return m_shininess; }
  float refractivity() const { return m_refractivity; }
  float refract_indice() const { return m_refract_indice; }
};

