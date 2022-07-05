target_sources(SetManager
    PRIVATE user-set.cpp
    PRIVATE global-set.cpp
    PRIVATE subset.cpp
    PRIVATE word-set.cpp
    PRIVATE faux-word-set.cpp
    PRIVATE directory-set.cpp

    PRIVATE derivative-set.cpp
    PRIVATE intersection-set.cpp
    PRIVATE union-set.cpp
    PRIVATE difference-set.cpp
    PRIVATE relative-complement-set.cpp
)