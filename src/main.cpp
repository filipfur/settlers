#include <iostream>
#include <memory>

#include "tile.h"
#include "glapplication.h"
#include "pipeline.h"
#include "gltinyobjloader.h"
#include "glfont.h"

class App : public lithium::Application
{
public:
    App() : Application{"settlers", glm::ivec2{1600, 900}, lithium::Application::Mode::MULTISAMPLED_4X, false}
    {
        auto mesh = lithium::tinyobjloader_load("assets/tile/tile.obj", {POSITION, NORMAL, UV});
        std::shared_ptr<lithium::ImageTexture> diffuseTextures[] = {
            std::shared_ptr<lithium::ImageTexture>(lithium::ImageTexture::load("assets/tile/diffuse-wheat.png", GL_SRGB, GL_RGB)),
            std::shared_ptr<lithium::ImageTexture>(lithium::ImageTexture::load("assets/tile/diffuse-clay.png", GL_SRGB, GL_RGB)),
            std::shared_ptr<lithium::ImageTexture>(lithium::ImageTexture::load("assets/tile/diffuse-rock.png", GL_SRGB, GL_RGB)),
            std::shared_ptr<lithium::ImageTexture>(lithium::ImageTexture::load("assets/tile/diffuse-sheep.png", GL_SRGB, GL_RGB)),
            std::shared_ptr<lithium::ImageTexture>(lithium::ImageTexture::load("assets/tile/diffuse-trees.png", GL_SRGB, GL_RGB)),
        };
        std::shared_ptr<lithium::ImageTexture> sandDiffuse{lithium::ImageTexture::load("assets/tile/diffuse-sand.png", GL_SRGB, GL_RGB)};

        std::shared_ptr<lithium::ImageTexture> fontDiffuse{lithium::ImageTexture::load("assets/Righteous.png", GL_RGB, GL_RGBA, 1, false)};
        _font = std::make_shared<lithium::Font>(fontDiffuse, "assets/Righteous.json");

        _pipeline = std::make_shared<Pipeline>(defaultFrameBufferResolution());

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

        for(int z{-4}; z < 4; ++z)
        {
            for(int x{-4}; x < 4; ++x)
            {
                glm::vec3 p0{x * 1.0f - (z % 2 == 0 ? 0.0f : 0.5f), 0.0f, z * 0.866025};
                if(p0.x * p0.x + p0.z * p0.z < 6.0f)
                {
                    bool isSand{x == 0 && z == 0};
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
                    }
                    std::shared_ptr<lithium::Object> object{new lithium::Object(mesh, {isSand ? sandDiffuse : diffuseTextures[tileId]})};
                    std::shared_ptr<lithium::Text> text{new lithium::Text(
                        _font,
                        isSand ? "" : std::to_string(roll),
                        1.0f / (roll > 9 ? 200.0f : 160.0f))};
                    object->setPosition(p0);
                    object->setScale(0.5f);
                    text->setPosition(glm::vec3{p0.x - text->width() * 0.5f, 0.1f, p0.z + text->height() * 0.5f});
                    //text->setScale(1.0f / 128.0f);
                    text->setRotation(glm::vec3{-90.0f, 0.0f, 0.0f});
                    text->setColor(glm::vec4{glm::vec3(0.7f), 1.0f});
                    _pipeline->addRenderable(text.get());
                    _pipeline->addRenderable(object.get());
                    _tiles.push_back(std::shared_ptr<Tile>{new Tile(object, text)});
                }
            }
        }
        //_text->setText("hej varld");
        //_text->update(0);
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
    }

    virtual ~App() noexcept
    {
        _tiles.clear();
        _pipeline = nullptr;
    }

    virtual void update(float dt) override
    {
        for(auto o : _tiles)
        {
            o->update(dt);
        }
        float t{time() * 0.05f};
        t = 0.0f;
        static const float cameraRadius{4.0f};
        _pipeline->camera()->setPosition(glm::vec3{sin(t) * cameraRadius, 4.0f, cos(t) * cameraRadius});
        _pipeline->camera()->update(dt);
        _pipeline->render();
    }

    virtual void onFramebufferResized(int width, int height)
    {
        glViewport(0, 0, width, height);
    }

private:
    std::shared_ptr<Pipeline> _pipeline{nullptr};
    std::vector<std::shared_ptr<Tile>> _tiles;
    std::shared_ptr<lithium::Text> _text;
    std::shared_ptr<lithium::Font> _font;
};

int main(int argc, const char* argv[])
{
    std::unique_ptr<App> app = std::make_unique<App>();
    app->run();
    return 0;
}