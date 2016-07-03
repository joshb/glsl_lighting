
cmake_minimum_required(VERSION 2.8)

if(APPLE)
    add_definitions(-DAPPLE)
endif(APPLE)

find_package(OpenGL)
find_package(GLUT)

set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}"/bin")

include_directories(
  ${OPENGL_INCLUDE_DIR}
  ${GLUT_INCLUDE_DIR}
)

add_executable(glsl_lighting
  ./main.c
  ./scene.c
  ./shader.c
  )
target_link_libraries(glsl_lighting
  m
  ${OPENGL_gl_LIBRARY}
  ${GLUT_glut_LIBRARY}
  )

