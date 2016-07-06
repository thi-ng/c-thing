workspace "cthing"
configurations { "debug", "release" }
platforms { "sse", "no_sse" }
language "C"
includedirs { "src" }
targetdir "bin/%{cfg.buildcfg}"
flags { "Symbols", "C++11" }
linkoptions "-lm"

filter "platforms:sse"
defines { "CT_FEATURE_SSE" }
buildoptions { "-msse", "-msse2", "-msse3", "-msse4.1" }

filter "configurations:debug"
defines { "DEBUG", "CT_FEATURE_CHECKS", "CT_FEATURE_CHECK_MEM" }

filter "configurations:release"
defines { "NDEBUG", "CT_FEATURE_LOG" }
optimize "Size"

----- test

project "test"
kind "ConsoleApp"
files { "src/**.h", "src/**.c", "test/**.c" }
removefiles {"src/geom/mesh.c" }
defines { "CT_FEATURE_ANSI" }
flags { "FatalWarnings" }

----- test w/ analyzer

project "test_ana"
kind "ConsoleApp"
files { "src/**.h", "src/**.c", "test/**.c" }
removefiles {"src/geom/mesh.c" }
defines { "CT_FEATURE_ANSI" }
flags { "FatalWarnings" }
toolset "clang"
buildoptions { "-fsanitize=address" }
linkoptions { "-fsanitize=address" }

----- lib

project "lib"
files { "src/**.h", "src/**.c" }
removefiles {"src/geom/mesh.c", "src/common/test.c" }
kind "StaticLib"
targetname "cthing"

----- ca1d -----

project "ex-ca1d"
kind "ConsoleApp"
files { "examples/ca/ca1d.c" }
includedirs { "examples/common", "examples/ca" }
links "lib"
dependson "lib"
flags { "LinkTimeOptimization" }

----- ca2d -----

project "ex-ca2d"
kind "ConsoleApp"
files { "examples/ca/ca2d.c" }
includedirs { "examples/common", "examples/ca" }
links "lib"
dependson "lib"
flags { "LinkTimeOptimization" }

----- dla -----

project "ex-dla"
kind "ConsoleApp"
files { "examples/dla/*.c" }
includedirs { "examples/common", "examples/dla" }
links "lib"
dependson "lib"
flags { "LinkTimeOptimization" }

----- poisson -----

project "ex-poisson"
kind "ConsoleApp"
files { "examples/poisson/*.c" }
includedirs { "examples/common", "examples/poisson" }
links "lib"
dependson "lib"
flags { "LinkTimeOptimization" }

----- verlet -----

project "ex-verlet"
kind "ConsoleApp"
files { "examples/verlet/*.c" }
includedirs { "examples/common", "examples/verlet" }
links "lib"
dependson "lib"
flags { "LinkTimeOptimization" }
