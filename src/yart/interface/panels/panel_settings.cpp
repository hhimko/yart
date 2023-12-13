////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Base classes for saveable UI panels and their settings types 
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "panel_settings.h"


#include <cstring>

#include "yart/common/utils/yart_utils.h"


namespace yart
{
    namespace Interface
    {
        PanelSettings* PanelSettings::FindPanelSettings(const Panel* const panel)
        {
            const PanelType type = panel->GetPanelType();
            const char* name = panel->GetPanelName();

            for (PanelSettings* settings : s_settings) {
                if (settings->panelType == type && strcmp(name, settings->panelName) == 0)
                    return settings;
            }

            return nullptr;
        }

        void PanelSettings::AddPanelSettings(PanelSettings* settings)
        {
            YART_ASSERT(settings != nullptr);
            s_settings.push_back(settings);
        }

        void PanelSettings::ClearPanelSettings()
        {
            for (PanelSettings* settings : s_settings) {
                delete settings;
            }
            
            s_settings.clear();
        }

    } // namespace Interface
} // namespace yart
