#pragma once

#include <memory>
#include <set>
#include <functional>

#include <glm/gtc/constants.hpp>
#include "gltext.h"
#include "globject.h"
#include "goptions.h"
#include "intersection.h"

class Tile
{
public:
    Tile(std::shared_ptr<lithium::Object> object, std::shared_ptr<lithium::Text> text) : _object{object}, _text{text}
    {

    }

    virtual ~Tile() noexcept
    {

    }

    void update(float dt)
    {
        _object->update(dt);
        _text->update(dt);
    }

    std::shared_ptr<lithium::Object> object() const
    {
        return _object;
    }

    std::shared_ptr<lithium::Text> text() const
    {
        return _text;
    }

    void addIntersection(std::shared_ptr<Intersection> intersection)
    {
        glm::vec3 d = intersection->object()->position() - _object->position();
        float val = (atan2(d.z, d.x) / glm::pi<double>() + 1.0f) * 0.5f;
        int index = (int)(val * goptions::intersectionsPerTile);
        index = std::max(0, std::min(goptions::intersectionsPerTile - 1, index));
        _intersections[index] = intersection;
    }

    void forEachIntersectionTuple(const std::function<void(std::shared_ptr<Intersection>,std::shared_ptr<Intersection>)>& callback)
    {
        for(int i{0}; i < goptions::intersectionsPerTile; ++i)
        {
            callback(_intersections[i == 0 ? goptions::intersectionsPerTile - 1 : i - 1], _intersections[i]);
        }
    }

private:
    std::shared_ptr<lithium::Object> _object;
    std::shared_ptr<lithium::Text> _text;
    std::shared_ptr<Intersection> _intersections[goptions::intersectionsPerTile];
};