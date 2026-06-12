#pragma once

#include <graphcontext.hpp>

class NodeExecutor 
{
public:
    virtual void execute(const GraphContext& context) = 0;
    virtual ~NodeExecutor() = default;
};