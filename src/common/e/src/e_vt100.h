#ifndef __E_VT100_H__
#define __E_VT100_H__

#if     CONFIG__E_VT100__ENABLE == (1)

    #define E_VT100__ESC 					"\x1b"
    #define E_VT100__CSI 					"\x1b["

    #define E_VT100__RED                     E_VT100__CSI"31;40m"
    #define E_VT100__GREEN                   E_VT100__CSI"32;40m"
    #define E_VT100__YELLOW                  E_VT100__CSI"33;40m"
    #define E_VT100__BLUE                    E_VT100__CSI"34;40m"
    #define E_VT100__MAGENTA 			     E_VT100__CSI"35;40m"
    #define E_VT100__CYAN    		 		 E_VT100__CSI"36;40m"
    #define E_VT100__WHITE   		 		 E_VT100__CSI"37;40m"
    #define E_VT100__CLEAR_SCREEN  	 		 E_VT100__CSI"2J"
    #define E_VT100__HIDE_CURSOR             E_VT100__CSI"?25l"
    #define E_VT100__CURSOR_HOME   	 		 E_VT100__CSI"H"
    #define E_VT100__CLEAR_LINE			     E_VT100__CSI"2K"
    #define E_VT100__MOVE_CURSOR_START	     E_VT100__CSI"1G"
    #define E_VT100__MOVE_CURSOR_TO_COLUMN   E_VT100__CSI"%dG"


#ifndef CONFIG__E_VT100_DEFAULT_COLOR
    #define E_VT100__DEFAULT                 E_VT100__WHITE
#else
    #define E_VT100__DEFAULT                 CONFIG__E_VT100_DEFAULT_COLOR
#endif

#else

    #define E_VT100__ESC                     ""
    #define E_VT100__CSI                     ""
    #define E_VT100__RED                     ""
    #define E_VT100__GREEN                   ""
    #define E_VT100__YELLOW                  ""
    #define E_VT100__BLUE                    ""
    #define E_VT100__MAGENTA                 ""
    #define E_VT100__CYAN                    ""
    #define E_VT100__WHITE                   ""
    #define E_VT100__CLEAR_SCREEN            ""
    #define E_VT100__HIDE_CURSOR             ""
    #define E_VT100__CURSOR_HOME             ""
    #define E_VT100__CLEAR_LINE              ""
    #define E_VT100__MOVE_CURSOR_START       ""
    #define E_VT100__MOVE_CURSOR_TO_COLUMN   ""

    #define E_VT100__DEFAULT                 ""

#endif


#endif
