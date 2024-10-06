#include "string.h"
#include "stdbool.h"
#include "stdarg.h"
#include "e.h"

#ifndef CONFIG__E_SHELL_PRINTF_MAX_LEN
	#define CONFIG__E_SHELL_PRINTF_MAX_LEN      (256U)
#endif

#ifndef CONFIG__E_SHELL_USE_HISTORY
	#define CONFIG__E_SHELL_USE_HISTORY 		(0U)
#endif

#ifndef CONFIG__E_SHELL_SEARCH_IN_HIST
	#define CONFIG__E_SHELL_SEARCH_IN_HIST 		(1U)
#endif

#ifndef CONFIG__E_SHELL_AUTO_COMPLETE
	#define CONFIG__E_SHELL_AUTO_COMPLETE 		(1U)
#endif

#ifndef CONFIG__E_SHELL_BUFFER_SIZE
	#define CONFIG__E_SHELL_BUFFER_SIZE 		(64)
#endif

#ifndef CONFIG__E_SHELL_MAX_ARGS
	#define CONFIG__E_SHELL_MAX_ARGS 			(8U)
#endif

#ifndef CONFIG__E_SHELL_HIST_MAX
	#define CONFIG__E_SHELL_HIST_MAX 			(4U)
#endif

#ifndef CONFIG__E_SHELL_MAX_CMD
	#define CONFIG__E_SHELL_MAX_CMD (32)
#endif



#define KEY_ESC (0x1BU)
#define KET_DEL (0x7FU)


static int32_t parse_line(e_shell__context_t ThisShellContext, const char *cmd, uint32_t len, char *argv[CONFIG__E_SHELL_MAX_ARGS]); /*!< parse line command */

static int32_t str_cmp(const char *str1, const char *str2, int32_t count); /*!< compare string command */

static void process_cmd(e_shell__context_t shell, const char *cmd); /*!< process a command */

static void get_history(e_shell__context_t shell, uint8_t hist_pos); /*!< get commands history */

static void autocomplete(e_shell__context_t shell); /*!< auto complete command */

static int32_t StrLen(const char *str); /*!< get string length */

static char *StrCopy(char *dest, const char *src, int32_t count); /*!< string copy */


void e_shell__crunch(e_shell__context_t shell)
{
    uint8_t ch;
    int32_t i;

    if (!shell)
    {
        return;
    }

    while(bq__dequeue(shell->rx_bq,&ch)==QUEUE_OK)
    {
                /* Special key */
                if (ch == KEY_ESC)
                {
                    shell->stat = kSHELL_Special;
                    continue;
                }
                else if (shell->stat == kSHELL_Special)
                {
                    /* Function key */
                    if (ch == '[')
                    {
                        shell->stat = kSHELL_Function;
                        continue;
                    }
                    shell->stat = kSHELL_Normal;
                }
                else if (shell->stat == kSHELL_Function)
                {
                    shell->stat = kSHELL_Normal;

                    switch ((uint8_t)ch)
                    {
                        /* History operation here */
                        case 'A': /* Up key */
                            get_history(shell, shell->hist_current);
                            if (shell->hist_current < (shell->hist_count - 1))
                            {
                                shell->hist_current++;
                            }
                            break;
                        case 'B': /* Down key */
                            get_history(shell, shell->hist_current);
                            if (shell->hist_current > 0)
                            {
                                shell->hist_current--;
                            }
                            break;
                        case 'D': /* Left key */
                            if (shell->c_pos)
                            {
                                bq__printf(shell->tx_bq,"\b");
                                shell->c_pos--;
                            }
                            break;
                        case 'C': /* Right key */
                            if (shell->c_pos < shell->l_pos)
                            {
                                if(shell->echo){bq__printf(shell->tx_bq,"%c", shell->line[shell->c_pos]);};
                                shell->c_pos++;
                            }
                            break;
                        default:
                            break;
                    }
                    continue;
                }
                /* Handle tab key */
                else if (ch == '\t')
                {
        #if CONFIG__E_SHELL_AUTO_COMPLETE
                    /* Move the cursor to the beginning of line */
                    for (i = 0; i < shell->c_pos; i++)
                    {
                        bq__printf(shell->tx_bq,"\b");
                    }
                    /* Do auto complete */
                    autocomplete(shell);
                    /* Move position to end */
                    shell->c_pos = shell->l_pos = StrLen(shell->line);
        #endif
                    continue;
                }
        #if CONFIG__E_SHELL_SEARCH_IN_HIST
                /* Search command in history */
                else if ((ch == '`') && (shell->l_pos == 0) && (shell->line[0] == 0x00))
                {
                }
        #endif
                /* Handle backspace key */
                else if ((ch == KET_DEL) || (ch == '\b'))
                {
                    /* There must be at last one char */
                    if (shell->c_pos == 0)
                    {
                        continue;
                    }

                    shell->l_pos--;
                    shell->c_pos--;

                    if (shell->l_pos > shell->c_pos)
                    {
                        memmove(&shell->line[shell->c_pos], &shell->line[shell->c_pos + 1],
                                shell->l_pos - shell->c_pos);
                        shell->line[shell->l_pos] = 0;

                        if(shell->echo){bq__printf(shell->tx_bq,"\b%s  \b", &shell->line[shell->c_pos]);};

                        /* Reset position */
                        for (i = shell->c_pos; i <= shell->l_pos; i++)
                        {
                            if(shell->echo){bq__printf(shell->tx_bq,"\b");};

                        }
                    }
                    else /* Normal backspace operation */
                    {
                        if(shell->echo){bq__printf(shell->tx_bq,"\b \b");};
                        shell->line[shell->l_pos] = 0;
                    }
                    continue;
                }
                else
                {
                }

                /* Input too long */
                if (shell->l_pos >= (CONFIG__E_SHELL_BUFFER_SIZE - 1))
                {
                    shell->l_pos = 0;
                }

                /* Handle end of line, break */
                if ((ch == '\r') || (ch == '\n'))
                {
                    if(shell->echo){bq__printf(shell->tx_bq,"\r\n");};
                    process_cmd(shell, shell->line);
                    /* Reset all params */
                    shell->c_pos = shell->l_pos = 0;
                    shell->hist_current = 0;
                    if(shell->echo){bq__printf(shell->tx_bq,shell->prompt);};
                    memset(shell->line, 0, sizeof(shell->line));
                    continue;
                }

                /* Normal character */
                if (shell->c_pos < shell->l_pos)
                {
                    memmove(&shell->line[shell->c_pos + 1], &shell->line[shell->c_pos],
                            shell->l_pos - shell->c_pos);
                    shell->line[shell->c_pos] = ch;

                    if(shell->echo){bq__printf(shell->tx_bq,"%s", &shell->line[shell->c_pos]);};
                    /* Move the cursor to new position */
                    for (i = shell->c_pos; i < shell->l_pos; i++)
                    {
                        if(shell->echo){bq__printf(shell->tx_bq,"\b");};
                    }
                }
                else
                {
                    shell->line[shell->l_pos] = ch;
                    if(shell->echo){bq__printf(shell->tx_bq,"%c", ch);}
                }

                ch = 0;
                shell->l_pos++;
                shell->c_pos++;
        }

}


static void process_cmd(e_shell__context_t shell, const char *cmd)
{
    static const e_shell__command_context_t *tmpCommand = NULL;
    static const char *tmpCommandString;
    int32_t argc;
    char *argv[CONFIG__E_SHELL_BUFFER_SIZE];
    uint8_t flag = 0;
    uint8_t tmpCommandLen;
    uint8_t tmpLen;
    uint8_t i = 0;

    tmpLen = StrLen(cmd);
    argc = parse_line(shell,cmd, tmpLen, argv);

    if ((tmpCommand == NULL) && (argc > 0))
    {
        for (i = 0; i < shell->ShellCommands.numberOfCommandInList; i++)
        {
            tmpCommand = &(shell->ShellCommands.CommandList[i]);
            tmpCommandString = tmpCommand->pcCommand;
            tmpCommandLen = StrLen(tmpCommandString);


            /* Compare with space or end of string */
            if ((cmd[tmpCommandLen] == ' ') || (cmd[tmpCommandLen] == 0x00))
            {
                if (str_cmp(tmpCommandString, argv[0], tmpCommandLen) == 0)
                {
                    if (argc > 0)
                    {
                        flag = 1;
                        break;
                    }
                }
            }
        }
    }
    else
    {
        return;  //no command
    }

    if(flag == 0)
    {
        if(shell->QuietOnBadCommand==0)
        {
            bq__printf(shell->tx_bq,
            				"\r\n\"%s\" Command not recognized.  Enter 'help' to view a list of available commands.\r\n\r\n",cmd);
               
        }

         tmpCommand = NULL;
    }

    if(tmpCommand != NULL)
    {
        tmpLen = StrLen(cmd);
        /* Compare with last command. Push back to history buffer if different */
        if (tmpLen != str_cmp(cmd, shell->hist_buf[0], StrLen(cmd)))
        {
            for (i = CONFIG__E_SHELL_HIST_MAX - 1; i > 0; i--)
            {
                memset(shell->hist_buf[i], '\0', CONFIG__E_SHELL_BUFFER_SIZE);
                tmpLen = StrLen(shell->hist_buf[i - 1]);
                StrCopy(shell->hist_buf[i], shell->hist_buf[i - 1], tmpLen);
            }
            memset(shell->hist_buf[0], '\0', CONFIG__E_SHELL_BUFFER_SIZE);
            tmpLen = StrLen(cmd);
            StrCopy(shell->hist_buf[0], cmd, tmpLen);
            if (shell->hist_count < CONFIG__E_SHELL_HIST_MAX)
            {
                shell->hist_count++;
            }
        }

        if(tmpCommand->pFuncCallBack == 0)
        {
            bq__printf(shell->tx_bq,"\r\nCommand %s has a null callback\r\n\r\n",tmpCommand->pcCommand);
        }
        else
        {

           if(shell->CurrentPrivilegeLevel >= tmpCommand->MinPrivilegeLevel)
                tmpCommand->pFuncCallBack((void *)shell, argc, argv);
            else
            {
                if(shell->QuietOnBadCommand==0)
                    bq__printf(shell->tx_bq,"\r\nYou do not have privilege to run %s\r\n\r\n",tmpCommand->pcCommand);
            }
            
        }

        tmpCommand = NULL;
    }

}

static void get_history(e_shell__context_t shell, uint8_t hist_pos)
{
    uint8_t i;
    uint32_t tmp;

    if (shell->hist_buf[0][0] == '\0')
    {
        shell->hist_current = 0;
        return;
    }
    if (hist_pos > CONFIG__E_SHELL_HIST_MAX)
    {
        hist_pos = CONFIG__E_SHELL_HIST_MAX - 1;
    }
    tmp = StrLen(shell->line);
    /* Clear current if have */
    if (tmp > 0)
    {
        memset(shell->line, '\0', tmp);
        for (i = 0; i < tmp; i++)
        {
            bq__printf(shell->tx_bq,"\b \b");
        }
    }

    shell->l_pos = StrLen(shell->hist_buf[hist_pos]);
    shell->c_pos = shell->l_pos;
    StrCopy(shell->line, shell->hist_buf[hist_pos], shell->l_pos);

    if(shell->echo){bq__printf(shell->tx_bq,shell->hist_buf[hist_pos]);};
}

static void autocomplete(e_shell__context_t shell)
{
    int32_t len;
    int32_t minLen;
    uint8_t i = 0;
    const e_shell__command_context_t *tmpCommand = NULL;
    const char *namePtr;
    const char *cmdName;

    minLen = 0;
    namePtr = NULL;

    if (!StrLen(shell->line))
    {
        return;
    }
    if(shell->echo){bq__printf(shell->tx_bq,"\r\n");};
    /* Empty tab, list all commands */
    if (shell->line[0] == '\0')
    {
        shell__help_handler(shell, 0, NULL);
        return;
    }
    /* Do auto complete */
    for (i = 0; i < shell->ShellCommands.numberOfCommandInList; i++)
    {
        tmpCommand = &shell->ShellCommands.CommandList[i];
        cmdName = tmpCommand->pcCommand;
        if (str_cmp(shell->line, cmdName, StrLen(shell->line)) == 0)
        {
            if (minLen == 0)
            {
                namePtr = cmdName;
                minLen = StrLen(namePtr);
                /* Show possible matches */
                if(shell->echo){bq__printf(shell->tx_bq,"%s\r\n", cmdName);};
                continue;
            }
            len = str_cmp(namePtr, cmdName, StrLen(namePtr));
            if (len < 0)
            {
                len = len * (-1);
            }
            if (len < minLen)
            {
                minLen = len;
            }
        }
    }
    /* Auto complete string */
    if (namePtr)
    {
        StrCopy(shell->line, namePtr, minLen);
    }
    if(shell->echo){bq__printf(shell->tx_bq,"%s%s", shell->prompt, shell->line);};
    return;
}

static char *StrCopy(char *dest, const char *src, int32_t count)
{
    char *ret = dest;
    int32_t i = 0;

    for (i = 0; i < count; i++)
    {
        dest[i] = src[i];
    }

    return ret;
}

static int32_t StrLen(const char *str)
{
    int32_t i = 0;

    while (*str)
    {
        str++;
        i++;
    }
    return i;
}

static int32_t str_cmp(const char *str1, const char *str2, int32_t count)
{
    while (count--)
    {
        if (*str1++ != *str2++)
        {
            return *(unsigned char *)(str1 - 1) - *(unsigned char *)(str2 - 1);
        }
    }
    return 0;
}

static int32_t parse_line(e_shell__context_t ThisShellContext,const char *cmd, uint32_t len, char *argv[CONFIG__E_SHELL_MAX_ARGS])
{
    uint32_t argc;
    char *p;
    uint32_t position;

    /* Init params */
    memset(ThisShellContext->g_paramBuffer, '\0', len + 1);
    StrCopy(ThisShellContext->g_paramBuffer, cmd, len);

    p = ThisShellContext->g_paramBuffer;
    position = 0;
    argc = 0;

    while (position < len)
    {
        /* Skip all blanks */
        while (((char)(*p) == ' ') && (position < len))
        {
            *p = '\0';
            p++;
            position++;
        }
        /* Process begin of a string */
        if (*p == '"')
        {
            p++;
            position++;
            argv[argc] = p;
            argc++;
            /* Skip this string */
            while ((*p != '"') && (position < len))
            {
                p++;
                position++;
            }
            /* Skip '"' */
            *p = '\0';
            p++;
            position++;
        }
        else /* Normal char */
        {
            argv[argc] = p;
            argc++;
            while (((char)*p != ' ') && ((char)*p != '\t') && (position < len))
            {
                p++;
                position++;
            }
        }
    }
    return argc;
}

int32_t e_shell__register_cmd(e_shell__context_t shell,
                              char *command, 
							  char *help_string,
                              cmd_function_t  cmd_callback, 
                              uint32_t min_privledge_level    
                       )
{
    int32_t result = 0;

    /* If have room  in command list */
    if (shell->ShellCommands.numberOfCommandInList < CONFIG__E_SHELL_MAX_CMD)
    {
        shell->ShellCommands.CommandList[shell->ShellCommands.numberOfCommandInList].pcCommand = command;
        shell->ShellCommands.CommandList[shell->ShellCommands.numberOfCommandInList].pcHelpString = help_string;
        shell->ShellCommands.CommandList[shell->ShellCommands.numberOfCommandInList].pFuncCallBack = cmd_callback;
        shell->ShellCommands.CommandList[shell->ShellCommands.numberOfCommandInList].MinPrivilegeLevel = min_privledge_level;
        shell->ShellCommands.numberOfCommandInList++;
    }
    else
    {
        result = -1;
    }
    return result;
}


cmd_function_t shell__help_handler(e_shell__context_t shell, int32_t argc, char **argv)
{
    uint32_t i = 0;

    bq__printf(shell->tx_bq,"\r\nCommands:");
    bq__printf(shell->tx_bq,"\r\n------------------------\r\n\r\n");

    for (i = 0; i < shell->ShellCommands.numberOfCommandInList; i++)
    {
        bq__printf(shell->tx_bq,E_VT100__YELLOW);

        
        bq__printf(shell->tx_bq,"%s",
                    shell->ShellCommands.CommandList[i].pcCommand);


        bq__printf(shell->tx_bq,E_VT100__WHITE);

        bq__printf(shell->tx_bq," > ");

        bq__printf(shell->tx_bq,E_VT100__CYAN);

        bq__printf(shell->tx_bq,"%s",
                        shell->ShellCommands.CommandList[i].pcHelpString);

        bq__printf(shell->tx_bq,"\r\n");
    }

    bq__printf(shell->tx_bq,E_VT100__DEFAULT);
    bq__printf(shell->tx_bq,"\r\n");
    return 0;
}

