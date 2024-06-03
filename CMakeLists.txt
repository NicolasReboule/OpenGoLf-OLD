cmake_minimum_required (VERSION 3.8)
project(OpenGoLf)
set(CMAKE_CXX_STANDARD 20)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
    cmake_policy(SET CMP0141 NEW)
    set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

include_directories(
        include
        include/utils
        include/extern
        include/extern/Cyclone
)

file(GLOB_RECURSE CYCLONE_FILES src/extern/Cyclone/*.cpp include/extern/Cyclone/*.h)
file(GLOB_RECURSE UTILS_FILES src/utils/*.cpp include/utils/*.h)

add_executable (${PROJECT_NAME} main.cpp
        ${CYCLONE_FILES}
        ${UTILS_FILES}
        src/extern/gl3w.c
        src/Window.cpp
        src/SkyBox.cpp
)

#find_package(OpenGL REQUIRED)

target_include_directories(${PROJECT_NAME} PUBLIC
        include
        include/utils
        include/extern
        include/extern/Cyclone
)
target_link_directories(${PROJECT_NAME} PUBLIC lib)
target_link_libraries(${PROJECT_NAME}
        opengl32.lib
        glu32.lib
        glfw3.lib
)

file(COPY ${CMAKE_SOURCE_DIR}/assets/ DESTINATION ${CMAKE_BINARY_DIR}/assets/)
file(COPY ${CMAKE_SOURCE_DIR}/shaders/ DESTINATION ${CMAKE_BINARY_DIR}/shaders/)
