find_package(Qt5 REQUIRED COMPONENTS Core Widgets)
include_directories(${Qt5_INCLUDES} ${Qt5_DIR})

file(READ "${CMAKE_SOURCE_DIR}/res/moc.txt" moclist)
string(REGEX REPLACE "\n" ";" moclist ${moclist}) # split into array by line

# note: i am aware of the existence of qt5_wrap_cpp and friends, but those functions
#       aren't aware of my code generator so i have to do it myself
#       also, qt5_wrap_ui just dumps headers in the root of the build dir. ew.

if(NOT QT_UIC_EXECUTABLE)
    string(REPLACE moc uic QT_UIC_EXECUTABLE "${QT_MOC_EXECUTABLE}")
endif()
  
set(mocsources)
set(qtcogged FALSE)
if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/src/qtgenerated")
        file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/src/qtgenerated")
endif()
foreach(mocfile ${moclist})
    if(EXISTS ${CMAKE_SOURCE_DIR}/${mocfile})
        string(REGEX REPLACE ".h\$" ".cpp" implfile_rel "${mocfile}")
        string(REGEX REPLACE "include/" "src/qtgenerated/moc_" implfile_rel "${implfile_rel}")
        set(implfile_abs "${CMAKE_CURRENT_BINARY_DIR}/${implfile_rel}")
        add_custom_command(OUTPUT "${implfile_abs}" PRE_BUILD
                COMMAND ${QT_MOC_EXECUTABLE} -o "${implfile_abs}" "${CMAKE_SOURCE_DIR}/${mocfile}"
                DEPENDS "${CMAKE_SOURCE_DIR}/${mocfile}"
                COMMENT "Qt MOC: ${BoldCyan}${mocfile}${ColourReset}")
    elseif(EXISTS ${CMAKE_SOURCE_DIR}/${mocfile}.cog)
        set(qtcogged TRUE)
        string(REGEX REPLACE ".h\$" ".cpp" implfile_rel "${mocfile}")
        string(REGEX REPLACE "include/" "src/qtgenerated/moc_" implfile_rel "${implfile_rel}")
        set(implfile_abs "${CMAKE_CURRENT_BINARY_DIR}/${implfile_rel}")
        add_custom_command(OUTPUT "${implfile_abs}" PRE_BUILD
                COMMAND ${QT_MOC_EXECUTABLE} -o "${implfile_abs}" "${CMAKE_CURRENT_BINARY_DIR}/${mocfile}"
                DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/${mocfile}"
                COMMENT "Qt MOC: ${BoldCyan}${mocfile}${ColourReset}")
    endif()

    set(mocsources ${mocsources} "${implfile_abs}")
endforeach()

file(GLOB_RECURSE uilist RELATIVE "${CMAKE_SOURCE_DIR}" "${CMAKE_SOURCE_DIR}/res/ui/*.ui")
set(uicsources)
foreach(uifile ${uilist})
    string(REGEX REPLACE "res/ui/" "" uiname ${uifile})
    string(REGEX REPLACE ".ui\$" "" uiname ${uiname})
    string(REGEX REPLACE "/" "_" uiname ${uiname})
    # res/ui/dlg/license.ui should result in ${uiname} being "dlg_license" at this point
    
    set(headerfile "${CMAKE_CURRENT_BINARY_DIR}/include/ui_${uiname}.h")

    add_custom_command(OUTPUT "${headerfile}" PRE_BUILD
            # TODO: not hardcode this path
            COMMAND ${QT_UIC_EXECUTABLE} -o "${headerfile}" "${CMAKE_SOURCE_DIR}/${uifile}"
            DEPENDS "${CMAKE_SOURCE_DIR}/${uifile}"
            COMMENT "Qt UIC: ${BoldCyan}${uifile}${ColourReset}")

    set(uicsources ${uicsources} "${headerfile}")
endforeach()

add_library(qtgenerated STATIC ${mocsources} ${uicsources})
target_link_libraries(qtgenerated Qt5::Core Qt5::Widgets)
if (qtcogged)
    add_dependencies(qtgenerated cog)
endif()

message(STATUS "Found Qt: ${Qt5_VERSION}")
