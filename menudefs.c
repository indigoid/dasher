#include <stddef.h>

#include "menus.h"
#include "handlers.h"

struct menu m_track = {
	.title = "Tracks",
	.menuitems = {
		MENUITEM("Start track", start_track),
		MENUITEM("Clear track", clear_track),
		MENUITEM("Browse track", NULL),
		MENUITEM("Fence nearest", NULL),
		MENUITEM("Save track", NULL),
		MENUITEMLAST(),
	}
};

struct menu m_main = {
	.title = "Dasher",
	.menuitems = {
		SUBMENU("Tracks", &m_track),
		MENUITEM("About", about_dasher),
		MENUITEMLAST(),
	}
};

