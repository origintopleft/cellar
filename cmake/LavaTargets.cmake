link_directories(${CMAKE_CURRENT_BINARY_DIR})

function(lava_create_library)
        set(multiValueArgs SUBDIRS DEPENDS LIBRARIES)
        set(oneValueArgs TARGET)
        cmake_parse_arguments(lava_create_library "" "${oneValueArgs}"
                "${multiValueArgs}" ${ARGN})

        set(target ${lava_create_library_TARGET})

        set(targetsources)
        foreach(subdir ${lava_create_library_SUBDIRS})
                cog_sources("src/${subdir}/*.cpp" subdirsources)

                foreach(source ${subdirsources})
                        set(targetsources ${targetsources} ${source})
                endforeach(source)
        endforeach(subdir)

        add_library(${target} SHARED ${targetsources})
	if(MINGW)
	    # MinGW (or CMake?) does a stupid, redundant thing where it names all output DLLs
        # "libblah.dll" and it's kinda stupid and redundant
	    # this removes the "lib" prefix because removing ".dll" causes things to break
	    set_target_properties(${target} PROPERTIES PREFIX "")
	endif()
        if(lava_create_library_LIBRARIES)
            foreach(library ${lava_create_library_LIBRARIES})
                target_link_libraries(${target} "${library}")
            endforeach()
        endif()
        if(lava_create_library_DEPENDS)
            add_dependencies(${target} ${lava_create_library_DEPENDS})
        endif()
endfunction(lava_create_library)

function(lava_create_gutlib)
        set(multiValueArgs SUBDIRS DEPENDS LIBRARIES LIBRARYVARS)
        cmake_parse_arguments(lava_create_gutlib "" ""
                "${multiValueArgs}" ${ARGN})

        foreach(subdir ${lava_create_gutlib_SUBDIRS})
                cog_sources("src/${subdir}/*.cpp" subdirsources)

                foreach(source ${subdirsources})
                        set(targetsources ${targetsources} ${source})
                endforeach(source)
        endforeach(subdir)

        add_library(gutlib SHARED ${targetsources})
        set_target_properties(gutlib PROPERTIES OUTPUT_NAME ${CMAKE_PROJECT_NAME})
	if(MINGW)
	    # MinGW (or CMake?) does a stupid, redundant thing where it names all output DLLs
        # "libblah.dll" and it's kinda stupid and redundant
	    # this removes the "lib" prefix because removing ".dll" causes things to break
	    set_target_properties(gutlib PROPERTIES PREFIX "")
	endif()
        if(lava_create_gutlib_LIBRARIES)
            foreach(library ${lava_create_gutlib_LIBRARIES})
               target_link_libraries(gutlib "${library}")
            endforeach()
        endif()
        if(lava_create_gutlib_DEPENDS)
            add_dependencies(gutlib ${lava_create_gutlib_DEPENDS})
        endif()
endfunction(lava_create_gutlib)

function(lava_create_executable)
        set(multiValueArgs SUBDIRS LIBRARIES DEPENDS)
        set(oneValueArgs TARGET)
        cmake_parse_arguments(lava_create_executable "" "${oneValueArgs}"
                "${multiValueArgs}" ${ARGN})

        set(target ${lava_create_executable_TARGET})

        set(targetsources)
        foreach(subdir ${lava_create_executable_SUBDIRS})
                cog_sources("src/${subdir}/*.cpp" subdirsources)

                foreach(source ${subdirsources})
                        set(targetsources ${targetsources} ${source})
                endforeach(source)
        endforeach(subdir)

        add_executable(${target} ${targetsources})
        if(lava_create_executable_LIBRARIES)
            foreach(library ${lava_create_executable_LIBRARIES})
                target_link_libraries(${target} "${library}")
            endforeach()
        endif()
        if(lava_create_executable_DEPENDS)
            add_dependencies(${target} ${lava_create_executable_DEPENDS})
        endif()
endfunction(lava_create_executable)
