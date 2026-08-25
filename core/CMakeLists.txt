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

install(TARGETS zte-core
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

# ponytail: minimal deps, no Quick here — keeps core headless testable
