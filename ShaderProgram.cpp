#include "ShaderProgram.h"

#include "Mesh.h"
#include "Message.h"
#include "Scene.h"
#include "Shader.h"

using namespace gl;

std::map<OpenGLState::OpenGLAttribute, std::stack<unsigned int> > OpenGLState::OpenGLStateAttribute;

void OpenGLState::Push(OpenGLAttribute attribute, unsigned int value)
{
    std::stack<unsigned int>& attributeStack = OpenGLState::OpenGLStateAttribute[attribute];
    GLboolean current;
    GLint current_i;

    switch (attribute)
    {
    case DepthMask:
        glGetBooleanv(GL_DEPTH_WRITEMASK, &current);
        glDepthMask(value);

        attributeStack.push(current);

        break;
    case DepthTest:
        glGetBooleanv(GL_DEPTH_TEST, &current);
        if ((GLboolean) value == GL_TRUE)
            glEnable(GL_DEPTH_TEST);
        else
            glDisable(GL_DEPTH_TEST);

        attributeStack.push(current);

        break;
    case DepthFunc:
        glGetIntegerv(GL_DEPTH_FUNC, &current_i);
        glDepthFunc(value);

        attributeStack.push(current_i);

        break;
    case CullFace:
        glGetBooleanv(GL_CULL_FACE, &current);
        if ((GLboolean) value == GL_TRUE)
            glEnable(GL_CULL_FACE);
        else
            glDisable(GL_CULL_FACE);

        attributeStack.push(current);

        break;
    }
}

void OpenGLState::Pop(OpenGLAttribute attribute)
{
    std::stack<unsigned int>& attributeStack = OpenGLState::OpenGLStateAttribute[attribute];

    if (attributeStack.empty())
        return;

    unsigned int value = attributeStack.top();

    switch (attribute)
    {
    case DepthMask:
        glDepthMask(value);

        break;
    case DepthTest:
        if ((GLboolean) value == GL_TRUE)
            glEnable(GL_DEPTH_TEST);
        else
            glDisable(GL_DEPTH_TEST);

        break;
    case DepthFunc:
        glDepthFunc(value);

        break;
    case CullFace:
        if ((GLboolean) value == GL_TRUE)
            glEnable(GL_CULL_FACE);
        else
            glDisable(GL_CULL_FACE);

        break;
    }

    attributeStack.pop();
}

ShaderProgram::ShaderProgram() :
    m_programId(0),
    m_shaderList(3),
    m_dataList(),
    m_isLinked(false),
    m_nbInstance(1),
    m_primitiveMode(TRIANGLES),
    m_attributeStack()
{
    m_programId = glCreateProgram();
}

ShaderProgram::~ShaderProgram()
{
    for (auto it = m_dataList.begin(); it != m_dataList.end(); ++it) {
        BaseData* baseData = it->second;
        delete baseData;
        baseData = nullptr;
    }
    glDeleteProgram(m_programId);
}

GLuint ShaderProgram::getProgramID() const
{
    return this->m_programId;
}

unsigned int ShaderProgram::getNbInstance() const
{
    return this->m_nbInstance;
}

void ShaderProgram::setNbInstance(unsigned int n)
{
    this->m_nbInstance = n;
}

PrimitiveMode ShaderProgram::getPrimitiveMode() const
{
    return this->m_primitiveMode;
}

void ShaderProgram::setPrimitiveMode(PrimitiveMode primitiveMode)
{
    this->m_primitiveMode = primitiveMode;
}

void ShaderProgram::set(OpenGLAttribute attribute, unsigned int value)
{
    this->m_attributeStack[attribute] = value;
}

void ShaderProgram::pushAttribute() const
{
    for (auto it : this->m_attributeStack) {
        OpenGLAttribute attribute = it.first;
        unsigned int value = it.second;
        OpenGLState::Push(attribute, value);
    }
}

void ShaderProgram::popAttribute() const
{
    for (auto it : this->m_attributeStack) {
        OpenGLAttribute attribute = it.first;
        OpenGLState::Pop(attribute);
    }
}

bool ShaderProgram::addShader(const Shader& shader)
{
    if (!shader.isCompiled())
        return false;

    typedef Shader::ShaderType ShaderType;
    ShaderType type = shader.getShaderType();

    if (m_shaderList[type] != nullptr) {
        msg_warning("ShaderProgram") << "Shader " << type << " already set in ShaderProgram " << m_programId;
        return false;
    }

    m_shaderList[type] = &shader;

    return true;
}

void ShaderProgram::link()
{
    GLint linked = 0;

    for (const Shader* shader : m_shaderList) {
        if (shader != nullptr)
            glAttachShader(m_programId, shader->getShaderID());
    }

    glLinkProgram(m_programId);

    glGetProgramiv(m_programId, GL_LINK_STATUS, &linked);
    if (!linked)
    {
        GLchar infoLog[512];
        glGetProgramInfoLog(m_programId, 512, nullptr, infoLog);
        msg_error("ShaderProgram") << "Linkage failed: " << infoLog;
    }

    m_isLinked = linked;

    for (const Shader* shader : m_shaderList) {
        if (shader != nullptr)
            glDetachShader(m_programId, shader->getShaderID());
    }
}

bool ShaderProgram::isLinked() const
{
    return m_isLinked;
}

void ShaderProgram::bind() const
{
    glUseProgram(m_programId);
    this->pushAttribute();
}

void ShaderProgram::unbind() const
{
    this->popAttribute();
    glUseProgram(0);
}

void ShaderProgram::updateDataIfDirty()
{
    for (auto it = m_dataList.begin(); it != m_dataList.end(); ++it) {
        BaseData* baseData = it->second;
        baseData->updateIfDirty();
    }
}

bool ShaderProgram::addUniformBlock(const char* name, unsigned int binding_index)
{
    unsigned int block_index = glGetUniformBlockIndex(this->m_programId, name);

    if (block_index == GL_INVALID_INDEX) {
        msg_warning("ShaderProgram") << "uniform block location '" << name << "' not found. Binding index not added";
        return false;
    }

    glUniformBlockBinding(this->m_programId, block_index, binding_index);

    return true;
}

int ShaderProgram::attributLocation(const char* name) const
{
    if (!m_isLinked)
        return -1;

    return glGetUniformLocation(this->m_programId, name);
}

void ShaderProgram::setUniformValue(const char* name, float x)
{
    int location = attributLocation(name);

    if (location == -1) {
        msg_warning("ShaderProgram") << "uniform location '" << name << "' not found. Uniform value not setted";
        return;
    }

    setUniformValue(location, x);
}

void ShaderProgram::setUniformValue(int attributLocation, float x)
{
    glUniform1f(attributLocation, x);
}

void ShaderProgram::setUniformValue(const char* name, const glm::vec2& v)
{
    int location = attributLocation(name);

    if (location == -1) {
        msg_warning("ShaderProgram") << "uniform location '" << name << "' not found. Uniform value not setted";
        return;
    }

    setUniformValue(location, v);
}

void ShaderProgram::setUniformValue(int attributLocation, const glm::vec2& v)
{
    glUniform2fv(attributLocation, 1, glm::value_ptr(v));
}

void ShaderProgram::setUniformValue(const char* name, const glm::vec3& v)
{
    int location = attributLocation(name);

    if (location == -1) {
        msg_warning("ShaderProgram") << "uniform location '" << name << "' not found. Uniform value not setted";
        return;
    }

    setUniformValue(location, v);
}

void ShaderProgram::setUniformValue(int attributLocation, const glm::vec3& v)
{
    glUniform3fv(attributLocation, 1, glm::value_ptr(v));
}

void ShaderProgram::setUniformValue(const char *name, const glm::vec4 &v)
{
    int location = attributLocation(name);

    if (location == -1) {
        msg_warning("ShaderProgram") << "uniform location '" << name << "' not found. Uniform value not setted";
        return;
    }

    setUniformValue(location, v);
}

void ShaderProgram::setUniformValue(int attributLocation, const glm::vec4& v)
{
    glUniform4fv(attributLocation, 1, glm::value_ptr(v));
}

void ShaderProgram::setUniformValue(const char* name, const glm::mat2& m)
{
    int location = attributLocation(name);

    if (location == -1) {
        msg_warning("ShaderProgram") << "uniform location '" << name << "' not found. Uniform value not setted";
        return;
    }

    setUniformValue(location, m);
}

void ShaderProgram::setUniformValue(int attributLocation, const glm::mat2& m)
{
    glUniformMatrix2fv(attributLocation, 1, GL_FALSE, glm::value_ptr(m));
}

void ShaderProgram::setUniformValue(const char* name, const glm::mat3& m)
{
    int location = attributLocation(name);

    if (location == -1) {
        msg_warning("ShaderProgram") << "uniform location '" << name << "' not found. Uniform value not setted";
        return;
    }

    setUniformValue(location, m);
}

void ShaderProgram::setUniformValue(int attributLocation, const glm::mat3& m)
{
    glUniformMatrix3fv(attributLocation, 1, GL_FALSE, glm::value_ptr(m));
}

void ShaderProgram::setUniformValue(const char* name, const glm::mat4& m)
{
    int location = attributLocation(name);

    if (location == -1) {
        msg_warning("ShaderProgram") << "uniform location '" << name << "' not found. Uniform value not setted";
        return;
    }

    setUniformValue(location, m);
}

void ShaderProgram::setUniformValue(int attributLocation, const glm::mat4& m)
{
    glUniformMatrix4fv(attributLocation, 1, GL_FALSE, glm::value_ptr(m));
}
