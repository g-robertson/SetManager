target_sources(SetManager
    PRIVATE helpers.cpp
    PRIVATE platform.cpp
) 

target_include_directories(SetManager
    PRIVATE menu
    PRIVATE user-set
)

add_subdirectory(menu)
add_subdirectory(user-set)