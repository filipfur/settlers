#pragma once

#include "globject.h"

class Intersection;

class Building : public lithium::Object
{
public:
    Building(std::shared_ptr<lithium::Mesh> mesh, const std::vector<lithium::Object::TexturePointer>& textures, Intersection* intersection)
        : lithium::Object{mesh, textures}, _intersection{intersection}
    {

    }

    Building(const Building& other) : lithium::Object{other}
    {

    }

    virtual ~Building() noexcept
    {

    }

    Intersection* intersection() const
    {
        return _intersection;
    }

private:
    Intersection* _intersection;
}