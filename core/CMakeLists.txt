qt_add_library(zte-core STATIC
    src/ZteClient.cpp
    src/Codec.cpp
    src/Models.cpp
    include/zte/ZteClient.h
    include/zte/Models.h
    include/zte/Codec.h
)

target_include_directories(zte-core PUBLIC include)

target_link_libraries(zte-core PUBLIC Qt6::Core Qt6::Network)

# ponytail: minimal deps, no Quick here — keeps core headless testable
