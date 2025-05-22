#include "ScriptsRegister.hpp"

ScriptsRegister& ScriptsRegister::instance()
{
    static ScriptsRegister instance;
    return instance;
}

void ScriptsRegister::registerScript(const std::string &name, const ScriptFactory &func)
{
    m_factories[name] = func;
    m_scriptNames.push_back(name);
}

std::shared_ptr<Script> ScriptsRegister::createScript(const std::string &scriptName) const
{
    auto it = m_factories.find(scriptName);
    if (it != m_factories.end())
        return it->second();

    return nullptr;
}

std::vector<std::string> ScriptsRegister::getScriptNames() const
{
    return m_scriptNames;
}
