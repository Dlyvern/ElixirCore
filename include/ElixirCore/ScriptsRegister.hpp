#ifndef SCRIPTS_REGISTER_HPP
#define SCRIPTS_REGISTER_HPP

#include "ScriptComponent.hpp"
#include <functional>
#include <memory>
#include "Script.hpp"

class ScriptsRegister
{
public:
    using ScriptFactory = std::function<std::shared_ptr<Script>()>;

    static ScriptsRegister& instance();

    void registerScript(const std::string& name, const ScriptFactory& func);

    std::shared_ptr<Script> createScript(const std::string& scriptName) const;

    std::vector<std::string> getScriptNames() const;
private:
    std::vector<std::string> m_scriptNames;
    std::unordered_map<std::string, ScriptFactory> m_factories;
};

#endif //SCRIPTS_REGISTER_HPP
