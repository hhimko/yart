
#pragma once


#include <imgui.h>

#include "yart/interface/panel.h"


namespace yart
{
    namespace Interface
    {
        class RenderViewportPanel : public Panel {
        public:
            bool Render(ImGuiWindow* window);

        };

    } // namespace Interface
} // namespace yart
