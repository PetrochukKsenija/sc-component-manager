file(GLOB_RECURSE SOURCES "*.cpp" "*.hpp")

add_library(init-lib SHARED ${SOURCES})
target_link_libraries(init-lib LINK_PUBLIC common-lib)
target_include_directories(init-lib PUBLIC ${CMAKE_CURRENT_LIST_DIR})

sc_codegen_ex(init-lib ${CMAKE_CURRENT_LIST_DIR}/module ${CMAKE_CURRENT_LIST_DIR}/generated)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(init-lib)
endif()
