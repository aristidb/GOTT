
/*
**
**		filename:		w_input.h
**
**		description:	input related functions
**		target-system:	microsoft windows
**		version-info:   $Id$
**
*/



int			_glsk_input_init();
void		_glsk_input_free();

int			_glsk_input_set_owner( glsk_window_t* window );

void		_glsk_input_enable( int value );
void		_glsk_input_update();
