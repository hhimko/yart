////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Definition of the abstract View base class 
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once


#include <imgui.h>


#define YART_VIEW_ICON_COLOR_GRAY 0xFF6F767D


namespace yart
{
    namespace Interface
    {
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Abstract base View class, for defining context panel views for a specific YART component
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        class View {
        public:
            /// @brief View class custom constructor
            /// @param name View name, for displaying in the context panel
            /// @param icon View icon code point from the icons font, for displaying in the context panel
            /// @param icon_color View icon color, for displaying in the context panel
            View(const char* name, const char* icon, ImU32 icon_color)
                : m_name(name), m_icon(icon), m_iconColor(icon_color) { };

            /// @brief Try to retrieve a target instance for the view
            /// @return Target instance pointer for this frame, or `nullptr` if no targets are active
            virtual void* GetViewTarget() const = 0;  

            /// @brief Issue view render commands for a given view target
            /// @param target View target instance, retrieved from View::GetViewTarget()
            /// @return Whether any changes were made by the user since the last frame
            virtual bool Render(void* target) const = 0;

            /// @brief Get the view name
            /// @return View name
            const char* GetName() const 
            { 
                return m_name; 
            }

            /// @brief Get the view icon code point from the icons font
            /// @return View icon code point
            const char* GetIcon() const 
            { 
                return m_icon; 
            }

            /// @brief Get the view icon color in `ARGB` format
            /// @return View icon color
            ImU32 GetIconColor() const 
            { 
                return m_iconColor; 
            }

            /// @brief Get all YART application view instances 
            /// @param views_count Size of the returned array
            /// @return YART views array
            const static View** GetAllViews(size_t* views_count);

        private:
            const char* m_name; ///< View name, for displaying in the context panel
            const char* m_icon; ///< View icon code point from the icons font, for displaying in the context panel
            const ImU32 m_iconColor; ///< View icon color, for displaying in the context panel

        };

    } // namespace Interface
} // namespace yart
