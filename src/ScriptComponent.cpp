#include "ScriptComponent.hpp"

#include "ScriptsLoader.hpp"
#include "ScriptsRegister.hpp"

void ScriptComponent::addScript(const std::string &name)
{
    using GetScriptsRegisterFunc = ScriptsRegister* (*)();
    auto function = (GetScriptsRegisterFunc)ScriptsLoader::instance().getFunction("getScriptsRegister", ScriptsLoader::instance().library);

    ScriptsRegister* s = function();

    auto script = s->createScript(name);

    if (!script)
        throw std::runtime_error("Script not found: " + name);

    script->setOwner(this->getOwner());
    m_scripts[name] = script;
}

void ScriptComponent::update(float deltaTime)
{
    for (auto& [name, script] : m_scripts)
        if (script) script->onUpdate(deltaTime);
}

const std::unordered_map<std::string, std::shared_ptr<Script>>& ScriptComponent::getScripts() const
{
    return m_scripts;
}

ScriptComponent::~ScriptComponent() = default;
