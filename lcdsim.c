#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ncurses.h>
#include <math.h>

#include "menus.h"
#include "menudefs.h"
#include "fragment.h"
#include "geofence.h"
#include "fdirection.h"

#define lcd_write(w,y,x,fmt, ...) mvwprintw(w,y+1,x+1,fmt, __VA_ARGS__)

void wclearbox(WINDOW* win);

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

struct fragment fragments[] = {
	FRAGMENT_TEXT(0,0,"Dasher Prototype v1"),
	FRAGMENT(1,13,random_uint,render_speed),
	FRAGMENT(1,5,random_uint,render_heading),
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
	while (!ISFRAGMENTLAST(frag)) {
		lcdwrite_fragment(c,win,frag);
		frag++;
	}
}

void lcdwrite_director(WINDOW* win, struct geo* ref, struct geo* pos) {
	assert(win != NULL);
	assert(ref != NULL);
	assert(pos != NULL);
	char director[3][3] = {
		{ '.', '.', '.' },
		{ '.', '*', '.' },
		{ '.', '.', '.' }
	};
	int dlat = fdirection(ref->latitude,pos->latitude) + 1;
	int dlong = fdirection(ref->longitude,pos->longitude) + 1;
	director[dlat][dlong] = 'X';
	wclearbox(win);
	lcd_write(win,0,0,"%c%c%c lat :%.6f%c",
		director[0][0], director[0][1], director[0][2],
		fabs(pos->latitude), pos->latitude > 0 ? 'N' : 'S');
	lcd_write(win,1,0,"%c%c%c long:%.6f%c",
		director[1][0], director[1][1], director[1][2],
		fabs(pos->longitude), pos->longitude > 0 ? 'E' : 'W');
	lcd_write(win,2,0,"%c%c%c dist:%.2fm",
		director[2][0], director[2][1], director[2][2],
		distance_in_metres(ref, pos));
	wrefresh(win);
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

WINDOW* setup_terminal(void) {
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

void shutdown_terminal(void) {
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
#define MOVEINCR 0.0005

int main(int argc, char** argv) {
	struct context ctx = { .tracking = 0, .menupos = 0 };
	int ch;
	WINDOW* lcd = setup_terminal();
	display_menu(&ctx, &m_track, lcd);
	refresh();
	struct geo ref = { .latitude = 0.0, .longitude = 0.0 };
	struct geo pos = ref;
	while ((ch = getch()) != 'q') {
		switch(ch) {
			/* menu navigation keys */
			case KEY_UP:
				menu_move(&ctx,&m_track,-1,lcd);
				break;
			case KEY_DOWN:
				menu_move(&ctx,&m_track,+1,lcd);
				break;
			/* select a menu item */
			case ' ':
				werase(stdscr);
				mvwprintw(stdscr,1,1,"Selected item: %s",m_track.menuitems[ctx.menupos].text);
				display_menu(&ctx,&m_track,lcd);
				break;
			/* display a ruler */
			case 'r': 
				wclearbox(lcd);
				lcd_write(lcd,0,0,"%s",RULER);
				wrefresh(lcd);
				break;
			/* test fragment display */
			case 'f':
				wclearbox(lcd);
				lcdwrite_all_fragments(&ctx,lcd,fragments);
				wrefresh(lcd);
				break;
			/* test compass director */
			case 'h': /* go WEST */
				pos.longitude -= MOVEINCR;
				lcdwrite_director(lcd,&ref,&pos);
				break;
			case 'j': /* go SOUTH */
				pos.latitude -= MOVEINCR;
				lcdwrite_director(lcd,&ref,&pos);
				break;
			case 'k': /* go NORTH */
				pos.latitude += MOVEINCR;
				lcdwrite_director(lcd,&ref,&pos);
				break;
			case 'l': /* go EAST */
				pos.longitude += MOVEINCR;
				lcdwrite_director(lcd,&ref,&pos);
				break;
			/* quit */
			case 'q':
				goto done;
				break;
		}
	}
done:
	shutdown_terminal();
	return 0;
}
