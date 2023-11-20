
#pragma once


#include <imgui.h>

#include "yart/interface/panel.h"


namespace yart
{
    namespace Interface
    {
        class ContextPanel : public Panel {
        public:
            bool Render(ImGuiWindow* window);


        private:
            void RenderContextNavBar();

        };

    } // namespace Interface
} // namespace yart

