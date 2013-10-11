#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ncurses.h>

#include "menus.h"
#include "menudefs.h"

#define lcd_write(w,y,x,fmt, ...) mvwprintw(w,y+1,x+1,fmt, __VA_ARGS__)

struct fragment {
	char 			text[LCD_COLS+1];
	uint8_t 		x,y;
	union {
		unsigned int	uint;
		signed int	sint;
		void*		ptr;
	} input;
	void (*update)(struct fragment* frag);
	void (*render)(struct fragment* frag);
};

void random_uint(struct fragment* frag) {
	assert(frag != NULL);
	frag->input.uint = rand() % 180;
}

void render_speed(struct fragment* frag) {
	assert(frag != NULL);
	snprintf(frag->text, LCD_COLS, "spd:%3d", frag->input.uint);
}

void render_heading(struct fragment* frag) {
	assert(frag != NULL);
	snprintf(frag->text, LCD_COLS, "dir:%3d", frag->input.uint);
}

#define FRAGMENT(_y,_x,_ucb,_rcb) {	\
	.y = _y,			\
	.x = _x,			\
	.update = _ucb,			\
	.render = _rcb,			\
	.input = { .ptr = NULL }	\
}

#define FRAGMENT_TEXT(_y, _x, _text) {	\
	.y = _y,			\
	.x = _x,			\
	.text = _text,			\
	.render = NULL,			\
	.update = NULL,			\
	.input = { .ptr = NULL }	\
}

#define FRAGMENTLAST() {		\
	.x = 0,				\
	.y = 0,				\
	.text = "",			\
	.render = NULL,			\
	.update = NULL,			\
	.input = { .ptr = NULL }	\
}

struct fragment fragments[] = {
	FRAGMENT_TEXT(0,0,"Dasher Prototype v1"),
	FRAGMENT(1,13,random_uint,render_speed),
	FRAGMENT(1,5,random_uint,render_speed),
	FRAGMENTLAST()
};

void lcdwrite_fragment(struct context* c, WINDOW* win, struct fragment* frag) {
	assert(c != NULL);
	assert(win != NULL);
	assert(frag != NULL);
	if (frag->update)
		frag->update(frag);
	if (frag->render)
		frag->render(frag);
	lcd_write(win, frag->y, frag->x, "%s", frag->text);
}

void lcdwrite_all_fragments(struct context* c, WINDOW* win, struct fragment* frags) {
	assert(c != NULL);
	assert(win != NULL);
	assert(frags != NULL);
	struct fragment* frag = fragments;
	while (1) {
		if (frag->render == NULL && frag->input.ptr == NULL && frag->text[0] == '\0')
			return;
		lcdwrite_fragment(c,win,frag);
		frag++;
	}
}

void display_menu(struct context* c, struct menu* m, WINDOW* win) {
	assert(c != NULL);
	assert(m != NULL);
	assert(win != NULL);
	assert(c->menupos >= 0);
	int printed = 0;
	box(win,0,0);
	lcd_write(win,0,0,"Menu: %s", m->title);
	for (uint8_t n = 0; ! ISMENUITEMLAST(m->menuitems[n]) && printed < LCD_ROWS-1; n++) {
		if (n < c->menupos)
			continue;
		struct menuitem* p = & m->menuitems[n];
		lcd_write(win, printed+1, 0,
			p->issubmenu ? "%s%d. %s ->\n" : "%s%d. %s",
			n == c->menupos ? ">>" : "  ",
			n+1, p->text);
		printed++;
	}
	wrefresh(win);
}

WINDOW* setup_terminal(struct context* ctx) {
	WINDOW* lcd;
	initscr();
	raw();
	keypad(stdscr, TRUE);
	noecho();
	int rows = LCD_ROWS+2, cols = LCD_COLS+2;
	int lcdoriginy = (LINES-rows)/2, lcdoriginx = (COLS-cols)/2;
	lcd = subwin(stdscr,rows,cols,lcdoriginy,lcdoriginx);
	box(lcd,0,0);
	wrefresh(lcd);
	return lcd;
}

void shutdown_terminal(struct context* ctx) {
	endwin();
}

void wclearbox(WINDOW* win) {
	wclear(win);
	box(win,0,0);
}

int menupos_ok(struct menu* m, uint8_t index) {
	for (uint8_t n = 0; ! ISMENUITEMLAST(m->menuitems[n]); n++) {
		if (n == index)
			return 1;
	}
	return 0;
}

void menu_move(struct context* ctx, struct menu* m, int8_t delta, WINDOW* win) {
	assert(delta != 0); // because that would be ridiculous
	if (delta < 0) { // moving down
		if (ctx->menupos == 0) {
			// maybe add an infuriating beep or something here
			return;
		} else {
			ctx->menupos--;
		}
	} else {
		if (menupos_ok(m,ctx->menupos+1)) {
			ctx->menupos++;
		}
	}
	werase(win);
	display_menu(ctx,m,win);
}

#define RULER "01234567890123456789"

int main(int argc, char** argv) {
	struct context ctx = { .tracking = 0, .menupos = 0 };
	int ch;
	WINDOW* lcd = setup_terminal(&ctx);
	display_menu(&ctx, &m_track, lcd);
	refresh();
	while ((ch = getch()) != 'q') {
		switch(ch) {
			case KEY_UP:
				menu_move(&ctx,&m_track,-1,lcd);
				break;
			case KEY_DOWN:
				menu_move(&ctx,&m_track,+1,lcd);
				break;
			case 'r': /* display ruler */
				wclearbox(lcd);
				lcd_write(lcd,0,0,"%s",RULER);
				wrefresh(lcd);
				break;
			case 'f': /* display fragments */
				wclearbox(lcd);
				lcdwrite_all_fragments(&ctx,lcd,fragments);
				wrefresh(lcd);
				break;
			case ' ':
				werase(stdscr);
				mvwprintw(stdscr,1,1,"Selected item: %s",m_track.menuitems[ctx.menupos].text);
				display_menu(&ctx,&m_track,lcd);
				break;
			case 'q':
				goto done;
		}
	}
done:
	shutdown_terminal(&ctx);
	return 0;
}
