/***************************************************************************

  vidhrdw.c

  Functions to emulate the video hardware of the machine.

***************************************************************************/

#include "driver.h"
#include "vidhrdw/generic.h"

static struct osd_bitmap *polybitmap1,*polybitmap2;
static struct osd_bitmap *polybitmap;

extern UINT8 irvg_clear;
extern UINT8 irobot_bufsel;
extern UINT8 irobot_alphamap;
extern UINT8 *irobot_combase;

static int ir_xmin, ir_ymin, ir_xmax, ir_ymax; /* clipping area */



/***************************************************************************

  Convert the color PROMs into a more useable format.

  5 bits from polygon ram address the palette ram

  Output of color RAM
  bit 8 -- inverter -- 1K ohm resistor  -- RED
  bit 7 -- inverter -- 2.2K ohm resistor  -- RED
        -- inverter -- 1K ohm resistor  -- GREEN
        -- inverter -- 2.2K ohm resistor  -- GREEN
        -- inverter -- 1K ohm resistor  -- BLUE
        -- inverter -- 2.2K ohm resistor  -- BLUE
        -- inverter -- 2.2K ohm resistor  -- INT
        -- inverter -- 4.7K ohm resistor  -- INT
  bit 0 -- inverter -- 9.1K ohm resistor  -- INT

  Alphanumeric colors are generated by ROM .125, it's outputs are connected
  to bits 1..8 as above. The inputs are:

  A0..1 - Character color
  A2    - Character image (1=pixel on/0=off)
  A3..4 - Alphamap 0,1 (appears that only Alphamap1 is used, it is set by
          the processor)

***************************************************************************/
void irobot_vh_convert_color_prom(unsigned char *palette, unsigned short *colortable,const unsigned char *color_prom)
{
  int i;
#define TOTAL_COLORS(gfxn) (Machine->gfx[gfxn]->total_colors * Machine->gfx[gfxn]->color_granularity)
#define COLOR(gfxn,offs) (colortable[Machine->drv->gfxdecodeinfo[gfxn].color_codes_start + offs])

  /* the palette will be initialized by the game. We just set it to some */
  /* pre-cooked values so the startup copyright notice can be displayed. */
  for (i = 0; i < 64; i++)
  {
    *(palette++) = ((i & 1) >> 0) * 0xff;
    *(palette++) = ((i & 2) >> 1) * 0xff;
    *(palette++) = ((i & 4) >> 2) * 0xff;
  }

  /* Convert the color prom for the text palette */
  for (i = 0; i < 32; i++)
  {
    int r,g,b;
    int bits,intensity;
    unsigned int color;

    color = *color_prom;
    intensity = color & 0x03;
    bits = (color >> 6) & 0x03;
    r = 28 * bits * intensity;
    bits = (color >> 4) & 0x03;
    g = 28 * bits * intensity;
    bits = (color >> 2) & 0x03;
    b = 28 * bits * intensity;
    *(palette++) = r;
    *(palette++) = g;
    *(palette++) = b;
    color_prom++;
  }

  /* polygons */
  for (i = 0; i < 64; i++)
    colortable[i] = i;

  /* text */
  for (i = 0; i < TOTAL_COLORS(0); i++)
  {
    COLOR(0,i) = ((i & 0x18) | ((i & 0x01) << 2) | ((i & 0x06) >> 1)) + 64;
  }
}


WRITE_HANDLER( irobot_paletteram_w )
{
  int r,g,b;
  int bits,intensity;
  unsigned int color;

  color = ((data << 1) | (offset & 0x01)) ^ 0x1ff;
  intensity = color & 0x07;
  bits = (color >> 3) & 0x03;
  b = 12 * bits * intensity;
  bits = (color >> 5) & 0x03;
  g = 12 * bits * intensity;
  bits = (color >> 7) & 0x03;
  r = 12 * bits * intensity;
  palette_change_color((offset >> 1) & 0x3F,r,g,b);
}


/***************************************************************************

  Fast line drawing.

***************************************************************************/
#define DRAW_HLINE_FUNC(NAME, TYPE, XSTART, YSTART, XADV) 				\
  static void NAME(int x1, int x2, int y, int col)					\
  {																	\
    TYPE *dest = &((TYPE *)polybitmap->line[YSTART])[XSTART];		\
    int dx = XADV;													\
    for ( ; x1 <= x2; x1++, dest += dx)								\
      *dest = col;												\
  }

DRAW_HLINE_FUNC(draw_hline_8, UINT8, x1, y, 1)
DRAW_HLINE_FUNC(draw_hline_8_fx, UINT8, ir_xmax - x1, y, -1)
DRAW_HLINE_FUNC(draw_hline_8_fy, UINT8, x1, ir_ymax - y, 1)
DRAW_HLINE_FUNC(draw_hline_8_fx_fy, UINT8, ir_xmax - x1, ir_ymax - y, -1)

DRAW_HLINE_FUNC(draw_hline_8_swap, UINT8, y, x1, polybitmap->line[1] - polybitmap->line[0])
DRAW_HLINE_FUNC(draw_hline_8_swap_fx, UINT8, ir_ymax - y, x1, polybitmap->line[1] - polybitmap->line[0])
DRAW_HLINE_FUNC(draw_hline_8_swap_fy, UINT8, y, ir_xmax - x1, polybitmap->line[0] - polybitmap->line[1])
DRAW_HLINE_FUNC(draw_hline_8_swap_fx_fy, UINT8, ir_ymax - y, ir_xmax - x1, polybitmap->line[0] - polybitmap->line[1])

DRAW_HLINE_FUNC(draw_hline_16, UINT16, x1, y, 1)
DRAW_HLINE_FUNC(draw_hline_16_fx, UINT16, ir_xmax - x1, y, -1)
DRAW_HLINE_FUNC(draw_hline_16_fy, UINT16, x1, ir_ymax - y, 1)
DRAW_HLINE_FUNC(draw_hline_16_fx_fy, UINT16, ir_xmax - x1, ir_ymax - y, -1)

DRAW_HLINE_FUNC(draw_hline_16_swap, UINT16, y, x1, (polybitmap->line[1] - polybitmap->line[0]) / 2)
DRAW_HLINE_FUNC(draw_hline_16_swap_fx, UINT16, ir_ymax - y, x1, (polybitmap->line[1] - polybitmap->line[0]) / 2)
DRAW_HLINE_FUNC(draw_hline_16_swap_fy, UINT16, y, ir_xmax - x1, (polybitmap->line[0] - polybitmap->line[1]) / 2)
DRAW_HLINE_FUNC(draw_hline_16_swap_fx_fy, UINT16, ir_ymax - y, ir_xmax - x1, (polybitmap->line[0] - polybitmap->line[1]) / 2)

static void (*draw_hline)(int x1, int x2, int y, int col);

static void (*hline_8_table[8])(int x1, int x2, int y, int col) =
{
  draw_hline_8, draw_hline_8_fx, draw_hline_8_fy, draw_hline_8_fx_fy,
  draw_hline_8_swap, draw_hline_8_swap_fx, draw_hline_8_swap_fy, draw_hline_8_swap_fx_fy
};

static void (*hline_16_table[8])(int x1, int x2, int y, int col) =
{
  draw_hline_16, draw_hline_16_fx, draw_hline_16_fy, draw_hline_16_fx_fy,
  draw_hline_16_swap, draw_hline_16_swap_fx, draw_hline_16_swap_fy, draw_hline_16_swap_fx_fy
};

/***************************************************************************

  Start the video hardware emulation.

***************************************************************************/
int irobot_vh_start(void)
{
  /* Setup 2 bitmaps for the polygon generator */
  if ((polybitmap1 = bitmap_alloc(Machine->drv->screen_width,Machine->drv->screen_height)) == 0)
    return 1;
  if ((polybitmap2 = bitmap_alloc(Machine->drv->screen_width,Machine->drv->screen_height)) == 0)
    return 1;

  /* Set clipping */
  ir_xmin = ir_ymin = 0;
  ir_xmax = Machine->drv->screen_width;
  ir_ymax = Machine->drv->screen_height;

  /* Compute orientation parameters */
  if (polybitmap1->depth == 8)
    draw_hline = hline_8_table[Machine->orientation & ORIENTATION_MASK];
  else
    draw_hline = hline_16_table[Machine->orientation & ORIENTATION_MASK];

  return 0;
}

/***************************************************************************

  Stop the video hardware emulation.

***************************************************************************/
void irobot_vh_stop(void)
{
  bitmap_free(polybitmap1);
  bitmap_free(polybitmap2);
}

/***************************************************************************

	Polygon Generator  (Preliminary information)
	The polygon communication ram works as follows (each location is a 16-bit word):

	0000-xxxx: Object pointer table
		bits 00..10: Address of object data
		bits 12..15: Object type
			0x4 = Polygon
			0x8 = Point
			0xC = Vector
		(0xFFFF means end of table)

	Point Object:
		Word 0, bits 0..15: X Position  (0xFFFF = end of point objects)
		Word 1, bits 7..15: Y Position
		bits 0..5: Color

	Vector Object:
		Word 0, bits 7..15: Ending Y   (0xFFFF = end of line objects)
		Word 1, bits 7..15: Starting Y
				bits 0..5: Color
		Word 2: Slope
		Word 3, bits 0..15: Starting X

	Polygon Object:
		Word 0, bits 0..10: Pointer to second slope list
		Word 1, bits 0..15: Starting X first vector
		Word 2, bits 0..15: Starting X second vector
		Word 3, bits 0..5: Color
		bits 7..15: Initial Y value

	Slope Lists: (one starts at Word 4, other starts at pointer in Word 0)
		Word 0, Slope (0xFFFF = side done)
		Word 1, bits 7..15: Ending Y of vector

	Each side is a continous set of vectors. Both sides are drawn at
	the same time and the space between them is filled in.

***************************************************************************/

void irobot_poly_clear(void)
{

  if (irobot_bufsel)
    osd_clearbitmap(polybitmap2);
  else
    osd_clearbitmap(polybitmap1);
}

INLINE void irobot_draw_pixel (int x, int y, int col)
{
  if (x < ir_xmin || x >= ir_xmax)
    return;
  if (y < ir_ymin || y >= ir_ymax)
    return;

  plot_pixel (polybitmap, x, y, col);
}

/*
     Line draw routine
     modified from a routine written by Andrew Caldwell
 */

void irobot_draw_line (int x1, int y1, int x2, int y2, int col)
{
  int dx,dy,sx,sy,cx,cy;

  dx = abs(x1-x2);
  dy = abs(y1-y2);
  sx = (x1 <= x2) ? 1: -1;
  sy = (y1 <= y2) ? 1: -1;
  cx = dx/2;
  cy = dy/2;

  if (dx>=dy)
  {
    for (;;)
    {
      irobot_draw_pixel (x1, y1, col);
      if (x1 == x2) break;
      x1 += sx;
      cx -= dy;
      if (cx < 0)
      {
        y1 += sy;
        cx += dx;
      }
    }
  }
  else
  {
    for (;;)
    {
      irobot_draw_pixel (x1, y1, col);
      if (y1 == y2) break;
      y1 += sy;
      cy -= dx;
      if (cy < 0)
      {
        x1 += sx;
        cy += dy;
      }
    }
  }
}


#define ROUND_TO_PIXEL(x)	((x >> 7) - 128)

void run_video(void)
{
  int sx,sy,ex,ey,sx2,ey2;
  int color;
  unsigned int d1;
  int lpnt,spnt,spnt2;
  int shp;
  INT32 word1,word2;

  //logerror("Starting Polygon Generator, Clear=%d\n",irvg_clear);

  if (irobot_bufsel)
    polybitmap = polybitmap2;
  else
    polybitmap = polybitmap1;

  lpnt=0;
  while (lpnt < 0xFFF)
  {
    d1 = READ_WORD(&irobot_combase[lpnt]);
    lpnt+=2;
    if (d1 == 0xFFFF) break;
    spnt = (d1 & 0x07FF) << 1;
    shp = (d1 & 0xF000) >> 12;

    /* Pixel */
    if (shp == 0x8)
    {
      while (spnt < 0xFFE)
      {
        sx = READ_WORD(&irobot_combase[spnt]);
        if (sx == 0xFFFF) break;
        sy = READ_WORD(&irobot_combase[spnt+2]);
        color = Machine->pens[sy & 0x3F];
        irobot_draw_pixel(ROUND_TO_PIXEL(sx),ROUND_TO_PIXEL(sy),color);
        spnt+=4;
      }//while object
    }//if point

    /* Line */
    if (shp == 0xC)
    {
      while (spnt < 0xFFF)
      {
        ey = READ_WORD(&irobot_combase[spnt]);
        if (ey == 0xFFFF) break;
        ey = ROUND_TO_PIXEL(ey);
        sy = READ_WORD(&irobot_combase[spnt+2]);
        color = Machine->pens[sy & 0x3F];
        sy = ROUND_TO_PIXEL(sy);
        sx = READ_WORD(&irobot_combase[spnt+6]);
        word1 = (INT16)READ_WORD(&irobot_combase[spnt+4]);
        ex = sx + word1 * (ey - sy + 1);
        irobot_draw_line(ROUND_TO_PIXEL(sx),sy,ROUND_TO_PIXEL(ex),ey,color);
        spnt+=8;
      }//while object
    }//if line

    /* Polygon */
    if (shp == 0x4)
    {
      spnt2 = READ_WORD(&irobot_combase[spnt]);
      spnt2 = (spnt2 & 0x7FF) << 1;

      sx = READ_WORD(&irobot_combase[spnt+2]);
      sx2 = READ_WORD(&irobot_combase[spnt+4]);
      sy = READ_WORD(&irobot_combase[spnt+6]);
      color = Machine->pens[sy & 0x3F];
      sy = ROUND_TO_PIXEL(sy);
      spnt+=8;

      word1 = (INT16)READ_WORD(&irobot_combase[spnt]);
      ey = READ_WORD(&irobot_combase[spnt+2]);
      if (word1 != -1 || ey != 0xFFFF)
      {
        ey = ROUND_TO_PIXEL(ey);
        spnt+=4;

        //	sx += word1;

        word2 = (INT16)READ_WORD(&irobot_combase[spnt2]);
        ey2 = ROUND_TO_PIXEL(READ_WORD(&irobot_combase[spnt2+2]));
        spnt2+=4;

        //	sx2 += word2;

        while(1)
        {


          if (sy >= ir_ymin && sy < ir_ymax)
          {
            int x1 = ROUND_TO_PIXEL(sx);
            int x2 = ROUND_TO_PIXEL(sx2);
            int temp;

            if (x1 > x2) temp = x1, x1 = x2, x2 = temp;
            if (x1 < ir_xmin) x1 = ir_xmin;
            if (x2 > ir_xmax) x2 = ir_xmax;
            if (x1 < x2)
              (*draw_hline)(x1 + 1, x2, sy, color);
          }
          sy++;

          if (sy > ey)
          {
            word1 = (INT16)READ_WORD(&irobot_combase[spnt]);
            ey = READ_WORD(&irobot_combase[spnt+2]);
            if (word1 == -1 && ey == 0xFFFF)
              break;
            ey = ROUND_TO_PIXEL(ey);
            spnt+=4;
          }
          else
            sx += word1;

          if (sy > ey2)
          {
            word2 = (INT16)READ_WORD(&irobot_combase[spnt2]);
            ey2 = ROUND_TO_PIXEL(READ_WORD(&irobot_combase[spnt2+2]));
            spnt2+=4;
          }
          else
            sx2 += word2;

        } //while polygon
      }//if at least 2 sides
    } //if polygon
  } //while object
}



/***************************************************************************

  Draw the game screen in the given osd_bitmap.
  Do NOT call osd_update_display() from this function, it will be called by
  the main emulation engine.

***************************************************************************/
void irobot_vh_screenrefresh(struct osd_bitmap *bitmap,int full_refresh)
{
  int x, y, offs;

  //logerror("Screen Refresh\n");

  palette_recalc();

  /* copy the polygon bitmap */
  if (irobot_bufsel)
    copybitmap(bitmap,polybitmap1,0,0,0,0,&Machine->visible_area,TRANSPARENCY_NONE,0);
  else
    copybitmap(bitmap,polybitmap2,0,0,0,0,&Machine->visible_area,TRANSPARENCY_NONE,0);

  /* redraw the non-zero characters in the alpha layer */
  for (y = offs = 0; y < 32; y++)
    for (x = 0; x < 32; x++, offs++)
      if (videoram[offs] != 0)
      {
        int code = videoram[offs] & 0x3f;
        int color = ((videoram[offs] & 0xC0) >> 6) | (irobot_alphamap >> 3);
        int transp=color + 64;

        drawgfx(bitmap,Machine->gfx[0],
                code, color,
                0,0,
                8*x,8*y,
                &Machine->visible_area,TRANSPARENCY_COLOR,transp);
      }
}
