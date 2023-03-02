#include <iostream>
#include <memory>

#include "tile.h"
#include "intersection.h"
#include "pathway.h"
#include "glapplication.h"
#include "pipeline.h"
#include "gltinyobjloader.h"
#include "glfont.h"
#include "goptions.h"
#include "glinstancedobject.h"
#include "glcollisionsystem.h"
#include "game.h"
#include "ecs.h"
#include "component.h"

class App : public lithium::Application
{
public:
    App() : Application{"settlers", glm::ivec2{1600, 900}, lithium::Application::Mode::MULTISAMPLED_4X, false}, _game{2}
    {
        static const std::vector<attr> objectAttributes = {POSITION, NORMAL, UV};

        auto tileMesh = lithium::tinyobjloader_load("assets/tile/tile.obj", objectAttributes);
        std::shared_ptr<lithium::ImageTexture> sandDiffuse{lithium::ImageTexture::load("assets/tile/diffuse-sand.png", GL_SRGB, GL_RGB)};

        AssetFactory::loadMeshes();
        AssetFactory::loadTextures();
        AssetFactory::loadFonts();
        AssetFactory::loadObjects();

        auto intersectionMesh = lithium::tinyobjloader_load("assets/intersection/intersection.obj", objectAttributes);
        //std::shared_ptr<lithium::ImageTexture> intersectionDiffuse{lithium::ImageTexture::load("assets/intersection/intersection.png", GL_SRGB, GL_RGB)};

        auto boardMesh = lithium::tinyobjloader_load("assets/board/board.obj", objectAttributes);
        std::shared_ptr<lithium::ImageTexture> boardDiffuse{lithium::ImageTexture::load("assets/board/upscale-beta.png", GL_SRGB, GL_RGB)};

        auto pathwayMesh = lithium::tinyobjloader_load("assets/pathway/pathway.obj", objectAttributes);
        std::shared_ptr<lithium::ImageTexture> pathwayDiffuse{lithium::ImageTexture::load("assets/pathway/diffuse.png", GL_SRGB, GL_RGB)};

        //std::shared_ptr<lithium::ImageTexture> fontDiffuse{lithium::ImageTexture::load("assets/Righteous.png", GL_RGB, GL_RGBA, 1, false)};
        //_font = std::make_shared<lithium::Font>(fontDiffuse, "assets/Righteous.json");

        auto cardMesh = lithium::tinyobjloader_load("assets/card/card.obj", objectAttributes);
        std::shared_ptr<lithium::ImageTexture> cardDiffuse{lithium::ImageTexture::load("assets/card/drone-master.png", GL_SRGB, GL_RGB)};

        auto floorPlaneMesh = std::shared_ptr<lithium::Mesh>(lithium::tinyobjloader_load("assets/plane.obj", objectAttributes));//, glm::vec2{20.0f}));
        _floorPlane = std::shared_ptr<lithium::Object>(
            new lithium::Object(floorPlaneMesh, {})
        );
        _floorPlane->setScale(20.0f);

        _pipeline = std::make_shared<Pipeline>(defaultFrameBufferResolution());


        _board = std::shared_ptr<lithium::Object>(new lithium::Object(boardMesh, {boardDiffuse}));
        _pipeline->addRenderable(_board.get());

        _card = std::shared_ptr<lithium::Object>(new lithium::Object(cardMesh, {cardDiffuse}));
        _card->setPosition(glm::vec3{0.0f, 0.6f, 0.0f});
        _card->setRotation(glm::vec3{-45.0f, 0.0f, 0.0f});
        _card->setVisible(false);
        _card->setScale(0.5f);
        _pipeline->addRenderable(_card.get());

        _keyCache = std::shared_ptr<lithium::Input::KeyCache>(new lithium::Input::KeyCache({GLFW_KEY_LEFT, GLFW_KEY_RIGHT}));
        input()->setKeyCache(_keyCache);

        input()->addReleasedCallback(GLFW_MOUSE_BUTTON_LEFT, [this](int key, int mods){
            /*for(auto o : _tiles)
            {
                lithium::SphereBB sphereBB{o->object()->position(), 0.5f};
                glm::vec2 normal;
                if(lithium::CollisionSystem::test2D(_currentCursor, sphereBB, normal))
                {
                    o->object()->setVisible(false);
                    break;
                }
            }*/
            for(auto o : _intersections)
            {
                lithium::SphereBB sphereBB{o->object()->position(), 0.1f};
                glm::vec2 normal;
                if(lithium::CollisionSystem::test2D(_currentCursor, sphereBB, normal))
                {
                    _game.playerSelectedStart();
                    _text->setText("Player " + std::to_string(_game.currentPlayer() + 1) + " turn");
                    o->setState(Intersection::State::Settlement);
                    o->forEachConnection([this](Intersection* intersection){
                        intersection->setState(Intersection::State::Idle);
                    });
                    break;
                }
            }
            return true;
        });

        for(int z{-4}; z < 4; ++z)
        {
            for(int x{-4}; x < 4; ++x)
            {
                glm::vec3 p0{x * (1.0f * goptions::tilePadding) - (z % 2 == 0 ? 0.0f : 0.5f * goptions::tilePadding),
                    0.0f,
                    z * (0.866025 * goptions::tilePadding)};
                if(p0.x * p0.x + p0.z * p0.z < 16.0f)
                {
                    int index{(int)_tiles.size()};
                    std::shared_ptr<lithium::Object> object{new lithium::Object(tileMesh, {sandDiffuse})};
                    std::shared_ptr<lithium::Text> text{new lithium::Text(
                        AssetFactory::getFonts()->righteousFont,
                        std::to_string(index)/*isSand ? "" : std::to_string(roll)*/,
                        1.0f / (index > 9 ? 200.0f : 160.0f))};
                    object->setPosition(p0);
                    object->setScale(0.5f);
                    text->setPosition(glm::vec3{p0.x - text->width() * 0.5f, 0.1f, p0.z + text->height() * 0.5f});
                    //text->setScale(1.0f / 128.0f);
                    text->setRotation(glm::vec3{-90.0f, 0.0f, 0.0f});
                    text->setColor(glm::vec4{glm::vec3(0.7f), 1.0f});
                    _pipeline->addRenderable(text.get());
                    //_pipeline->addRenderable(object.get());
                    ecs::Entity entity;
                    ecs::attach<component::Translation,component::Rotation,component::Scale>(entity);
                    ecs::attach<component::ModelMatrix>(entity);
                    ecs::attach<component::Time>(entity);
                    ecs::attach<component::Wiggable>(entity);
                    _entities.push_back(entity);
                    std::shared_ptr<Tile> tile{new Tile(object, text)};
                    _rows[z + goptions::numRows / 2].push_back(tile);
                    _tiles.push_back(tile);
                }
            }
        }

        _text = std::shared_ptr<lithium::Text>(new lithium::Text(
            AssetFactory::getFonts()->righteousFont,
            "Player " + std::to_string(_game.currentPlayer() + 1) + " turn",
            2.0f));
        _text->setPosition(glm::vec3{100.0f, 100.0f, 0.0f});
        _pipeline->addRenderable(_text.get());

        createIntersections(AssetFactory::getMeshes()->tower, pathwayDiffuse);

        processTiles();

        createPathways(pathwayMesh, pathwayDiffuse);

        auto roadMesh = lithium::tinyobjloader_load("assets/pathway/road.obj", objectAttributes);
        auto pathway = _pathways.at(7);
        auto road = new lithium::Object(roadMesh, {pathwayDiffuse});
        road->setPosition(pathway->object()->position());
        road->setRotation(pathway->object()->rotation());
        road->updateModel();
        _pipeline->addRenderable(road);


        _intersectionInstances = std::shared_ptr<lithium::InstancedObject<glm::mat4>>(
            new lithium::InstancedObject<glm::mat4>(intersectionMesh, {pathwayDiffuse}));
        _pathwayInstances = std::shared_ptr<lithium::InstancedObject<glm::mat4>>(
            new lithium::InstancedObject<glm::mat4>(pathwayMesh, {pathwayDiffuse}));

        createInstancesFromObjects(_intersectionInstances, _intersections);
        createInstancesFromObjects(_pathwayInstances, _pathways);

        _pipeline->addRenderable(_intersectionInstances.get());
        _pipeline->addRenderable(_pathwayInstances.get());

        _zShader = std::make_shared<lithium::ShaderProgram>( "shaders/shadowdepth.vert", "shaders/shadowdepth.frag" );
        _zBuffer = std::make_shared<lithium::FrameBuffer>(defaultFrameBufferResolution());
        _zBuffer->bind();
            _zBuffer->createTexture(GL_DEPTH_ATTACHMENT, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT);
            glDrawBuffer(GL_NONE);
            glReadBuffer(GL_NONE);
            _zBuffer->texture(GL_DEPTH_ATTACHMENT)->setWrap(GL_CLAMP_TO_BORDER);
            //_zBuffer->bindTexture(GL_DEPTH_ATTACHMENT);
            //float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
            //glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
            //glBindTexture(GL_TEXTURE_2D, 0);
        _zBuffer->unbind();

        auto msaaHandler = [this](int key, int mods) {
            if(key == GLFW_KEY_1)
            {
                _pipeline->setMsaa(true);
            }
            else if(key == GLFW_KEY_2)
            {
                _pipeline->setMsaa(false);
            }
            return true;
        };
        input()->addPressedCallback(GLFW_KEY_1, msaaHandler);
        input()->addPressedCallback(GLFW_KEY_2, msaaHandler);
        _game.start();

    }

    virtual ~App() noexcept
    {
        _tiles.clear();
        _pipeline = nullptr;
    }

    virtual void update(float dt) override
    {
        lithium::Application::update(dt);
        for(auto o : _tiles)
        {
            o->update(dt);
        }
        for(auto o : _intersections)
        {
            o->update(dt);
        }
        for(auto o : _pathways)
        {
            o->update(dt);
        }

        glm::mat4 viewProj = _pipeline->camera()->projection() * _pipeline->camera()->view();
        _zShader->setUniform("u_view", viewProj);
        _zBuffer->bind();
        {
            glClear(GL_DEPTH_BUFFER_BIT);
            lithium::Texture<unsigned char>::activate(GL_TEXTURE0);
            _floorPlane->shade(_zShader.get());
            _floorPlane->draw();
            glm::vec2 mp = input()->mousePosition(); // Default frame buffer resolution == LITHIUM_VIEWER_WIDTH ?
            //glReadBuffer(GL_DEPTH_ATTACHMENT);
            glReadPixels( mp.x, defaultFrameBufferResolution().y - mp.y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &_winZ );
        }
        _zBuffer->unbind();

        /*glm::vec3 cursorWorldPos = mouseToWorldCoordinates();
        glm::vec3 delta = cursorWorldPos - _currentCursor;
        glm::vec3 norm = glm::normalize(delta);
        float dl2 = glm::length2(delta);
        if(dl2 > 0.01f)
        {
            _currentCursor += (dl2 > 1 ? delta : norm) * 12.0f * dt;
        }
        else
        {
            _currentCursor = cursorWorldPos;
        }
        _board->setPosition(_currentCursor);*/

        _currentCursor = mouseToWorldCoordinates();

        if(_keyCache->isPressed(GLFW_KEY_LEFT))
        {
            _delta = std::max(-2.0f, _delta - dt * 2.0f);
        }
        else if(_keyCache->isPressed(GLFW_KEY_RIGHT))
        {
            _delta = std::min(2.0f, _delta + dt * 2.0f);
        }
        else
        {
            if(_delta * _delta < 0.01f)
            {
                _delta = 0.0f;
            }
            else
            {
                float norm = _delta / abs(_delta);
                _delta = _delta - norm * dt;
            }
        }
        _cameraTurn += _delta * dt;
        static const float cameraRadius{4.0f};
        _pipeline->camera()->setPosition(glm::vec3{sin(_cameraTurn) * cameraRadius, 5.0f, cos(_cameraTurn) * cameraRadius});
        _pipeline->camera()->update(dt);
        _pipeline->setTime(time());
        _pipeline->render();

        _wiggleSystem.update(_entities,[](ecs::Entity& entity, const Time& time, const Wiggable& wiggable, glm::vec3& scale){
            if(wiggable.wiggeling)
            {
                scale.x = scale.y = scale.z = 1.0f + sin(time.seconds) * 0.06f;
            }
        });
    }

    virtual void onFramebufferResized(int width, int height)
    {
        glViewport(0, 0, width, height);
    }

private:

    glm::vec3 mouseToWorldCoordinates() const
    {
        static glm::vec4 viewport{0.0f, 0.0f, defaultFrameBufferResolution()};
        glm::vec2 mousePos = input()->mousePosition();
        glm::vec3 screenPos = glm::vec3(mousePos.x, viewport.w - mousePos.y, _winZ);
        //glm::mat4 _view = glm::lookAt(_camera->position(), _camera->target(), glm::vec3{0.0f, 1.0f, 0.0f});
        //glm::mat4 modelView = _view;
        return glm::unProject(screenPos, _pipeline->camera()->view(), _pipeline->camera()->projection(), viewport);
    }

    template <typename T>
    void createInstancesFromObjects(std::shared_ptr<lithium::InstancedObject<glm::mat4>> instances, const std::vector<std::shared_ptr<T>>& objects)
    {
        for(auto o : objects)
        {
            o->object()->updateModel();
            instances->addInstance(o->object()->model());
        }
        instances->allocateBufferData();
        instances->linkBuffer({
            lithium::AttributePointer<GL_FLOAT>{0, 4, sizeof(glm::mat4), (void*)0},
            lithium::AttributePointer<GL_FLOAT>{1, 4, sizeof(glm::mat4), (void*)(sizeof(glm::vec4))},
            lithium::AttributePointer<GL_FLOAT>{2, 4, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4))},
            lithium::AttributePointer<GL_FLOAT>{3, 4, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4))},
        });
        instances->mesh()->unbind();
    }

    void createIntersections(std::shared_ptr<lithium::Mesh> mesh, std::shared_ptr<lithium::ImageTexture> texture)
    {
        auto addIntersection = [this](lithium::Object* obj, std::shared_ptr<Tile> tileA, std::shared_ptr<Tile> tileB, std::shared_ptr<Tile> tileC){
            std::shared_ptr<lithium::Object> object{obj};
            object->setPosition(glm::vec3{
                (tileA->object()->position().x + tileB->object()->position().x + tileC->object()->position().x) / 3.0f,
                0.0f,
                (tileA->object()->position().z + tileB->object()->position().z + tileC->object()->position().z) / 3.0f,
            });
            //object->setScale(0.08f);
            auto intersection = std::make_shared<Intersection>(object);
            tileA->addIntersection(intersection);
            tileB->addIntersection(intersection);
            tileC->addIntersection(intersection);
            //object->setVisible(false);
            object->setColor(glm::vec4{1.0f, 1.0f, 1.0f, 1.0f});
            intersection->setState(Intersection::State::Dropable);
            _pipeline->addRenderable(object.get());
            _intersections.push_back(intersection);
        };

        int i{0};
        for(int r{0}; r < goptions::numRows / 2; ++r)
        {
            for(int c{0}; c < _rows[r].size(); ++c)
            {
                std::shared_ptr<Tile> tileA = _rows[r][c];
                std::shared_ptr<Tile> tileB = _tiles[i + _rows[r].size()];
                std::shared_ptr<Tile> tileC = _tiles[i + _rows[r].size() + 1];
                addIntersection((new lithium::Object(mesh, {texture}))->setRotation(glm::vec3{0.0f, 180.0f, 0.0f}), tileA, tileB, tileC);
                if(c < _rows[r].size() - 1)
                {
                    std::shared_ptr<Tile> tileD = _rows[r][c + 1];
                    addIntersection(new lithium::Object(mesh, {texture}), tileA, tileD, tileC);
                }
                ++i;
            }
        }

        i = 0;
        for(int r{goptions::numRows - 1}; r > goptions::numRows / 2; --r)
        {
            for(int c{(int)_rows[r].size() - 1}; c >= 0; --c)
            {
                std::shared_ptr<Tile> tileA = _rows[r][c];
                std::shared_ptr<Tile> tileB = _tiles[_tiles.size() - 1 - (i + _rows[r].size())];
                std::shared_ptr<Tile> tileC = _tiles[_tiles.size() - 1 - (i + _rows[r].size() + 1)];
                addIntersection(new lithium::Object(mesh, {texture}), tileA, tileB, tileC);
                if(c > 0)
                {
                    std::shared_ptr<Tile> tileD = _rows[r][c - 1];
                    addIntersection((new lithium::Object(mesh, {texture}))->setRotation(glm::vec3{0.0f, 180.0f, 0.0f}), tileA, tileD, tileC);
                }
                ++i;
            }
        }
        std::cout << "n intersections: " << _intersections.size() << std::endl;
    }

    void createPathways(std::shared_ptr<lithium::Mesh> mesh, std::shared_ptr<lithium::ImageTexture> texture)
    {
        for(auto tile : _tiles)
        {
            if(tile->object()->visible())
            {
                tile->forEachIntersectionTuple([this, mesh, texture](std::shared_ptr<Intersection> a, std::shared_ptr<Intersection> b){
                    if(a && b && !a->connectedTo(b.get()))
                    {
                        glm::vec3 d = b->object()->position() - a->object()->position();
                        const float rad = atan2(d.x, d.z);
                        auto obj = std::shared_ptr<lithium::Object>(new lithium::Object(mesh, {texture}));
                        obj->setPosition(a->object()->position() + d * 0.5f);
                        obj->setRotation(glm::vec3{0.0f, glm::degrees(rad), 0.0f});
                        auto pathway = std::make_shared<Pathway>(obj);
                        _pathways.push_back(pathway);
                        a->connect(b.get(), pathway);
                        b->connect(a.get(), pathway);
                        //_pipeline->addRenderable(obj.get());
                    }
                });
            }
        }
        std::cout << "n pathways: " << _pathways.size() << std::endl;
    }

    void processTiles()
    {
        static std::vector<int> rolls = {
            2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12
        };

        static std::vector<int> tileIds = {
            0, 0, 0, 0,
            1, 1, 1,
            2, 2, 2,
            3, 3, 3, 3,
            4, 4, 4, 4
        };

        std::shared_ptr<lithium::ImageTexture> tileTextures[] = {
            std::shared_ptr<lithium::ImageTexture>(lithium::ImageTexture::load("assets/tile/diffuse-wheat.png", GL_SRGB, GL_RGB)),
            std::shared_ptr<lithium::ImageTexture>(lithium::ImageTexture::load("assets/tile/diffuse-clay.png", GL_SRGB, GL_RGB)),
            std::shared_ptr<lithium::ImageTexture>(lithium::ImageTexture::load("assets/tile/diffuse-rock.png", GL_SRGB, GL_RGB)),
            std::shared_ptr<lithium::ImageTexture>(lithium::ImageTexture::load("assets/tile/diffuse-sheep.png", GL_SRGB, GL_RGB)),
            std::shared_ptr<lithium::ImageTexture>(lithium::ImageTexture::load("assets/tile/diffuse-trees.png", GL_SRGB, GL_RGB)),
        };

        for(int r{0}; r < goptions::numRows; ++r)
        {
            for(int c{0}; c < _rows[r].size(); ++c)
            {
                if(r == 0 || r == (goptions::numRows - 1) || c == 0 || c == _rows[r].size() - 1)
                {
                    _rows[r][c]->object()->setVisible(false);
                    _rows[r][c]->text()->setVisible(false);
                }
                else
                {
                    bool isSand{r == (goptions::numRows / 2) && c == _rows[r].size() / 2 };
                    int roll{0};
                    int tileId{0};
                    if(!isSand)
                    {
                        int index = rand() % rolls.size();
                        roll = rolls[index];
                        rolls.erase((rolls.begin() + index));
                        index = rand() % tileIds.size();
                        tileId = tileIds[index];
                        tileIds.erase(tileIds.begin() + index);
                        _rows[r][c]->text()->setTextScale(1.0f / (roll > 9 ? 200.0f : 160.0f));
                        _rows[r][c]->text()->setText(std::to_string(roll));
                        _rows[r][c]->object()->setTextures({tileTextures[tileId]});
                    }
                    else
                    {
                        _rows[r][c]->text()->setVisible(false);
                    }
                }
            }
        }
    }

    std::shared_ptr<Pipeline> _pipeline{nullptr};
    std::vector<std::shared_ptr<Tile>> _tiles;
    std::vector<std::shared_ptr<Intersection>> _intersections;
    std::shared_ptr<lithium::InstancedObject<glm::mat4>> _intersectionInstances;
    std::shared_ptr<lithium::InstancedObject<glm::mat4>> _pathwayInstances;
    std::vector<std::shared_ptr<Pathway>> _pathways;
    std::shared_ptr<lithium::Text> _text;
    std::shared_ptr<lithium::Font> _font;
    std::shared_ptr<lithium::Object> _board;
    std::shared_ptr<lithium::Object> _card;
    Game _game;
    glm::vec3 _currentCursor{0.0f};
    float _cameraTurn{0.0f};
    float _delta{0.0f};
    float _winZ;
    std::shared_ptr<lithium::ShaderProgram> _zShader{nullptr};
    std::shared_ptr<lithium::FrameBuffer> _zBuffer{nullptr};
    std::shared_ptr<lithium::Object> _floorPlane{nullptr};
    std::vector<std::shared_ptr<Tile>> _rows[goptions::numRows];
    std::shared_ptr<lithium::Input::KeyCache> _keyCache;
    std::vector<ecs::Entity> _entities;
    ecs::System<component::Translation,component::Rotation,component::Scale> _transformationSystem;
    ecs::System<const component::Time,const component::Wiggable,component::Scale> _wiggleSystem;
};

int main(int argc, const char* argv[])
{
    std::unique_ptr<App> app = std::make_unique<App>();
    app->run();
    return 0;
}