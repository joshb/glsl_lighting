
cmake_minimum_required(VERSION 2.8)

find_package(GLUT)

set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}"/bin")

include_directories(
  ${GLUT_INCLUDE_DIR}
)

add_executable(glsl_lighting
  ./main.c
  ./scene.c
  ./shader.c
  )
target_link_libraries(glsl_lighting
  m GL 
  ${GLUT_glut_LIBRARY}
  )

