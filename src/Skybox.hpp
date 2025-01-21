#pragma once

#include "geometry.h"

#include <string>

class Skybox {
private:
  int m_width;
  int m_height;
  int m_channel_count;
  unsigned char* m_image;
  
public:
  Skybox(std::string path);
  const vec3 GetDirectionColor(const vec3& direction) const;
  
  ~Skybox();
};
