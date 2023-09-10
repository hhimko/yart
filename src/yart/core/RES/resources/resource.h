////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Declaration of the Resource base class
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once


#include <string>

#include "yart/core/utils/yart_utils.h"


namespace yart
{
    namespace RES
    {
        /// @brief Type for specifying resource UUIDs
        typedef size_t resourceID_t;

        /// @brief Type for storing `Resource` subclasses class IDs
        /// @details Used for uniquely identifying `Resource` child classes
        /// @note Should be compatible with `ResourceType_` enum's underlying type from `res_internal.h`
        typedef uint8_t ResourceType;


        /// @brief Base class for visual resources 
        /// @warning This class is not intended to be instantiated without deriving
        class Resource {
        public:
            /// @brief Get the uniquely identifying resource ID
            /// @return Resource ID 
            resourceID_t GetID() const { return m_id; }

            /// @brief Get the resource name
            /// @return Resource name
            const char* GetName() const { return m_name.c_str(); }

        protected:
            /// @brief Construct a Resource object
            /// @param name Display name
            /// @param id Resource ID
            Resource(const char* name, resourceID_t id);

            /// @brief Construct a Resource object
            /// @param name Display name
            /// @param id Resource ID
            explicit Resource(const std::string& name, resourceID_t id);

        private:
            /// @brief Load and return the default instance
            /// @details The caller is expected to free the returned default instance 
            /// @warning This method is not intended to be called as is, but should rather be implemented by child subclasses.
            ///     The Resource::LoadDefault() was left for compatibility with Resource-derived classes
            /// @note LoadDefault() should always ensure to return a valid pointer
            /// @return The default resource
            static Resource* LoadDefault() 
            { 
                YART_ABORT("Resource::LoadDefault() should never be called.");
                return nullptr; 
            }

        private:
            /// @brief `Resource` subclass identifier number
            /// @warning This variable is not intended for use as is, but should rather be redefined in child subclasses.
            ///     The Resource::CLASS_ID was left for compatibility with Resource-derived classes
            static const ResourceType CLASS_ID;

            /// @brief Uniquely identifying resource ID
            resourceID_t m_id;
            /// @brief Display name of the resource
            std::string m_name;

        };

    } // namespace RES
} // namespace yart
