#include "dadio.h"
#include "stdbool.h"
#include "keyboard.h"
#include "hal.h"
#include "timer.h"
#include "simplefs.h"
#include "phymem.h"

#define MAX_COMMAND_SIZE 50
#define MAX_TOKEN_SIZE 25
#define MAX_NAME_SIZE 20

#ifndef NULL
#define NULL ((void *)0)
#endif

extern char __VGA_text_memory[];
extern void wait_for_keyboard();
extern uint32_t pmmngr_allocate_block_and_read();
static void extract_token(int token_no);
static void parse_command();
static void flush_command_buffer();
static bool string_compare(char *str1, char *str2);
static void string_copy(char *strd, char *strs);
static void command_help();
static void command_fresh();
static void command_timer();
static void command_picture();
static void command_name();
static void command_ball();
static void command_quote();
static void command_whoami();
static void command_lsusr();
static void command_lsfs();
static void command_readfs();
static void command_writefs();

static char _cmd_buffer[MAX_COMMAND_SIZE];
static char _tkn_buffer[MAX_TOKEN_SIZE];
static char _shell_name[MAX_NAME_SIZE] = "shell";
char *usr;
char *passw;
char *users[] =
	{
		"root"
	};
char *passwords[] =
	{
		"abc"
	};
int users_amount = sizeof(users) / sizeof(users[0]);

static char *usr_name_login();
static char *enter_passw();
void check_input();

void kshell()
{
    simplefs_init();
	command_fresh();
	monitor_puts("\nWelcome to KSOS Kernel Shell 0.01");
	monitor_puts("\nLogin at: ");
	monitor_puts(_shell_name);
	monitor_puts(": ");
	usr = usr_name_login();
	flush_command_buffer();
	for (int i = 0; i < MAX_NAME_SIZE; i++)
		_shell_name[i] = 0;
	if (usr)
	{
		string_copy(_shell_name, usr);
	}
	enter_passw();
	flush_command_buffer();
	while (1)
	{
		check_input();
	}
}

static char *usr_name_login()
{
	while (1)
	{
	    monitor_puts("\nEnter username: ");
		flush_command_buffer();

		for (int i = 0; i < MAX_COMMAND_SIZE; i++)
		{
			char input = get_monitor_char();
			if (input == '\n')
			{
				_cmd_buffer[i] = '\0';
				break;
			}
			_cmd_buffer[i] = input;
			putc(input);
		}

		for (int i = 0; i < users_amount; i++)
		{
			if (string_compare(_cmd_buffer, users[i]))
			{
				return users[i];
			}
		}

		monitor_puts("\nInvalid username. Try again.");
	}
}

static char *enter_passw()
{
	int counter = 0;
	while (1)
	{
	    monitor_puts("\nEnter password: ");
		flush_command_buffer();

		for (int i = 0; i < MAX_COMMAND_SIZE; i++)
		{
			char input = get_monitor_char();
			if (input == '\n')
			{
				_cmd_buffer[i] = '\0';
				break;
			}
			_cmd_buffer[i] = input;
		}

		for (int i = 0; i < users_amount; i++)
		{
			if (string_compare(_cmd_buffer, passwords[i]))
			{
				return passwords[i];
			}
		}
		if (counter == 3)
		{
			return NULL;
		}

		monitor_puts("\nInvalid password. Try again.");
		counter ++;
	}
}

static void parse_command()
{
    extract_token(0);
	if (string_compare(_tkn_buffer, "help"))
	{
		command_help();
		return;
	}
	if (string_compare(_tkn_buffer, "fresh"))
	{
		command_fresh();
		return;
	}
	if (string_compare(_tkn_buffer, "timer"))
	{
		command_timer();
		return;
	}
	if (string_compare(_tkn_buffer, "picture"))
	{
		command_picture();
		return;
	}
	if (string_compare(_tkn_buffer, "ball"))
	{
		command_ball();
		return;
	}
	if (string_compare(_tkn_buffer, "quote"))
	{
		command_quote();
		return;
	}
	if (string_compare(_tkn_buffer, "name"))
	{
		command_name();
		return;
	}
	if (string_compare(_tkn_buffer, "whoami"))
	{
		command_whoami();
		return;
	}
	if (string_compare(_tkn_buffer, "addusr"))
	{
		return;
	}
	if (string_compare(_tkn_buffer, "lsusr"))
	{
		command_lsusr();
		return;
	}
	if (string_compare(_tkn_buffer, "lsfs"))
	{
		command_lsfs();
		return;
	}
	if (string_compare(_tkn_buffer, "readfs"))
	{
		command_readfs();
		return;
	}
	if (string_compare(_tkn_buffer, "writefs"))
	{
		command_writefs();
		return;
	}
	if (string_compare(_tkn_buffer, "") | string_compare(_tkn_buffer, "\n"))
	{
		return;
	}

	monitor_puts(" - Command not found: ");
	monitor_puts(_tkn_buffer);
}

void check_input()
{
	monitor_puts("\n");
	monitor_puts(_shell_name);
	monitor_puts(" >");
	flush_command_buffer();
	for (int i = 0; i < MAX_COMMAND_SIZE; i++)
	{
		char input = get_monitor_char();
		if (input == 17)
		{
			if (i != 0)
			{
				uint32_t pointer = get_cursor();
				i -= 2;
				pointer--;
				set_cursor(pointer);
			}
			else
				i--;
			continue;
		}
		else if (input == 18)
		{
			if (i == 0 && _cmd_buffer[i] == 0)
			{
				i--;
				continue;
			}
			uint32_t pointer = get_cursor();
			if (_cmd_buffer[i] != 0)
			{
				pointer++;
			}
			else
				i--;
			set_cursor(pointer);
			continue;
		}
		else if (input == '\b')
		{
			if (i > 0)
			{
				putc('\b');
				if (_cmd_buffer[i] != 0)
				{
					i--;
					int k = i;
					uint32_t pointer = get_cursor();
					while (_cmd_buffer[k] != 0)
					{
						_cmd_buffer[k] = _cmd_buffer[k + 1];
						putc(_cmd_buffer[k]);
						k++;
					}
					set_cursor(pointer);
				}
				else
				{
					i--;
					_cmd_buffer[i] = 0;
				}
			}
			i--;
			continue;
		}
		else if (input == '\n')
		{
			parse_command();
			break;
		}
		else
		{
			_cmd_buffer[i] = input;
			putc(input);
		}
	}
}

static void flush_command_buffer()
{
	for (int i = 0; i < MAX_COMMAND_SIZE; i++)
	{
		if (_cmd_buffer[i] == 0)
			break;
		_cmd_buffer[i] = 0;
	}
}

static void extract_token(int token_no)
{
    for (int i = 0; i < MAX_TOKEN_SIZE; i++)
    {
        _tkn_buffer[i] = 0;
    }

    char *pointer = _cmd_buffer;

    while (*pointer == ' ')
        pointer++;

    for (int i = 0; i < token_no; i++)
    {
        while (*pointer != ' ' && *pointer != '\0')
            pointer++;
        while (*pointer == ' ')
            pointer++;
    }

    for (int i = 0; i < MAX_TOKEN_SIZE - 1; i++)
    {
        if (*pointer == ' ' || *pointer == '\0')
        {
            _tkn_buffer[i] = '\0';
            return;
        }
        _tkn_buffer[i] = *(pointer++);
    }

    _tkn_buffer[MAX_TOKEN_SIZE - 1] = '\0';
}

static void command_help()
{
	monitor_puts("\nList of commands (use `command` help for usage):");
	monitor_puts("\nhelp");
	monitor_puts(" refresh");
	monitor_puts(" timer");
	monitor_puts(" picture");
	monitor_puts(" ball");
	monitor_puts(" addusr");
	monitor_puts(" lsusr");
	monitor_puts(" whoami");
	monitor_puts(" lsfs");
	monitor_puts(" readfs");
	monitor_puts(" writefs\n");
}

static void command_fresh()
{
	static int color[2] = {LIGHT_BLUE, BLACK};

	for (int i = 0; i < 2; i++)
	{
		extract_token(i + 1);
		if (string_compare(_tkn_buffer, "black"))
			color[i] = BLACK;
		if (string_compare(_tkn_buffer, "blue"))
			color[i] = BLUE;
		if (string_compare(_tkn_buffer, "green"))
			color[i] = GREEN;
		if (string_compare(_tkn_buffer, "cyan"))
			color[i] = CYAN;
		if (string_compare(_tkn_buffer, "red"))
			color[i] = RED;
		if (string_compare(_tkn_buffer, "magenta"))
			color[i] = MAGENTA;
		if (string_compare(_tkn_buffer, "brown"))
			color[i] = BROWN;
		if (string_compare(_tkn_buffer, "light_grey"))
			color[i] = LIGHT_GREY;
		if (string_compare(_tkn_buffer, "dark_grey"))
			color[i] = DARK_GREY;
		if (string_compare(_tkn_buffer, "light_blue"))
			color[i] = LIGHT_BLUE;
		if (string_compare(_tkn_buffer, "light_green"))
			color[i] = LIGHT_GREEN;
		if (string_compare(_tkn_buffer, "light_cyan"))
			color[i] = LIGHT_CYAN;
		if (string_compare(_tkn_buffer, "light_red"))
			color[i] = LIGHT_RED;
		if (string_compare(_tkn_buffer, "light_magenta"))
			color[i] = LIGHT_MAGENTA;
		if (string_compare(_tkn_buffer, "light_brown"))
			color[i] = LIGHT_BROWN;
		if (string_compare(_tkn_buffer, "white"))
			color[i] = WHITE;
		if (string_compare(_tkn_buffer, "help"))
		{
			monitor_puts("\tUsage: fresh c1 c2\ncolors:");
			monitor_puts("  black");
			monitor_puts("  blue");
			monitor_puts("  green");
			monitor_puts("  cyan");
			monitor_puts("  red");
			monitor_puts("  magenta");
			monitor_puts("  brown");
			monitor_puts("  light_grey");
			monitor_puts("  dark_grey");
			monitor_puts("  light_blue");
			monitor_puts("  light_green");
			monitor_puts("  light_cyan");
			monitor_puts("  light_red");
			monitor_puts("  light_magenta");
			monitor_puts("  light_brown");
			monitor_puts("  white");
			return;
		}
	}

	set_fg_color(color[0]);
	set_bg_color(color[1]);
	clear();

	set_fg_color(color[1]);
	set_bg_color(color[0]);
	for (int i = 0; i < 80; i++)
		putc(' ');
	set_cursor(20);
	monitor_puts("KSOS KERNEL SHELL 0.01 (help displays commands)");

	set_fg_color(color[0]);
	set_bg_color(color[1]);
}

static void command_timer()
{
	extract_token(1);
	if (string_compare(_tkn_buffer, "fast"))
		set_timer(0xffff >> 2);
	if (string_compare(_tkn_buffer, "medium"))
		set_timer(0xffff >> 1);
	if (string_compare(_tkn_buffer, "slow"))
		set_timer(0xffff);
	if (string_compare(_tkn_buffer, "help"))
		monitor_puts("\tUsage: timer fast/medium/slow");
}

static void command_picture()
{
	monitor_puts("\nx;;;.',,,,,,,,;,;ckO000000000OxddddxxxkkkkkkkkOOOOOOO00OOOO00OOOkx");
	monitor_puts("\nx;;'.,,,,,,,,,,,,',oOOOOO0000000OOO00OOOOOOOOOOOOO0OOO00OOOOOOxl::");
	monitor_puts("\nx;,.',,,,,,,,,,,,,,,;ldxO0000000000000OOO0000000000000OOOOOkoc;;,,");
	monitor_puts("\nx;'.,,,''','''''''',''lkO0000000000000OOO00OOOO0000000Okdoc;;;;;,,");
	monitor_puts("\nx,.''''''''','.....'':kOOOOOOO00000000OO00OOOOOkkOOOO0OOkc,;;;;;,,");
	monitor_puts("\nx..'''''''',,'....'',dOOOOddc,oOO0000OOOOOOOOddl':xOOOOOOkc,;;;;;;");
	monitor_puts("\no.'''......'',,,,;;':kOOOkc,..;kOOOOOOOOOOOOOo,'.'dOOOOOOOx;;:cccc");
	monitor_puts("\no.''.........,,,;;,'dOOOO0OdoldOOOkxxkOOOOOOOOxoldOOOOOOOOOl;ccccc");
	monitor_puts("\nd,'..........,,;;;',loxkO00000OOOOxl:lkOOOOOOOOOOOOkxxxkOOOx:ccccc");
	monitor_puts("\nx,,'''',,,,,;;;:;,.,::::dO0000000OOOOOOOOOOOOOOOOOdc::ccdOOkc:cccc");
	monitor_puts("\nx,,,,,,,,,,;:::;;..;ccc:dO000000OkolclodxkOOOOOOOkl:ccc:ckOOx;;:::");
	monitor_puts("\nx,,,,,,,,,,,,,,,,.;c::cokO000000Ol':odddlokOO000OOdccccldOOOOo;:::");
	monitor_puts("\nx,,,,,,,,,,,,,,,''ckkkOO00000000Od:ldxxdookOO0000OOOkkkkOOOOOk::cc");
	monitor_puts("\nx;;;;,,,;;;;;;;;;;,lOO00000000000OkolllllxOOOOOOOOOOOOOOOOOOOOo;::");
	monitor_puts("\nd,,;::cccccccccccc:;oOOO0000OOOO00OOOOOOOOOOOOOOOOOOOOOOOOOOOOx;;;");
	monitor_puts("\noo'',,,;;:::::::;,,,;dkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkx:,,");
}

static void command_name()
{
	extract_token(1);
	if (string_compare(_tkn_buffer, "help"))
	{
		monitor_puts("\tUsage: name string");
		return;
	}
	for (int i = 0; i < MAX_NAME_SIZE; i++)
		_shell_name[i] = 0;
	string_copy(_shell_name, _tkn_buffer);
}

#define BALL 'o'
#define STAR '*'
static void command_ball()
{
	extract_token(1);
	if (string_compare(_tkn_buffer, "help"))
	{
		monitor_puts("\nPlay with balls. Kick out the other ball to win! (w/a/s/d) (i/j/k/l)");
		return;
	}
	clear();
	set_cursor(25 * 80);
	char *vga_pointer = (char *)__VGA_text_memory;
	int ball1_x = 0;
	int ball1_y = 0;
	int ball2_x = 79;
	int ball2_y = 24;
	int y1_dir = 0, x1_dir = 0;
	int y2_dir = 0, x2_dir = 0;

	while (1)
	{
		kernel_wait();
		if (_is_keyboard_interrupt)
		{
			_is_keyboard_interrupt = 0;
			char input = get_latest_char();
			switch (input)
			{
			case 'w':
				y1_dir += -1;
				if (y1_dir < -1)
					y1_dir = -1;
				break;
			case 's':
				y1_dir += 1;
				if (y1_dir > 1)
					y1_dir = 1;
				break;
			case 'a':
				x1_dir += -1;
				if (x1_dir < -1)
					x1_dir = -1;
				break;
			case 'd':
				x1_dir += 1;
				if (x1_dir > 1)
					x1_dir = 1;
				break;

			case 'i':
				y2_dir += -1;
				if (y2_dir < -1)
					y2_dir = -1;
				break;
			case 'k':
				y2_dir += 1;
				if (y2_dir > 1)
					y2_dir = 1;
				break;
			case 'j':
				x2_dir += -1;
				if (x2_dir < -1)
					x2_dir = -1;
				break;
			case 'l':
				x2_dir += 1;
				if (x2_dir > 1)
					x2_dir = 1;
				break;
			}
		}
		if (_is_timer_interrupt)
		{
			_is_timer_interrupt = 0;
			vga_pointer[2 * ball1_x + (160 * ball1_y)] = 0;
			vga_pointer[2 * ball2_x + (160 * ball2_y)] = 0;
			ball1_x += x1_dir;
			ball1_y += y1_dir;
			if (ball1_x == ball2_x && ball1_y == ball2_y)
			{
				int temp = x1_dir;
				x1_dir = x2_dir;
				x2_dir = temp;
				temp = y1_dir;
				y1_dir = y2_dir;
				y2_dir = temp;
			}
			ball2_x += x2_dir;
			ball2_y += y2_dir;
			if (ball1_x == 80 || ball1_x < 0 || ball1_y < 0 || ball1_y == 25)
			{
				set_cursor(0);
				monitor_puts("P1 loses!");
				monitor_puts("\nPress x to exit");
				while (get_monitor_char() != 'x')
					;
				break;
			}

			if (ball2_x == 80 || ball2_x < 0 || ball2_y < 0 || ball2_y == 25)
			{
				set_cursor(0);
				monitor_puts("P2 loses!");
				monitor_puts("\nPress x to exit");
				while (get_monitor_char() != 'x')
					;
				break;
			}

			vga_pointer[2 * ball1_x + (160 * ball1_y)] = BALL;
			vga_pointer[2 * ball2_x + (160 * ball2_y)] = STAR;
		}
	}
	command_fresh();
}

static void command_quote()
{
	return;
}
static void string_copy(char *strd, char *strs)
{
	for (int i = 0; strs[i]; i++)
	{
		strd[i] = strs[i];
	}
}

static bool string_compare(char *str1, char *str2)
{
	int i = 0;
	while (str1[i])
	{
		if (str1[i] != str2[i])
			return false;
		i++;
	}
	if (str2[i])
		return false;
	return true;
}

static void command_whoami()
{
	monitor_puts("\n");
	monitor_puts(usr);
}

static int string_length(const char *s) {
    int i = 0;
    while (s[i]) i++;
    return i;
}

static void command_lsusr()
{
	monitor_puts("\n");
    for (int i = 0; i < users_amount; i++)
    {
        monitor_puts(users[i]);
        monitor_puts("\n");
    }
}

static void command_lsfs() {
    simplefs_list_files();
}

static void command_readfs() {
    extract_token(1);
    uint16_t buf[64]; // 128 bytes, 2-byte aligned
    int maxlen = sizeof(buf) - 1;
    int n = simplefs_read_file(_tkn_buffer, (char*)buf, maxlen);
    if (n < 0) {
        monitor_puts("\nFile not found\n");
        return;
    }
    if (n > maxlen) n = maxlen;
    monitor_puts("\nContent: ");
    int printable = 1;
    for (int i = 0; i < n; i++) {
        char c = ((char*)buf)[i];
        if (c < 32 && c != '\n' && c != '\r' && c != '\t') { printable = 0; break; }
        if (c > 126) { printable = 0; break; }
    }
    if (printable) {
        ((char*)buf)[n] = '\0';
        monitor_puts((char*)buf);
    } else {
        for (int i = 0; i < n && i < 128; i++) {
            char hex[5] = {0};
            unsigned char c = ((unsigned char*)buf)[i];
            hex[0] = '0'; hex[1] = 'x';
            const char* digits = "0123456789ABCDEF";
            hex[2] = digits[(c >> 4) & 0xF];
            hex[3] = digits[c & 0xF];
            hex[4] = ' ';
            monitor_puts(hex);
            if ((i+1) % 16 == 0) monitor_puts("\n");
        }
    }
    monitor_puts("\n");
    uint32_t val = pmmngr_allocate_block_and_read();
    monitor_puts("[pmmngr_allocate_block_and_read() returned: ");
    printhex(val);
    monitor_puts("]\n");
}

static void command_writefs() {
    extract_token(1);
    char *fname = _tkn_buffer;
    extract_token(2);
    char *content = _tkn_buffer;
    if (simplefs_write_file(fname, content, string_length(content)) == 0) {
        monitor_puts("\nFile written.\n");
    } else {
        monitor_puts("\nWrite failed.\n");
    }
}