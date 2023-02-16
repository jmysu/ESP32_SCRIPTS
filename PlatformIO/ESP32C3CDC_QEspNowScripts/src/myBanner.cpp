#include <Arduino.h>
#include "_QEspNowScript.h"
#include "ansi.h"

extern ANSI ansi;

 void showBanner() {
    while(!Serial){;}
       Serial.println();/*
       Serial.println("       ██╗██╗███╗   ███╗███╗   ███╗██╗   ██╗    ██████╗██████╗  █████╗ ███████╗████████╗");
       Serial.println("       ██║   ████╗ ████║████╗ ████║╚██╗ ██╔╝   ██╔════╝██╔══██╗██╔══██╗██╔════╝╚══██╔══╝");
       Serial.println("       ██║██║██╔████╔██║██╔████╔██║ ╚████╔╝    ██║     ██████╔╝███████║█████╗     ██║   ");
       Serial.println("  ██   ██║██║██║╚██╔╝██║██║╚██╔╝██║  ╚██╔╝     ██║     ██╔══██╗██╔══██║██╔══╝     ██║   ");
       Serial.println("  ╚█████╔╝██║██║ ╚═╝ ██║██║ ╚═╝ ██║   ██║      ╚██████╗██║  ██║██║  ██║██║        ██║   ");
       Serial.println("   ╚════╝ ╚═╝╚═╝     ╚═╝╚═╝     ╚═╝   ╚═╝       ╚═════╝╚═╝  ╚═╝╚═╝  ╚═╝╚═╝        ╚═╝   ");*/
       #if CONFIG_ARDUHAL_LOG_COLORS
        Serial.println( _grn"                             " _GRN" #####                             "     "   "     "####### #     # "); 
        Serial.println( _grn"     # # #    # #    # #   # " _GRN"#     # #####    ##   ###### ##### "     "   "     "   #    #     # "); 
        Serial.println( _grn"     #   ##  ## ##  ##  # #  " _GRN"#       #    #  #  #  #        #   "     "   "     "   #    #  #  # "); 
        Serial.println( _grn"     # # # ## # # ## #   #   " _GRN"#       #    # #    # #####    #   "     "   "     "   #    #  #  # "); 
        Serial.println( _grn"     # # #    # #    #   #   " _GRN"#       #####  ###### #        #   "     "   "     "   #    #  #  # "); 
        Serial.println( _grn"#    # # #    # #    #   #   " _GRN"#     # #   #  #    # #        #   " _grn"## " _GRN"   #    #  #  # "); 
        Serial.print  ( _grn" ####  # #    # #    #   #   " _GRN" #####  #    # #    # #        #   " _grn"## " _GRN"   #     ## ##  ");     
        Serial.print( _grn"  V:" _RED);Serial.print(FW_MAJOR);Serial.print(".");Serial.print(FW_MID);Serial.print(".");Serial.println(FW_MINOR);
        Serial.print(_WHT); 
         for (int i=0;i<100;i++) {
            ansi.color(ansi.white, ansi.grey2color(i));
            Serial.print("\u203E"); //overline
            }
         ansi.normal();  
         Serial.println();
       #else
        Serial.println("                              #####                            "); 
        Serial.println("     # # #    # #    # #   # #     # #####    ##   ###### #####"); 
        Serial.println("     #   ##  ## ##  ##  # #  #       #    #  #  #  #        #  "); 
        Serial.println("     # # # ## # # ## #   #   #       #    # #    # #####    #  "); 
        Serial.println("     # # #    # #    #   #   #       #####  ###### #        #  "); 
        Serial.println("#    # # #    # #    #   #   #     # #   #  #    # #        #  "); 
        Serial.print  (" ####  # #    # #    #   #    #####  #    # #    # #        #  ");     
        Serial.print("V:");Serial.print(FW_MAJOR);Serial.print(".");Serial.print(FW_MID);Serial.print(".");Serial.println(FW_MINOR);
       #endif
 }