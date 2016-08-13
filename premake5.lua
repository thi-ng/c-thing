workspace "cthing"
configurations { "debug", "release" }
platforms { "sse", "no_sse" }
language "C"
includedirs { "ext", "src" }
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
files { "ext/**.c", "test/**.c" }
defines { "CT_FEATURE_ANSI" }
links "lib"
dependson "lib"
flags { "FatalWarnings", "LinkTimeOptimization" }

----- test w/ addr sanitizer

project "test_asan"
kind "ConsoleApp"
files { "ext/**.c", "test/**.c" }
defines { "CT_FEATURE_ANSI", "CT_FEATURE_CHECKS", "CT_FEATURE_CHECK_MEM" }
links "lib"
dependson "lib"
flags { "FatalWarnings" }
buildoptions { "-fsanitize=address", "-fno-omit-frame-pointer" }
linkoptions { "-fsanitize=address" }

----- test w/ mem sanitizer (currently Linux x86_64 only & clang)
--[[--
project "test_msan"
kind "ConsoleApp"
files { "src/**.h", "src/**.c", "test/**.c" }
removefiles {"src/geom/mesh.c" }
defines { "CT_FEATURE_ANSI" }
flags { "FatalWarnings" }
buildoptions { "-fsanitize=mem", "-fno-omit-frame-pointer", "-fsanitize-memory-track-origins" }
linkoptions { "-fsanitize=mem", "-fsanitize-memory-track-origins" }
--]]--

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

----- chull -----

project "ex-chull"
kind "ConsoleApp"
files { "examples/convexhull/main.c" }
includedirs { "examples/common", "examples/convexhull" }
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
files { "examples/verlet/main.c" }
includedirs { "examples/common", "examples/verlet" }
links "lib"
dependson "lib"
flags { "LinkTimeOptimization" }

project "ex-verlet-pack"
kind "ConsoleApp"
files { "examples/verlet/cpack.c" }
includedirs { "examples/common", "examples/verlet" }
links "lib"
dependson "lib"
flags { "LinkTimeOptimization" }

----- clipping -----

project "ex-polyclip"
kind "ConsoleApp"
files { "examples/polyclip/*.c" }
includedirs { "examples/common", "examples/polyclip" }
links "lib"
dependson "lib"
flags { "LinkTimeOptimization" }

----- offset -----

project "ex-polyoffset"
kind "ConsoleApp"
files { "examples/polyoffset/*.c" }
includedirs { "examples/common", "examples/polyoffset" }
links "lib"
dependson "lib"
flags { "LinkTimeOptimization" }

----- voronoi -----

project "ex-voronoi"
kind "ConsoleApp"
files { "examples/voronoi/*.c" }
includedirs { "examples/common", "examples/voronoi" }
links "lib"
dependson "lib"
flags { "LinkTimeOptimization" }

----- glfw -----

project "ex-glfw01"
kind "ConsoleApp"
files { "examples/glfw/*.c" }
includedirs { "examples/common", "examples/glfw" }
links "lib"
dependson "lib"
flags { "LinkTimeOptimization" }
linkoptions { "-lglfw3", "-framework Cocoa", "-framework OpenGL" }
