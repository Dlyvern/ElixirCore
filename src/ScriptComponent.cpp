#include "ScriptComponent.hpp"

#include <iostream>

#include "ScriptsLoader.hpp"
#include "ScriptsRegister.hpp"

void ScriptComponent::addScript(const std::string &name)
{
    // using GetScriptsRegisterFunc = ScriptsRegister* (*)();
    //
    // auto function = (GetScriptsRegisterFunc)ScriptsLoader::instance().getFunction("getScriptsRegister", ScriptsLoader::instance().library);
    //
    // if (!function)
    // {
    //     std::cerr << "ScriptComponent::addScript(): Script " << name << " not found!" << std::endl;
    //     return;
    // }
    //
    // ScriptsRegister* s = function();

    // auto script = s->createScript(name);

    auto script = ScriptsRegister::instance().createScript(name);

        if (!script)
    {
        std::cerr << "ScriptComponent::addScript(): Script " << name << " not found!" << std::endl;
        return;
    }

    script->setOwner(this->getOwner());
    m_scripts[name] = script;
}

void ScriptComponent::update(float deltaTime)
{
    if (!m_updateScripts)
        return;

    for (auto& [name, script] : m_scripts)
        if (script) script->onUpdate(deltaTime);
}

void ScriptComponent::setUpdateScripts(bool flag)
{
    m_updateScripts = flag;
}

const std::unordered_map<std::string, std::shared_ptr<Script>>& ScriptComponent::getScripts() const
{
    return m_scripts;
}

ScriptComponent::~ScriptComponent() = default;
