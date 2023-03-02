workspace "yart"
   architecture "x64"
   configurations { "Debug", "Release" }
   startproject "yart"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "yart"
   kind "ConsoleApp"
   language "C++"
   cppdialect "C++17"
   targetdir "build/bin/%{cfg.buildcfg}"
   staticruntime "off"

   files { "src/**.h", "src/**.cpp" }

   includedirs
   {
      "lib/include",
      "lib/glm"
   }

   targetdir ("build/bin/" .. outputdir .. "/%{prj.name}")
   objdir ("build/bin-int/" .. outputdir .. "/%{prj.name}")

   filter "system:windows"
      systemversion "latest"
      defines { "PLATFORM_WINDOWS" }

   filter "configurations:Debug"
      defines { "DEBUG" }
      runtime "Debug"
      symbols "On"

   filter "configurations:Release"
      defines { "NDEBUG" }
      runtime "Release"
      optimize "On"
      symbols "Off"
