workspace "cthing"
configurations { "debug", "release" }
platforms { "sse", "no_sse" }
language "C"
includedirs { "src" }
targetdir "bin/%{cfg.buildcfg}"
flags { "Symbols", "FatalWarnings", "C++11" }

filter "platforms:sse"
defines { "CT_FEATURE_SSE" }

filter "configurations:debug"
defines { "DEBUG", "CT_FEATURE_CHECKS", "CT_FEATURE_CHECK_MEM" }

filter "configurations:release"
defines { "NDEBUG", "CT_FEATURE_LOG" }
optimize "Size"
linkoptions { "-flto" }

----- test

project "test"
kind "ConsoleApp"
files { "src/**.h", "src/**.c", "test/**.c" }
removefiles {"src/geom/mesh.c" }
defines { "CT_FEATURE_ANSI" }

----- lib

project "lib"
files { "src/**.h", "src/**.c" }
removefiles {"src/geom/mesh.c", "src/common/test.c" }
kind "StaticLib"
targetname "cthing"

----- poisson -----

project "ex-poisson"
kind "ConsoleApp"
files { "examples/poisson/*.c" }
includedirs { "examples/common", "examples/poisson" }
links "lib"

----- dla -----

project "ex-dla"
kind "ConsoleApp"
files { "examples/dla/*.c" }
includedirs { "examples/common", "examples/dla" }
links "lib"
