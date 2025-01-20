find_package(PythonInterp)
find_package(PythonModule)
find_python_module(cogapp REQUIRED)

function(cog_sources globtarget output)
        file(GLOB_RECURSE targetsources RELATIVE "${CMAKE_SOURCE_DIR}"
                "${CMAKE_SOURCE_DIR}/${globtarget}")
        file(GLOB_RECURSE targetcoggedsources RELATIVE "${CMAKE_SOURCE_DIR}"
                "${CMAKE_SOURCE_DIR}/${globtarget}.cog")
        foreach(targetcog ${targetcoggedsources})
                string(REGEX REPLACE ".cog\$" "" this "${targetcog}")
                set(targetsources ${targetsources} ${this})
        endforeach(targetcog)

        set(${output} ${targetsources} PARENT_SCOPE)
endfunction(cog_sources)

macro(cog_target)
    # macro to add target to run cog for ALL files
    # useful for dependency management
    set(coggedfiles)
    
    file(GLOB_RECURSE cogfiles RELATIVE "${CMAKE_SOURCE_DIR}"
            "${CMAKE_SOURCE_DIR}/*.cog")
    
    foreach(cogfile ${cogfiles})
            # thisfile = absolute path to output file
            # outfile = relative path
            # TODO: fix that
            string(REGEX REPLACE ".cog\$" "" outfile "${cogfile}")
            set(thisfile "${CMAKE_CURRENT_BINARY_DIR}/${outfile}")
    
            add_custom_command(OUTPUT "${thisfile}" PRE_BUILD
                    COMMAND ${PYTHON_EXECUTABLE} -m cogapp -d -o "${thisfile}" "${cogfile}"
                    DEPENDS ${cogfile}
                    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                    COMMENT "Greasing the cog for ${BoldCyan}${outfile}${ColourReset}")
    
            set(coggedfiles ${coggedfiles} "${thisfile}")
    endforeach(cogfile)
    
    add_custom_target(cog DEPENDS ${coggedfiles})
endmacro(cog_target)
