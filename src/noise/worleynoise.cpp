#include <worleynoise.hpp>

std::vector<float> WorleyNoise::generate(const glm::vec3& volumeResolution, const glm::vec3& latticeResolution) {

    std::vector<glm::vec3> lattice(latticeResolution.x * latticeResolution.y * latticeResolution.z);
    std::vector<float> volumeData(volumeResolution.x * volumeResolution.y * volumeResolution.z, 0.0f);

    srand(static_cast<unsigned int>(time(nullptr))); 

    for (size_t i = 0; i < lattice.size(); ++i)
    {
        lattice[i] = glm::vec3(
            rand() / static_cast<float>(RAND_MAX),
            rand() / static_cast<float>(RAND_MAX),
            rand() / static_cast<float>(RAND_MAX));
    }

    int latticeCountX = static_cast<int>(latticeResolution.x);
    int latticeCountY = static_cast<int>(latticeResolution.y);
    int latticeCountZ = static_cast<int>(latticeResolution.z);

    #pragma omp parallel
    {
        #pragma omp for collapse(3) schedule(dynamic)
        for (int z = 0; z < static_cast<int>(volumeResolution.z); z++)
        {
            for (int y = 0; y < static_cast<int>(volumeResolution.y); y++)
            {
                for (int x = 0; x < static_cast<int>(volumeResolution.x); x++)
                {
                    glm::vec3 latticeUV = (glm::vec3(x, y, z) / volumeResolution) * latticeResolution;

                    glm::vec3 currentCell = glm::floor(latticeUV);
                    glm::vec3 localUV = glm::fract(latticeUV);

                    float minDist = 8.0f;

                    #pragma clang loop unroll(full)
                    for (int j = -1; j <= 1; j++)
                    {
                        for (int i = -1; i <= 1; i++)
                        {
                            for (int k = -1; k <= 1; k++)
                            {
                                glm::vec3 neighborOffset = glm::vec3(i, j, k);
                                glm::vec3 targetCell = currentCell + neighborOffset;

                                glm::vec3 wrappedTargetCell = glm::mod(glm::mod(targetCell, latticeResolution) + latticeResolution, latticeResolution);

                                int latticeIdx = static_cast<int>(wrappedTargetCell.z) * static_cast<int>(latticeResolution.x) * static_cast<int>(latticeResolution.y) + static_cast<int>(wrappedTargetCell.y) * static_cast<int>(latticeResolution.x) + static_cast<int>(wrappedTargetCell.x);
                                glm::vec3 featurePointOffset = lattice[latticeIdx];

                                glm::vec3 diff = neighborOffset + featurePointOffset - localUV;
                                float dist = glm::length(diff);

                                minDist = glm::min(minDist, dist);
                            }
                        }
                    }

                    int textureIdx = z * static_cast<int>(volumeResolution.x) * static_cast<int>(volumeResolution.y) + y * static_cast<int>(volumeResolution.x) + x;
                    volumeData[textureIdx] = 1.0f - glm::clamp(minDist, 0.0f, 1.0f);
                }
            }
        }
    }

    return std::move(volumeData);
}