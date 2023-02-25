#pragma once

#include <unordered_map>
#include "globject.h"
#include "pathway.h"
#include "glupdateable.h"
#include "assetfactory.h"

class Intersection : public lithium::Updateable
{
public:
    enum class State
    {
        Idle,
        Dropable,
        Settlement,
        City,
        Inactive
    };

    Intersection(std::shared_ptr<lithium::Object> object) : _object{object}
    {

    }

    virtual ~Intersection() noexcept
    {

    }

    void update(float dt)
    {
        _object->update(dt);
        switch(_state)
        {
            case Intersection::State::Dropable:
                _object->setScale(1.0f + sin(_object->time() * 8.0f) * 0.06f);
                break;
            default:
                _object->setScale(1.0f);
                break;
        }
    }

    void setState(Intersection::State state)
    {
        switch(state)
        {
            case Intersection::State::Idle:
                _object->setVisible(false);
                break;
            case Intersection::State::Dropable:
                _object->setVisible(true);
                _object->setColor(glm::vec3{0.5f, 1.0f, 0.0f});
                _object->setMesh(AssetFactory::getMeshes()->indicator);
                break;
            case Intersection::State::Settlement:
                _object->setMesh(AssetFactory::getMeshes()->tower);
                _object->setColor(glm::vec3{1.0f});
                _object->setVisible(true);
                break;
            case Intersection::State::Inactive:

                break;
        }
        _state = state;
    }

    State state() const
    {
        return _state;
    }

    std::shared_ptr<lithium::Object> object() const
    {
        return _object;
    }

    void connect(Intersection* intersection, std::shared_ptr<Pathway> pathway)
    {
        _connections.emplace(intersection, pathway);
    }

    bool connectedTo(Intersection* intersection)
    {
        return _connections.find(intersection) != _connections.end();
    }

    void forEachConnection(const std::function<void(Intersection*)>& callback)
    {
        for(auto connection : _connections)
        {
            callback(connection.first);
        }
    }

private:
    std::shared_ptr<lithium::Object> _object;
    std::unordered_map<Intersection*, std::shared_ptr<Pathway>> _connections;
    Intersection::State _state{Intersection::State::Idle};
};