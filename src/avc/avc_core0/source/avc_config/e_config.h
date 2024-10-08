#include "avc__master_config.h"

#ifndef E__CONFIG_H_
#define E__CONFIG_H_

#define E__PORT_OPTION                              E__PORT_OPTION__NXP__MCXN

#include <e_config__port.h>

#define CONFIG__E_FOREGROUND_TRIGGER_CHECK_INTERVAL (CONFIG__E_TICK_PERIOD__MS)

#define CONFIG__E_VT100__ENABLE                    (0)

#define CONFIG__E_VT100_DEFAULT_COLOR               E_VT100__WHITE

#define CONFIG__E_LOG__ENABLE					    (1)

#define CONFIG__E_LOG__RTT_ENABLE				    (1)

#define CONFIG__E_LOG__RTT_TX_BUFFER_SIZE           (4096)

#include "e_queue.h"
extern byte_queue_t UART4_TX_Q;

#define CONFIG__E_ADDTIONAL_LOGGER(...) 			bq__printf(&UART4_TX_Q,__VA_ARGS__)

#define CONFIG__E_SHELL__ENABLE			    	    (0)

#define CONFIG__E_SHELL_PRINTF_MAX_LEN      	    (256U)

#define CONFIG__E_SHELL_USE_HISTORY 			    (0U)

#define CONFIG__E_SHELL_SEARCH_IN_HIST 			    (1U)

#define CONFIG__E_SHELL_AUTO_COMPLETE 			    (1U)

#define CONFIG__E_SHELL_BUFFER_SIZE 			    (64)

#define CONFIG__E_SHELL_MAX_ARGS 				    (8U)

#define CONFIG__E_SHELL_HIST_MAX 				    (4U)

#define CONFIG__E_SHELL_MAX_CMD					    (16)

#define CONFIG__BQ_MAX_PRINTF_LEN				    (64)

#endif

