#include <glad/glad.h>

#include "Shader.hpp"
#include "Logger.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>

#include <glm/gtc/type_ptr.hpp>

namespace
{
    std::string readFile(const std::string& filePath)
    {
        std::ifstream file(filePath);

        if (!file.is_open())
            return {};

        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }

    int checkCompileErrors(const unsigned int shader, const char* type)
    {
        int success;
        char infoLog[1024];

        if (type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
                LOG_ERROR("Shader compilation error of type: %d %s", type, infoLog);
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);

            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
                LOG_ERROR("Shader linking error of type: %d %s", type, infoLog);
            }
        }

        return success;
    }
}

elix::Shader::Shader() = default;

elix::Shader::Shader(const std::string &vertexPath, const std::string &fragmentPath, const std::string& geometryPath)
{
    load(vertexPath, fragmentPath, geometryPath);
}

void elix::Shader::load(const std::string &vertexPath, const std::string &fragmentPath, const std::string& geometryPath)
{
    if (m_id)
    {
        glDeleteProgram(m_id);
        m_uniformCache.clear();
    }

    const std::string vertexSource = ::readFile(vertexPath);
    const std::string fragmentSource = ::readFile(fragmentPath);

    const char* vertexCode = vertexSource.c_str();
    const char* fragmentCode = fragmentSource.c_str();

    const GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertexCode, nullptr);
    glCompileShader(vertex);
    ::checkCompileErrors(vertex, "VERTEX");

    const GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragmentCode, nullptr);
    glCompileShader(fragment);
    ::checkCompileErrors(fragment, "FRAGMENT");

    GLuint geometry = 0;

    if (!geometryPath.empty())
    {
        const std::string geometrySource = ::readFile(geometryPath);
        const char* geometryCode = geometrySource.c_str();

        geometry = glCreateShader(GL_GEOMETRY_SHADER);

        glShaderSource(geometry, 1, &geometryCode, nullptr);
        glCompileShader(geometry);
        ::checkCompileErrors(geometry, "GEOMETRY");
    }

    const int tempID = glCreateProgram();
    glAttachShader(tempID, vertex);
    glAttachShader(tempID, fragment);

    if (geometry)
        glAttachShader(tempID, geometry);

    glLinkProgram(tempID);

    if (::checkCompileErrors(tempID, "PROGRAM"))
    {
        if (m_id != -1)
            glDeleteProgram(m_id);

        m_id = tempID;
    }
    else
        LOG_ERROR("Shader failed to compile ", vertexPath.c_str(), fragmentPath.c_str());

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    if (geometry)
        glDeleteShader(geometry);
}

void elix::Shader::loadBinaries(const char *vertexSource, const char *fragmentSource, const char *geometrySource)
{
    if (m_id)
    {
        glDeleteProgram(m_id);
        m_uniformCache.clear();
    }

    const GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertexSource, nullptr);
    glCompileShader(vertex);
    ::checkCompileErrors(vertex, "VERTEX");

    const GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragmentSource, nullptr);
    glCompileShader(fragment);
    ::checkCompileErrors(fragment, "FRAGMENT");

    GLuint geometry = 0;

    if (geometrySource)
    {
        geometry = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometry, 1, &geometrySource, nullptr);
        glCompileShader(geometry);
        ::checkCompileErrors(geometry, "GEOMETRY");
    }

    const int tempID = glCreateProgram();
    glAttachShader(tempID, vertex);
    glAttachShader(tempID, fragment);

    if (geometry)
        glAttachShader(tempID, geometry);

    glLinkProgram(tempID);

    if (::checkCompileErrors(tempID, "PROGRAM"))
    {
        if (m_id != -1)
            glDeleteProgram(m_id);

        m_id = tempID;
    }
    else
        LOG_ERROR("Shader failed to compile embedded sources");

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    if (geometry)
        glDeleteShader(geometry);
}

void elix::Shader::bind() const
{
    glUseProgram(m_id);
}

void elix::Shader::unbind() const
{
    glUseProgram(0);
}

int elix::Shader::getId() const
{
    return m_id;
}

bool elix::Shader::isValid() const
{
    return m_id != 0;
}

void elix::Shader::setMat4Array(const std::string &name, const std::vector<glm::mat4> &value) const
{
    GLint location = getUniformLocation(name);
    glUniformMatrix4fv(location, value.size(), GL_FALSE, glm::value_ptr(value[0]));
}

void elix::Shader::setMat4(const std::string& name, const glm::mat4& value) const
{
    const GLint location = getUniformLocation(name);
    glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
}

void elix::Shader::setVec3(const std::string& name, const glm::vec3 &value) const
{
    const GLint location = getUniformLocation(name);
    glUniform3fv(location, 1, &value[0]);
}

void elix::Shader::setVec4(const std::string& name, const glm::vec4 &value) const
{
    const GLint location = getUniformLocation(name);
    glUniform4fv(location, 1, &value[0]);
}

void elix::Shader::setFloat(const std::string& name, float value) const
{
    const GLint location = getUniformLocation(name);
    glUniform1f(location, value);
}

void elix::Shader::setInt(const std::string& name, int value) const
{
    const GLint location = getUniformLocation(name);
    glUniform1i(location, value);
}

elix::Shader::~Shader() = default;

int elix::Shader::getUniformLocation(const std::string& name) const
{
    if (const auto it = m_uniformCache.find(name); it != m_uniformCache.end())
        return it->second;

    const GLint location = glGetUniformLocation(m_id, name.c_str());

    if (location == -1)
        LOG_WARN("Uniform ", name, " not found");

    m_uniformCache[name] = location;

    return location;
}
