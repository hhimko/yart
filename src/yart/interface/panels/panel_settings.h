////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Base classes for saveable UI panels and their settings types 
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once


#include <vector>

#include "yart/interface/panel.h"


namespace yart
{
    namespace Interface
    {
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Base class for saveable UI panel settings
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        class PanelSettings {
        public:
            /// @brief PanelSettings custom constructor
            /// @param panel Panel instance, for which the settings apply
            PanelSettings(const Panel* const panel)
                : panelType(panel->GetPanelType()), panelName(panel->GetPanelName()) { }

            /// @brief PanelSettings class virtual destructor
            virtual ~PanelSettings() = default;

            /// @brief Find saved settings for a particular panel
            /// @param panel Panel instance, for which to find settings
            /// @return Cached settings object, or `nullptr` if not found
            static PanelSettings* FindPanelSettings(const Panel* const panel);

            /// @brief Save a new settings object, for future retrieval
            /// @param settings Settings object for a particular panel
            static void AddPanelSettings(PanelSettings* settings);

            /// @brief Remove all saved panel settings
            static void ClearPanelSettings();

        public:
            PanelType panelType; ///< Type of the panel, for which the settings apply
            const char* panelName; ///< Name of the panel, for which the settings apply

        private:
            inline static std::vector<PanelSettings*> s_settings; ///< Vector of all saved settings

        };


        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Base class for saveable UI panels
        /// @tparam SettingsT settings class for the panel type. Must inherit class Interface::PanelSettings
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        template<typename SettingsT>
        class SaveablePanel {
        public:
            /// @brief SaveablePanel class custom constructor
            /// @param panel Panel instance
            SaveablePanel(const Panel* const panel)
                : m_panel(panel) { }

            /// @brief SaveablePanel class virtual destructor
            virtual ~SaveablePanel() = default;

        protected:
            /// @brief Save the current panel state 
            /// @details Intended to be called in destructors of implementing classes
            void SavePanelSettings() const
            {
                // Get the saveable state of this panel 
                SettingsT new_settings = GetPanelSettings();

                // Try to retrieve saved settings for this panel, if any 
                PanelSettings* old_settings = PanelSettings::FindPanelSettings(m_panel);

                // Save the settings
                if (old_settings == nullptr) {
                    // If no settings for this panel exist yet, create a new object
                    SettingsT* settings = new SettingsT(new_settings); // Managed by PanelSettings class
                    PanelSettings::AddPanelSettings(dynamic_cast<PanelSettings*>(settings));
                } else {
                    // If settings for this object already exist, update it
                    SettingsT* settings = dynamic_cast<SettingsT*>(old_settings);
                    *settings = new_settings; 
                }
            }

            /// @brief Try to load and apply previously saved state for this panel, if it exists
            /// @details Intended to be called at the end of constructors of implementing classes
            void LoadAndApplyPanelSettings()
            {
                const PanelSettings* const settings = PanelSettings::FindPanelSettings(m_panel);
                if (settings != nullptr)
                    ApplyPanelSettings(dynamic_cast<const SettingsT* const>(settings));
            }

        private:
            /// @brief Get the saveable state of this panel 
            /// @return New panel settings object for this instance
            virtual SettingsT GetPanelSettings() const = 0;

            /// @brief Apply a given settings object to this panel
            /// @param settings Panel settings object from which to recreate panel state
            virtual void ApplyPanelSettings(const SettingsT* const settings) = 0;

        private:
            const Panel* const m_panel;

        };

    } // namespace Interface
} // namespace yart
