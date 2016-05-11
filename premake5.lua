workspace "cthing"
configurations { "debug", "release", "test" }

----- test

project "test"
removeconfigurations { "debug", "release" }
kind "ConsoleApp"
language "C"
targetdir "bin/test"
includedirs { "src" }
files { "src/**.h", "src/**.c", "test/**.c" }

filter "configurations:test"
defines { "DEBUG", "TRACE_MPOOL" }
flags { "Symbols" }
optimize "Size"

----- lib

project "lib"
removeconfigurations "test"
kind "StaticLib"
language "C"
targetdir "bin/%{cfg.buildcfg}"
files { "src/**.h", "src/**.c" }

filter "configurations:debug"
defines { "DEBUG" }
flags { "Symbols" }

filter "configurations:release"
defines { "NDEBUG" }
optimize "On"
