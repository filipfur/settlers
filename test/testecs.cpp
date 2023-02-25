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
    float _x;
    float _y;
    float _z;

    vec3 operator+(vec3 other)
    {
        return vec3{_x+other._x, _y+other._y, _z+other._z};
    }
};

typedef ecs::Component<vec3,0> PositionComponent;
typedef ecs::Component<vec3,1> ScaleComponent;


struct LevelUpData
{
    int level{1};
    float experience{0};
};

#define ECS_SYSTEM(system, entities, body, ...) system.update<void(const ecs::Entity* entity,__VA_ARGS__)>(entities, \
[&](const ecs::Entity* entity, __VA_ARGS__) body);

int main(int argc, const char* argv[])
{
    MyEntity ent{0x11};
    MyEntity turtos{0x0};
    ecs::Entity zebra;
    ecs::Component<LevelUpData>::attach(ent);
    //PositionComponent::attach(ent);
    //ScaleComponent::attach(ent);
    ecs::attach<PositionComponent,ScaleComponent>(ent);
    ecs::attach<PositionComponent,ScaleComponent>(turtos);
    ecs::attach<PositionComponent>(zebra);

    std::vector<ecs::Entity> entities = {ent, turtos, zebra};

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


    ecs::System<PositionComponent,ScaleComponent> _randomSystem;
    _randomSystem.update(entities,
    [&](const ecs::Entity* entity, vec3& pos, vec3& scale) {
        pos._x += entity->id() * 1000 + rand() % 64;
        pos._y += entity->id() * 1000 + rand() % 64;
        pos._z += entity->id() * 1000 + rand() % 64;
    });

    ecs::System<ecs::Component<LevelUpData>> levelSystem;
    levelSystem.update(entities,
    [&](const ecs::Entity* entity, LevelUpData& levelUpData) {
        levelUpData.experience += 0.1f;
        if(levelUpData.experience >= 1.0f)
        {
            levelUpData.level += 1;
        }
    });
    /*ECS_SYSTEM(levelSystem, entities, {
        levelUpData.experience += 0.1f;
        if(levelUpData.experience >= 1.0f)
        {
            levelUpData.level += 1;
        }
    }, LevelUpData& levelUpData);*/

    ecs::System<PositionComponent> _printSystem;
    _printSystem.update(entities,
    [&](const ecs::Entity* entity, vec3& pos) {
        std::cout << "entity#" << entity->id() << " pos [" << pos._x << ", " << pos._y << ", " << pos._z << "]\n";
    });

    return 0;
}