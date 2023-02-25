#include "assetfactory.h"

AssetFactory::AssetFactory()
{

}

AssetFactory::~AssetFactory() noexcept
{

}

using attr = lithium::VertexArrayBuffer::AttributeType;
static constexpr attr POSITION{attr::VEC3};
static constexpr attr NORMAL{attr::VEC3};
static constexpr attr UV{attr::VEC2};
static constexpr attr BONE_IDS{attr::VEC4};
static constexpr attr BONE_WEIGHTS{attr::VEC4};

const std::vector<attr> AssetFactory::objectAttributes{POSITION, NORMAL, UV};
const std::vector<attr> AssetFactory::modelAttributes{POSITION, NORMAL, UV, BONE_IDS, BONE_WEIGHTS};

static const std::vector<attr> screenMeshAttributes = { POSITION, NORMAL, UV };

static const std::vector<GLfloat> screenMeshVertices = {
    -1.0f, -1.0f, +0.0f, 	+0.0f, +1.0f, +0.0f,	+0.0f, +0.0f,
    -1.0f, +1.0f, +0.0f, 	+0.0f, +1.0f, +0.0f,	+0.0f, +1.0f,
    +1.0f, +1.0f, +0.0f,	+0.0f, +1.0f, +0.0f,	+1.0f, +1.0f,
    +1.0f, -1.0f, +0.0f, 	+0.0f, +1.0f, +0.0f,	+1.0f, +0.0f,
};

static const std::vector<GLuint> screenMeshIndices = {
    0, 2, 1,
    0, 3, 2
};

void AssetFactory::loadMeshes()
{
    AssetFactory& instance = getInstance();
    instance._meshes.tower = lithium::tinyobjloader_load("assets/intersection/tower.obj", AssetFactory::objectAttributes);
    instance._meshes.indicator = lithium::tinyobjloader_load("assets/intersection/indicator.obj", AssetFactory::objectAttributes);
}

void AssetFactory::loadTextures()
{
    AssetFactory& instance = getInstance();
    instance._textures.righteousDiffuse.reset(lithium::ImageTexture::load("assets/Righteous.png", GL_RGB, GL_RGBA, 1, false));
}

void AssetFactory::loadObjects()
{
    AssetFactory& instance = getInstance();
}

void AssetFactory::loadFonts()
{
    AssetFactory& instance = getInstance();
    instance._fonts.righteousFont = std::make_shared<lithium::Font>(instance._textures.righteousDiffuse, "assets/Righteous.json");
}

const AssetFactory::Meshes* AssetFactory::getMeshes()
{
    return &AssetFactory::getInstance()._meshes;
}

const AssetFactory::Textures* AssetFactory::getTextures()
{
    return &AssetFactory::getInstance()._textures;
}

const AssetFactory::Objects* AssetFactory::getObjects()
{
    return &AssetFactory::getInstance()._objects;
}

const AssetFactory::Fonts* AssetFactory::getFonts()
{
    return &AssetFactory::getInstance()._fonts;
}

AssetFactory& AssetFactory::getInstance()
{
    static AssetFactory instance;
    return instance;
}
