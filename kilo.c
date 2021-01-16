/*
* Kilo
* Build Your Own Text Editor
* Following a tutorial by Pailey (Snaptoken / viewsourcecode.org)
* https://viewsourcecode.org/snaptoken/kilo/index.html
*/

# include <ctype.h>
# include <errno.h>
# include <stdio.h>
# include <stdlib.h>
# include <termios.h>
# include <unistd.h>

/*
* defines
*/

#define CTRL_KEY(k) ((k) & 0x1f)

/*
* data
*/

struct termios orig_termios;

/*
* terminal
*/

void die(const char *s) {
	/* Clear screen & reposition cursor*/
	write(STDOUT_FILENO, "\x1b[2J", 4);
	write(STDOUT_FILENO, "\x1b[H", 3);

	perror(s);
	exit(1);
}

void disable_raw_mode() {
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
		die("tcsetattr");
}

void enable_raw_mode() {
	if (tcsetattr(STDIN_FILENO, &orig_termios) == -1)
		die("tcsetattr");
	atexit(disable_raw_mode);

	struct termios raw = orig_termios;
	raw.c_lflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
	raw.c_lflag &= ~(OPOST);
	raw.c_lflag &= ~(CS8);
	raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
	raw.c_cc[VMIN] = 0;
	raw.c_cc[VTIME] = 1;

	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
		die("tcsetattr");
}

char editor_read_key() {
	int nread;
	char c;
	while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
		if (nread == -1 && errno != EAGAIN) die("read");
	}
}

/*
* output
*/

void editor_refresh_screen() {
	write(STDOUT_FILENO, "\x1b[2J", 4);
	write(STDOUT_FILENO, "\x1b[H", 3);
}

/*
* input
*/
void editor_process_keypress() {
	char c = editor_read_key();

	switch (c) {
		case CTRL_KEY('q'):
		/* Clear screen & reposition cursor*/
		write(STDOUT_FILENO, "\x1b[2J", 4);
		write(STDOUT_FILENO, "\x1b[H", 3);

		exit(0);
		break;
	}
}

/*
* init
*/

int main() {
	enable_raw_mode();

	while(1) {
		editor_refresh_screen();
		editor_process_keypress();
	}

	return 0;
}