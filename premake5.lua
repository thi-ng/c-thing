workspace "cthing"
configurations { "debug", "release", "test" }
language "C"
includedirs { "src" }
targetdir "bin/%{cfg.buildcfg}"
files { "src/**.h", "src/**.c" }
flags { "Symbols", "FatalWarnings", "C++11" }

----- test

project "test"
removeconfigurations { "debug", "release" }
removeflags "Symbols"
kind "ConsoleApp"
files { "test/**.c" }

filter "configurations:test"
defines { "DEBUG", "TRACE_MPOOL" }
optimize "Size"

----- lib

project "lib"
removeconfigurations "test"
kind "StaticLib"
targetname "thing-geom"

filter "configurations:debug"
defines { "DEBUG" }

filter "configurations:release"
defines { "NDEBUG" }
optimize "Size"
