#pragma once

#include <glm/glm.hpp>

struct alignas(16) Material
{
    glm::vec3 albedo;
    float _pad0;
    float metallic;
    float roughness;
    float clearcoat;
    float _pad1;
};