#ifndef IG_FRAGMENT_H__
#define IG_FRAGMENT_H__

#include <stdint.h>
#include <stddef.h>

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

#define ISFRAGMENTLAST(f) (		\
	f->y == 0			\
	&& f->x == 0 			\
	&& f->text[0] == '\0'		\
	&& f->render == NULL		\
	&& f->update == NULL		\
	&& f->input.ptr == NULL		\
)

#endif /* IG_FRAGMENT_H__ */
