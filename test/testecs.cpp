#include "ecscomponent.h"
#include "ecsentity.h"
#include "ecssystem.h"

#include <cassert>
#include <iostream>

class MyEntity : public ecs::Entity
{
    public:
        MyEntity(uint8_t mesh) : _mesh{mesh}
        {

        }

    private:
        uint8_t _mesh;
};

struct vec3
{
    float x;
    float y;
    float z;

    vec3 operator+(vec3 other)
    {
        return vec3{x+other.x, y+other.y, z+other.z};
    }
};

struct LevelUpData
{
    int level{1};
    float experience{0};
};

struct Time
{
    float seconds{0.0f};
    float increment{0.0f};
};

typedef ecs::Component<vec3,0> PositionComponent;
typedef ecs::Component<vec3,1> ScaleComponent;
typedef ecs::Component<Time,0,true> TimeComponent;

#define ECS_SYSTEM(system, entities, body, ...) system.update<void(const ecs::Entity* entity,__VA_ARGS__)>(entities, \
[&](const ecs::Entity* entity, __VA_ARGS__) body);

int main(int argc, const char* argv[])
{
    MyEntity ent{0x11};
    MyEntity turtos{0x0};
    ecs::Entity zebra;
    TimeComponent::attach(ent);
    //PositionComponent::attach(ent);
    //ScaleComponent::attach(ent);
    ecs::attach<PositionComponent,ScaleComponent>(ent);
    ecs::attach<PositionComponent>(turtos);
    ecs::attach<PositionComponent,ScaleComponent>(zebra);

    assert(ent.hasComponents(0b111));
    assert(ent.hasComponents(0b011));
    assert(ent.hasComponents(0b101));
    assert(ent.hasComponents(0b100));
    assert(ent.hasComponents(0b1001) == false);
    ecs::Component<double>::attach(ent);
    assert(ent.hasComponents(0b1001));
    assert(ent.hasComponents(0b1111));
    ecs::Component<double>::detach(ent);
    assert(ent.hasComponents(0b1111) == false);
    assert(ent.hasComponents(0b0111));
    assert(ent.hasComponents(0b0010));

    TimeComponent::attach(ent, turtos, zebra);

    std::vector<ecs::Entity> entities = {ent, turtos, zebra};

    ecs::System<PositionComponent,ScaleComponent> _randomSystem;
    ecs::System<ecs::Component<LevelUpData>> levelSystem;
    ecs::System<const PositionComponent> _printSystem;
    ecs::System<const TimeComponent,PositionComponent> _gravitySystem;

    _randomSystem.update(entities,
    [&](ecs::Entity& entity, vec3& pos, vec3& scale) {
        pos.x += entity.id() * 1000 + rand() % 64;
        pos.y += entity.id() * 1000 + rand() % 64;
        pos.z += entity.id() * 1000 + rand() % 64;
    });

    levelSystem.update(entities,
    [&](ecs::Entity& entity, LevelUpData& levelUpData) {
        levelUpData.experience += 0.1f;
        if(levelUpData.experience >= 1.0f)
        {
            levelUpData.level += 1;
        }
    });

    for(auto i{0u}; i < 10u; ++i)
    {
        std::cout << "Tick!" << std::endl;
        float dt = 0.1f;
        TimeComponent::get().seconds += dt;
        TimeComponent::get().increment = dt;
        TimeComponent::refresh();
        _gravitySystem.update(entities,
        [](ecs::Entity& entity, const Time& time, vec3& pos) {
            pos.y -= time.increment * 8.2f;
        });
        _printSystem.update(entities,
        [&](ecs::Entity& entity, const vec3& pos) {
            std::cout << "entity#" << entity.id() << " pos [" << pos.x << ", " << pos.y << ", " << pos.z << "]\n";
        });
    }

    //std::cout << "PositionComponent" << PositionComponent::_ts.size() << std::endl;
    //std::cout << "ScaleComponent" << ScaleComponent::_ts.size() << std::endl;

    return 0;
}