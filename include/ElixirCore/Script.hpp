#ifndef SCRIPT_HPP
#define SCRIPT_HPP

#include <string>

class GameObject;

class Script
{
public:
    virtual ~Script() = default;

    virtual void onStart() = 0;
    virtual void onUpdate(float deltaTime) = 0;
    virtual std::string getScriptName() const = 0;

    void setOwner(GameObject* owner) { owner_ = owner; }

protected:
    GameObject* owner_{nullptr};
};

#endif //SCRIPT_HPP
