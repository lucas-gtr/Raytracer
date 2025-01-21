#include "Skybox.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Skybox::Skybox(std::string path) {
  m_image = stbi_load(path.c_str(), &m_width, &m_height, &m_channel_count, 0);
  if (m_image) {
    std::cout << "Skybox " << path << " loaded!" << '\n';
  }
}

const vec3 Skybox::GetDirectionColor(const vec3& direction) const {
  double theta = std::acos(direction.y);
  double phi = std::atan2(direction.x, -direction.z);
  if (phi < 0) phi += 2 * M_PI;
  
  int x = static_cast<int>((phi / (2 * M_PI)) * m_width);
  int y = static_cast<int>((theta / M_PI) * m_height);
  
  int index = (y * m_width + x) * m_channel_count;
  
  vec3 color = vec3(m_image[index], m_image[index + 1], m_image[index + 2]);
  
  return color / 255;
}

Skybox::~Skybox() {
  stbi_image_free(m_image);
}

