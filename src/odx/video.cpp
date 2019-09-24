#include "driver.h"
#include <math.h>
#include "vidhrdw/vector.h"
#include "dirty.h"

#include "minimal.h"

dirtygrid grid1;
dirtygrid grid2;
char *dirty_old=grid1;
char *dirty_new=grid2;

/* in msdos/sound.c */
int msdos_update_audio(void);

/* specialized update_screen functions defined in blit.c */
/* dirty mode 1 (VIDEO_SUPPORTS_DIRTY) */
void blitscreen_dirty1_color8(struct osd_bitmap *bitmap);
void blitscreen_dirty1_color16(struct osd_bitmap *bitmap);
void blitscreen_dirty1_palettized16(struct osd_bitmap *bitmap);
/* dirty mode 0 (no osd_mark_dirty calls) */
void blitscreen_dirty0_color8(struct osd_bitmap *bitmap);
void blitscreen_dirty0_color16(struct osd_bitmap *bitmap);
void blitscreen_dirty0_palettized16(struct osd_bitmap *bitmap);


void init_blit_data();

static void update_screen_dummy(struct osd_bitmap *bitmap);
void (*update_screen)(struct osd_bitmap *bitmap) = update_screen_dummy;

static int video_depth,video_fps;
static int modifiable_palette;
static int screen_colors;
static unsigned char *current_palette;
static unsigned int *dirtycolor;
static int dirtypalette;
static int dirty_bright;
static int bright_lookup[256];
extern UINT32 *palette_16bit_lookup;

int frameskip,autoframeskip;
#define FRAMESKIP_LEVELS 12

int video_sync;
int wait_vsync;
int vsync_frame_rate;
int skiplines;
int skipcolumns;
int use_dirty;
float osd_gamma_correction = 1.0;
int brightness;
float brightness_paused_adjust;
int gfx_width;
int gfx_height;
int x_aspect_lookup[ODX_SCREEN_WIDTH];
int y_aspect_lookup[ODX_SCREEN_HEIGHT];

unsigned int iAddX;
unsigned int iModuloX;
unsigned int iAddY;
unsigned int iModuloY;
unsigned int scaled_display_columns;
unsigned int scaled_display_lines;

static int viswidth;
static int visheight;
static int skiplinesmax;
static int skipcolumnsmax;
static int skiplinesmin;
static int skipcolumnsmin;

static int vector_game;

int gfx_xoffset;
int gfx_yoffset;
int gfx_display_lines;
int gfx_display_columns;
static int xmultiply,ymultiply;
int throttle = 1;       /* toggled by F10 */

static int frameskip_counter;

#include "minimal.h"

#define TICKER unsigned int
#define ticker() odx_timer_read()
#define TICKS_PER_SEC 1000000

#define vsync() odx_video_wait_vsync()
#define makecol(r,g,b) odx_video_color16(r,g,b,0)
#define getr(c) odx_video_getr16(c)
#define getg(c) odx_video_getg16(c)
#define getb(c) odx_video_getb16(c)

int video_scale=0;
int video_border=0;
int video_aspect=0;

/* Create a bitmap. Also calls osd_clearbitmap() to appropriately initialize */
/* it to the background color. */
/* VERY IMPORTANT: the function must allocate also a "safety area" 16 pixels wide all */
/* around the bitmap. This is required because, for performance reasons, some graphic */
/* routines don't clip at boundaries of the bitmap. */

const int safety = 16;

struct osd_bitmap *osd_alloc_bitmap(int width,int height,int depth)
{
  struct osd_bitmap *bitmap;


  if ((bitmap = (struct osd_bitmap *)malloc(sizeof(struct osd_bitmap))) != 0)
  {
    int i,rowlen,rdwidth;
    unsigned char *bm;


    if (depth != 8 && depth != 16) depth = 8;

    bitmap->depth = depth;
    bitmap->width = width;
    bitmap->height = height;

    rdwidth = (width + 7) & ~7;     /* round width to a quadword */
    if (depth == 16)
      rowlen = 2 * (rdwidth + 2 * safety) * sizeof(unsigned char);
    else
      rowlen =     (rdwidth + 2 * safety) * sizeof(unsigned char);

    if ((bm = (unsigned char*)malloc((height + 2 * safety) * rowlen)) == 0)
    {
      free(bitmap);
      return 0;
    }

    /* clear ALL bitmap, including safety area, to avoid garbage on right */
    /* side of screen is width is not a multiple of 4 */
    memset(bm,0,(height + 2 * safety) * rowlen);

    if ((bitmap->line = (unsigned char**)malloc((height + 2 * safety) * sizeof(unsigned char *))) == 0)
    {
      free(bm);
      free(bitmap);
      return 0;
    }

    for (i = 0; i < height + 2 * safety; i++)
    {
      if (depth == 16)
        bitmap->line[i] = &bm[i * rowlen + 2*safety];
      else
        bitmap->line[i] = &bm[i * rowlen + safety];
    }
    bitmap->line += safety;

    bitmap->_private = bm;

    osd_clearbitmap(bitmap);
  }

  return bitmap;
}



/* set the bitmap to black */
void osd_clearbitmap(struct osd_bitmap *bitmap)
{
  int i;


  for (i = 0; i < bitmap->height; i++)
  {
    if (bitmap->depth == 16)
      memset(bitmap->line[i],0,2*bitmap->width);
    else
      memset(bitmap->line[i],0,bitmap->width);
  }


  if (bitmap == Machine->scrbitmap)
  {
    extern int bitmap_dirty;        /* in mame.c */

    osd_mark_dirty (0,0,bitmap->width-1,bitmap->height-1,1);
    bitmap_dirty = 1;
  }
}



void osd_free_bitmap(struct osd_bitmap *bitmap)
{
  if (bitmap)
  {
    bitmap->line -= safety;
    free(bitmap->line);
    free(bitmap->_private);
    free(bitmap);
    bitmap = (struct osd_bitmap *) 0;
  }
}


void osd_mark_dirty(int _x1, int _y1, int _x2, int _y2, int ui)
{
  if (use_dirty)
  {
    int x, y;

//        logerror("mark_dirty %3d,%3d - %3d,%3d\n", _x1,_y1, _x2,_y2);

    _x1 -= skipcolumns;
    _x2 -= skipcolumns;
    _y1 -= skiplines;
    _y2 -= skiplines;

    if (_y1 >= gfx_display_lines || _y2 < 0 || _x1 > gfx_display_columns || _x2 < 0) return;
    if (_y1 < 0) _y1 = 0;
    if (_y2 >= gfx_display_lines) _y2 = gfx_display_lines - 1;
    if (_x1 < 0) _x1 = 0;
    if (_x2 >= gfx_display_columns) _x2 = gfx_display_columns - 1;

    for (y = _y1; y <= _y2 + 15; y += 16)
      for (x = _x1; x <= _x2 + 15; x += 16)
        MARKDIRTY(x,y);
  }
}

static void init_dirty(char dirty)
{
  memset(dirty_new, dirty, MAX_GFX_WIDTH/16 * MAX_GFX_HEIGHT/16);
}

INLINE void swap_dirty(void)
{
  char *tmp;

  tmp = dirty_old;
  dirty_old = dirty_new;
  dirty_new = tmp;
}



/*
 * This function tries to find the best display mode.
 */
static void select_display_mode(int width,int height,int depth,int attributes,int orientation)
{
  /* 16 bit color is supported only by VESA modes */
  if (depth == 16 || depth == 32)
  {
    logerror("Game needs %d-bit colors.\n",depth);
  }

  if (!gfx_width && !gfx_height)
  {
    gfx_width = ODX_SCREEN_WIDTH;
    gfx_height = ODX_SCREEN_HEIGHT;
  }

  /* Video scaling - Horizontal */
  if (video_scale ==1)
  {
    if (width>ODX_SCREEN_WIDTH)
    {
      if ((ODX_SCREEN_WIDTH/(width-ODX_SCREEN_WIDTH))*(width-ODX_SCREEN_WIDTH)==ODX_SCREEN_WIDTH)
      {
        gfx_width=width;
      }
      else
      {
        video_scale=0;
      }
    }
    else if (width<ODX_SCREEN_WIDTH)
    {
      if ((ODX_SCREEN_WIDTH/(ODX_SCREEN_WIDTH-width))*(ODX_SCREEN_WIDTH-width)==ODX_SCREEN_WIDTH)
      {
        gfx_width=width;
      }
      else
      {
        video_scale=0;
      }
    }
    else
    {
      video_scale=0;
    }
  }

  /* Video scaling - Best Fit */
  if (video_scale == 3 || video_scale == 4)
  {
    iAddX = iModuloX = iAddY = iModuloY = 1;

    if (ODX_SCREEN_WIDTH < width)
    {
      iAddX = ODX_SCREEN_WIDTH;
      iModuloX = width;
      scaled_display_columns = ODX_SCREEN_WIDTH;
    }
    if (ODX_SCREEN_HEIGHT < height)
    {
      iAddY = ODX_SCREEN_HEIGHT;
      iModuloY = height;
      scaled_display_lines = ODX_SCREEN_HEIGHT;
    }
    if (ODX_SCREEN_WIDTH < width || ODX_SCREEN_HEIGHT < height)
    {
      if (height * ODX_SCREEN_WIDTH < ODX_SCREEN_HEIGHT * width)
      {
        // scale is determined by horizontal scale
        iAddY = iAddX;
        iModuloY = iModuloX;
        scaled_display_lines = (height * ODX_SCREEN_WIDTH + width - 1) / width;
        if (scaled_display_lines > ODX_SCREEN_HEIGHT)
        {
          scaled_display_lines = ODX_SCREEN_HEIGHT;
        }
      }
      else
      {
        // scale is determined by vertical scale
        iAddX = iAddY;
        iModuloX = iModuloY;
        scaled_display_columns = (width * ODX_SCREEN_HEIGHT + height - 1) / height;
        if (scaled_display_columns > ODX_SCREEN_WIDTH)
        {
          scaled_display_columns = ODX_SCREEN_WIDTH;
        }
      }
    }
    else
    {
      video_scale = 0;
    }
  }


  /* Border TV-Out */
  if (video_border)
  {
    gfx_width=(((gfx_width+((gfx_width*6)/100))+7)&~7);
    gfx_height=(((gfx_height+((gfx_height*8)/100))+7)&~7);
  }

  /* vector games always use maximum resolution */
  if (vector_game)
  {
    gfx_width = ODX_SCREEN_WIDTH;
    gfx_height = ODX_SCREEN_HEIGHT;
  }
}



/* center image inside the display based on the visual area */
void osd_set_visible_area(int min_x,int max_x,int min_y,int max_y)
{

  logerror("set visible area %d-%d %d-%d\n",min_x,max_x,min_y,max_y);
  logerror("video_scale %d\n",video_scale);

  viswidth  = max_x - min_x + 1;
  visheight = max_y - min_y + 1;

  /* setup xmultiply to handle SVGA driver's (possible) double width */
  xmultiply = 1;
  ymultiply = 1;

  switch(video_scale)
  {
    case 2:		// halfscale
    {
      gfx_display_lines = visheight;
      gfx_display_columns = viswidth;
      gfx_xoffset = ((gfx_width<<1) - viswidth * xmultiply) / 2;
      gfx_yoffset = ((gfx_height<<1) - visheight * ymultiply) / 2;
    }
    break;

    case 3:		// bestscale
    case 4:		// fastscale
    {
      gfx_display_lines = visheight;
      gfx_display_columns = viswidth;

      gfx_xoffset = (gfx_width - scaled_display_columns * xmultiply) / 2;
      gfx_yoffset = (gfx_height - scaled_display_lines * ymultiply) / 2;
    }
    break;

    case 5:		// double vertical
    case 9:		// double scanlines
    {
      if(visheight * 2 > gfx_height)
        visheight = gfx_height / 2;

      gfx_display_lines = visheight;
      gfx_display_columns = viswidth;

      gfx_xoffset = (gfx_width - viswidth * xmultiply) / 2;
      gfx_yoffset = (gfx_height - (visheight << 1) * ymultiply) / 2;
    }
    break;

    case 6:		// aspect scale
    case 7:		// aspect scale fast
    {
      if(visheight > gfx_height)
        visheight = gfx_height;

      gfx_display_lines = visheight;
      gfx_display_columns = viswidth;

      gfx_xoffset = (gfx_width - viswidth * xmultiply) / 2;
      gfx_yoffset = 0;//(gfx_height - visheight * ymultiply) / 2;

      // Generate aspect scaling lookups.
      float gfx_x_aspect = (float)gfx_display_columns / (float)gfx_width;
      float gfx_y_aspect = (float)gfx_display_lines / (float)gfx_height;

      for(int set = 0; set < gfx_width; set++)
        x_aspect_lookup[set] = set * gfx_x_aspect;
      for(int set = 0; set < gfx_height; set++)
        y_aspect_lookup[set] = set * gfx_y_aspect;
    }
    break;

    case 8:		// full screen scale
    {
      if(visheight > gfx_height)
        visheight = gfx_height;

      gfx_display_lines = visheight;
      gfx_display_columns = viswidth;

      gfx_xoffset = gfx_yoffset = 0;

      // Generate aspect scaling lookups.
      float gfx_x_aspect = (float)gfx_display_columns / (float)gfx_width;
      float gfx_y_aspect = (float)gfx_display_lines / (float)gfx_height;

      for(int set = 0; set < gfx_width; set++)
        x_aspect_lookup[set] = set * gfx_x_aspect;
      for(int set = 0; set < gfx_height; set++)
        y_aspect_lookup[set] = set * gfx_y_aspect;
    }
    break;

    default:	// horizscale
    {
      gfx_display_lines = visheight;
      gfx_display_columns = viswidth;

      gfx_xoffset = (gfx_width - viswidth * xmultiply) / 2;
      if (gfx_display_columns > gfx_width / xmultiply)
        gfx_display_columns = gfx_width / xmultiply;

      gfx_yoffset = (gfx_height - visheight * ymultiply) / 2;
      if (gfx_display_lines > gfx_height / ymultiply)
        gfx_display_lines = gfx_height / ymultiply;
    }
  }

  skiplinesmin = min_y;
  skiplinesmax = visheight - gfx_display_lines + min_y;
  skipcolumnsmin = min_x;
  skipcolumnsmax = viswidth - gfx_display_columns + min_x;

  /* Align on a quadword !*/
  gfx_xoffset &= ~7;

  /* the skipcolumns from mame.cfg/cmdline is relative to the visible area */
  skipcolumns = min_x + skipcolumns;
  skiplines   = min_y + skiplines;

  /* Just in case the visual area doesn't fit */
  if (gfx_xoffset < 0)
  {
    skipcolumns -= gfx_xoffset;
    gfx_xoffset = 0;
  }
  if (gfx_yoffset < 0)
  {
    skiplines   -= gfx_yoffset;
    gfx_yoffset = 0;
  }

  /* Failsafe against silly parameters */
  if (skiplines < skiplinesmin)
    skiplines = skiplinesmin;
  if (skipcolumns < skipcolumnsmin)
    skipcolumns = skipcolumnsmin;
  if (skiplines > skiplinesmax)
    skiplines = skiplinesmax;
  if (skipcolumns > skipcolumnsmax)
    skipcolumns = skipcolumnsmax;

  logerror("gfx_width = %d gfx_height = %d\n"
           "gfx_xoffset = %d gfx_yoffset = %d\n"
           "xmin %d ymin %d xmax %d ymax %d\n"
           "skiplines %d skipcolumns %d\n"
           "gfx_display_lines %d gfx_display_columns %d\n"
           "xmultiply %d ymultiply %d\n",
           gfx_width,gfx_height,
           gfx_xoffset,gfx_yoffset,
           min_x, min_y, max_x, max_y, skiplines, skipcolumns,gfx_display_lines,gfx_display_columns,xmultiply,ymultiply);

  set_ui_visarea(skipcolumns, skiplines, skipcolumns+gfx_display_columns-1, skiplines+gfx_display_lines-1);

  /* round to a multiple of 4 to avoid missing pixels on the right side */
  gfx_display_columns  = (gfx_display_columns + 3) & ~3;

  init_blit_data();

}



/*
Create a display screen, or window, of the given dimensions (or larger).
Attributes are the ones defined in driver.h.
Returns 0 on success.
*/
int osd_create_display(int width,int height,int depth,int fps,int attributes,int orientation)
{
  logerror("width %d, height %d\n", width,height);

  video_depth = depth;
  video_fps = fps;

  brightness = 100;
  brightness_paused_adjust = 1.0;
  dirty_bright = 1;

  if (frameskip < 0) frameskip = 0;
  if (frameskip >= FRAMESKIP_LEVELS) frameskip = FRAMESKIP_LEVELS-1;

  /* Look if this is a vector game */
  if (attributes & VIDEO_TYPE_VECTOR)
    vector_game = 1;
  else
    vector_game = 0;


  if (use_dirty == -1)	/* dirty=auto in mame.cfg? */
  {
    /* Is the game using a dirty system? */
    if ((attributes & VIDEO_SUPPORTS_DIRTY) || vector_game)
      use_dirty = 1;
    else
      use_dirty = 0;
  }

  select_display_mode(width,height,depth,attributes,orientation);

  if (!osd_set_display(width,height,depth,attributes,orientation))
    return 1;

  /* set visible area to nothing just to initialize it - it will be set by the core */
  osd_set_visible_area(0,0,0,0);

  return 0;
}

/* set the actual display screen but don't allocate the screen bitmap */
int osd_set_display(int width,int height,int depth,int attributes,int orientation)
{
  int     i;

  if (!gfx_height || !gfx_width)
  {
    printf("Please specify height AND width (e.g. -640x480)\n");
    return 0;
  }

  /* Mark the dirty buffers as dirty */

  if (use_dirty)
  {
    if (vector_game)
      /* vector games only use one dirty buffer */
      init_dirty (0);
    else
      init_dirty(1);
    swap_dirty();
    init_dirty(1);
  }
  if (dirtycolor)
  {
    for (i = 0; i < screen_colors; i++)
      dirtycolor[i] = 1;
    dirtypalette = 1;
  }

  /* Set video mode */
  odx_set_video_mode(depth,gfx_width,gfx_height);

  vsync_frame_rate = video_fps;

  if (video_sync)
  {
    TICKER a,b;
    float rate;


    /* wait some time to let everything stabilize */
    for (i = 0; i < 60; i++)
    {
      vsync();
      a = ticker();
    }

    /* small delay for really really fast machines */
    for (i = 0; i < 100000; i++) ;

    vsync();
    b = ticker();

    rate = ((float)TICKS_PER_SEC)/(b-a);

    logerror("target frame rate = %ffps, video frame rate = %3.2fHz\n",video_fps,rate);

    /* don't allow more than 8% difference between target and actual frame rate */
    while (rate > video_fps * 108 / 100)
      rate /= 2;

    if (rate < video_fps * 92 / 100)
    {
      osd_close_display();
      logerror("-vsync option cannot be used with this display mode:\n"
               "video refresh frequency = %dHz, target frame rate = %ffps\n",
               (int)(TICKS_PER_SEC/(b-a)),video_fps);
      return 0;
    }

    logerror("adjusted video frame rate = %3.2fHz\n",rate);
    vsync_frame_rate = rate;

    if (Machine->sample_rate)
    {
      Machine->sample_rate = Machine->sample_rate * video_fps / rate;
      logerror("sample rate adjusted to match video freq: %d\n",Machine->sample_rate);
    }
  }

  return 1;
}

/* shut up the display */
void osd_close_display(void)
{
  free(dirtycolor);
  dirtycolor = 0;
  free(current_palette);
  current_palette = 0;
  free(palette_16bit_lookup);
  palette_16bit_lookup = 0;
}

int osd_allocate_colors(unsigned int totalcolors,const unsigned char *palette,unsigned short *pens,int modifiable)
{
  int i;

  modifiable_palette = modifiable;
  screen_colors = totalcolors;
  if (video_depth != 8){
    screen_colors += 2;
  }
  else {
    screen_colors = 256;
  }

  dirtycolor = (unsigned int*)malloc(screen_colors * sizeof(int));
  current_palette = (unsigned char*)malloc(3 * screen_colors * sizeof(unsigned char));
  palette_16bit_lookup = (UINT32*)malloc(screen_colors * sizeof(palette_16bit_lookup[0]));
  if (dirtycolor == 0 || current_palette == 0 || palette_16bit_lookup == 0){
    return 1;
  }

  for (i = 0; i < screen_colors; i++){
    dirtycolor[i] = 1;
  }
  dirtypalette = 1;
  for (i = 0; i < screen_colors; i++){
    current_palette[3*i+0] = current_palette[3*i+1] = current_palette[3*i+2] = 0;
  }

  if (video_depth != 8 && modifiable == 0)
  {
    int r,g,b;

    for (i = 0; i < totalcolors; i++)
    {
      r = 255 * brightness * pow(palette[3*i+0] / 255.0, 1 / osd_gamma_correction) / 100;
      g = 255 * brightness * pow(palette[3*i+1] / 255.0, 1 / osd_gamma_correction) / 100;
      b = 255 * brightness * pow(palette[3*i+2] / 255.0, 1 / osd_gamma_correction) / 100;
      *pens++ = makecol(r,g,b);
    }

    Machine->uifont->colortable[0] = makecol(0x00,0x00,0x00);
    Machine->uifont->colortable[1] = makecol(0xff,0xff,0xff);
    Machine->uifont->colortable[2] = makecol(0xff,0xff,0xff);
    Machine->uifont->colortable[3] = makecol(0x00,0x00,0x00);
  }
  else
  {
    if (video_depth == 8 && totalcolors >= 255)
    {
      int bestblack,bestwhite;
      int bestblackscore,bestwhitescore;


      bestblack = bestwhite = 0;
      bestblackscore = 3*255*255;
      bestwhitescore = 0;
      for (i = 0; i < totalcolors; i++)
      {
        int r,g,b,score;

        r = palette[3*i+0];
        g = palette[3*i+1];
        b = palette[3*i+2];
        score = r*r + g*g + b*b;

        if (score < bestblackscore)
        {
          bestblack = i;
          bestblackscore = score;
        }
        if (score > bestwhitescore)
        {
          bestwhite = i;
          bestwhitescore = score;
        }
      }

      for (i = 0; i < totalcolors; i++)
        pens[i] = i;

      /* map black to pen 0, otherwise the screen border will not be black */
      pens[bestblack] = 0;
      pens[0] = bestblack;

      Machine->uifont->colortable[0] = pens[bestblack];
      Machine->uifont->colortable[1] = pens[bestwhite];
      Machine->uifont->colortable[2] = pens[bestwhite];
      Machine->uifont->colortable[3] = pens[bestblack];
    }
    else
    {
      /* reserve color 1 for the user interface text */
      current_palette[3*1+0] = current_palette[3*1+1] = current_palette[3*1+2] = 0xff;
      Machine->uifont->colortable[0] = 0;
      Machine->uifont->colortable[1] = 1;
      Machine->uifont->colortable[2] = 1;
      Machine->uifont->colortable[3] = 0;

      /* fill the palette starting from the end, so we mess up badly written */
      /* drivers which don't go through Machine->pens[] */
      for (i = 0; i < totalcolors; i++)
        pens[i] = (screen_colors-1)-i;
    }

    for (i = 0; i < totalcolors; i++)
    {
      current_palette[3*pens[i]+0] = palette[3*i];
      current_palette[3*pens[i]+1] = palette[3*i+1];
      current_palette[3*pens[i]+2] = palette[3*i+2];
    }
  }

  if (video_depth == 16)
  {
    if (modifiable_palette)
    {
      if (use_dirty)
      {
        update_screen = blitscreen_dirty1_palettized16;
        logerror("blitscreen_dirty1_palettized16\n");
      }
      else
      {
        update_screen = blitscreen_dirty0_palettized16;
        logerror("blitscreen_dirty0_palettized16\n");
      }
    }
    else
    {
      if (use_dirty)
      {
        update_screen = blitscreen_dirty1_color16;
        logerror("blitscreen_dirty1_color16\n");
      }
      else
      {
        update_screen = blitscreen_dirty0_color16;
        logerror("blitscreen_dirty0_color16\n");
      }
    }
  }
  else
  {
    if (use_dirty) // supports dirty ?
    {
      update_screen = blitscreen_dirty1_color8;
      logerror("blitscreen_dirty1_color8\n");
    }
    else
    {
      update_screen = blitscreen_dirty0_color8;
      logerror("blitscreen_dirty0_color8\n");
    }
  }
  return 0;
}


void osd_modify_pen(int pen,unsigned char red, unsigned char green, unsigned char blue)
{
  if (modifiable_palette == 0)
  {
    logerror("error: osd_modify_pen() called with modifiable_palette == 0\n");
    return;
  }

  if (current_palette[3*pen+0] != red ||
      current_palette[3*pen+1] != green ||
      current_palette[3*pen+2] != blue)
  {
    current_palette[3*pen+0] = red;
    current_palette[3*pen+1] = green;
    current_palette[3*pen+2] = blue;
    dirtycolor[pen] = 1;
    dirtypalette = 1;
  }
}



void osd_get_pen(int pen,unsigned char *red, unsigned char *green, unsigned char *blue)
{
  if (video_depth != 8 && modifiable_palette == 0)
  {
    *red =   getr(pen);
    *green = getg(pen);
    *blue =  getb(pen);
  }
  else
  {
    *red =   current_palette[3*pen+0];
    *green = current_palette[3*pen+1];
    *blue =  current_palette[3*pen+2];
  }
}



static void update_screen_dummy(struct osd_bitmap *bitmap)
{
  logerror("msdos/video.c: undefined update_screen() function for %d x %d!\n",xmultiply,ymultiply);
}

INLINE void pan_display(void)
{
  int pan_changed = 0;

  /* horizontal panning */
  if (input_ui_pressed_repeat(IPT_UI_PAN_LEFT,1))
    if (skipcolumns < skipcolumnsmax)
    {
      skipcolumns++;
      osd_mark_dirty (0,0,Machine->scrbitmap->width-1,Machine->scrbitmap->height-1,1);
      pan_changed = 1;
    }
  if (input_ui_pressed_repeat(IPT_UI_PAN_RIGHT,1))
    if (skipcolumns > skipcolumnsmin)
    {
      skipcolumns--;
      osd_mark_dirty (0,0,Machine->scrbitmap->width-1,Machine->scrbitmap->height-1,1);
      pan_changed = 1;
    }
  if (input_ui_pressed_repeat(IPT_UI_PAN_DOWN,1))
    if (skiplines < skiplinesmax)
    {
      skiplines++;
      osd_mark_dirty (0,0,Machine->scrbitmap->width-1,Machine->scrbitmap->height-1,1);
      pan_changed = 1;
    }
  if (input_ui_pressed_repeat(IPT_UI_PAN_UP,1))
    if (skiplines > skiplinesmin)
    {
      skiplines--;
      osd_mark_dirty (0,0,Machine->scrbitmap->width-1,Machine->scrbitmap->height-1,1);
      pan_changed = 1;
    }

  if (pan_changed)
  {
    if (use_dirty) init_dirty(1);

    set_ui_visarea (skipcolumns, skiplines, skipcolumns+gfx_display_columns-1, skiplines+gfx_display_lines-1);
  }
}

int osd_skip_this_frame(void)
{
  static const int skiptable[FRAMESKIP_LEVELS][FRAMESKIP_LEVELS] =
  {
    { 0,0,0,0,0,0,0,0,0,0,0,0 },
    { 0,0,0,0,0,0,0,0,0,0,0,1 },
    { 0,0,0,0,0,1,0,0,0,0,0,1 },
    { 0,0,0,1,0,0,0,1,0,0,0,1 },
    { 0,0,1,0,0,1,0,0,1,0,0,1 },
    { 0,1,0,0,1,0,1,0,0,1,0,1 },
    { 0,1,0,1,0,1,0,1,0,1,0,1 },
    { 0,1,0,1,1,0,1,0,1,1,0,1 },
    { 0,1,1,0,1,1,0,1,1,0,1,1 },
    { 0,1,1,1,0,1,1,1,0,1,1,1 },
    { 0,1,1,1,1,1,0,1,1,1,1,1 },
    { 0,1,1,1,1,1,1,1,1,1,1,1 }
  };
  return skiptable[frameskip][frameskip_counter];
}

/* Update the display. */
void osd_update_video_and_audio(struct osd_bitmap *bitmap)
{
  static const int waittable[FRAMESKIP_LEVELS][FRAMESKIP_LEVELS] =
  {
    { 1,1,1,1,1,1,1,1,1,1,1,1 },
    { 2,1,1,1,1,1,1,1,1,1,1,0 },
    { 2,1,1,1,1,0,2,1,1,1,1,0 },
    { 2,1,1,0,2,1,1,0,2,1,1,0 },
    { 2,1,0,2,1,0,2,1,0,2,1,0 },
    { 2,0,2,1,0,2,0,2,1,0,2,0 },
    { 2,0,2,0,2,0,2,0,2,0,2,0 },
    { 2,0,2,0,0,3,0,2,0,0,3,0 },
    { 3,0,0,3,0,0,3,0,0,3,0,0 },
    { 4,0,0,0,4,0,0,0,4,0,0,0 },
    { 6,0,0,0,0,0,6,0,0,0,0,0 },
    {12,0,0,0,0,0,0,0,0,0,0,0 }
  };
  int i;
  static int showfps=0,showfpstemp=0;
  TICKER curr;
  static TICKER prev_measure=0,this_frame_base,prev;
  static int speed = 100;
  static int vups,vfcount;
  int have_to_clear_bitmap = 0;

  if (prev_measure==0)
  {
    /* first time through, initialize timer */
    prev_measure = ticker() - FRAMESKIP_LEVELS * TICKS_PER_SEC/video_fps;
  }

  if (frameskip_counter == 0)
    this_frame_base = prev_measure + FRAMESKIP_LEVELS * TICKS_PER_SEC/video_fps;

  /* update audio */
  msdos_update_audio();

  if (osd_skip_this_frame() == 0)
  {
    if (showfpstemp)
    {
      showfpstemp--;
      if (showfps == 0 && showfpstemp == 0)
      {
        have_to_clear_bitmap = 1;
      }
    }

    if (input_ui_pressed(IPT_UI_SHOW_FPS))
    {
      if (showfpstemp)
      {
        showfpstemp = 0;
        have_to_clear_bitmap = 1;
      }
      else
      {
        showfps ^= 1;
        if (showfps == 0)
        {
          have_to_clear_bitmap = 1;
        }
      }
    }

    /* now wait until it's time to update the screen */
    if (throttle)
    {
      profiler_mark(PROFILER_IDLE);
      if (video_sync)
      {
        static TICKER last;
        do
        {
          vsync();
          curr = ticker();
        }
        while (TICKS_PER_SEC / (curr - last) > video_fps * 11 /10);
        last = curr;
      }
      else
      {
        TICKER target;
        /* wait for video sync but use normal throttling */
        if (wait_vsync)
          vsync();
        curr = ticker();
        target = this_frame_base + frameskip_counter * TICKS_PER_SEC/video_fps;
        if ((curr < target) && (target-curr<TICKS_PER_SEC))
        {
          do
          {
            curr = ticker();
          }
          while ((curr < target) && (target-curr<TICKS_PER_SEC));
        }
      }
      profiler_mark(PROFILER_END);
    }
    else curr = ticker();

    if (frameskip_counter == 0)
    {
      int divdr;
      divdr = video_fps * (curr - prev_measure) / (100 * FRAMESKIP_LEVELS);
      if (divdr==0)
        divdr=1;
      speed = (TICKS_PER_SEC + divdr/2) / divdr;
      prev_measure = curr;
    }

    prev = curr;

    vfcount += waittable[frameskip][frameskip_counter];
    if (vfcount >= video_fps)
    {
      extern int vector_updates; /* avgdvg_go_w()'s per Mame frame, should be 1 */
      vfcount = 0;
      vups = vector_updates;
      vector_updates = 0;
    }

    if (showfps || showfpstemp)
    {
      int fps;
      char buf[30];
      int divdr = 100 * FRAMESKIP_LEVELS;
      fps = (video_fps * (FRAMESKIP_LEVELS - frameskip) * speed + (divdr>> 1)) / divdr;
      sprintf(buf, "%s%2d%4d%%%4d/%d fps", autoframeskip ? "auto":"skip", frameskip, speed, fps, (int)(video_fps+0.5));
      ui_text(bitmap, buf, Machine->uiwidth-strlen(buf) * Machine->uifontwidth, 1);
      if (vector_game)
      {
        sprintf(buf," %d vector updates", vups);
        ui_text(bitmap, buf, Machine->uiwidth-strlen(buf) * Machine->uifontwidth, Machine->uifontheight);
      }
    }

    if (bitmap->depth == 8)
    {
      if (dirty_bright)
      {
        dirty_bright = 0;
        for (i = 0; i < 256; i++)
        {
          float rate = brightness * brightness_paused_adjust * pow(i / 255.0, 1 / osd_gamma_correction) / 100;
          bright_lookup[i] = 255 * rate + 0.5;
        }
      }
      if (dirtypalette)
      {
        dirtypalette = 0;
        for (i = 0; i < screen_colors; i++)
        {
          if (dirtycolor[i])
          {
            unsigned char r,g,b;

            dirtycolor[i] = 0;

            r = current_palette[3*i+0];
            g = current_palette[3*i+1];
            b = current_palette[3*i+2];
            if (i != Machine->uifont->colortable[1])	/* don't adjust the user interface text */
            {
              r = bright_lookup[r];
              g = bright_lookup[g];
              b = bright_lookup[b];
            }
            odx_video_color8(i,r,g,b);
          }
        }
        odx_video_setpalette();
      }
    }
    else
    {
      if (dirty_bright)
      {
        dirty_bright = 0;
        for (i = 0; i < 256; i++)
        {
          float rate = brightness * brightness_paused_adjust * pow(i / 255.0, 1 / osd_gamma_correction) / 100;
          bright_lookup[i] = 255 * rate + 0.5;
        }
      }
      if (dirtypalette)
      {
        if (use_dirty) init_dirty(1);	/* have to redraw the whole screen */

        dirtypalette = 0;
        for (i = 0; i < screen_colors; i++)
        {
          if (dirtycolor[i])
          {
            int r,g,b;

            dirtycolor[i] = 0;

            r = current_palette[3*i+0];
            g = current_palette[3*i+1];
            b = current_palette[3*i+2];
            if (i != Machine->uifont->colortable[1])	/* don't adjust the user interface text */
            {
              r = bright_lookup[r];
              g = bright_lookup[g];
              b = bright_lookup[b];
            }
            palette_16bit_lookup[i] = makecol(r,g,b);
          }
        }
      }
    }

    /* copy the bitmap to screen memory */
    profiler_mark(PROFILER_BLIT);
    update_screen(bitmap);
    profiler_mark(PROFILER_END);

    if (have_to_clear_bitmap)
      osd_clearbitmap(bitmap);

    if (use_dirty)
    {
      if (!vector_game)
        swap_dirty();
      init_dirty(0);
    }

    if (have_to_clear_bitmap)
      osd_clearbitmap(bitmap);

    if (throttle && autoframeskip && frameskip_counter == 0)
    {
      static int frameskipadjust;
      int adjspeed;

      /* adjust speed to video refresh rate if vsync is on */
      adjspeed = speed * video_fps / vsync_frame_rate;

      if (adjspeed >= 92)
      {
        frameskipadjust++;
        if (frameskipadjust >= 3)
        {
          frameskipadjust = 0;
          if (frameskip > 0) frameskip--;
        }
      }
      else
      {
        if (adjspeed < 80)
          frameskipadjust -= (90 - adjspeed) / 5;
        else
        {
          /* don't push frameskip too far if we are close to 100% speed */
          if (frameskip < 8)
            frameskipadjust--;
        }

        while (frameskipadjust <= -2)
        {
          frameskipadjust += 2;
          //if (frameskip < FRAMESKIP_LEVELS-1) frameskip++;
          if (frameskip < 7) frameskip++;
        }
      }
    }
  }

#if 0
  /* Check for PGUP, PGDN and pan screen */
  pan_display();

  if (input_ui_pressed(IPT_UI_FRAMESKIP_INC))
  {
    if (autoframeskip)
    {
      autoframeskip = 0;
      frameskip = 0;
    }
    else
    {
      if (frameskip == FRAMESKIP_LEVELS-1)
      {
        frameskip = 0;
        autoframeskip = 1;
      }
      else
        frameskip++;
    }

    if (showfps == 0)
      showfpstemp = 2*video_fps;
  }

  if (input_ui_pressed(IPT_UI_FRAMESKIP_DEC))
  {
    if (autoframeskip)
    {
      autoframeskip = 0;
      frameskip = FRAMESKIP_LEVELS-1;
    }
    else
    {
      if (frameskip == 0)
        autoframeskip = 1;
      else
        frameskip--;
    }

    if (showfps == 0)
      showfpstemp = 2*video_fps;
  }

  if (input_ui_pressed(IPT_UI_THROTTLE))
  {
    throttle ^= 1;
  }
#endif

  frameskip_counter = (frameskip_counter + 1) % FRAMESKIP_LEVELS;
}



void osd_set_gamma(float _gamma)
{
  int i;

  osd_gamma_correction = _gamma;

  for (i = 0; i < screen_colors; i++)
    dirtycolor[i] = 1;
  dirtypalette = 1;
  dirty_bright = 1;
}

float osd_get_gamma(void)
{
  return osd_gamma_correction;
}

/* brightess = percentage 0-100% */
void osd_set_brightness(int _brightness)
{
  int i;

  brightness = _brightness;

  for (i = 0; i < screen_colors; i++)
    dirtycolor[i] = 1;
  dirtypalette = 1;
  dirty_bright = 1;
}

int osd_get_brightness(void)
{
  return brightness;
}


void osd_save_snapshot(struct osd_bitmap *bitmap)
{
  save_screen_snapshot(bitmap);
}

void osd_pause(int paused)
{
  int i;

  if (paused) brightness_paused_adjust = 0.65;
  else brightness_paused_adjust = 1.0;

  for (i = 0; i < screen_colors; i++)
    dirtycolor[i] = 1;
  dirtypalette = 1;
  dirty_bright = 1;
}

