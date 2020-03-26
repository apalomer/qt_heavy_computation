add_library(heavy_computation_widget SHARED
    heavy_computation_widget.cpp
    heavy_computation_widget.h
    heavy_computation_widget.ui
    heavy_computation_object.h
    heavy_computation_object.cpp)
target_compile_definitions(heavy_computation_widget PRIVATE QTDOULESPINBOXHEAVYCOMPUTATION)
target_link_libraries(heavy_computation_widget Qt5::Core Qt5::Widgets)

add_executable(heavy_computation heavy_computation.cpp)
target_link_libraries(heavy_computation Qt5::Core heavy_computation_widget)
