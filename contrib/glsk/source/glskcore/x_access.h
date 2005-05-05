
/*
**
**		filename:		x_access.c
**
**		description:	window access functions
**		target-system:	X-Windows
**		version-info:   $Id$
**
*/


typedef struct glsk_motif_wm_hints_s
{
    unsigned long flags;
    unsigned long functions;
    unsigned long decorations;
    long input_mode;
    unsigned long status;
	
} glsk_motif_wm_hints_t;


#define MWM_HINTS_DECORATIONS   (1L << 1)
#define _XA_MOTIF_WM_HINTS		"_MOTIF_WM_HINTS"
