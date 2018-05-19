#ifndef HELPER_H
#define HELPER_H

#include "Camera.h"
#include "GLFWApplication.h"
#include "Message.h"
#include "Program.h"
#include "Scene.h"
#include "Shader.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include "Viewer.h"

// Standard Library
#include <fstream>
#include <string>

// Qt
#include <QFile>
#include <QString>


namespace gl {

namespace helper {

static bool getStringFromFile(const std::string& filename, std::string& dest) {
    std::ifstream filestream(filename, std::ios::in);

    if(!filestream.is_open()) {
        msg_error("Error") << "File not found: " << filename;
        return false;
    }

    dest = "";
    std::string line;

    while(getline(filestream, line)) {
        dest += line + "\n";
    }

    filestream.close();

    return true;
}

static bool getStringFromQrcFile(const std::string& filename, std::string& dest) {
    QFile file(QString::fromStdString(filename));
    file.open(QIODevice::ReadOnly);

    if (!file.isOpen()) {
        msg_error("Error") << "Could not open file " << filename;
        return false;
    }

    dest = file.readAll().toStdString();

    file.close();

    return true;
}

static ShaderProgram* CreateShaderProgram(ShaderProgram::ShaderProgramType shaderProgramType)
{
    typedef Shader::ShaderType ShaderType;
    typedef ShaderProgram::ShaderProgramType ShaderProgramType;

    ShaderProgram* shaderProgram = new ShaderProgram();

    std::string vs = "";
    std::string gs = "";
    std::string fs = "";

    Shader vertexShader(ShaderType::Vertex);
    Shader geometryShader(ShaderType::Geometry);
    Shader fragmentShader(ShaderType::Fragment);

    switch (shaderProgramType)
    {
    case ShaderProgramType::Basic:

        getStringFromQrcFile(":/shaders/basic.vs", vs);
        getStringFromQrcFile(":/shaders/basic.fs", fs);

        break;
    case ShaderProgramType::Normal:

        getStringFromQrcFile(":/shaders/normal.vs", vs);
        getStringFromQrcFile(":/shaders/normal.gs", gs);
        getStringFromQrcFile(":/shaders/normal.fs", fs);

        break;
    case ShaderProgramType::FlatShading:

        getStringFromQrcFile(":/shaders/flatShading.vs", vs);
        getStringFromQrcFile(":/shaders/flatShading.gs", gs);
        getStringFromQrcFile(":/shaders/flatShading.fs", fs);

        break;
    case ShaderProgramType::GouraudShading:

        getStringFromQrcFile(":/shaders/gouraudShading.vs", vs);
        getStringFromQrcFile(":/shaders/gouraudShading.fs", fs);

        break;
    case ShaderProgramType::PhongShading:

        getStringFromQrcFile(":/shaders/phongShading.vs", vs);
        getStringFromQrcFile(":/shaders/phongShading.fs", fs);

        break;

    case ShaderProgramType::Frame:

        getStringFromQrcFile(":/shaders/frame.vs", vs);
        getStringFromQrcFile(":/shaders/frame.fs", fs);

        break;
    case ShaderProgramType::HighLight:

        getStringFromQrcFile(":/shaders/highLight.vs", vs);
        getStringFromQrcFile(":/shaders/highLight.gs", gs);
        getStringFromQrcFile(":/shaders/highLight.fs", fs);

        break;
    case ShaderProgramType::Texturing:

        getStringFromQrcFile(":/shaders/texturing.vs", vs);
        getStringFromQrcFile(":/shaders/texturing.fs", fs);

        break;
    case ShaderProgramType::TangentSpace:

        getStringFromQrcFile(":/shaders/tangentSpace.vs", vs);
        getStringFromQrcFile(":/shaders/tangentSpace.gs", gs);
        getStringFromQrcFile(":/shaders/tangentSpace.fs", fs);

        break;
    case ShaderProgramType::Picking:

        getStringFromQrcFile(":/shaders/picking.vs", vs);
        getStringFromQrcFile(":/shaders/picking.fs", fs);

        break;
    case ShaderProgramType::OutLine:

        getStringFromQrcFile(":/shaders/outline.vs", vs);
        getStringFromQrcFile(":/shaders/outline.fs", fs);

        break;
    case ShaderProgramType::VaoQuad:

        getStringFromQrcFile(":/shaders/vaoQuad.vs", vs);
        getStringFromQrcFile(":/shaders/vaoQuad.fs", fs);

        break;
    }

    if (vs != "") {
        vertexShader.compileSourceCode(vs);
        shaderProgram->addShader(vertexShader);
    }

    if (gs != "") {
        geometryShader.compileSourceCode(gs);
        shaderProgram->addShader(geometryShader);
    }

    if (fs != "") {
        fragmentShader.compileSourceCode(fs);
        shaderProgram->addShader(fragmentShader);
    }

    shaderProgram->link();

    GLFWApplication* app = GLFWApplication::getInstance();
    Camera* camera = app->getViewer()->camera();
    float normalScale = 0.5;

    glm::vec3 dir_light(-1,-1,-1);
    glm::vec3 color(1,0,0);
    static Texture colorMap;
    if (!colorMap.isLoaded())
        colorMap.load("/home/henry/dev/QtProject/OpenGL/share/textures/chesterfield-color.png");
    static Texture normalMap;
    if (!normalMap.isLoaded())
        normalMap.load("/home/henry/dev/QtProject/OpenGL/share/textures/chesterfield-normal.png");

    switch (shaderProgramType)
    {
    case ShaderProgramType::Basic:

        shaderProgram->addData<Camera, glm::mat4>("mvp", camera, &Camera::mvp);
        shaderProgram->addData<glm::vec3>("dir_light", dir_light);
        shaderProgram->addData<glm::vec3>("color", color);

        break;
    case ShaderProgramType::Normal:

        shaderProgram->addData<float>("scale", normalScale);
        shaderProgram->addUniformBlock("transform", 1);
        shaderProgram->addUniformBlock("camera", 2);

        break;
    case ShaderProgramType::FlatShading:

        shaderProgram->addData<glm::vec3>("dir_light", dir_light);
        shaderProgram->addUniformBlock("transform", 1);
        shaderProgram->addUniformBlock("camera", 2);

        break;
    case ShaderProgramType::GouraudShading:

        shaderProgram->addData<Camera, glm::mat4>("mvp", camera, &Camera::mvp);
        shaderProgram->addData<glm::vec3>("dir_light", dir_light);
        shaderProgram->addData<glm::vec3>("color", color);

        break;
    case ShaderProgramType::PhongShading:

        shaderProgram->addData<glm::vec3>("dir_light", dir_light);
        shaderProgram->addUniformBlock("transform", 1);
        shaderProgram->addUniformBlock("camera", 2);

        break;
    case ShaderProgramType::Frame:

        shaderProgram->addData<Camera, glm::mat4>("mvp", camera, &Camera::mvp);
        shaderProgram->addData<glm::vec3>("dir_light", dir_light);
        shaderProgram->addData<glm::vec3>("scale", glm::vec3(1,1,1));
        //shaderProgram->setNbInstance(3);

        break;
    case ShaderProgramType::HighLight:

//        shaderProgram->setPrimitiveMode(PrimitiveMode::TRIANGLES);
        shaderProgram->addData<glm::vec3>("color", glm::vec3(1.0,0.8,0.0));
        shaderProgram->addUniformBlock("transform", 1);
        shaderProgram->addUniformBlock("camera", 2);

        break;
    case ShaderProgramType::Texturing:

        shaderProgram->addData<glm::vec3>("dir_light", dir_light);
        shaderProgram->addData<Texture>("colorMap", colorMap);
        shaderProgram->addData<Texture>("normalMap", normalMap);
        shaderProgram->addUniformBlock("transform", 1);
        shaderProgram->addUniformBlock("camera", 2);

        break;
    case ShaderProgramType::TangentSpace:

        shaderProgram->setPrimitiveMode(PrimitiveMode::POINTS);
        shaderProgram->addData<float>("scale", normalScale);
        shaderProgram->addUniformBlock("transform", 1);
        shaderProgram->addUniformBlock("camera", 2);

        break;
    case ShaderProgramType::Picking:

        //shaderProgram->addData<glm::vec4>("index", glm::vec4());
        shaderProgram->addUniformBlock("transform", 1);
        shaderProgram->addUniformBlock("camera", 2);

        break;
    case ShaderProgramType::OutLine:

        shaderProgram->addUniformBlock("transform", 1);
        shaderProgram->addUniformBlock("camera", 2);

        break;
    case ShaderProgramType::VaoQuad:

        break;
    }

    return shaderProgram;
}

}   // namespace helper

}   // namespace gl

#endif
