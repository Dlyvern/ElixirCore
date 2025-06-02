#ifndef SHADER_HPP
#define SHADER_HPP

#include <string>
#include <glm/mat4x4.hpp>
#include <unordered_map>

namespace elix
{
    class Shader
    {
    public:
        Shader();

        Shader(const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath = {});

        void load(const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath = {});
        void bind() const;
        void unbind() const;

        [[nodiscard]] int getId() const;
        [[nodiscard]] bool isValid() const;

        void setMat4(const std::string& name, const glm::mat4& value) const;
        void setVec3(const std::string& name, const glm::vec3& value) const;
        void setVec4(const std::string& name, const glm::vec4& value) const;
        void setFloat(const std::string& name, float value) const;
        void setInt(const std::string& name, int value) const;

        ~Shader();

    private:
        int m_id{0};
        mutable std::unordered_map<std::string, int> m_uniformCache;
        int getUniformLocation(const std::string& name) const;
    };

} //namespace elix

#endif //SHADER_HPP
