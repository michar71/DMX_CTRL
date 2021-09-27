
#include "serial_tracer.h"
#include "shell.h"
#include "vt100.h"


/* Shell configuration */
#define SHELL_CMD_HISTORY_LEN           8

/* external variables */
extern const shell_cmd_t shell_cmd_list[];
extern const int SHELL_CMD_NUM;
extern uint8_t UART_mode_SERIAL;
extern uint8_t UART_mode_USB;

/* local variables */
static char cmd_buf[SHELL_CMD_BUF_LEN];
static int cmd_buf_idx = 0;
static char cmd_hist[SHELL_CMD_HISTORY_LEN][SHELL_CMD_BUF_LEN];
static int cmd_hist_put_idx = 0;
static int cmd_hist_get_idx = 0;

/* macros */
#define DEBUG_MAX_MSG_LEN 128

/* file scope variables */
static char dbgMsg[DEBUG_MAX_MSG_LEN];

/* UART Functions */
int uart_receive_byte( uint8_t * byte)
{
	unsigned char data = tracer_getchar();
	if (data == 0)
	{ 
		*byte = 0;
		return 0;
	}
	else
	{
		*byte = data;
		return 1;
	}
}

int uart_send(uint8_t *bytes, uint16_t size)
{
	tracer_puts(bytes,size);
	return 1;
}


void print(const char *fmt, ...)
{	
    uint32_t len;
    va_list args;

    /* format the variable length argument list */
    va_start(args, fmt);
    (void) vsnprintf(dbgMsg, sizeof(dbgMsg)-4, fmt, args);
    va_end(args);

    /* add carriage return and line feed */
    strcat(dbgMsg, "\r\n");
    len = strlen(dbgMsg);

    /* send the debug message out of UART1 */
    (void) uart_send((uint8_t *)dbgMsg, (uint16_t) len);		
}

void print_no_newline(const char *fmt, ...)
{	
    va_list args;

    /* format the variable length argument list */
    va_start(args, fmt);
    (void) vsnprintf(dbgMsg, sizeof(dbgMsg)-4, fmt, args);
    va_end(args);

    /* send the debug message out of UART1 */
    (void) uart_send((uint8_t *)dbgMsg, (uint16_t) strlen(dbgMsg));		
}

/* functions */
static void cmd_buf_add_char(char c)
{
    if ((unsigned int)cmd_buf_idx < sizeof(cmd_buf))
    {
        cmd_buf[cmd_buf_idx++] = c;
    }
}

static void cmd_buf_remove_char(void)
{
    if (cmd_buf_idx > 0)
    {
        cmd_buf[--cmd_buf_idx] = 0;
    }
}

static void cmd_buf_reset(void)
{
    cmd_buf_idx = 0;
    memset(cmd_buf, 0, sizeof(cmd_buf));
}

static int parse_shell_cmd(char * buffer, char ** tokens)
{
	int arg_num = 0;
	
	tokens[arg_num] = strtok(buffer, " ");
	
	while(NULL != tokens[arg_num] && arg_num < (MAX_ARGUMENT_NUM-1))
	{
		tokens[++arg_num] = strtok(NULL, " ");
	}
	
	return arg_num - 1;
}

static void save_command_to_command_history(void)
{
    strncpy(cmd_hist[cmd_hist_put_idx], cmd_buf, sizeof(cmd_buf));
    cmd_hist_put_idx++;
    if (cmd_hist_put_idx >= SHELL_CMD_HISTORY_LEN)
    {
        cmd_hist_put_idx = 0;
    }
    cmd_hist_get_idx = cmd_hist_put_idx;
}

static void load_command_from_command_history(uint8_t dir)
{
    /* backwards */
    if (0 == dir)
    {
        if (0 == cmd_hist_get_idx)
        {
            if (SHELL_CMD_HISTORY_LEN - 1 == cmd_hist_put_idx)
            {
                return;
            }
            cmd_hist_get_idx = SHELL_CMD_HISTORY_LEN - 1;
        }
        else
        {
            if (cmd_hist_get_idx - 1 == cmd_hist_put_idx)
            {
                return;
            }
            cmd_hist_get_idx--;
        }
    }
    /* forward */
    else
    {
        if (cmd_hist_get_idx == cmd_hist_put_idx)
        {
            return;
        }
        
        /* Returned home */
        if (SHELL_CMD_HISTORY_LEN - 1 == cmd_hist_get_idx)
        {
            if (0 == cmd_hist_put_idx)
            {   
                cmd_hist_get_idx = 0;
                cmd_buf_reset();
                return;
            }
        }
        else
        {
            if (cmd_hist_get_idx + 1 == cmd_hist_put_idx)
            {
                cmd_hist_get_idx++;
                cmd_buf_reset();
                return;
            }
        }
        
        cmd_hist_get_idx++;
        if (SHELL_CMD_HISTORY_LEN == cmd_hist_get_idx)
        {
            cmd_hist_get_idx = 0;
        }
    }
    
    cmd_buf_reset();
    strncpy(cmd_buf, cmd_hist[cmd_hist_get_idx], sizeof(cmd_buf));
    cmd_buf_idx = strlen(cmd_buf);
}

static void send_vt100_key(vt100_key_t key)
{
    vt100_keycode_t * keycode = vt100_get_keycode(key);
    if (NULL != keycode)
    {
        uart_send( keycode->code, keycode->code_len);
    }
}

static void process_shell_cmd(void)
{
    char * argv[MAX_ARGUMENT_NUM + 1];
	int argc = 0;
    int i;
	
	if (0 != strlen(cmd_buf))
    {
        save_command_to_command_history();
    }
    
    argc = parse_shell_cmd(cmd_buf, argv);
    
    if (0 == strncmp(argv[0], "help", SHELL_CMD_BUF_LEN))
    {
        print("");
        for(int i = 0; i < SHELL_CMD_NUM; ++i)
        {
            print(shell_cmd_list[i].cmd);
        }
    }
    if (0 == strncmp(argv[0], "cls", SHELL_CMD_BUF_LEN))
    {
        send_vt100_key(VT100_KEY_CLEAR_SCREEN);
        send_vt100_key(VT100_KEY_MOVE_CURSOR_HOME);
    }
    else
    {
        for(i = 0; i < SHELL_CMD_NUM; ++i)
        {
            if(0 == strncmp(argv[0], shell_cmd_list[i].cmd, SHELL_CMD_BUF_LEN))
            {
                if(0 == shell_cmd_list[i].handler(argc, argv))
                {
                    print(shell_cmd_list[i].desc);
                }
                goto exit;
            }
        }
        if (i == SHELL_CMD_NUM)
         print("???");
    }
    exit:
    cmd_buf_reset();
}

void shell_process(void)
{
	char byte;
	const char prompt[] = "DBG>";
	
	//Both Ports are set to Serial DMX
	if (UART_mode_SERIAL && UART_mode_USB)
		return;

	while (1 == uart_receive_byte((uint8_t*)&byte))
	{
        vt100_key_t key = vt100_process_byte(byte);
        
        if (VT100_KEY_PRINTABLE == key)
        {
            cmd_buf_add_char(byte);
            uart_send((uint8_t*) &byte, 1);
        }
        else if (VT100_KEY_ENTER == key)
        {
			print("");
			process_shell_cmd();
            uart_send((uint8_t*) &byte, 1);
			print_no_newline("\n%s", prompt);
        }
        else if (VT100_KEY_INVALID == key)
        {
            /* Do nothing */
        }
        else if (VT100_KEY_BACKSPACE == key)
        {
            cmd_buf_remove_char();
            send_vt100_key(VT100_KEY_BACKSPACE);
            send_vt100_key(VT100_KEY_CLEAR_EOL);
        }
        else if (VT100_KEY_CURSOR_UP == key)
        {
            send_vt100_key(VT100_KEY_CLEAR_ENTIRE_LINE);
            for (unsigned int ii = 0; ii < cmd_buf_idx + sizeof(prompt) - 1; ++ii)
            {
                send_vt100_key(VT100_KEY_MOVE_CURSOR_LEFT);
            }
			print_no_newline(prompt);
            load_command_from_command_history(0);
            print_no_newline(cmd_buf);
        }
        else if (VT100_KEY_CURSOR_DOWN == key)
        {
            send_vt100_key(VT100_KEY_CLEAR_ENTIRE_LINE);
            for (int ii = 0; ii < cmd_buf_idx; ++ii)
            {
                send_vt100_key(VT100_KEY_MOVE_CURSOR_LEFT);
            }
            load_command_from_command_history(1);
            print_no_newline(cmd_buf);
        }
	}
}
