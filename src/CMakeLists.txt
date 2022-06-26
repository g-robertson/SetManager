target_sources(SetManager
    PRIVATE helpers.cpp
) 

target_include_directories(SetManager
    PRIVATE menu
    PRIVATE user-set
)

add_subdirectory(menu)
add_subdirectory(user-set)