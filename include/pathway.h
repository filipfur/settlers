#pragma once

#include <memory>
#include <unordered_set>
#include "globject.h"

class Pathway
{
public:
    Pathway(std::shared_ptr<lithium::Object> object) : _object{object}
    {

    }

    virtual ~Pathway() noexcept
    {

    }

    void update(float dt)
    {
        _object->update(dt);
    }

    std::shared_ptr<lithium::Object> object() const
    {
        return _object;
    }

private:
    std::shared_ptr<lithium::Object> _object;
};