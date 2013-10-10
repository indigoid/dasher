#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ncurses.h>

#include "menus.h"
#include "menudefs.h"

void display_menu(struct context* c, struct menu* m, WINDOW* win) {
	assert(c != NULL);
	assert(m != NULL);
	assert(win != NULL);
	assert(c->menupos >= 0);
	int ox = 1, oy = 1, printed = 0;
	box(win,0,0);
	mvwprintw(win,oy,ox,"Menu: %s", m->title);
	for (uint8_t n = 0; ! ISMENUITEMLAST(m->menuitems[n]) && printed < LCD_ROWS-1; n++) {
		if (n < c->menupos)
			continue;
		struct menuitem* p = & m->menuitems[n];
		mvwprintw(win, ox+printed+1, ox,
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
