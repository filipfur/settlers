#pragma once

#include "glrenderpipeline.h"
#include "glsimplecamera.h"
#include "glframebuffer.h"
#include "glorthographiccamera.h"
#include "gltext.h"
#include "glinstancedobject.h"

using attr = lithium::VertexArrayBuffer::AttributeType;
static constexpr attr POSITION{attr::VEC3};
static constexpr attr NORMAL{attr::VEC3};
static constexpr attr UV{attr::VEC2};
static constexpr attr COLOR{attr::VEC3};

class Pipeline : public lithium::RenderPipeline
{
public:
    Pipeline(const glm::ivec2& resolution) : lithium::RenderPipeline{resolution},
        _frameBuffer{std::make_shared<lithium::FrameBuffer>(resolution, lithium::FrameBuffer::Mode::MULTISAMPLED)},
        _camera{new lithium::SimpleCamera(glm::perspective(glm::radians(45.0f), (float)resolution.x / (float)resolution.y, 0.1f, 100.0f))},
        _orthoProj{glm::ortho(0.0f, (float)resolution.x, 0.0f, (float)resolution.y, -10000.0f, 10000.0f)},
	    _orthoView{glm::inverse((glm::translate(glm::mat4(1.0f), glm::vec3{0.0f}) * glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(0, 0, 1))))},
	    _orthoProjView{_orthoProj * _orthoView}
    {
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glCullFace(GL_BACK);
        glEnable(GL_CULL_FACE);
        _screenMesh.reset(new lithium::Mesh({POSITION, NORMAL, UV, COLOR}, {
            -1.0, -1.0, 0.0f, 	0.0f, 1.0f, 0.0f,	0.0f, 0.0f,   1.0f, 0.0f, 0.0f,
            -1.0,  1.0, 0.0f, 	0.0f, 1.0f, 0.0f,	0.0f, 1.0,    0.0f, 1.0f, 0.0f,
            1.0,  1.0, 0.0f,	0.0f, 1.0f, 0.0f,	1.0, 1.0,     0.0f, 0.0f, 1.0f,
            1.0, -1.0, 0.0f, 	0.0f, 1.0f, 0.0f,	1.0, 0.0f,    1.0f, 1.0f, 0.0f
        },
        {
            0, 2, 1,
            0, 3, 2
        }));
        _blockShader = std::make_shared<lithium::ShaderProgram>("shaders/object.vert", "shaders/object.frag");
        _blockShader->setUniform("u_texture_0", 0);
        _blockShader->setUniform("u_projection", _camera->projection(), true);
        _screenShader = std::make_shared<lithium::ShaderProgram>("shaders/screenshader.vert", "shaders/screenshader.frag");
        _sdfTextShader = std::make_shared<lithium::ShaderProgram>("shaders/sdfText.vert", "shaders/sdfText.frag");
        _sdfTextShader->setUniform("u_projection", _camera->projection(), true);
        _sdfTextShader->setUniform("u_texture", 0);
        _orthoCamera = new lithium::OrthographicCamera(0, resolution.x, 0, resolution.y, -10000.0f, 10000.0f);
        _sdfTextOrthoShader = std::make_shared<lithium::ShaderProgram>("shaders/sdfTextOrtho.vert", "shaders/sdfText.frag");
        _sdfTextOrthoShader->setUniform("u_texture", 0);
        _sdfTextOrthoShader->setUniform("u_camera", _orthoProjView);
        _instShader = std::make_shared<lithium::ShaderProgram>("shaders/instances.vert", "shaders/object.frag");
        _instShader->setUniform("u_projection", _camera->projection(), true);
        _instShader->setUniform("u_texture_0", 0);
        _msaaShader = std::make_shared<lithium::ShaderProgram>("shaders/screenshader.vert", "shaders/msaa.frag");
        _camera->setPosition(glm::vec3{4.0f, 8.0f, 8.0f});
        _camera->setTarget(glm::vec3{0.0f, -1.0f, 0.0f});

        _frameBuffer->bind();
            _frameBuffer->createTexture(GL_COLOR_ATTACHMENT0, GL_RGBA16F, GL_RGBA, GL_FLOAT);
            _frameBuffer->createTexture(GL_COLOR_ATTACHMENT1, GL_RGBA16F, GL_RGBA, GL_FLOAT);
            _frameBuffer->declareBuffers();
            _frameBuffer->createRenderBuffer(lithium::RenderBuffer::Mode::MULTISAMPLED, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT);
        _frameBuffer->unbind();

        _mainGroup = createRenderGroup([this](lithium::Renderable* renderable) -> bool {
            return dynamic_cast<lithium::Object*>(renderable) && dynamic_cast<lithium::Text*>(renderable) == nullptr && dynamic_cast<lithium::InstancedObject<glm::mat4>*>(renderable) == nullptr;
        });

        _instGroup = createRenderGroup([this](lithium::Renderable* renderable) -> bool {
            return dynamic_cast<lithium::InstancedObject<glm::mat4>*>(renderable);
        });

        _textGroup = createRenderGroup([this](lithium::Renderable* renderable) -> bool {
            return dynamic_cast<lithium::Text*>(renderable);
        });

        _text2DGroup = createRenderGroup([this](lithium::Renderable* renderable) -> bool {
            return dynamic_cast<lithium::Text*>(renderable);
        });

        glm::ivec4 viewport{0, 0, resolution.x, resolution.y};

        _mainStage = addRenderStage(std::make_shared<lithium::RenderStage>(_frameBuffer, viewport, [this](){
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            _screenShader->use();
            glDepthMask(GL_FALSE);
            _screenMesh->bind();
            _screenMesh->draw();
            glDepthMask(GL_TRUE);
            _blockShader->setUniform("u_view", _camera->view());
            _blockShader->setUniform("u_view_pos", _camera->position());
            _blockShader->setUniform("u_time", _time);
            _mainGroup->render(_blockShader.get());
            _instShader->setUniform("u_view", _camera->view());
            _instShader->setUniform("u_view_pos", _camera->position());
            _instGroup->render(_instShader.get());
            _sdfTextShader->setUniform("u_view", _camera->view());
            _textGroup->render(_sdfTextShader.get());
            _text2DGroup->render(_sdfTextOrthoShader.get());
        }));

        _finalStage = addRenderStage(std::make_shared<lithium::RenderStage>(nullptr, viewport, [this](){
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            _msaaShader->use();
            _mainStage->frameBuffer()->texture(GL_COLOR_ATTACHMENT0)->bind(GL_TEXTURE0);
            _screenMesh->bind();
            _screenMesh->draw();
        }));

        _simpleStage = addRenderStage(std::make_shared<lithium::RenderStage>(nullptr, viewport, [this](){
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            _screenShader->use();
            glDepthMask(GL_FALSE);
            _screenMesh->bind();
            _screenMesh->draw();
            glDepthMask(GL_TRUE);
            _blockShader->setUniform("u_view", _camera->view());
            _mainGroup->render(_blockShader.get());
        }));

        _simpleStage->setEnabled(false);

        setViewportToResolution();
    }

    ~Pipeline()
    {
        _blockShader = nullptr;
        _screenShader = nullptr;
        _screenMesh = nullptr;
    }

    std::shared_ptr<lithium::SimpleCamera> camera()
    {
        return _camera;
    }

    void setMsaa(bool msaa)
    {
        _simpleStage->setEnabled(!msaa);
        _mainStage->setEnabled(msaa);
        _finalStage->setEnabled(msaa);
    }

    void setTime(float time)
    {
        _time = time;
    }

private:
    std::shared_ptr<lithium::ShaderProgram> _blockShader{nullptr};
    std::shared_ptr<lithium::ShaderProgram> _instShader{nullptr};
    std::shared_ptr<lithium::ShaderProgram> _screenShader{nullptr};
    std::shared_ptr<lithium::ShaderProgram> _sdfTextShader{nullptr};
    std::shared_ptr<lithium::ShaderProgram> _sdfTextOrthoShader{nullptr};
    std::shared_ptr<lithium::ShaderProgram> _msaaShader{nullptr};
    std::shared_ptr<lithium::SimpleCamera> _camera{nullptr};
    std::shared_ptr<lithium::FrameBuffer> _frameBuffer{nullptr};
    std::shared_ptr<lithium::Object> _object;
    std::shared_ptr<lithium::Mesh> _screenMesh;
    std::shared_ptr<lithium::RenderGroup> _mainGroup;
    std::shared_ptr<lithium::RenderGroup> _instGroup;
    std::shared_ptr<lithium::RenderGroup> _textGroup;
    std::shared_ptr<lithium::RenderGroup> _text2DGroup;
    std::shared_ptr<lithium::RenderStage> _mainStage;
    std::shared_ptr<lithium::RenderStage> _finalStage;
    std::shared_ptr<lithium::RenderStage> _simpleStage;
    glm::mat4 _orthoProj;
    glm::mat4 _orthoView;
    glm::mat4 _orthoProjView;
    lithium::OrthographicCamera* _orthoCamera;
    float _time{0.0f};
};