#ifndef MODEL_HPP
#define MODEL_HPP


#include "Skeleton.hpp"
#include "Mesh.hpp"
#include <memory>

namespace elix
{
    class Model
    {
    public:
        Model(const std::string& name, const std::vector<elix::Mesh>& meshes, std::unique_ptr<Skeleton> skeleton = nullptr);

        void draw() const;

        void addAnimation(common::Animation* animation);

        void drawWithMaterials(std::unordered_map<int, Material*>& materials) const;

        [[nodiscard]] common::Animation* getAnimation(int index) const;
        [[nodiscard]] common::Animation* getAnimation(const std::string& name) const;
        [[nodiscard]] const std::vector<common::Animation*>& getAnimations() const;
        [[nodiscard]] Skeleton* getSkeleton() const;
        [[nodiscard]] std::string getName() const;
        [[nodiscard]] size_t getNumMeshes() const;
        [[nodiscard]] elix::Mesh* getMesh(int meshIndex);
        [[nodiscard]] bool hasSkeleton() const;
    private:
        std::string m_name;
        std::vector<elix::Mesh> m_meshes;
        std::unique_ptr<Skeleton> m_skeleton{nullptr};
        std::vector<common::Animation*> m_animations;
    };
} //namespace elix

#endif //MODEL_HPP