#pragma once

#include <vector>
#include <cstdlib>
#include <ctime>

#include<glm/glm.hpp>

class WorleyNoise
{
public:
    static std::vector<float> generate(const glm::vec3& volumeResolution, const glm::vec3& latticeResolution);
};