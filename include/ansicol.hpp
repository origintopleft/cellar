/* This file contains string definitions for ANSI color sequences
 * it was written by Nicole O'Connor and is available  to all 
 * under the MIT license.
 *
 * Usage:
 *     std::cout << ansicol::red_bold << "I am bold red text!" << ansicol::reset << std::endl;
 *
 *     ansicol::red         = regular red
 *     ansicol::red_bold    = bold red
 *     ansicol::red_bg      = red background
 *
 *     colors are red, green, yellow, blue, cyan, magenta, white
 *
 * this file was built with cog due to its monotony - while you can safely use this file
 * in your project without bringing in cog as a build dependency, you probably shouldn't
 * edit it by hand without good reason. cog can always be installed with this command:
 *
 *     pip install --user cogapp
 *
 * I do not develop that program, I'm merely a fan of it.
 */
#ifndef __ANSICOL_H
#define __ANSICOL_H
#pragma once
#include <string>

using std::string;

namespace ansicol {
    string escape = "\33[";

    /*[[[cog
         import cog

         colindex = 1
         for color in ["red", "green", "yellow", "blue", "cyan", "magenta", "white"]:
             cog.outl("string {0} = escape + \"0;3{1}m\";".format(color, colindex))
             cog.outl("string {0}_bold = escape + \"1;3{1}m\";".format(color, colindex))
             cog.outl("string {0}_bg = escape + \"0;4{1}m\";".format(color, colindex))
             cog.outl("")
             colindex = colindex + 1
      ]]]*/
    string red = escape + "0;31m";
    string red_bold = escape + "1;31m";
    string red_bg = escape + "0;41m";

    string green = escape + "0;32m";
    string green_bold = escape + "1;32m";
    string green_bg = escape + "0;42m";

    string yellow = escape + "0;33m";
    string yellow_bold = escape + "1;33m";
    string yellow_bg = escape + "0;43m";

    string blue = escape + "0;34m";
    string blue_bold = escape + "1;34m";
    string blue_bg = escape + "0;44m";

    string cyan = escape + "0;35m";
    string cyan_bold = escape + "1;35m";
    string cyan_bg = escape + "0;45m";

    string magenta = escape + "0;36m";
    string magenta_bold = escape + "1;36m";
    string magenta_bg = escape + "0;46m";

    string white = escape + "0;37m";
    string white_bold = escape + "1;37m";
    string white_bg = escape + "0;47m";

    //[[[end]]] (checksum: 597ec07673e8edcafc770a1dcf284956)

    string reset = escape + "0m";
}

#endif // __ANSICOL_H
