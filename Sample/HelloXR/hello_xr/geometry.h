// Copyright (c) 2017-2020 The Khronos Group Inc
//
// SPDX-License-Identifier: Apache-2.0

#pragma once

namespace Geometry
{

    struct Vertex {
        XrVector3f Position;
        XrVector3f Color;
    };

    constexpr XrVector3f Red{1, 0, 0};
    constexpr XrVector3f DarkRed{0.25f, 0, 0};
    constexpr XrVector3f Green{0, 1, 0};
    constexpr XrVector3f DarkGreen{0, 0.25f, 0};
    constexpr XrVector3f Blue{0, 0, 1};
    constexpr XrVector3f DarkBlue{0, 0, 0.25f};

    // Vertices for a 1x1x1 meter cube. (Left/Right, Top/Bottom, Front/Back)
    constexpr XrVector3f LBB{-0.5f, -0.5f, -0.5f};
    constexpr XrVector3f LBF{-0.5f, -0.5f, 0.5f};
    constexpr XrVector3f LTB{-0.5f, 0.5f, -0.5f};
    constexpr XrVector3f LTF{-0.5f, 0.5f, 0.5f};
    constexpr XrVector3f RBB{0.5f, -0.5f, -0.5f};
    constexpr XrVector3f RBF{0.5f, -0.5f, 0.5f};
    constexpr XrVector3f RTB{0.5f, 0.5f, -0.5f};
    constexpr XrVector3f RTF{0.5f, 0.5f, 0.5f};

    #define CUBE_SIDE(V1, V2, V3, V4, V5, V6, COLOR) {V1, COLOR}, {V2, COLOR}, {V3, COLOR}, {V4, COLOR}, {V5, COLOR}, {V6, COLOR},

    constexpr Vertex c_cubeVertices[] = {
        CUBE_SIDE(LTB, LBF, LBB, LTB, LTF, LBF, DarkRed)    // -X
        CUBE_SIDE(RTB, RBB, RBF, RTB, RBF, RTF, Red)        // +X
        CUBE_SIDE(LBB, LBF, RBF, LBB, RBF, RBB, DarkGreen)  // -Y
        CUBE_SIDE(LTB, RTB, RTF, LTB, RTF, LTF, Green)      // +Y
        CUBE_SIDE(LBB, RBB, RTB, LBB, RTB, LTB, DarkBlue)   // -Z
        CUBE_SIDE(LBF, LTF, RTF, LBF, RTF, RBF, Blue)       // +Z
    };

    constexpr float RayNear = 0.5f;
    constexpr float RaySize = 0.0125f;
    constexpr float RayLength = 35.0f;
    constexpr float RayRemoteFactor = 0.25f;

    constexpr XrVector3f LBB2{-RaySize * RayRemoteFactor, -RaySize * RayRemoteFactor, -RayLength};
    constexpr XrVector3f LBF2{-RaySize, -RaySize, RayNear};
    constexpr XrVector3f LTB2{-RaySize * RayRemoteFactor, RaySize * RayRemoteFactor, -RayLength};
    constexpr XrVector3f LTF2{-RaySize, RaySize, RayNear};
    constexpr XrVector3f RBB2{RaySize * RayRemoteFactor, -RaySize * RayRemoteFactor, -RayLength};
    constexpr XrVector3f RBF2{RaySize, -RaySize, RayNear};
    constexpr XrVector3f RTB2{RaySize * RayRemoteFactor, RaySize * RayRemoteFactor, -RayLength};
    constexpr XrVector3f RTF2{RaySize, RaySize, RayNear};

    constexpr XrVector3f Red2{1, 0, 0};
    constexpr XrVector3f DarkRed2{0.25f, 0, 0};
    constexpr XrVector3f Green2{0, 1, 0};
    constexpr XrVector3f DarkGreen2{0, 0.25f, 0};
    constexpr XrVector3f Blue2{0, 0, 1};
    constexpr XrVector3f DarkBlue2{0, 0, 0.25f};
    constexpr XrVector3f White{1.0f, 1.0f, 1.0f};
    constexpr XrVector3f LightBlue{0.529, 0.808, 0.98};

    constexpr Vertex c_cubeVertices2[] = {
        CUBE_SIDE(LTB2, LBF2, LBB2, LTB2, LTF2, LBF2, LightBlue)    // -X
        CUBE_SIDE(RTB2, RBB2, RBF2, RTB2, RBF2, RTF2, LightBlue)        // +X
        CUBE_SIDE(LBB2, LBF2, RBF2, LBB2, RBF2, RBB2, LightBlue)  // -Y
        CUBE_SIDE(LTB2, RTB2, RTF2, LTB2, RTF2, LTF2, LightBlue)      // +Y
        CUBE_SIDE(LBB2, RBB2, RTB2, LBB2, RTB2, LTB2, LightBlue)   // -Z
        CUBE_SIDE(LBF2, LTF2, RTF2, LBF2, RTF2, RBF2, LightBlue)       // +Z
    };

    // Winding order is clockwise. Each side uses a different color.
    constexpr unsigned short c_cubeIndices[] = {
        0,  1,  2,  3,  4,  5,   // -X
        6,  7,  8,  9,  10, 11,  // +X
        12, 13, 14, 15, 16, 17,  // -Y
        18, 19, 20, 21, 22, 23,  // +Y
        24, 25, 26, 27, 28, 29,  // -Z
        30, 31, 32, 33, 34, 35,  // +Z
    };

}  // namespace Geometry