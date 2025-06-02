#ifndef SCRIPT_COMPONENT_HPP
#define SCRIPT_COMPONENT_HPP

#include <memory>

#include "Component.hpp"
#include <string>
#include <unordered_map>
#include "Script.hpp"

class ScriptComponent final : public Component
{
public:
    void addScript(const std::string& name);

    void update(float deltaTime) override;

    void setUpdateScripts(bool flag);

    const std::unordered_map<std::string, std::shared_ptr<Script>>& getScripts() const;

    ~ScriptComponent() override;

private:
    std::unordered_map<std::string, std::shared_ptr<Script>> m_scripts;
    bool m_updateScripts{false};
};

#endif //SCRIPT_COMPONENT_HPP
