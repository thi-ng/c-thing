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
kind "ConsoleApp"
files { "test/**.c" }

filter "configurations:test"
defines { "DEBUG", "CT_FEATURE_ANSI", "CT_FEATURE_CHECKS", "CT_FEATURE_TRACE_MPOOL", "CT_FEATURE_SSE" }
optimize "Size"

----- lib

project "lib"
removeconfigurations "test"
kind "StaticLib"
targetname "thing-geom"

filter "configurations:debug"
defines { "DEBUG", "CT_FEATURE_CHECKS", "CT_FEATURE_TRACE_MPOOL", "CT_FEATURE_SSE" }

filter "configurations:release"
defines { "NDEBUG", "CT_FEATURE_CHECKS", "CT_FEATURE_SSE" }
optimize "Size"
