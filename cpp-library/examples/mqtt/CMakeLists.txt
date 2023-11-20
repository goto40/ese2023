file(GLOB EXAMPLES ./*.cpp)

find_package(PahoMqttCpp REQUIRED)

foreach(CPPFILE ${EXAMPLES})
    get_filename_component(EXE_NAME ${CPPFILE} NAME_WLE)
    add_executable(${EXE_NAME} ${CPPFILE})
    target_include_directories(${EXE_NAME} PRIVATE ../../src-gen)
    target_link_libraries(${EXE_NAME} PRIVATE meta PahoMqttCpp::paho-mqttpp3)
endforeach()
