////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Implementation of the abstract View base class 
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "view.h"


#include "yart/interface/views/renderer_view.h"
#include "yart/interface/views/viewport_view.h"
#include "yart/interface/views/world_view.h"


namespace yart
{
    namespace Interface
    {
        const View** View::GetAllViews(size_t* views_count)
        {
            static constexpr size_t count = 3;
            *views_count = count;

            // Views are sorted in order they should be rendered
            static const View* views[count] = {
                dynamic_cast<View*>(RendererView::Get()),
                dynamic_cast<View*>(ViewportView::Get()),
                dynamic_cast<View*>(WorldView::Get())
            };

            return views;
        }

    } // namespace Interface
} // namespace yart
