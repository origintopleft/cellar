include(GenerateExportHeader)

link_directories(${CMAKE_CURRENT_BINARY_DIR})
set(BUILD_SHARED_LIBS ON)
if (MSVC)
    add_compile_options("/utf-8")
endif()

function(build_library)
    set(multiValueArgs SUBDIRS DEPENDS LIBRARIES)
    set(oneValueArgs TARGET)
    cmake_parse_arguments(build_library "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    set(target ${build_library_TARGET})
    set(targetsources)
    foreach(subdir ${build_library_SUBDIRS})
        set(found_files)
        #file(GLOB_RECURSE found_files RELATIVE "${CMAKE_SOURCE_DIR}" "${CMAKE_SOURCE_DIR}/src/${subdir}/*.cpp")
        cog_sources("src/${subdir}/*.cpp" found_files)
        set(targetsources ${targetsources} ${found_files})
    endforeach()
    
    if (MSVC AND EXISTS "${CMAKE_SOURCE_DIR}/res/${target}.rc")
        set(targetsources ${targetsources} "res/${target}.rc")
    endif()

    add_library(${target} SHARED ${targetsources})
    if (MSVC)
        set_target_properties(${target} PROPERTIES PREFIX "")
        set_target_properties(${target} PROPERTIES LINK_FLAGS "/MAP")
        GENERATE_EXPORT_HEADER(${target}
                               BASE_NAME ${target}
                               EXPORT_MACRO_NAME ${target}_EXPORT
                               EXPORT_FILE_NAME include/${target}_Export.h
                               STATIC_DEFINE ${target}_BUILT_AS_STATIC)
    endif()
    
    if (build_library_LIBRARIES)
        foreach (library ${build_library_LIBRARIES})
            target_link_libraries(${target} ${library})
        endforeach()
    endif()

    if (build_library_DEPENDS)
        add_dependencies(${target} ${build_library_DEPENDS})
    endif()
endfunction()

function(build_executable)
    set(multiValueArgs SUBDIRS DEPENDS LIBRARIES)
    set(oneValueArgs TARGET)
    cmake_parse_arguments(build_executable "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    set(target ${build_executable_TARGET})
    set(targetsources)
    foreach(subdir ${build_executable_SUBDIRS})
        set(found_files)
        #file(GLOB_RECURSE found_files RELATIVE "${CMAKE_SOURCE_DIR}" "${CMAKE_SOURCE_DIR}/src/${subdir}/*.cpp")
        cog_sources("src/${subdir}/*.cpp" found_files)
        set(targetsources ${targetsources} ${found_files})
    endforeach()

    if (MSVC AND EXISTS "${CMAKE_SOURCE_DIR}/res/${target}.rc")
        set(targetsources ${targetsources} "res/${target}.rc")
    endif()

    add_executable(${target} ${targetsources})
    if (MSVC)
        # tells MSVC to use int main() but still hide the console window
        set_target_properties(${target} PROPERTIES LINK_FLAGS "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
    endif()
    
    if (build_executable_LIBRARIES)
        foreach (library ${build_executable_LIBRARIES})
            target_link_libraries(${target} ${library})
        endforeach()
    endif()

    if (build_executable_DEPENDS)
        add_dependencies(${target} ${build_executable_DEPENDS})
    endif()
endfunction()