cmake_minimum_required(VERSION 3.6)

# begin with compilation of SDL2 copy 
add_subdirectory(SDL2)


project(TP_PRG_AV LANGUAGES CXX)


# convenient function for easy compilation of TP
# usage: compilation(EXEC nom_executable SRC fichiers sources .cpp et .h USE_SDL)
# add USE_SDL if you want to use SDL2 library

function(COMPILATION)
    cmake_parse_arguments( COMPILATION_PREFIX "USE_SDL" "EXEC" "SRC" ${ARGN})
    add_executable(${COMPILATION_PREFIX_EXEC} "")
    target_sources(${COMPILATION_PREFIX_EXEC} PRIVATE ${COMPILATION_PREFIX_SRC})
    target_include_directories(${COMPILATION_PREFIX_EXEC} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}})
    set_target_properties(${COMPILATION_PREFIX_EXEC} PROPERTIES CXX_STANDARD 17)
    if (${COMPILATION_PREFIX_USE_SDL})
        target_link_libraries(${COMPILATION_PREFIX_EXEC} PRIVATE SDL2main SDL2-static SDL2_ttf)
    endif()
endfunction()


set(SRC_FILES_TP1
    code/Game.cpp
    )
compilation(EXEC game SRC ${SRC_FILES_TP1}  USE_SDL )

