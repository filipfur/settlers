#pragma once

#include "gltext.h"
#include "globject.h"

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

private:
    std::shared_ptr<lithium::Object> _object;
    std::shared_ptr<lithium::Text> _text;
};