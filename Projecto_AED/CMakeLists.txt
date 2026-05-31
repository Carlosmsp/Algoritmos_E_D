cmake_minimum_required(VERSION 3.16)
project(MobilidadeLisboa CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Flags de compilação recomendadas

# Executável principal
add_executable(mobilidade src/main.cpp)

# Caminhos de inclusão:
#   include/  → nlohmann/json.hpp (cópia local)
#   src/      → cabeçalhos do projeto
target_include_directories(mobilidade PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

# Ligação com stdc++fs (necessária em algumas versões do GCC < 9)
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0)
    target_link_libraries(mobilidade stdc++fs)
endif()

# ── Instruções de compilação ─────────────────────────────────────────────────
# mkdir build && cd build
# cmake ..
# cmake --build .
# ./mobilidade ../data ../output
