/**********************************************************************
 * 								      *
 * 								      *
 * 		   Copyright 1989 By Case T. Larsen		      *
 * 			 All Rights Reserved			      *
 * 				   				      *
 * 				   				      *
 **********************************************************************/

#include <pixrect/pixrect_hs.h>
#include "interp.h"

#define Y(x) (((x)/100)*5)
#define X(x) (((x)%100)*5)

static struct pixrect *screen;
static struct pixrect *bm;
static bitm[8000];
static xpos[8000], ypos[8000];

xcore_init ()
{
  int i;

  screen = pr_open("/dev/fb");
  bm = mem_create(500,400,1);
  for (i=0; i<8000;i++) {
    bitm[i] = 0;
    xpos[i] = X(i); ypos[i] = Y(i);
    zero(xpos[i],ypos[i]);
  }
  pr_rop (screen,0,0,500,400,PIX_SRC,bm,0,0);
} /* xcore_init */

zero (x,y)
     int x,y;
{
  pr_put (bm,x,y,1);
  pr_put (bm,x,y+1,1);
  pr_put (bm,x,y+2,1);
  pr_put (bm,x+1,y,1);
  pr_put (bm,x+1,y+1,1);
  pr_put (bm,x+1,y+2,1);
  pr_put (bm,x+2,y,1);
  pr_put (bm,x+2,y+1,1);
  pr_put (bm,x+2,y+2,1);
} /* zero */

one (x,y)
     int x,y;
{
  pr_put (bm,x,y,0);
  pr_put (bm,x,y+1,0);
  pr_put (bm,x,y+2,0);
  pr_put (bm,x+1,y,0);
  pr_put (bm,x+1,y+1,0);
  pr_put (bm,x+1,y+2,0);
  pr_put (bm,x+2,y,0);
  pr_put (bm,x+2,y+1,0);
  pr_put (bm,x+2,y+2,0);
} /* zero */

two (x,y)
     int x,y;
{
  pr_put (bm,x,y,1);
  pr_put (bm,x,y+1,0);
  pr_put (bm,x,y+2,0);
  pr_put (bm,x+1,y,0);
  pr_put (bm,x+1,y+1,1);
  pr_put (bm,x+1,y+2,0);
  pr_put (bm,x+2,y,0);
  pr_put (bm,x+2,y+1,0);
  pr_put (bm,x+2,y+2,1);
} /* zero */

xcore_display (mem)
     cell mem[];
{
  register int i;
  register int owner;

  for (i=0;i<8000;i++) {
    if (mem[i].lastmod != bitm[i]) {
      owner = bitm[i] = mem[i].lastmod;
      if (owner) {
	if (owner == 1) {
	  one (xpos[i],ypos[i]);
	} else
	  two (xpos[i],ypos[i]);
      } else
	zero (xpos[i],ypos[i]);
    }
  }
  pr_rop (screen,0,0,500,400,PIX_SRC,bm,0,0);
} /* xcore_display */

xcore_done ()
{
  pr_close(screen);
} /* xcore_done */
