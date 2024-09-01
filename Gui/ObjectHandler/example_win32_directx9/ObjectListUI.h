#pragma once

#include "imgui.h"
#include "ObjectList.h"

#include <iostream>

void DoDockSpace();

namespace Table 
{
    void RenderUI(ObjectsList& objList);
}

namespace Menu
{
    void RenderUI(ObjectsList& objList);
}

namespace AddObjectMenu
{
    void RenderUI(ObjectsList& objList);
}




