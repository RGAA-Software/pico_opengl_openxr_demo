// Copyright (c) 2017-2020 The Khronos Group Inc.
//
// SPDX-License-Identifier: Apache-2.0

#include "pch.h"
#include "common.h"
#include "geometry.h"
#include "graphicsplugin.h"

#include "components/Line.h"
#include "components/Circle.h"
#include "components/Rectangle.h"
#include "components/Director.h"

#ifdef XR_USE_GRAPHICS_API_OPENGL_ES

#include "common/gfxwrapper_opengl.h"
#include <common/xr_linear.h>

namespace {
constexpr float DarkSlateGray[] = {0.184313729f, 0.309803933f, 0.609803933f, 1.0f};

static const char* VertexShaderGlsl = R"_(
    #version 320 es

    in vec3 VertexPos;
    in vec3 VertexColor;

    out vec3 PSVertexColor;

    uniform mat4 ModelViewProjection;

    void main() {
       gl_Position = ModelViewProjection * vec4(VertexPos, 1.0);
       PSVertexColor = VertexColor;
    }
    )_";

static const char* FragmentShaderGlsl = R"_(
    #version 320 es

    in lowp vec3 PSVertexColor;
    out lowp vec4 FragColor;

    void main() {
       FragColor = vec4(PSVertexColor, 1);
    }
    )_";

class RectWrapper {
public:
    static std::shared_ptr<RectWrapper> Make(const std::shared_ptr<vr::Rectangle>& rect, const glm::vec3& trans, const glm::vec3& scale) {
        return std::shared_ptr<RectWrapper>(new RectWrapper(rect, trans, scale));
    }
private:
    RectWrapper(const std::shared_ptr<vr::Rectangle>& rect, const glm::vec3& trans, const glm::vec3& scale) {
        this->rect_ = rect;
        this->translation_ = trans;
        this->scale_ = scale;
    }
public:
    std::shared_ptr<vr::Rectangle> rect_ = nullptr;
    glm::vec3 translation_ = glm::vec3(0, 0, 0);
    glm::vec3 scale_ = glm::vec3(0.3f, 0.3f, 0.3f);
};

struct OpenGLESGraphicsPlugin : public IGraphicsPlugin {
    OpenGLESGraphicsPlugin(const std::shared_ptr<Options>& /*unused*/, const std::shared_ptr<IPlatformPlugin> /*unused*/&){

        director = std::make_shared<vr::Director>();

    };

    OpenGLESGraphicsPlugin(const OpenGLESGraphicsPlugin&) = delete;
    OpenGLESGraphicsPlugin& operator=(const OpenGLESGraphicsPlugin&) = delete;
    OpenGLESGraphicsPlugin(OpenGLESGraphicsPlugin&&) = delete;
    OpenGLESGraphicsPlugin& operator=(OpenGLESGraphicsPlugin&&) = delete;

    ~OpenGLESGraphicsPlugin() override {
        if (m_swapchainFramebuffer != 0) {
            glDeleteFramebuffers(1, &m_swapchainFramebuffer);
        }
        if (m_program != 0) {
            glDeleteProgram(m_program);
        }
        if (m_vao != 0) {
            glDeleteVertexArrays(1, &m_vao);
        }
        if (m_cubeVertexBuffer != 0) {
            glDeleteBuffers(1, &m_cubeVertexBuffer);
        }
        if (m_cubeIndexBuffer != 0) {
            glDeleteBuffers(1, &m_cubeIndexBuffer);
        }

        for (auto& colorToDepth : m_colorToDepthMap) {
            if (colorToDepth.second != 0) {
                glDeleteTextures(1, &colorToDepth.second);
            }
        }
    }

    std::vector<std::string> GetInstanceExtensions() const override { return {XR_KHR_OPENGL_ES_ENABLE_EXTENSION_NAME}; }

    ksGpuWindow window{};

    std::shared_ptr<vr::Director> director = nullptr;
    std::shared_ptr<vr::Rectangle> rectangle = nullptr;
    std::shared_ptr<vr::Rectangle> rectangle2 = nullptr;
    std::vector<std::shared_ptr<RectWrapper>> rectangles;
    std::shared_ptr<vr::Circle> circle = nullptr;
    std::shared_ptr<vr::Line> line = nullptr;

    void DebugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message) {
        (void)source;
        (void)type;
        (void)id;
        (void)severity;
        Log::Write(Log::Level::Info, "GLES Debug: " + std::string(message, 0, length));
    }

    void InitializeDevice(XrInstance instance, XrSystemId systemId) override {
        // Extension function must be loaded by name
        PFN_xrGetOpenGLESGraphicsRequirementsKHR pfnGetOpenGLESGraphicsRequirementsKHR = nullptr;
        CHECK_XRCMD(xrGetInstanceProcAddr(instance, "xrGetOpenGLESGraphicsRequirementsKHR",
                                          reinterpret_cast<PFN_xrVoidFunction*>(&pfnGetOpenGLESGraphicsRequirementsKHR)));

        XrGraphicsRequirementsOpenGLESKHR graphicsRequirements{XR_TYPE_GRAPHICS_REQUIREMENTS_OPENGL_ES_KHR};
        CHECK_XRCMD(pfnGetOpenGLESGraphicsRequirementsKHR(instance, systemId, &graphicsRequirements));

        // Initialize the gl extensions. Note we have to open a window.
        ksDriverInstance driverInstance{};
        ksGpuQueueInfo queueInfo{};
        ksGpuSurfaceColorFormat colorFormat{KS_GPU_SURFACE_COLOR_FORMAT_B8G8R8A8};
        ksGpuSurfaceDepthFormat depthFormat{KS_GPU_SURFACE_DEPTH_FORMAT_D24};
        ksGpuSampleCount sampleCount{KS_GPU_SAMPLE_COUNT_1};
        if (!ksGpuWindow_Create(&window, &driverInstance, &queueInfo, 0, colorFormat, depthFormat, sampleCount, 640, 480, false)) {
            THROW("Unable to create GL context");
        }

        GLint major = 0;
        GLint minor = 0;
        glGetIntegerv(GL_MAJOR_VERSION, &major);
        glGetIntegerv(GL_MINOR_VERSION, &minor);

        const XrVersion desiredApiVersion = XR_MAKE_VERSION(major, minor, 0);
        if (graphicsRequirements.minApiVersionSupported > desiredApiVersion) {
            THROW("Runtime does not support desired Graphics API and/or version");
        }

#if defined(XR_USE_PLATFORM_ANDROID)
        m_graphicsBinding.display = window.display;
        m_graphicsBinding.config = (EGLConfig)0;
        m_graphicsBinding.context = window.context.context;
#endif

        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(
            [](GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message,
               const void* userParam) {
                //((OpenGLESGraphicsPlugin*)userParam)->DebugMessageCallback(source, type, id, severity, length, message);
            },
            this);

        InitializeResources();
    }

    void InitializeResources() {
        glGenFramebuffers(1, &m_swapchainFramebuffer);

        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &VertexShaderGlsl, nullptr);
        glCompileShader(vertexShader);
        CheckShader(vertexShader);

        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &FragmentShaderGlsl, nullptr);
        glCompileShader(fragmentShader);
        CheckShader(fragmentShader);

        m_program = glCreateProgram();
        glAttachShader(m_program, vertexShader);
        glAttachShader(m_program, fragmentShader);
        glLinkProgram(m_program);
        CheckProgram(m_program);

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        m_modelViewProjectionUniformLocation = glGetUniformLocation(m_program, "ModelViewProjection");

        m_vertexAttribCoords = glGetAttribLocation(m_program, "VertexPos");
        m_vertexAttribColor = glGetAttribLocation(m_program, "VertexColor");

        glGenBuffers(1, &m_cubeVertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, m_cubeVertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Geometry::c_cubeVertices), Geometry::c_cubeVertices, GL_STATIC_DRAW);

        glGenBuffers(1, &m_cubeIndexBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_cubeIndexBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Geometry::c_cubeIndices), Geometry::c_cubeIndices, GL_STATIC_DRAW);

        glGenVertexArrays(1, &m_vao);
        glBindVertexArray(m_vao);
        glEnableVertexAttribArray(m_vertexAttribCoords);
        glEnableVertexAttribArray(m_vertexAttribColor);
        glBindBuffer(GL_ARRAY_BUFFER, m_cubeVertexBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_cubeIndexBuffer);
        glVertexAttribPointer(m_vertexAttribCoords, 3, GL_FLOAT, GL_FALSE, sizeof(Geometry::Vertex), nullptr);
        glVertexAttribPointer(m_vertexAttribColor, 3, GL_FLOAT, GL_FALSE, sizeof(Geometry::Vertex),
                              reinterpret_cast<const void*>(sizeof(XrVector3f)));
    }

    void CheckShader(GLuint shader) {
        GLint r = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &r);
        if (r == GL_FALSE) {
            GLchar msg[4096] = {};
            GLsizei length;
            glGetShaderInfoLog(shader, sizeof(msg), &length, msg);
            THROW(Fmt("Compile shader failed: %s", msg));
        }
    }

    void CheckProgram(GLuint prog) {
        GLint r = 0;
        glGetProgramiv(prog, GL_LINK_STATUS, &r);
        if (r == GL_FALSE) {
            GLchar msg[4096] = {};
            GLsizei length;
            glGetProgramInfoLog(prog, sizeof(msg), &length, msg);
            THROW(Fmt("Link program failed: %s", msg));
        }
    }

    int64_t SelectColorSwapchainFormat(const std::vector<int64_t>& runtimeFormats) const override {
        // List of supported color swapchain formats.
        constexpr int64_t SupportedColorSwapchainFormats[] = {
            GL_RGBA8,
            GL_RGBA8_SNORM,
        };

        auto swapchainFormatIt =
            std::find_first_of(runtimeFormats.begin(), runtimeFormats.end(), std::begin(SupportedColorSwapchainFormats),
                               std::end(SupportedColorSwapchainFormats));
        if (swapchainFormatIt == runtimeFormats.end()) {
            THROW("No runtime swapchain format supported for color swapchain");
        }

        return *swapchainFormatIt;
    }

    const XrBaseInStructure* GetGraphicsBinding() const override {
        return reinterpret_cast<const XrBaseInStructure*>(&m_graphicsBinding);
    }

    std::vector<XrSwapchainImageBaseHeader*> AllocateSwapchainImageStructs(
        uint32_t capacity, const XrSwapchainCreateInfo& /*swapchainCreateInfo*/) override {
        // Allocate and initialize the buffer of image structs (must be sequential in memory for xrEnumerateSwapchainImages).
        // Return back an array of pointers to each swapchain image struct so the consumer doesn't need to know the type/size.
        std::vector<XrSwapchainImageOpenGLESKHR> swapchainImageBuffer(capacity);
        std::vector<XrSwapchainImageBaseHeader*> swapchainImageBase;
        for (XrSwapchainImageOpenGLESKHR& image : swapchainImageBuffer) {
            image.type = XR_TYPE_SWAPCHAIN_IMAGE_OPENGL_ES_KHR;
            swapchainImageBase.push_back(reinterpret_cast<XrSwapchainImageBaseHeader*>(&image));
        }

        // Keep the buffer alive by moving it into the list of buffers.
        m_swapchainImageBuffers.push_back(std::move(swapchainImageBuffer));

        return swapchainImageBase;
    }

    uint32_t GetDepthTexture(uint32_t colorTexture) {
        // If a depth-stencil view has already been created for this back-buffer, use it.
        auto depthBufferIt = m_colorToDepthMap.find(colorTexture);
        if (depthBufferIt != m_colorToDepthMap.end()) {
            return depthBufferIt->second;
        }

        // This back-buffer has no corresponding depth-stencil texture, so create one with matching dimensions.

        GLint width;
        GLint height;
        glBindTexture(GL_TEXTURE_2D, colorTexture);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);

        uint32_t depthTexture;
        glGenTextures(1, &depthTexture);
        glBindTexture(GL_TEXTURE_2D, depthTexture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, nullptr);

        m_colorToDepthMap.insert(std::make_pair(colorTexture, depthTexture));

        return depthTexture;
    }

    void RenderView(const XrCompositionLayerProjectionView& layerView, const XrSwapchainImageBaseHeader* swapchainImage,
                    int64_t swapchainFormat, const std::vector<Cube>& cubes) override {
        CHECK(layerView.subImage.imageArrayIndex == 0);  // Texture arrays not supported.
        UNUSED_PARM(swapchainFormat);                    // Not used in this function for now.

        glBindFramebuffer(GL_FRAMEBUFFER, m_swapchainFramebuffer);

        const uint32_t colorTexture = reinterpret_cast<const XrSwapchainImageOpenGLESKHR*>(swapchainImage)->image;

        glViewport(static_cast<GLint>(layerView.subImage.imageRect.offset.x),
                   static_cast<GLint>(layerView.subImage.imageRect.offset.y),
                   static_cast<GLsizei>(layerView.subImage.imageRect.extent.width),
                   static_cast<GLsizei>(layerView.subImage.imageRect.extent.height));

//        Log::Write(Log::Level::Info, Fmt("offset x : %d, offset y : %d, width : %d, height : %d",
//                                         layerView.subImage.imageRect.offset.x, layerView.subImage.imageRect.offset.y,
//                                         layerView.subImage.imageRect.extent.width,layerView.subImage.imageRect.extent.height
//                                         ));

        glFrontFace(GL_CCW);
        glCullFace(GL_BACK);

        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);

        const uint32_t depthTexture = GetDepthTexture(colorTexture);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);

        // Clear swapchain and depth buffer.
        glClearColor(DarkSlateGray[0], DarkSlateGray[1], DarkSlateGray[2], DarkSlateGray[3]);
        glClearDepthf(1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        // Set shaders and uniform variables.
        glUseProgram(m_program);

        const auto& pose = layerView.pose;
        XrMatrix4x4f proj;
        XrMatrix4x4f_CreateProjectionFov(&proj, GRAPHICS_OPENGL_ES, layerView.fov, 0.05f, 100.0f);
        XrMatrix4x4f toView;
        XrVector3f scale{1.f, 1.f, 1.f};
        XrMatrix4x4f_CreateTranslationRotationScale(&toView, &pose.position, &pose.orientation, &scale);
        XrMatrix4x4f view;
        XrMatrix4x4f_InvertRigidBody(&view, &toView);
        XrMatrix4x4f vp;
        XrMatrix4x4f_Multiply(&vp, &proj, &view);

        glm::mat4 vp_matrix(1.0f);
        memcpy(glm::value_ptr(vp_matrix), vp.m, sizeof(vp.m));

        // Set cube primitive data.
        glBindVertexArray(m_vao);

        // Render each cube

        glm::vec3 right_position;
        glm::vec3 right_orientation;

        for (const Cube& cube : cubes) {
            if (cube.Ray) {
                right_position.x = cube.Pose.position.x;
                right_position.y = cube.Pose.position.y;
                right_position.z = cube.Pose.position.z;
            }
            // Compute the model-view-projection transform and set it..
            XrMatrix4x4f model;
            XrMatrix4x4f_CreateTranslationRotationScale(&model, &cube.Pose.position, &cube.Pose.orientation, &cube.Scale);

            XrMatrix4x4f mvp;
            XrMatrix4x4f_Multiply(&mvp, &vp, &model);
            glUniformMatrix4fv(m_modelViewProjectionUniformLocation, 1, GL_FALSE, reinterpret_cast<const GLfloat*>(&mvp));

            if (cube.Ray) {
                glBindBuffer(GL_ARRAY_BUFFER, m_cubeVertexBuffer);
                glBufferData(GL_ARRAY_BUFFER, sizeof(Geometry::c_cubeVertices2), Geometry::c_cubeVertices2, GL_STATIC_DRAW);
                // 0 left, 1 right
                if (cube.Side == 1) {

                    glm::quat orientation(cube.Pose.orientation.w, cube.Pose.orientation.x, cube.Pose.orientation.y, cube.Pose.orientation.z);

                    auto GetDirectionFromQuaternion = [](const glm::quat& quaternion) -> glm::vec3
                    {
                        glm::vec3 direction = glm::vec3(0.0f, 0.0f, 1.0f); // 默认朝向正z轴方向

                        glm::mat4 rotationMatrix = glm::mat4_cast(quaternion);
                        direction = glm::mat3(rotationMatrix) * direction;

                        return glm::normalize(direction);
                    };

                    auto o = GetDirectionFromQuaternion(orientation);
                    right_orientation = o;

                    auto pos = glm::vec3(cube.Pose.position.x, cube.Pose.position.y, cube.Pose.position.z);
                    director->UpdateHandInfo(vr::HandSide::kRight, pos, o, 1.0f);

                }
            }
            else {
                glBindBuffer(GL_ARRAY_BUFFER, m_cubeVertexBuffer);
                glBufferData(GL_ARRAY_BUFFER, sizeof(Geometry::c_cubeVertices), Geometry::c_cubeVertices, GL_STATIC_DRAW);
            }

            // Draw the cube.
            glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(ArraySize(Geometry::c_cubeIndices)), GL_UNSIGNED_SHORT, nullptr);
        }

        glBindVertexArray(0);
        glUseProgram(0);

        float NH = -1.0f;
        float PH = 1.0f;
        float ZERO = 0.0f;
        float ONE = 1.0f;
        float Z = -2.5f;

        glm::vec3 p1(NH, NH, Z);
        glm::vec3 p2(PH, NH, Z);
        glm::vec3 p3(PH, PH, Z);

        // 计算平面的法线向量
        glm::vec3 normal = glm::cross(p2 - p1, p3 - p1);
        normal = glm::normalize(normal);

        glm::vec3 rayOrigin(right_position);
        glm::vec3 rayDirection(right_orientation);
        rayDirection = glm::normalize(rayDirection);

        float t = glm::dot(p1 - rayOrigin, normal) / glm::dot(rayDirection, normal);
        glm::vec3 intersectionPoint = rayOrigin + t * rayDirection;

        director->UpdateIntersectionPoint(vr::HandSide::kRight, glm::vec2(intersectionPoint.x, intersectionPoint.y));
        director->UpdateViewProjectionMatrix(vp_matrix);

        if (rectangles.empty()) {
            for (int i = 0; i < 6; i++) {
                auto rect = vr::Rectangle::MakeSingleColor(director, glm::vec3(0.3, 0.3, 0.6 + i* 0.06));
                auto trans = glm::vec3(0.5 * i, 0, 0.0);
                auto scale = glm::vec3(0.2, 0.2, 1.0);
                rectangles.push_back(RectWrapper::Make(rect, trans, scale));
            }
            for (int i = 0; i < 6; i++) {
                auto rect = vr::Rectangle::MakeSingleColor(director, glm::vec3(0.4+ i* 0.06, 0.5, 0.6));
                auto trans = glm::vec3(0.5 * i, 0.5, 0.0);
                auto scale = glm::vec3(0.2, 0.2, 1.0);
                rectangles.push_back(RectWrapper::Make(rect, trans, scale));
            }
            for (int i = 0; i < 6; i++) {
                auto rect = vr::Rectangle::MakeSingleColor(director, glm::vec3(0.4, 0.5+ i* 0.06, 0.6));
                auto trans = glm::vec3(0.5 * i, -0.5, 0.0);
                auto scale = glm::vec3(0.2, 0.2, 1.0);
                rectangles.push_back(RectWrapper::Make(rect, trans, scale));
            }
            for (int i = 0; i < 6; i++) {
                auto rect = vr::Rectangle::MakeSingleColor(director, glm::vec3(0.4+ i* 0.06, 0.5+ i* 0.06, 0.6));
                auto trans = glm::vec3(0.5 * i, -1, 0.0);
                auto scale = glm::vec3(0.2, 0.2, 1.0);
                rectangles.push_back(RectWrapper::Make(rect, trans, scale));
            }
            for (int i = 0; i < 6; i++) {
                auto rect = vr::Rectangle::MakeSingleColor(director, glm::vec3(0.4+ i* 0.06, 0.5+ i* 0.06, 0.6+ i* 0.06));
                auto trans = glm::vec3(0.5 * i, 1, 0.0);
                auto scale = glm::vec3(0.2, 0.2, 1.0);
                rectangles.push_back(RectWrapper::Make(rect, trans, scale));
            }
        }

        for (auto& rect_wrapper : rectangles) {
            glm::mat4 model(1.0f);
            model = glm::translate(model, rect_wrapper->translation_);
            model = glm::scale(model, rect_wrapper->scale_);
            rect_wrapper->rect_->UpdateModelMatrix(model);
            rect_wrapper->rect_->Render(0);
        }

        {
#if 0
            if (!rectangle) {
                rectangle = vr::Rectangle::MakeSingleColor(director, glm::vec3(0.4, 0.5, 0.6));
            }
            {
                glm::mat4 model(1.0f);
                model = glm::translate(model, glm::vec3(0, 0, 0));
                model = glm::scale(model, glm::vec3(1.0f, 0.5f, 1.0f));
                rectangle->UpdateModelMatrix(model);
                rectangle->Render(0);
            }

            if (!rectangle2) {
                rectangle2 = vr::Rectangle::MakeSingleColor(director, glm::vec3(0.5, 0.4, 0.2));
                rectangle2->SetDebugEnabled(true);
            }
            {
                glm::mat4 model(1.0f);
                model = glm::translate(model, glm::vec3(2.6, 1, 0));
                model = glm::scale(model, glm::vec3(1.5f, 0.7f, 1.0f));

                rectangle2->UpdateModelMatrix(model);
                rectangle2->Render(0);
            }
#endif
            if (!line) {
                line = vr::Line::Make(director, glm::vec3(0, 0, 0), glm::vec3(1, 2, -2), glm::vec3(1, 0, 0), glm::vec3(0, 1, 0));
            }

            float distance = -2.45f;
            glm::vec3 P = right_position + distance * right_orientation;

            {
                glm::mat4 model(1.0f);
                line->UpdateModelMatrix(model);
                line->UpdateEnds(right_position, P);
                line->Render(0);
            }
            if (!circle) {
                circle = vr::Circle::MakeAttribColor(director);
            }
            {

                glm::mat4 model(1.0f);
                model = glm::translate(model, glm::vec3(P.x, P.y, P.z));
                circle->UpdateModelMatrix(model);
                circle->Render(0);
            }

            {

                //XrMatrix4x4f model;

                glm::mat4 model(1.0f);
                model = glm::translate(model, glm::vec3(intersectionPoint.x, intersectionPoint.y, intersectionPoint.z + 0.01));
                //memcpy(model.m, glm::value_ptr(gmodel), sizeof(model.m));

                //XrMatrix4x4f mvp;
                //XrMatrix4x4f_Multiply(&mvp, &vp, &model);

//                glm::mat4 MVP;
//                float* MVP_PTR = glm::value_ptr(MVP);
//                memcpy(MVP_PTR, mvp.m, sizeof(mvp.m));
//                if (director) {
//                    director->UpdateMVP(MVP);
//                }
                circle->UpdateModelMatrix(model);
                circle->Render(0);
            }

        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Swap our window every other eye for RenderDoc
        static int everyOther = 0;
        if ((everyOther++ & 1) != 0) {
            ksGpuWindow_SwapBuffers(&window);
        }
    }

   private:
#ifdef XR_USE_PLATFORM_ANDROID
    XrGraphicsBindingOpenGLESAndroidKHR m_graphicsBinding{XR_TYPE_GRAPHICS_BINDING_OPENGL_ES_ANDROID_KHR};
#endif

    std::list<std::vector<XrSwapchainImageOpenGLESKHR>> m_swapchainImageBuffers;
    GLuint m_swapchainFramebuffer{0};
    GLuint m_program{0};
    GLint m_modelViewProjectionUniformLocation{0};
    GLint m_vertexAttribCoords{0};
    GLint m_vertexAttribColor{0};
    GLuint m_vao{0};
    GLuint m_cubeVertexBuffer{0};
    GLuint m_cubeIndexBuffer{0};

    // Map color buffer to associated depth buffer. This map is populated on demand.
    std::map<uint32_t, uint32_t> m_colorToDepthMap;
};
}  // namespace

std::shared_ptr<IGraphicsPlugin> CreateGraphicsPlugin_OpenGLES(const std::shared_ptr<Options>& options,
                                                               std::shared_ptr<IPlatformPlugin> platformPlugin) {
    return std::make_shared<OpenGLESGraphicsPlugin>(options, platformPlugin);
}

#endif
