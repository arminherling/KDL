project(AllInOne)

set(CMAKE_VS_JUST_MY_CODE_DEBUGGING ON)

qt_add_executable(
    ${PROJECT_NAME} 
    "${PROJECT_SOURCE_DIR}/../LexerTests/source/LexerTests.cpp"
    "${PROJECT_SOURCE_DIR}/../ParserTests/source/ParserTests.cpp"
    "source/main.cpp")

target_link_libraries(${PROJECT_NAME} PRIVATE KDL AalTest Qt6::Core)

if (WIN32)
    add_custom_command(
        TARGET ${PROJECT_NAME} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_RUNTIME_DLLS:${PROJECT_NAME}> $<TARGET_FILE_DIR:${PROJECT_NAME}>
        COMMAND_EXPAND_LISTS
    )
endif ()
