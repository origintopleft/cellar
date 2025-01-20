find_program(DVIPNG dvipng)
find_program(DVISVGM dvisvgm)
find_program(SPHINX_BUILD sphinx-build)

if(NOT DOCS_TOO)
    set(DOCS_TOO 0)
endif()

if(NOT DVIPNG OR NOT DVISVGM OR NOT SPHINX_BUILD)
        message(WARNING "${BoldYellow}Cannot build library documentation.${ColourReset}")

    # report what's missing
    if(NOT DVIPNG)
        message(WARNING "    dvipng not installed.")
    endif()
    if(NOT DVISVGM)
        message(WARNING "    dvisvgm not installed.")
    endif()
    if(NOT SPHINX_BUILD)
        message(WARNING "    Sphinx not installed.")
    endif()

    if (DOCS_TOO EQUAL 1)
        message(FATAL_ERROR "${BoldRed}This is a problem, since you specified DOCS_TOO.${ColourReset}")
    endif()
else()
    if(DOCS_TOO EQUAL 1)
        set(MAYBE_ALL ALL)
    else()
            message(STATUS "${Green}Not building docs automatically. Build them with: ${BoldGreen}${CMAKE_MAKE_PROGRAM} sphinx${ColourReset}
   An alternative strategy would be to run CMake again with ${BoldYellow}-DDOCS_TOO=1${ColourReset}")
        set(MAYBE_ALL "")
    endif()
    
    add_custom_target(sphinx ${MAYBE_ALL}
        COMMAND ${SPHINX_BUILD} -b html "${CMAKE_SOURCE_DIR}/doc/sphinx" "${CMAKE_CURRENT_BINARY_DIR}/doc")
endif()
