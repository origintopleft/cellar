# basically an assembled version of https://stackoverflow.com/questions/18968979/how-to-get-colorized-output-with-cmake
# spelled with a u because the guy in the answer seemed to insist on it

if(NOT MSVC)
    string(ASCII 27 Esc)
    set(ColourReset "${Esc}[0m")
    set(ColourBold  "${Esc}[1m")
    set(Red         "${Esc}[0;31m")
    set(Green       "${Esc}[0;32m")
    set(Yellow      "${Esc}[0;33m")
    set(Blue        "${Esc}[0;34m")
    set(Magenta     "${Esc}[0;35m")
    set(Cyan        "${Esc}[0;36m")
    set(White       "${Esc}[0;37m")
    set(BoldRed     "${Esc}[1;31m")
    set(BoldGreen   "${Esc}[1;32m")
    set(BoldYellow  "${Esc}[1;33m")
    set(BoldBlue    "${Esc}[1;34m")
    set(BoldMagenta "${Esc}[1;35m")
    set(BoldCyan    "${Esc}[1;36m")
    set(BoldWhite   "${Esc}[1;37m")
endif()


