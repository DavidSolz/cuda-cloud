#pragma once

#include <renderqueue.hpp>

#include <texture2d.hpp>
#include <register.hpp>

struct GraphContext {
    Register<Texture2D> textureRegister;
    RenderQueue renderQueue;
};