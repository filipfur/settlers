#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include "ecscomponent.h"

/*struct TranslationRotationScale
{
    glm::vec3 translation{0.0f};
    glm::quat rotation{1.0f, 0.0f, 0.0f, 0.0f};
    glm::vec3 scale{1.0f};
};*/



struct Wiggable
{
    bool wiggeling{true};
};

struct Time
{
    float seconds;
    float increment;
};

namespace component
{
    typedef ecs::Component<glm::vec3,0> Translation;
    typedef ecs::Component<glm::quat> Rotation;
    typedef ecs::Component<glm::vec3,1> Scale;
    typedef ecs::Component<glm::mat4,0> ModelMatrix;
    //typedef ecs::Component<TranslationRotationScale> TranslationRotationScale;
    typedef ecs::Component<Wiggable> Wiggable;
    typedef ecs::Component<Time,0,true> Time;
}
