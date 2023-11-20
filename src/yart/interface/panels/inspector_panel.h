
#pragma once


#include <imgui.h>

#include "yart/interface/panel.h"


namespace yart
{
    namespace Interface
    {
        class InspectorPanel : public Panel {
        public:
            bool Render(ImGuiWindow* window);

        };

    } // namespace Interface
} // namespace yart

