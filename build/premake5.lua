solution "activity-tracker"
  configurations { "Debug32", "Release32", "Debug64", "Release64" }

  filter "configurations:*32"
    architecture "x86"

  filter "configurations:*64"
    architecture "x86_64"

  filter { "configurations:Debug*" }
    symbols "On"

  filter { "configurations:Release*" }
    optimize "On"

  characterset "Unicode"

  project "activity-tracker"
    kind "ConsoleApp"
    language "C++"
    files {
      "../src/**.h",
      "../src/**.cpp",
      "../libs/jsoncpp-1.7.7/**.h",
      "../libs/jsoncpp-1.7.7/**.cpp",
    }
    includedirs {
      "../src",
      "../src/logger",
      "../libs/jsoncpp-1.7.7",
    }
    links { "User32", "Ws2_32", "Advapi32" }

  configuration "Debug32"
    defines { "DEBUG" }
    targetdir ("Debug32")

  configuration "Release32"
    defines { "RELEASE", "NDEBUG" }
    targetdir ("Release32")

  configuration "Debug64"
    defines { "DEBUG" }
    targetdir ("Debug64")

  configuration "Release64"
    defines { "RELEASE", "NDEBUG" }
    targetdir ("Release64")

  if _ACTION == "clean" then
    os.rmdir("obj")
    os.rmdir("Debug32")
    os.rmdir("Release32")
    os.rmdir("Debug64")
    os.rmdir("Release64")
  end
