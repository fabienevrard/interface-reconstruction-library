set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build Type")

set(CMAKE_CXX_COMPILER "g++-9" CACHE STRING "C++ Compiler")
set(CMAKE_Fortran_COMPILER "gfortran-9" CACHE STRING "Fortran Compiler")


set(IRL_CXX_FLAGS "-g -O0 -pedantic -Wall -Wextra -Wcast-align -Wcast-qual \
		     -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 \
		     -Winit-self -Wlogical-op -Wmissing-declarations \
		     -Wmissing-include-dirs -Wnoexcept -Wold-style-cast \
		     -Woverloaded-virtual -Wredundant-decls -Wshadow \
		     -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel \
		     -Wstrict-overflow=5 -Wswitch-default -Wundef -Werror -Wno-unused \
		     -Wconversion -Winline -Wno-error=padded -Wno-error=strict-overflow \
		     -Wstrict-aliasing=1 -Wunsafe-loop-optimizations"
    CACHE STRING "C++ compile flags")

set(IRL_Fortran_FLAGS "-g -O0 -ffree-line-length-none -fbacktrace"
    CACHE STRING "Fortran compile flags")