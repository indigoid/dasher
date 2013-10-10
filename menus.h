#ifndef IG_DASHER_MENUS_H__
#define IG_DASHER_MENUS_H__

#include <stdint.h>

#define LCD_COLS	20
#define LCD_ROWS	4
#define MAX_MENUHISTORY	4

struct context {
	uint8_t		tracking;
	struct menu*	menuhistory[MAX_MENUHISTORY];
	uint8_t		menupos;
};

typedef void (*menufunc)(struct context*);

struct menu;

struct menuitem {
	uint8_t				issubmenu;
	char				text[LCD_COLS+1];
	union {
		struct menu* 		submenu;
		menufunc		handler;
	};
};

struct menu {
	char*				title;
	struct menuitem			menuitems[];
};

#define MENUITEM(_text,_handler) { 	\
	.issubmenu = 0,			\
	.text = _text,			\
	.handler = _handler		\
}

#define SUBMENU(_text,_submenu) {	\
	.issubmenu = 1,			\
	.text = _text,			\
	.submenu = _submenu		\
}

#define MENUITEMLAST() {		\
	.issubmenu = 0,			\
	.text = "",			\
	.handler = NULL			\
}

#define ISMENUITEMLAST(x) (		\
	x.handler == NULL && 		\
	x.issubmenu == 0 && 		\
	x.text[0] == '\0'		\
)

#endif /* IG_DASHER_MENUS_H__ */
