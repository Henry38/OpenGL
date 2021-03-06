#include "GLFWApplication.h"

#include <Controller.h>
#include <Message.h>
#include <SceneView.h>
#include <VisualManager.h>

// Glad
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

// Standard Library
#include <memory>

using namespace gl;

int GLFWApplication::OpenGLMajorVersion = 3;
int GLFWApplication::OpenGLMinorVersion = 3;

GLFWApplication* GLFWApplication::OurInstance = nullptr;

GLFWApplication* GLFWApplication::Instance()
{
    return OurInstance;
}

GLFWApplication* GLFWApplication::CreateWindow(int width, int height)
{
    if (OurInstance != nullptr) {
        msg_error("GLFWApplication") << "Cannot create a second GLFWApplication. Singleton pattern is used";
        return nullptr;
    }

    /* GLFW initialization */
    if (!glfwInit()) {
        msg_error("GLFWApplication") << "oh no ! glfwInit() has failed";
        return nullptr;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, GLFWApplication::OpenGLMajorVersion);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, GLFWApplication::OpenGLMinorVersion);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    BaseApplication::ScreenWidth = uint(width);
    BaseApplication::ScreenHeight = uint(height);

    /* Create a windowed mode window and its OpenGL context */
    GLFWwindow* windowHandle = glfwCreateWindow(width, height, "OpenGL", nullptr, nullptr);

    if (windowHandle == nullptr)
    {
        // Problem: window creation failed, something went wrong.
        glfwTerminate();
        msg_error("GLFW") << "Cannot create a window and its opengl context";
        return nullptr;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(windowHandle);

    // Glad initialization
    if (!gladLoadGLLoader(GLADloadproc(glfwGetProcAddress)))
    {
        // Problem: glad initialization failed, something is seriously wrong.
        glfwTerminate();
        msg_error("OpenGL") << "oh no ! glad initialization has failed";
        return nullptr;
    }

    GLFWApplication* app = new GLFWApplication(windowHandle);

    VisualManager::Init();

    /* Set the number of screen updates to wait from the time glfwSwapBuffers was called before swapping */
    // 0 = no waiting for rendering the next frame
    // 1 = draw 1 image for each frames displayed on the screen (60Hz monitor = 60fps)
    // 2 = draw 1 image every 2 frames displayed on the screen (60hz monitor = 30 draw/s = 30fps)
    // 4 = draw 1 image every 4 frames displayed on the screen (60Hz monitor = 15 draw/s = 15fps)
    // etc ...
    glfwSwapInterval(2);

    // Specifies background color
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    // Enable depth buffer test
    glEnable(GL_DEPTH_TEST);
    // Enable eliminaton of hidden faces
    glEnable(GL_CULL_FACE);
    // Specifies whether front or back facing facets are candidates for culling
    glCullFace(GL_BACK);
    // Specifies the orientation of front-facing polygons
    glFrontFace(GL_CCW);

    int glMajor;
    int glMinor;
    glGetIntegerv(GL_MAJOR_VERSION, &glMajor);
    glGetIntegerv(GL_MINOR_VERSION, &glMinor);

    msg_info("OpenGL") << "Congrat's ! You're running OpenGL " << glMajor << "." << glMinor;

    return app;
}

void GLFWApplication::Terminate()
{
    GLFWApplication* app = GLFWApplication::Instance();
    VisualManager::Clean();

    if (app && !app->m_windowHandle) {
        glfwDestroyWindow(app->m_windowHandle);
        app->setWindow(nullptr);
    }

    glfwTerminate();
}

void GLFWApplication::FramebufferSizeCallback(GLFWwindow* handle, int width, int height)
{
    static GLFWApplication* app = GLFWApplication::Instance();

    for (const std::shared_ptr<SceneView>& sceneView : app->sceneViews()) {
        if(sceneView && sceneView->interface())
            sceneView->interface()->framebufferSizeCallback(handle, width, height);
    }

    BaseApplication::ScreenWidth = uint(width);
    BaseApplication::ScreenHeight = uint(height);
}

void GLFWApplication::MouseButtonCallback(GLFWwindow* handle, int button, int action, int mods)
{
    static GLFWApplication* app = GLFWApplication::Instance();

    for (const std::shared_ptr<SceneView>& sceneView : app->sceneViews()) {
        if(sceneView && sceneView->interface()) {
            const Rect& rect = sceneView->rect();
            double xpos = -1;
            double ypos = -1;
            glfwGetCursorPos(handle, &xpos, &ypos);
            if (rect.contains(int(xpos), int(ypos)))
                sceneView->interface()->mouseButtonCallback(handle, button, action, mods);
        }
    }
}

void GLFWApplication::CursorPosCallback(GLFWwindow* handle, double xpos, double ypos)
{
    static GLFWApplication* app = GLFWApplication::Instance();

    for (const std::shared_ptr<SceneView>& sceneView : app->sceneViews()) {
        if(sceneView && sceneView->interface()) {
            const Rect& rect = sceneView->rect();
            if (rect.contains(int(xpos), int(ypos))) {
                const Rect& rect = sceneView->rect();
                double xpos = -1;
                double ypos = -1;
                glfwGetCursorPos(handle, &xpos, &ypos);
                if (rect.contains(int(xpos), int(ypos)))
                    sceneView->interface()->cursorPosCallback(handle, xpos, ypos);
            }
        }
    }
}

void GLFWApplication::ScrollCallback(GLFWwindow* handle, double xoffset, double yoffset)
{
    static GLFWApplication* app = GLFWApplication::Instance();

    for (const std::shared_ptr<SceneView>& sceneView : app->sceneViews()) {
        if(sceneView && sceneView->interface()) {
            const Rect& rect = sceneView->rect();
            double xpos = -1;
            double ypos = -1;
            glfwGetCursorPos(handle, &xpos, &ypos);
            if (rect.contains(int(xpos), int(ypos)))
                sceneView->interface()->scrollCallback(handle, xoffset, yoffset);
        }
    }
}

void GLFWApplication::KeyCallback(GLFWwindow* handle, int key, int scancode, int action, int mods)
{
    static GLFWApplication* app = GLFWApplication::Instance();

    for (const std::shared_ptr<SceneView>& sceneView : app->sceneViews()) {
        if(sceneView && sceneView->interface())
            sceneView->interface()->keyCallback(handle, key, scancode, action, mods);
    }
}

GLFWApplication::GLFWApplication(GLFWwindow* handle) : BaseApplication(),
    m_windowHandle(nullptr)
{
    setWindow(handle);

    OurInstance = this;
}

GLFWApplication::~GLFWApplication()
{
}

float GLFWApplication::time() const
{
    return glfwGetTime();
}

void GLFWApplication::loop()
{
    if (this->m_windowHandle == nullptr) {
        msg_error("App") << "Can't loop, set window first";
        return;
    }

//    double lastTime = glfwGetTime();
//    unsigned int nbFrames = 0;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(this->m_windowHandle))
    {
//        // Measure speed
//        double currentTime = glfwGetTime();
//        nbFrames++;
//        /* If last print was more than 1 sec ago */
//        if (currentTime - lastTime >= 1.0)
//        {
//            msg_info("App") << 1000.0 / double(nbFrames) << " ms/frame";
//            nbFrames = 0;
//            lastTime += 1.0;
//        }

        /* Call the drawing function */
        this->draw();

        /* Swap front and back buffers */
        glfwSwapBuffers(this->m_windowHandle);

        /* Poll for and process events */
        glfwPollEvents();
    }
}

Rect GLFWApplication::windowRect() const
{
    GLFWwindow* window = getWindow();

    if (window == nullptr)
        return Rect();

    int x = 0;
    int y = 0;
    glfwGetWindowPos(window, &x, &y);

    int width = static_cast<int>(BaseApplication::ScreenWidth);
    int height = static_cast<int>(BaseApplication::ScreenHeight);

    return Rect(x,y,width,height);
}

void GLFWApplication::setWindowRect(const Rect& rect)
{
    GLFWwindow* window = getWindow();

    if (window == nullptr)
        return;

    glfwSetWindowPos(window, rect.x(), rect.y());
    glfwSetWindowSize(window, rect.width(), rect.height());
}

GLFWwindow* GLFWApplication::getWindow() const
{
    return this->m_windowHandle;
}

void GLFWApplication::setWindow(GLFWwindow* newHandle)
{
    GLFWwindow* oldHandle = this->m_windowHandle;

    /* Remove callbacks set to the old handle */
    if (oldHandle != nullptr) {
        glfwSetFramebufferSizeCallback(oldHandle, nullptr);
        glfwSetMouseButtonCallback(oldHandle, nullptr);
        glfwSetCursorPosCallback(oldHandle, nullptr);
        glfwSetScrollCallback(oldHandle, nullptr);
        glfwSetKeyCallback(oldHandle, nullptr);
    }

    this->m_windowHandle = newHandle;

    /* Add callbacks to the new handle */
    if (newHandle != nullptr) {
        glfwSetFramebufferSizeCallback(newHandle, GLFWApplication::FramebufferSizeCallback);
        glfwSetMouseButtonCallback(newHandle, GLFWApplication::MouseButtonCallback);
        glfwSetCursorPosCallback(newHandle, GLFWApplication::CursorPosCallback);
        glfwSetScrollCallback(newHandle, GLFWApplication::ScrollCallback);
        glfwSetKeyCallback(newHandle, GLFWApplication::KeyCallback);
    }
}
