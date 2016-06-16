workspace "cthing"
configurations { "debug", "release", "test" }
platforms { "sse", "no_sse" }
language "C"
includedirs { "src" }
targetdir "bin/%{cfg.buildcfg}"
flags { "Symbols", "FatalWarnings", "C++11" }

filter "platforms:sse"
defines { "CT_FEATURE_SSE" }

----- test

project "test"
removeconfigurations { "debug", "release" }
kind "ConsoleApp"
files { "src/**.h", "src/**.c", "test/**.c" }
removefiles {"src/geom/mesh.c" }

filter "configurations:test"
defines { "NDEBUG", "CT_FEATURE_ANSI", "CT_FEATURE_CHECKS", "CT_FEATURE_CHECK_MEM" }
optimize "Size"

----- lib

project "lib"
removeconfigurations "test"
files { "src/**.h", "src/**.c" }
removefiles {"src/geom/mesh.c", "src/common/test.c" }
kind "StaticLib"
targetname "cthing"

filter "configurations:debug"
defines { "DEBUG", "CT_FEATURE_CHECKS", "CT_FEATURE_CHECK_MEM" }

filter "configurations:release"
defines { "NDEBUG", "CT_FEATURE_LOG" }
optimize "Size"

----- examples -----

----- poisson -----

project "ex-poisson"
removeconfigurations "test"
kind "ConsoleApp"
files { "examples/poisson/*.c" }
includedirs { "examples/common", "examples/poisson" }
links "lib"

filter "configurations:debug"
defines { "DEBUG", "CT_FEATURE_ANSI", "CT_FEATURE_CHECKS", "CT_FEATURE_CHECK_MEM" }

filter "configurations:release"
defines { "NDEBUG", "CT_FEATURE_LOG" }
optimize "Size"
linkoptions { "-flto" }

----- dla -----

project "ex-dla"
removeconfigurations "test"
kind "ConsoleApp"
files { "examples/dla/*.c" }
includedirs { "examples/common", "examples/dla" }
links "lib"

filter "configurations:debug"
defines { "DEBUG", "CT_FEATURE_ANSI", "CT_FEATURE_CHECKS", "CT_FEATURE_CHECK_MEM" }

filter "configurations:release"
defines { "NDEBUG", "CT_FEATURE_LOG" }
optimize "Size"
linkoptions { "-flto" }
