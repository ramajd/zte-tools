qt_add_executable(zte-gui
    src/main.cpp
    src/ZteService.cpp
    src/ZteService.h
)
# Finder double-click on bare Mach-O does nothing (no Terminal, no log) — bundle fixes it
set_target_properties(zte-gui PROPERTIES
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_GUI_IDENTIFIER "io.github.ramajd.zte.tools"
    MACOSX_BUNDLE_BUNDLE_NAME "ZTE Tools"
    WIN32_EXECUTABLE TRUE
)

qt_add_qml_module(zte-gui
    URI ZteTools
    VERSION 1.0
    QML_FILES
        qml/Main.qml
        qml/pages/Dashboard.qml
        qml/pages/ApnPage.qml
        qml/pages/SmsPage.qml
        qml/pages/WanPage.qml
        qml/components/StatCard.qml
)

target_include_directories(zte-gui PRIVATE src)
target_link_libraries(zte-gui PRIVATE zte-core Qt6::Quick Qt6::Qml Qt6::Core Qt6::Network)

# ponytail: single executable, no plugin — QML embedded, fastest load
