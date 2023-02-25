#include "ecscomponent.h"
#include "ecsentity.h"
#include "ecssystem.h"

#include <cassert>

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
};

typedef ecs::Component<vec3,0> PositionComponent;
typedef ecs::Component<vec3,1> ScaleComponent;

int main(int argc, const char* argv[])
{
    MyEntity ent{0x11};
    ecs::Component<float>::attach(ent);
    //PositionComponent::attach(ent);
    //ScaleComponent::attach(ent);
    ecs::attach<PositionComponent,ScaleComponent>(ent);

    assert(ent.hasComponents(0b111));
    assert(ent.hasComponents(0b011));
    assert(ent.hasComponents(0b101));
    assert(ent.hasComponents(0b100));
    assert(ent.hasComponents(0b1001) == false);
    ecs::Component<double>::attach(ent);
    assert(ent.hasComponents(0b1001));

    assert(ent.hasComponents(0b1111));
    ScaleComponent::detach(ent);
    assert(ent.hasComponents(0b1111) == false);
    assert(ent.hasComponents(0b1011));

    assert(ent.hasComponents(0b1000));


    //ecs::System<PositionComponent> _printSystem{};

    return 0;
}