#ifndef TEXTURES_MANAGER_HPP
#define TEXTURES_MANAGER_HPP
#include <vector>

class TexturesManager
{
public:
    TexturesManager();

    static TexturesManager& instance();

    void reset();

    int bindTexture(unsigned int textureId, unsigned int target);

private:
    std::vector<std::pair<unsigned int, unsigned int>> m_usedUnits;
    int m_nextUnit{0};
};

#endif //TEXTURES_MANAGER_HPP
