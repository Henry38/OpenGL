#include "VisualModel.h"

#include "Mesh.h"


using namespace gl;

//VisualModel::VisualModel() :
//    m_mesh(nullptr),
//    m_transform(),
//    m_material()
//{

//}

VisualModel::VisualModel(const Mesh *mesh, const Material& material) :
    m_mesh(mesh),
    m_transform(),
    m_material(material)
{

}

VisualModel::~VisualModel()
{

}

const Mesh* VisualModel::mesh() const
{
    return this->m_mesh;
}

Transform& VisualModel::transform()
{
    return this->m_transform;
}

const Transform& VisualModel::transform() const
{
    return this->m_transform;
}

Material& VisualModel::material()
{
    return this->m_material;
}

const Material& VisualModel::material() const
{
    return this->m_material;
}

void VisualModel::draw(PrimitiveMode primitiveMode) const
{
    if (this->m_mesh)
        this->m_mesh->draw(primitiveMode);
}

TexturedVisualModel::TexturedVisualModel(const Mesh* mesh, const Texture* texture) : VisualModel(mesh),
    m_texture(texture)
{

}

TexturedVisualModel::~TexturedVisualModel()
{

}

const Texture* TexturedVisualModel::texture() const
{
    return this->m_texture;
}

//Texture& TexturedVisualModel::texture()
//{
//    return this->m_texture;
//}

//const Texture& TexturedVisualModel::texture() const
//{
//    return this->m_texture;
//}
