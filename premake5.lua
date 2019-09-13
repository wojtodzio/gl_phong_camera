local SDK_ROOT = "sdk"

workspace "zaliczenie"
  configurations { "Debug", "Release" }
  location "build"
  includedirs {
    SDK_ROOT .. "/glew/include",
    SDK_ROOT .. "/glfw-3.3/include"
  }
  libdirs {
    SDK_ROOT .. "/glew/lib",
    SDK_ROOT .. "/glfw-3.3/lib-vc2019"
  }

  configuration "windows"
    links {
      "kernel32",
      "user32",
      "gdi32",
      "winspool",
      "shell32",
      "ole32",
      "oleaut32",
      "uuid",
      "comdlg32",
      "advapi32"
    }

  configuration "Debug"
    debugdir "bin"
    targetdir "bin"

  configuration "Release"
    targetdir "release"

  filter "configurations:*"
    defines { "_WINDOWS", "WIN32" }

  filter "configurations:Debug"
    defines { "DEBUG", "_DEBUG" }
    symbols "On"

  filter "configurations:Release"
    defines { "NDEBUG" }
    optimize "On"


project "camera"
  kind "ConsoleApp"
  language "C++"
  objdir "build/camera"
  includedirs {
    "camera"
  }
  links {
    "opengl32",
    "glew32",
    "glfw3"
  }
  postbuildcommands {
    "{COPY} ../" .. SDK_ROOT .. "/glew/bin/glew32.dll %{cfg.targetdir}",
    "{COPY} ../" .. SDK_ROOT .. "/glfw-3.3/lib-vc2019/glfw3.dll %{cfg.targetdir}"
  }
  files { "camera/**.h", "camera/**.c", "camera/**.cpp", "camera/**.inl", "camera/**.hpp" }