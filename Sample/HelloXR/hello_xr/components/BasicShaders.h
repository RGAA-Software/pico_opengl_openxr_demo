//
// Created by hy on 2023/6/14.
//

#pragma once

#include <string>

static const std::string kBasicVertexShader = R"(

#version 320 es

in vec3 VertexPos;
in vec3 VertexColor;
in vec2 VertexUV;

out vec3 PSVertexColor;
out vec2 PSVertexUV;

uniform mat4 MVP;

void main() {
    gl_Position = MVP * vec4(VertexPos, 1.0);
    PSVertexColor = VertexColor;
    PSVertexUV = VertexUV;
}

)";

static const std::string kBasicColorFragmentShader = R"(

#version 320 es

in lowp vec3 PSVertexColor;
out lowp vec4 FragColor;

void main() {
   FragColor = vec4(PSVertexColor, 1);
}

)";

static const std::string kBasicSingleColorFragmentShader = R"(
#version 320 es

uniform lowp vec3 UniformColor;
out lowp vec4 FragColor;

void main() {
   FragColor = vec4(UniformColor, 1);
}

)";
