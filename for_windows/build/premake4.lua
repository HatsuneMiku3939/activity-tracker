solution "ActLogger"
  configurations { "Debug", "Release" }

  project "ActLogger"
    kind "WindowedApp"
    language "C++"
    files {
      "../src/**.h",
      "../src/**.cpp",
      "../src/**.rc",
      "../../**.h",
      "../../**.cpp"
    }
    excludes {
      "../../tracker.cpp"
    }

  configuration "Debug"
    defines { "DEBUG", "WIN32", "PSAPI_VERSION=1" }
    includedirs {
    }
    libdirs {
    }
    links { "kernel32", "user32", "gdi32", "winspool", "shell32", "ole32", "oleaut32", "uuid", "comdlg32", "advapi32", "comctl32", "rpcrt4", "shlwapi", "glu32", "opengl32", "psapi", "wininet", "iphlpapi",
    }
    flags { "Symbols", "EnableSSE", "EnableSSE2", "UNICODE", "StaticRuntime", "WinMain", "NoManifest" }
    linkoptions { "/ignore:4099" }
    targetdir ("Debug")

  configuration "Release"
    defines { "RELEASE", "NDEBUG", "WIN32", "PSAPI_VERSION=1" }
    includedirs {
    }
    libdirs {
    }
    links { "kernel32", "user32", "gdi32", "winspool", "shell32", "ole32", "oleaut32", "uuid", "comdlg32", "advapi32", "comctl32", "rpcrt4", "shlwapi", "glu32", "opengl32", "psapi", "wininet", "iphlpapi",
    }
    flags { "Symbols", "Optimize", "EnableSSE", "EnableSSE2", "UNICODE", "StaticRuntime", "WinMain", "NoManifest" }
    linkoptions { "/ignore:4099" }
    targetdir ("Release")

  newaction {
    trigger = "debug",
    description = "Debug build",
    execute = function()
      os.execute("msbuild actlogger.sln /p:Configuration=Debug")
    end
  }

  newaction {
    trigger = "release",
    description = "Release build",
    execute = function()
      os.execute("msbuild actlogger.sln /p:Configuration=Release")
    end
  }

  if _ACTION == "clean" then
    os.rmdir("obj")
    os.rmdir("Debug")
    os.rmdir("Release")
  end
