IRLAddExample_cpp(
  TARGET_NAME
    advector
  HDRS
    "basic_mesh.h"
    "circle_rotation_2d.h"
    "data.h"
    "deformation_2d.h"
    "reconstruction_types.h"
    "solver.h"
    "vof_advection.h"
  SRCS
    "main.cpp"
    "circle_rotation_2d.cpp"
    "basic_mesh.cpp"
    "vof_advection.cpp"
    "solver.cpp"
    "reconstruction_types.cpp"
    "deformation_2d.cpp"
  OUTPUT_DIR
    "${IRL_EXAMPLES_OUTPUT_DIR}/advector"
  COLLECTION
    "examples_advector"
)

configure_file("visualize.m" "${IRL_EXAMPLES_OUTPUT_DIR}/advector/visualize.m" COPYONLY)
