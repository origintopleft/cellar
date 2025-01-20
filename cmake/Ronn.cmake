find_program(RONN ronn)
find_program(GZIP gzip)

if(NOT RONN OR NOT GZIP)
    message(WARNING "${BoldYellow}Not generating manpages${ColourReset}")
	if(NOT RONN)
		message(WARNING "    ronn not installed")
	endif(NOT RONN)
	if (NOT GZIP)
		message(WARNING "    gzip (somehow) not installed")
	endif(NOT GZIP)
	macro(generate_manpage)  # Empty, won't do anything
	endmacro(generate_manpage)
    macro(add_manpage_target) # ditto
    endmacro(add_manpage_target)
else(NOT RONN OR NOT GZIP)
    set(manpages)
    file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/man")
    macro(generate_manpage TARGET SECTION)
        if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/man/man${SECTION}")
            file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/man/man${SECTION}")
        endif()
		set(ronnfile "${CMAKE_SOURCE_DIR}/doc/${TARGET}.${SECTION}.ronn")
        set(outfile "${CMAKE_CURRENT_BINARY_DIR}/man/man${SECTION}/${TARGET}.${SECTION}")
        add_custom_command(OUTPUT "${outfile}"
                DEPENDS "${ronnfile}"
                COMMAND ${RONN}
                # BULLSHIT: ronn doesn't let you specify your own output filenames,
                #           have to hack it with shell redirecting
                ARGS    -r --pipe "${ronnfile}" > "${outfile}"
        )
        add_custom_command(OUTPUT "${outfile}.gz"
                DEPENDS "${outfile}"
                COMMAND "${GZIP}"
                ARGS    -fk "${outfile}"
        )
        set(manpages ${manpages} "${outfile}.gz")
    endmacro(generate_manpage)

    macro(add_manpage_target) # this is a macro so we can call it after we've generated all our manpages
        add_custom_target(man ALL DEPENDS ${manpages})
        install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/man/"
                DESTINATION share/man
                FILES_MATCHING PATTERN "*.gz")
    endmacro(add_manpage_target)
endif(NOT RONN OR NOT GZIP)
