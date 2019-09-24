#include "../machine/mappy.cpp"
#include "../vidhrdw/mappy.cpp"

/***************************************************************************

Mappy memory map (preliminary)

driver by Aaron Giles


CPU #1:
0000-07ff video RAM
0800-0fff color RAM
1000-177f RAM
1780-17ff sprite data 1 (sprite number & color)
1800-1f7f RAM
1f80-1fff sprite data 2 (x, y position)
2000-277f RAM
2780-27ff sprite data 3 (high bit of y, flip flags, double-size flags)
3800-3fff scroll register map
4040-43ff RAM shared with CPU #2
4800-480f custom I/O chip #1
4810-481f custom I/O chip #2
5002-5003 IRQ enable
5004-5005 cocktail flipscreen
5008      CPU #2 reset & disable I/O chips
5009      enable I/O chips
500a-500b CPU #2 enable
8000      watchdog timer
a000-ffff ROM

CPU #2:
0000-0040 sound registers
0040-03ff RAM shared with CPU #1
2000-2001 IRQ enable
2006-2007 sound enable
e000-ffff ROM

Interrupts:
CPU #1 IRQ generated by VBLANK
CPU #2 IRQ generated by VBLANK

Changes:
Aug 1999   Proper cocktail emulation implemented by Chad Hendrickson

***************************************************************************/

#include "driver.h"
#include "vidhrdw/generic.h"



/* machine driver data & functions */
extern unsigned char *mappy_sharedram;
extern unsigned char *mappy_customio_1,*mappy_customio_2;
void mappy_init_machine(void);
void motos_init_machine(void);
READ_HANDLER( mappy_sharedram_r );
WRITE_HANDLER( mappy_sharedram_w );
WRITE_HANDLER( mappy_customio_1_w );
WRITE_HANDLER( mappy_customio_2_w );
int mappy_interrupt_1(void);
int mappy_interrupt_2(void);
WRITE_HANDLER( mappy_interrupt_enable_1_w );
WRITE_HANDLER( mappy_interrupt_enable_2_w );
WRITE_HANDLER( mappy_cpu_enable_w );
WRITE_HANDLER( mappy_reset_2_w );
WRITE_HANDLER( mappy_io_chips_enable_w );
WRITE_HANDLER( mappy_flipscreen_w );

READ_HANDLER( mappy_customio_1_r );
READ_HANDLER( mappy_customio_2_r );

READ_HANDLER( digdug2_customio_1_r );
READ_HANDLER( digdug2_customio_2_r );

READ_HANDLER( motos_customio_1_r );
READ_HANDLER( motos_customio_2_r );

READ_HANDLER( todruaga_customio_1_r );
READ_HANDLER( todruaga_customio_2_r );

/* video driver data & functions */
int mappy_vh_start(void);
int motos_vh_start(void);
int todruaga_vh_start(void);
void mappy_vh_stop(void);
void mappy_vh_screenrefresh(struct osd_bitmap *bitmap,int full_refresh);
WRITE_HANDLER( mappy_videoram_w );
WRITE_HANDLER( mappy_colorram_w );
WRITE_HANDLER( mappy_scroll_w );
void mappy_vh_convert_color_prom(unsigned char *palette, unsigned short *colortable,const unsigned char *color_prom);

/* sound driver data & functions */
extern unsigned char *mappy_soundregs;
WRITE_HANDLER( mappy_sound_w );
WRITE_HANDLER( mappy_sound_enable_w );


/* CPU 1 read addresses */
static struct MemoryReadAddress mappy_readmem_cpu1[] =
{
  { 0x4040, 0x43ff, MRA_RAM },			/* shared RAM with the sound CPU */
  { 0x4800, 0x480f, mappy_customio_1_r },	/* custom I/O chip #1 interface */
  { 0x4810, 0x481f, mappy_customio_2_r },	/* custom I/O chip #2 interface */
  { 0x0000, 0x9fff, MRA_RAM },			/* RAM everywhere else */
  { 0xa000, 0xffff, MRA_ROM },			/* ROM code */
  { -1 }  /* end of table */
};

static struct MemoryReadAddress digdug2_readmem_cpu1[] =
{
  { 0x4040, 0x43ff, MRA_RAM },				/* shared RAM with the sound CPU */
  { 0x4800, 0x480f, digdug2_customio_1_r },	/* custom I/O chip #1 interface */
  { 0x4810, 0x481f, digdug2_customio_2_r },	/* custom I/O chip #2 interface */
  { 0x4820, 0x4bff, MRA_RAM },				/* extra RAM for Dig Dug 2 */
  { 0x0000, 0x7fff, MRA_RAM },				/* RAM everywhere else */
  { 0x8000, 0xffff, MRA_ROM },				/* ROM code */
  { -1 }  /* end of table */
};

static struct MemoryReadAddress motos_readmem_cpu1[] =
{
  { 0x4040, 0x43ff, MRA_RAM },			/* shared RAM with the sound CPU */
  { 0x4800, 0x480f, motos_customio_1_r },	/* custom I/O chip #1 interface */
  { 0x4810, 0x481f, motos_customio_2_r },	/* custom I/O chip #2 interface */
  { 0x0000, 0x7fff, MRA_RAM },			/* RAM everywhere else */
  { 0x8000, 0xffff, MRA_ROM },			/* ROM code */
  { -1 }  /* end of table */
};

static struct MemoryReadAddress todruaga_readmem_cpu1[] =
{
  { 0x4040, 0x43ff, MRA_RAM },				/* shared RAM with the sound CPU */
  { 0x4800, 0x480f, todruaga_customio_1_r },	/* custom I/O chip #1 interface */
  { 0x4810, 0x481f, todruaga_customio_2_r },	/* custom I/O chip #2 interface */
  { 0x0000, 0x7fff, MRA_RAM },				/* RAM everywhere else */
  { 0x8000, 0xffff, MRA_ROM },				/* ROM code */
  { -1 }  /* end of table */
};


/* CPU 1 write addresses */
static struct MemoryWriteAddress writemem_cpu1[] =
{
  { 0x1000, 0x177f, MWA_RAM },                                 /* general RAM, area 1 */
  { 0x1800, 0x1f7f, MWA_RAM },                                 /* general RAM, area 2 */
  { 0x2000, 0x277f, MWA_RAM },                                 /* general RAM, area 3 */
  { 0x4040, 0x43ff, MWA_RAM, &mappy_sharedram },               /* shared RAM with the sound CPU */
  { 0x4820, 0x4bff, MWA_RAM },                                 /* extra RAM for Dig Dug 2 */
  { 0x0000, 0x07ff, mappy_videoram_w, &videoram, &videoram_size },/* video RAM */
  { 0x0800, 0x0fff, mappy_colorram_w, &colorram },             /* color RAM */
  { 0x1780, 0x17ff, MWA_RAM, &spriteram, &spriteram_size },    /* sprite RAM, area 1 */
  { 0x1f80, 0x1fff, MWA_RAM, &spriteram_2 },                   /* sprite RAM, area 2 */
  { 0x2780, 0x27ff, MWA_RAM, &spriteram_3 },                   /* sprite RAM, area 3 */
  { 0x3800, 0x3fff, mappy_scroll_w },                          /* scroll registers */
  { 0x4800, 0x480f, mappy_customio_1_w, &mappy_customio_1 },   /* custom I/O chip #1 interface */
  { 0x4810, 0x481f, mappy_customio_2_w, &mappy_customio_2 },   /* custom I/O chip #2 interface */
  { 0x5002, 0x5003, mappy_interrupt_enable_1_w },              /* interrupt enable */
  { 0x5004, 0x5005, mappy_flipscreen_w },				 /* cocktail flipscreen */
  { 0x5008, 0x5008, mappy_reset_2_w },			       /* reset CPU #2 & disable I/O chips */
  { 0x5009, 0x5009, mappy_io_chips_enable_w },		       /* enable I/O chips */
  { 0x500a, 0x500b, mappy_cpu_enable_w },                      /* sound CPU enable */
  { 0x8000, 0x8000, MWA_NOP },                                 /* watchdog timer */
  { 0x8000, 0xffff, MWA_ROM },                                 /* ROM code */

  { -1 }  /* end of table */
};


/* CPU 2 read addresses */
static struct MemoryReadAddress mappy_readmem_cpu2[] =
{
  { 0xe000, 0xffff, MRA_ROM },                                 /* ROM code */
  { 0x0040, 0x03ff, mappy_sharedram_r },                      /* shared RAM with the main CPU */

  { -1 }  /* end of table */
};

static struct MemoryReadAddress digdug2_readmem_cpu2[] =
{
  { 0xe000, 0xffff, MRA_ROM },                                 /* ROM code */
  { 0x0040, 0x03ff, mappy_sharedram_r },                    /* shared RAM with the main CPU */

  { -1 }  /* end of table */
};

static struct MemoryReadAddress motos_readmem_cpu2[] =
{
  { 0xe000, 0xffff, MRA_ROM },                                 /* ROM code */
  { 0x0040, 0x03ff, mappy_sharedram_r },						 /* shared RAM with the main CPU */

  { -1 }  /* end of table */
};

static struct MemoryReadAddress todruaga_readmem_cpu2[] =
{
  { 0xe000, 0xffff, MRA_ROM },                                 /* ROM code */
  { 0x0040, 0x03ff, mappy_sharedram_r },						 /* shared RAM with the main CPU */

  { -1 }  /* end of table */
};


/* CPU 2 write addresses */
static struct MemoryWriteAddress writemem_cpu2[] =
{
  { 0x0040, 0x03ff, mappy_sharedram_w },                       /* shared RAM with the main CPU */
  { 0x0000, 0x003f, mappy_sound_w, &mappy_soundregs },         /* sound control registers */
  { 0x2000, 0x2001, mappy_interrupt_enable_2_w },              /* interrupt enable */
  { 0x2006, 0x2007, mappy_sound_enable_w },                    /* sound enable */
  { 0xe000, 0xffff, MWA_ROM },                                 /* ROM code */

  { -1 }  /* end of table */
};


/* input from the outside world */
INPUT_PORTS_START( mappy )
PORT_START      /* DSW0 */
/* According to the manual, 0x04, 0x08 and 0x10 should always be off,
but... */
PORT_DIPNAME( 0x07, 0x00, "Rank" )
PORT_DIPSETTING(    0x00, "A" )
PORT_DIPSETTING(    0x01, "B" )
PORT_DIPSETTING(    0x02, "C" )
PORT_DIPSETTING(    0x03, "D" )
PORT_DIPSETTING(    0x04, "E" )
PORT_DIPSETTING(    0x05, "F" )
PORT_DIPSETTING(    0x06, "G" )
PORT_DIPSETTING(    0x07, "H" )
PORT_DIPNAME( 0x18, 0x00, DEF_STR( Coin_B ) )
PORT_DIPSETTING(    0x18, DEF_STR( 2C_1C ) )
PORT_DIPSETTING(    0x00, DEF_STR( 1C_1C ) )
PORT_DIPSETTING(    0x08, DEF_STR( 1C_5C) )
PORT_DIPSETTING(    0x10, DEF_STR( 1C_7C ) )
PORT_DIPNAME( 0x20, 0x00, DEF_STR( Demo_Sounds ) )
PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
PORT_DIPSETTING(    0x00, DEF_STR( On ) )
PORT_BITX(    0x40, 0x00, IPT_DIPSWITCH_NAME | IPF_CHEAT, "Rack Test", KEYCODE_F1, IP_JOY_NONE )
PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
PORT_DIPSETTING(    0x40, DEF_STR( On ) )
PORT_DIPNAME( 0x80, 0x00, "Freeze" )
PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
PORT_DIPSETTING(    0x80, DEF_STR( On ) )

PORT_START      /* DSW1 */
PORT_DIPNAME( 0x07, 0x00, DEF_STR( Coin_A ) )
PORT_DIPSETTING(    0x06, DEF_STR( 3C_1C ) )
PORT_DIPSETTING(    0x04, DEF_STR( 2C_1C ) )
PORT_DIPSETTING(    0x07, DEF_STR( 3C_2C ) )
PORT_DIPSETTING(    0x00, DEF_STR( 1C_1C ) )
PORT_DIPSETTING(    0x05, DEF_STR( 2C_3C ) )
PORT_DIPSETTING(    0x01, DEF_STR( 1C_2C ) )
PORT_DIPSETTING(    0x02, DEF_STR( 1C_3C ) )
PORT_DIPSETTING(    0x03, DEF_STR( 1C_6C ) )
/* TODO: bonus scores are different for 5 lives */
PORT_DIPNAME( 0x38, 0x00, DEF_STR( Bonus_Life ) )
PORT_DIPSETTING(    0x28, "20k 70k and every 70k" )
PORT_DIPSETTING(    0x30, "20k 80k and every 80k" )
PORT_DIPSETTING(    0x08, "20k 60k" )
PORT_DIPSETTING(    0x00, "20k 70k" )
PORT_DIPSETTING(    0x10, "20k 80k" )
PORT_DIPSETTING(    0x18, "30k 100k" )
PORT_DIPSETTING(    0x20, "20k" )
PORT_DIPSETTING(    0x38, "None" )
/* those are the bonus with 5 lives
	PORT_DIPNAME( 0x38, 0x00, DEF_STR( Bonus_Life ) )
	PORT_DIPSETTING(    0x28, "30k 100k and every 100k" )
	PORT_DIPSETTING(    0x30, "40k 120k and every 120k" )
	PORT_DIPSETTING(    0x00, "30k 80k" )
	PORT_DIPSETTING(    0x08, "30k 100k" )
	PORT_DIPSETTING(    0x10, "30k 120k" )
	PORT_DIPSETTING(    0x18, "30k" )
	PORT_DIPSETTING(    0x20, "40k" )
	PORT_DIPSETTING(    0x38, "None" ) */
PORT_DIPNAME( 0xc0, 0x00, DEF_STR( Lives ) )
PORT_DIPSETTING(    0x80, "1" )
PORT_DIPSETTING(    0xc0, "2" )
PORT_DIPSETTING(    0x00, "3" )
PORT_DIPSETTING(    0x40, "5" )

PORT_START      /* DSW2 */
PORT_BIT( 0x03, IP_ACTIVE_HIGH, IPT_UNUSED )
PORT_DIPNAME( 0x04, 0x00, DEF_STR( Cabinet ) )
PORT_DIPSETTING(    0x00, DEF_STR( Upright ) )
PORT_DIPSETTING(    0x04, DEF_STR( Cocktail ) )
PORT_SERVICE( 0x08, IP_ACTIVE_HIGH )
PORT_BIT( 0xf0, IP_ACTIVE_HIGH, IPT_UNUSED )

PORT_START      /* FAKE */
/* The player inputs are not memory mapped, they are handled by an I/O chip. */
/* These fake input ports are read by mappy_customio_data_r() */
PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_UNUSED )
PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT | IPF_2WAY )
PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_UNUSED )
PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT | IPF_2WAY )
PORT_BIT_IMPULSE( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON1, 1 )
PORT_BITX(0x20, IP_ACTIVE_HIGH, IPT_BUTTON1, 0, IP_KEY_PREVIOUS, IP_JOY_PREVIOUS )
PORT_BIT( 0xc0, IP_ACTIVE_HIGH, IPT_UNUSED )

PORT_START      /* FAKE */
PORT_BIT_IMPULSE( 0x01, IP_ACTIVE_HIGH, IPT_COIN1, 1 )
/* Coin 2 is not working */
PORT_BIT_IMPULSE( 0x02, IP_ACTIVE_HIGH, IPT_COIN2, 1 )
PORT_BIT( 0x0c, IP_ACTIVE_HIGH, IPT_UNUSED )
PORT_BIT_IMPULSE( 0x10, IP_ACTIVE_HIGH, IPT_START1, 1 )
PORT_BIT_IMPULSE( 0x20, IP_ACTIVE_HIGH, IPT_START2, 1 )
PORT_BIT( 0xc0, IP_ACTIVE_HIGH, IPT_UNUSED )

PORT_START	    /* FAKE */
PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT | IPF_2WAY | IPF_COCKTAIL )
PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT | IPF_2WAY | IPF_COCKTAIL )
PORT_BIT_IMPULSE( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON1 | IPF_COCKTAIL, 1 )
PORT_BITX(0x20, IP_ACTIVE_HIGH, IPT_BUTTON1 | IPF_COCKTAIL, 0, IP_KEY_PREVIOUS, IP_JOY_PREVIOUS )

INPUT_PORTS_END


INPUT_PORTS_START( digdug2 )
PORT_START      /* DSW0 */
PORT_DIPNAME( 0x01, 0x00, DEF_STR( Unknown ) )
PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
PORT_DIPSETTING(    0x01, DEF_STR( On ) )
PORT_DIPNAME( 0x02, 0x00, DEF_STR( Unknown ) )
PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
PORT_DIPSETTING(    0x02, DEF_STR( On ) )
PORT_DIPNAME( 0x04, 0x00, DEF_STR( Unknown ) )
PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
PORT_DIPSETTING(    0x04, DEF_STR( On ) )
PORT_DIPNAME( 0x08, 0x00, DEF_STR( Unknown ) )
PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
PORT_DIPSETTING(    0x08, DEF_STR( On ) )
PORT_DIPNAME( 0x10, 0x00, "Reset" )
PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
PORT_DIPSETTING(    0x10, DEF_STR( On ) )
PORT_DIPNAME( 0x20, 0x00, DEF_STR( Lives ) )
PORT_DIPSETTING(    0x00, "3" )
PORT_DIPSETTING(    0x20, "5" )
PORT_DIPNAME( 0xc0, 0x00, DEF_STR( Coinage ) )
PORT_DIPSETTING(    0x40, DEF_STR( 2C_1C ) )
PORT_DIPSETTING(    0xc0, "2 Coins/1 Credit 3C/2C" )
PORT_DIPSETTING(    0x00, DEF_STR( 1C_1C ) )
PORT_DIPSETTING(    0x80, DEF_STR( 1C_2C ) )

PORT_START      /* DSW1 */
PORT_DIPNAME( 0x03, 0x00, DEF_STR( Bonus_Life ) )
PORT_DIPSETTING(    0x00, "30k 80k and ..." )
PORT_DIPSETTING(    0x01, "30k 100k and ..." )
PORT_DIPSETTING(    0x02, "30k 120k and ..." )
PORT_DIPSETTING(    0x03, "30k 150k and..." )
PORT_DIPNAME( 0x04, 0x00, "Level Select" )
PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
PORT_DIPSETTING(    0x04, DEF_STR( On ) )
PORT_DIPNAME( 0x08, 0x00, "Freeze?" )
PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
PORT_DIPSETTING(    0x08, DEF_STR( On ) )
PORT_BIT_IMPULSE( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON2, 1 )
PORT_BITX(0x20, IP_ACTIVE_HIGH, IPT_BUTTON2, 0, IP_KEY_PREVIOUS, IP_JOY_PREVIOUS )
PORT_DIPNAME( 0x40, 0x00, DEF_STR( Unknown ) )
PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
PORT_DIPSETTING(    0x40, DEF_STR( On ) )
//	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNUSED )
PORT_DIPNAME( 0x80, 0x00, DEF_STR( Cabinet ) )
PORT_DIPSETTING(    0x00, DEF_STR( Upright ) )
PORT_DIPSETTING(    0x80, DEF_STR( Cocktail ) )

PORT_START      /* DSW2 */
PORT_BIT( 0x07, IP_ACTIVE_HIGH, IPT_UNUSED )
PORT_SERVICE( 0x08, IP_ACTIVE_HIGH )
PORT_BIT( 0xf0, IP_ACTIVE_HIGH, IPT_UNUSED )

PORT_START      /* FAKE */
/* The player inputs are not memory mapped, they are handled by an I/O chip. */
/* These fake input ports are read by mappy_customio_data_r() */
PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP    | IPF_4WAY )
PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT | IPF_4WAY )
PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN  | IPF_4WAY )
PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT  | IPF_4WAY )
PORT_BIT_IMPULSE( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON1, 1 )
PORT_BITX(0x20, IP_ACTIVE_HIGH, IPT_BUTTON1, 0, IP_KEY_PREVIOUS, IP_JOY_PREVIOUS )
PORT_BIT( 0xc0, IP_ACTIVE_HIGH, IPT_UNUSED )

PORT_START      /* FAKE */
PORT_BIT_IMPULSE( 0x01, IP_ACTIVE_HIGH, IPT_COIN1, 1 )
/* Coin 2 is not working */
PORT_BIT_IMPULSE( 0x02, IP_ACTIVE_HIGH, IPT_COIN2, 1 )
PORT_BIT( 0x0c, IP_ACTIVE_HIGH, IPT_UNUSED )
PORT_BIT_IMPULSE( 0x10, IP_ACTIVE_HIGH, IPT_START1, 1 )
PORT_BIT_IMPULSE( 0x20, IP_ACTIVE_HIGH, IPT_START2, 1 )
PORT_BIT( 0xc0, IP_ACTIVE_HIGH, IPT_UNUSED )

PORT_START      /* FAKE */
/* The player inputs are not memory mapped, they are handled by an I/O chip. */
/* These fake input ports are read by mappy_customio_data_r() */
PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP    | IPF_4WAY | IPF_COCKTAIL )
PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT | IPF_4WAY | IPF_COCKTAIL )
PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN  | IPF_4WAY | IPF_COCKTAIL )
PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT  | IPF_4WAY | IPF_COCKTAIL )
PORT_BIT_IMPULSE( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON1 | IPF_COCKTAIL, 1 )
PORT_BITX(0x20, IP_ACTIVE_HIGH, IPT_BUTTON1 | IPF_COCKTAIL, 0, IP_KEY_PREVIOUS, IP_JOY_PREVIOUS )
PORT_BIT( 0xc0, IP_ACTIVE_HIGH, IPT_UNUSED )

INPUT_PORTS_END


INPUT_PORTS_START( motos )
PORT_START      /* DSW0 */
PORT_DIPNAME( 0x01, 0x00, "Reset" )
PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
PORT_DIPSETTING(    0x01, DEF_STR( On ) )
PORT_DIPNAME( 0x06, 0x00, DEF_STR( Coinage ) )
PORT_DIPSETTING(    0x06, DEF_STR( 3C_1C ) )
PORT_DIPSETTING(    0x04, DEF_STR( 2C_1C ) )
PORT_DIPSETTING(    0x00, DEF_STR( 1C_1C ) )
PORT_DIPSETTING(    0x02, DEF_STR( 1C_2C ) )
PORT_DIPNAME( 0x08, 0x00, DEF_STR( Lives ) )
PORT_DIPSETTING(    0x00, "3" )
PORT_DIPSETTING(    0x08, "5" )
PORT_DIPNAME( 0x10, 0x00, "Rank" )
PORT_DIPSETTING(    0x00, "A" )
PORT_DIPSETTING(    0x10, "B" )
PORT_DIPNAME( 0x60, 0x00, DEF_STR( Bonus_Life ) )
PORT_DIPSETTING(    0x00, "10k 30k and every 50k" )
PORT_DIPSETTING(    0x20, "20k and every 50k" )
PORT_DIPSETTING(    0x40, "30k and every 70k" )
PORT_DIPSETTING(    0x60, "20k 70k" )
PORT_DIPNAME( 0x80, 0x00, DEF_STR( Demo_Sounds ) )
PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
PORT_DIPSETTING(    0x00, DEF_STR( On ) )

PORT_START      /* DSW1 */
PORT_BIT( 0x3f, IP_ACTIVE_HIGH, IPT_UNUSED )
PORT_DIPNAME( 0x40, 0x00, DEF_STR( Cabinet ) )
PORT_DIPSETTING(    0x00, DEF_STR( Upright ) )
PORT_DIPSETTING(    0x40, DEF_STR( Cocktail ) )
PORT_SERVICE( 0x80, IP_ACTIVE_HIGH )

PORT_START      /* FAKE */
/* The player inputs are not memory mapped, they are handled by an I/O chip. */
/* These fake input ports are read by mappy_customio_data_r() */
PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP    | IPF_8WAY )
PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT | IPF_8WAY )
PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN  | IPF_8WAY )
PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT  | IPF_8WAY )
PORT_BIT_IMPULSE( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON1, 2 )
PORT_BITX(0x20, IP_ACTIVE_HIGH, IPT_BUTTON1, 0, IP_KEY_PREVIOUS, IP_JOY_PREVIOUS )
PORT_BIT( 0xc0, IP_ACTIVE_HIGH, IPT_UNUSED )

PORT_START      /* FAKE */
PORT_BIT_IMPULSE( 0x01, IP_ACTIVE_HIGH, IPT_COIN1, 2 )
PORT_BIT_IMPULSE( 0x02, IP_ACTIVE_HIGH, IPT_COIN2, 2 )
PORT_BIT( 0x0c, IP_ACTIVE_HIGH, IPT_UNUSED )
PORT_BIT_IMPULSE( 0x40, IP_ACTIVE_HIGH, IPT_START1, 2 )
PORT_BIT_IMPULSE( 0x80, IP_ACTIVE_HIGH, IPT_START2, 2 )
PORT_BIT( 0x30, IP_ACTIVE_HIGH, IPT_UNUSED )

PORT_START      /* FAKE */
/* The player inputs are not memory mapped, they are handled by an I/O chip. */
/* These fake input ports are read by mappy_customio_data_r() */
PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP    | IPF_8WAY | IPF_COCKTAIL )
PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT | IPF_8WAY | IPF_COCKTAIL )
PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN  | IPF_8WAY | IPF_COCKTAIL )
PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT  | IPF_8WAY | IPF_COCKTAIL )
PORT_BIT_IMPULSE( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON1 | IPF_COCKTAIL, 2 )
PORT_BITX(0x20, IP_ACTIVE_HIGH, IPT_BUTTON1 | IPF_COCKTAIL, 0, IP_KEY_PREVIOUS, IP_JOY_PREVIOUS )
PORT_BIT( 0xc0, IP_ACTIVE_HIGH, IPT_UNUSED )


INPUT_PORTS_END


INPUT_PORTS_START( todruaga )
PORT_START      /* DSW0 */
PORT_BIT( 0x0f, IP_ACTIVE_HIGH, IPT_UNUSED )
PORT_DIPNAME( 0x30, 0x00, DEF_STR( Lives ) )
PORT_DIPSETTING(    0x20, "1" )
PORT_DIPSETTING(    0x10, "2" )
PORT_DIPSETTING(    0x00, "3" )
PORT_DIPSETTING(    0x30, "5" )
PORT_DIPNAME( 0xc0, 0x00, DEF_STR( Coin_A ) )
PORT_DIPSETTING(    0xc0, DEF_STR( 3C_1C ) )
PORT_DIPSETTING(    0x40, DEF_STR( 2C_1C ) )
PORT_DIPSETTING(    0x00, DEF_STR( 1C_1C ) )
PORT_DIPSETTING(    0x80, DEF_STR( 1C_2C ) )

PORT_START      /* DSW1 */
PORT_DIPNAME( 0x01, 0x00, "Freeze" )
PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
PORT_DIPSETTING(    0x01, DEF_STR( On ) )
PORT_DIPNAME( 0x02, 0x00, "Reset" )
PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
PORT_DIPSETTING(    0x02, DEF_STR( On ) )
PORT_DIPNAME( 0x0c, 0x00, DEF_STR( Coin_B ) )
PORT_DIPSETTING(    0x0c, DEF_STR( 3C_1C ) )
PORT_DIPSETTING(    0x04, DEF_STR( 2C_1C ) )
PORT_DIPSETTING(    0x00, DEF_STR( 1C_1C ) )
PORT_DIPSETTING(    0x08, DEF_STR( 1C_2C ) )
PORT_BIT( 0x70, IP_ACTIVE_HIGH, IPT_UNUSED )
PORT_DIPNAME( 0x80, 0x00, DEF_STR( Cabinet ) )
PORT_DIPSETTING(    0x00, DEF_STR( Upright ) )
PORT_DIPSETTING(    0x80, DEF_STR( Cocktail ) )

PORT_START      /* DSW2 */
PORT_BIT( 0x07, IP_ACTIVE_HIGH, IPT_UNUSED )
PORT_SERVICE( 0x08, IP_ACTIVE_HIGH )
PORT_BIT( 0xf0, IP_ACTIVE_HIGH, IPT_UNUSED )

PORT_START      /* FAKE */
/* The player inputs are not memory mapped, they are handled by an I/O chip. */
/* These fake input ports are read by mappy_customio_data_r() */
PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP    | IPF_4WAY )
PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT | IPF_4WAY )
PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN  | IPF_4WAY )
PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT  | IPF_4WAY )
PORT_BIT_IMPULSE( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON1, 1 )
PORT_BITX(0x20, IP_ACTIVE_HIGH, IPT_BUTTON1, 0, IP_KEY_PREVIOUS, IP_JOY_PREVIOUS )
PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNUSED )
PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_START1 )	/* used on level 31 */

PORT_START      /* FAKE */
PORT_BIT( 0x0f, IP_ACTIVE_HIGH, IPT_UNUSED )
/* this is just a guess */
PORT_BIT_IMPULSE( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON2, 1 )
PORT_BITX(0x20, IP_ACTIVE_HIGH, IPT_BUTTON2, 0, IP_KEY_PREVIOUS, IP_JOY_PREVIOUS )
PORT_BIT( 0xc0, IP_ACTIVE_HIGH, IPT_UNUSED )

PORT_START      /* FAKE */
PORT_BIT_IMPULSE( 0x01, IP_ACTIVE_HIGH, IPT_COIN1, 1 )
/* Coin 2 is not working */
PORT_BIT_IMPULSE( 0x02, IP_ACTIVE_HIGH, IPT_COIN2, 1 )
PORT_BIT( 0x0c, IP_ACTIVE_HIGH, IPT_UNUSED )
PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNUSED )	/* we take it from port 3 */
//	PORT_BIT_IMPULSE( 0x10, IP_ACTIVE_HIGH, IPT_START1, 1 )
PORT_BIT_IMPULSE( 0x20, IP_ACTIVE_HIGH, IPT_START2, 1 )
PORT_BIT( 0xc0, IP_ACTIVE_HIGH, IPT_UNUSED )

PORT_START      /* FAKE */
/* The player inputs are not memory mapped, they are handled by an I/O chip. */
/* These fake input ports are read by mappy_customio_data_r() */
PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP    | IPF_4WAY | IPF_COCKTAIL )
PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT | IPF_4WAY | IPF_COCKTAIL )
PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN  | IPF_4WAY | IPF_COCKTAIL )
PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT  | IPF_4WAY | IPF_COCKTAIL )
PORT_BIT_IMPULSE( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON1 | IPF_COCKTAIL, 1 )
PORT_BITX(0x20, IP_ACTIVE_HIGH, IPT_BUTTON1 | IPF_COCKTAIL, 0, IP_KEY_PREVIOUS, IP_JOY_PREVIOUS )
PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNUSED )


INPUT_PORTS_END


/* layout of the 8x8x2 character data */
static struct GfxLayout charlayout =
{
  8,8,             /* 8*8 characters */
  256,             /* 256 characters */
  2,             /* 2 bits per pixel */
  { 0, 4 },      /* the two bitplanes for 4 pixels are packed into one byte */
  { 8*8+0, 8*8+1, 8*8+2, 8*8+3, 0, 1, 2, 3 },   /* bits are packed in groups of four */
  { 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8 },   /* characters are rotated 90 degrees */
  16*8           /* every char takes 16 bytes */
};


/* layout of the 16x16x4 sprite data */
static struct GfxLayout mappy_spritelayout =
{
  16,16,       /* 16*16 sprites */
  128,            /* 128 sprites */
  4,                 /* 4 bits per pixel */
  { 0, 4, 8192*8, 8192*8+4 },     /* the two bitplanes for 4 pixels are packed into one byte */
  {
    0, 1, 2, 3, 8*8, 8*8+1, 8*8+2, 8*8+3, 16*8+0, 16*8+1, 16*8+2, 16*8+3,
    24*8+0, 24*8+1, 24*8+2, 24*8+3
  },
  {
    0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8,
    32*8, 33*8, 34*8, 35*8, 36*8, 37*8, 38*8, 39*8
  },
  64*8    /* every sprite takes 64 bytes */
};


static struct GfxLayout digdug2_spritelayout =
{
  16,16,       /* 16*16 sprites */
  256,            /* 256 sprites */
  4,                 /* 4 bits per pixel */
  { 0, 4, 16384*8, 16384*8+4 },   /* the two bitplanes for 4 pixels are packed into one byte */
  {
    0, 1, 2, 3, 8*8, 8*8+1, 8*8+2, 8*8+3, 16*8+0, 16*8+1, 16*8+2, 16*8+3,
    24*8+0, 24*8+1, 24*8+2, 24*8+3
  },
  {
    0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8,
    32*8, 33*8, 34*8, 35*8, 36*8, 37*8, 38*8, 39*8
  },
  64*8    /* every sprite takes 64 bytes */
};


/* pointers to the appropriate memory locations and their associated decode structs */
static struct GfxDecodeInfo mappy_gfxdecodeinfo[] =
{
  { REGION_GFX1, 0, &charlayout,            0, 64 },
  { REGION_GFX2, 0, &mappy_spritelayout, 64*4, 16 },
  { -1 } /* end of array */
};

static struct GfxDecodeInfo digdug2_gfxdecodeinfo[] =
{
  { REGION_GFX1, 0, &charlayout,              0, 64 },
  { REGION_GFX2, 0, &digdug2_spritelayout, 64*4, 16 },
  { -1 } /* end of array */
};

static struct GfxDecodeInfo todruaga_gfxdecodeinfo[] =
{
  { REGION_GFX1, 0, &charlayout,            0, 64 },
  { REGION_GFX2, 0, &mappy_spritelayout, 64*4, 64 },
  { -1 } /* end of array */
};

static struct namco_interface namco_interface =
{
  23920,	/* sample rate (approximate value) */
  8,		/* number of voices */
  100,	/* playback volume */
  REGION_SOUND1	/* memory region */
};



/* the machine driver: 2 6809s running at 1MHz */
static struct MachineDriver machine_driver_mappy =
{
  /* basic machine hardware */
  {
    {
      CPU_M6809,
      1100000,                        /* 1.1 Mhz */
      mappy_readmem_cpu1,writemem_cpu1,0,0,
      mappy_interrupt_1,1
    },
    {
      CPU_M6809,
      1100000,                        /* 1.1 Mhz */
      mappy_readmem_cpu2,writemem_cpu2,0,0,
      mappy_interrupt_2,1
    }
  },
  60, DEFAULT_60HZ_VBLANK_DURATION,	/* frames per second, vblank duration */
  100,    /* 100 CPU slices per frame - an high value to ensure proper */
  /* synchronization of the CPUs */
  mappy_init_machine,

  /* video hardware */
  36*8, 28*8, { 0*8, 36*8-1, 0*8, 28*8-1 },
  mappy_gfxdecodeinfo,
  32,64*4+16*16,
  mappy_vh_convert_color_prom,

  VIDEO_TYPE_RASTER,
  0,
  mappy_vh_start,
  mappy_vh_stop,
  mappy_vh_screenrefresh,

  /* sound hardware */
  0,0,0,0,
  {
    {
      SOUND_NAMCO,
      &namco_interface
    }
  }
};

static struct MachineDriver machine_driver_digdug2 =
{
  /* basic machine hardware */
  {
    {
      CPU_M6809,
      1600000,                        /* 1.6 Mhz */
      digdug2_readmem_cpu1,writemem_cpu1,0,0,
      mappy_interrupt_1,1
    },
    {
      CPU_M6809,
      1600000,                        /* 1.6 Mhz */
      digdug2_readmem_cpu2,writemem_cpu2,0,0,
      mappy_interrupt_2,1
    }
  },
  60, DEFAULT_60HZ_VBLANK_DURATION,	/* frames per second, vblank duration */
  100,    /* 100 CPU slices per frame - an high value to ensure proper */
  /* synchronization of the CPUs */
  mappy_init_machine,

  /* video hardware */
  36*8, 28*8, { 0*8, 36*8-1, 0*8, 28*8-1 },
  digdug2_gfxdecodeinfo,
  32,64*4+16*16,
  mappy_vh_convert_color_prom,

  VIDEO_TYPE_RASTER,
  0,
  mappy_vh_start,
  mappy_vh_stop,
  mappy_vh_screenrefresh,

  /* sound hardware */
  0,0,0,0,
  {
    {
      SOUND_NAMCO,
      &namco_interface
    }
  }
};

static struct MachineDriver machine_driver_motos =
{
  /* basic machine hardware */
  {
    {
      CPU_M6809,
      1600000,                        /* 1.6 Mhz */
      motos_readmem_cpu1,writemem_cpu1,0,0,
      mappy_interrupt_1,1
    },
    {
      CPU_M6809,
      1600000,                        /* 1.6 Mhz */
      motos_readmem_cpu2,writemem_cpu2,0,0,
      mappy_interrupt_2,1
    }
  },
  60, DEFAULT_60HZ_VBLANK_DURATION,	/* frames per second, vblank duration */
  100,    /* 100 CPU slices per frame - an high value to ensure proper */
  /* synchronization of the CPUs */
  motos_init_machine,

  /* video hardware */
  36*8, 28*8, { 0*8, 36*8-1, 0*8, 28*8-1 },
  digdug2_gfxdecodeinfo,
  32,64*4+16*16,
  mappy_vh_convert_color_prom,

  VIDEO_TYPE_RASTER,
  0,
  motos_vh_start,
  mappy_vh_stop,
  mappy_vh_screenrefresh,

  /* sound hardware */
  0,0,0,0,
  {
    {
      SOUND_NAMCO,
      &namco_interface
    }
  }
};

static struct MachineDriver machine_driver_todruaga =
{
  /* basic machine hardware */
  {
    {
      CPU_M6809,
      1600000,                        /* 1.6 Mhz */
      todruaga_readmem_cpu1,writemem_cpu1,0,0,
      mappy_interrupt_1,1
    },
    {
      CPU_M6809,
      1600000,                        /* 1.6 Mhz */
      todruaga_readmem_cpu2,writemem_cpu2,0,0,
      mappy_interrupt_2,1
    }
  },
  60, DEFAULT_60HZ_VBLANK_DURATION,	/* frames per second, vblank duration */
  100,    /* 100 CPU slices per frame - an high value to ensure proper */
  /* synchronization of the CPUs */
  mappy_init_machine,

  /* video hardware */
  36*8, 28*8, { 0*8, 36*8-1, 0*8, 28*8-1 },
  todruaga_gfxdecodeinfo,
  32,64*4+64*16,
  mappy_vh_convert_color_prom,

  VIDEO_TYPE_RASTER,
  0,
  todruaga_vh_start,
  mappy_vh_stop,
  mappy_vh_screenrefresh,

  /* sound hardware */
  0,0,0,0,
  {
    {
      SOUND_NAMCO,
      &namco_interface
    }
  }
};



ROM_START( mappy )
ROM_REGION( 0x10000, REGION_CPU1 )     /* 64k for code for the first CPU  */
ROM_LOAD( "mappy1d.64",   0xa000, 0x2000, 0x52e6c708 )
ROM_LOAD( "mappy1c.64",   0xc000, 0x2000, 0xa958a61c )
ROM_LOAD( "mappy1b.64",   0xe000, 0x2000, 0x203766d4 )

ROM_REGION( 0x10000, REGION_CPU2 )     /* 64k for the second CPU */
ROM_LOAD( "mappy1k.64",   0xe000, 0x2000, 0x8182dd5b )

ROM_REGION( 0x1000, REGION_GFX1 | REGIONFLAG_DISPOSE )
ROM_LOAD( "mappy3b.32",   0x0000, 0x1000, 0x16498b9f )

ROM_REGION( 0x4000, REGION_GFX2 | REGIONFLAG_DISPOSE )
ROM_LOAD( "mappy3m.64",   0x0000, 0x2000, 0xf2d9647a )
ROM_LOAD( "mappy3n.64",   0x2000, 0x2000, 0x757cf2b6 )

ROM_REGION( 0x0220, REGION_PROMS )
ROM_LOAD( "mappy.pr1",    0x0000, 0x0020, 0x56531268 ) /* palette */
ROM_LOAD( "mappy.pr2",    0x0020, 0x0100, 0x50765082 ) /* characters */
ROM_LOAD( "mappy.pr3",    0x0120, 0x0100, 0x5396bd78 ) /* sprites */

ROM_REGION( 0x0100, REGION_SOUND1 )	/* sound prom */
ROM_LOAD( "mappy.spr",    0x0000, 0x0100, 0x16a9166a )
ROM_END

ROM_START( mappyjp )
ROM_REGION( 0x10000, REGION_CPU1 )     /* 64k for code for the first CPU  */
ROM_LOAD( "mappy3.bin",   0xa000, 0x2000, 0xdb9d5ab5 )
ROM_LOAD( "mappy1c.64",   0xc000, 0x2000, 0xa958a61c )
ROM_LOAD( "mappy1.bin",   0xe000, 0x2000, 0x77c0b492 )

ROM_REGION( 0x10000, REGION_CPU2 )     /* 64k for the second CPU */
ROM_LOAD( "mappy1k.64",   0xe000, 0x2000, 0x8182dd5b )

ROM_REGION( 0x1000, REGION_GFX1 | REGIONFLAG_DISPOSE )
ROM_LOAD( "mappy3b.32",   0x0000, 0x1000, 0x16498b9f )

ROM_REGION( 0x4000, REGION_GFX2 | REGIONFLAG_DISPOSE )
ROM_LOAD( "mappy3m.64",   0x0000, 0x2000, 0xf2d9647a )
ROM_LOAD( "mappy3n.64",   0x2000, 0x2000, 0x757cf2b6 )

ROM_REGION( 0x0220, REGION_PROMS )
ROM_LOAD( "mappy.pr1",    0x0000, 0x0020, 0x56531268 ) /* palette */
ROM_LOAD( "mappy.pr2",    0x0020, 0x0100, 0x50765082 ) /* characters */
ROM_LOAD( "mappy.pr3",    0x0120, 0x0100, 0x5396bd78 ) /* sprites */

ROM_REGION( 0x0100, REGION_SOUND1 )	/* sound prom */
ROM_LOAD( "mappy.spr",    0x0000, 0x0100, 0x16a9166a )
ROM_END

ROM_START( digdug2 )
ROM_REGION( 0x10000, REGION_CPU1 )     /* 64k for code for the first CPU  */
ROM_LOAD( "ddug2-3.bin",  0x8000, 0x4000, 0xbe7ec80b )
ROM_LOAD( "ddug2-1.bin",  0xc000, 0x4000, 0x5c77c0d4 )

ROM_REGION( 0x10000, REGION_CPU2 )     /* 64k for the second CPU */
ROM_LOAD( "ddug2-4.bin",  0xe000, 0x2000, 0x737443b1 )

ROM_REGION( 0x1000, REGION_GFX1 | REGIONFLAG_DISPOSE )
ROM_LOAD( "ddug2-3b.bin", 0x0000, 0x1000, 0xafcb4509 )

ROM_REGION( 0x8000, REGION_GFX2 | REGIONFLAG_DISPOSE )
ROM_LOAD( "ddug2-3m.bin", 0x0000, 0x4000, 0xdf1f4ad8 )
ROM_LOAD( "ddug2-3n.bin", 0x4000, 0x4000, 0xccadb3ea )

ROM_REGION( 0x0220, REGION_PROMS )
ROM_LOAD( "ddclr-5b.bin", 0x0000, 0x0020, 0x9b169db5 ) /* palette */
ROM_LOAD( "ddclr-4c.bin", 0x0020, 0x0100, 0x55a88695 ) /* characters */
ROM_LOAD( "ddclr-5k.bin", 0x0120, 0x0100, 0x1525a4d1 ) /* sprites */

ROM_REGION( 0x0100, REGION_SOUND1 )	/* sound prom */
ROM_LOAD( "ddsnd.bin",    0x0000, 0x0100, 0xe0074ee2 )
ROM_END

ROM_START( digdug2a )
ROM_REGION( 0x10000, REGION_CPU1 )     /* 64k for code for the first CPU  */
ROM_LOAD( "ddug2a_3.bin",  0x8000, 0x4000, 0xcc155338 )
ROM_LOAD( "ddug2a_1.bin",  0xc000, 0x4000, 0x40e46af8 )

ROM_REGION( 0x10000, REGION_CPU2 )     /* 64k for the second CPU */
ROM_LOAD( "ddug2-4.bin",  0xe000, 0x2000, 0x737443b1 )

ROM_REGION( 0x1000, REGION_GFX1 | REGIONFLAG_DISPOSE )
ROM_LOAD( "ddug2-3b.bin", 0x0000, 0x1000, 0xafcb4509 )

ROM_REGION( 0x8000, REGION_GFX2 | REGIONFLAG_DISPOSE )
ROM_LOAD( "ddug2-3m.bin", 0x0000, 0x4000, 0xdf1f4ad8 )
ROM_LOAD( "ddug2-3n.bin", 0x4000, 0x4000, 0xccadb3ea )

ROM_REGION( 0x0220, REGION_PROMS )
ROM_LOAD( "ddclr-5b.bin", 0x0000, 0x0020, 0x9b169db5 ) /* palette */
ROM_LOAD( "ddclr-4c.bin", 0x0020, 0x0100, 0x55a88695 ) /* characters */
ROM_LOAD( "ddclr_5k.bin", 0x0120, 0x0100, 0x9c55feda ) /* sprites */
/* Can't see the difference on screen, but CRC differs. */

ROM_REGION( 0x0100, REGION_SOUND1 )	/* sound prom */
ROM_LOAD( "ddsnd.bin",    0x0000, 0x0100, 0xe0074ee2 )
ROM_END

ROM_START( motos )
ROM_REGION( 0x10000, REGION_CPU1 )     /* 64k for code for the first CPU  */
ROM_LOAD( "mts_1d.bin",   0x8000, 0x4000, 0x1104abb2 )
ROM_LOAD( "mts_1b.bin",   0xc000, 0x4000, 0x57b157e2 )

ROM_REGION( 0x10000, REGION_CPU2 )     /* 64k for the second CPU */
ROM_LOAD( "mts_1k.bin",   0xe000, 0x2000, 0x55e45d21 )

ROM_REGION( 0x1000, REGION_GFX1 | REGIONFLAG_DISPOSE )
ROM_LOAD( "mts_3b.bin",   0x0000, 0x1000, 0x5d4a2a22 )

ROM_REGION( 0x8000, REGION_GFX2 | REGIONFLAG_DISPOSE )
ROM_LOAD( "mts_3m.bin",   0x0000, 0x4000, 0x2f0e396e )
ROM_LOAD( "mts_3n.bin",   0x4000, 0x4000, 0xcf8a3b86 )

ROM_REGION( 0x0220, REGION_PROMS )
ROM_LOAD( "motos.pr1",    0x0000, 0x0020, 0x71972383 ) /* palette */
ROM_LOAD( "motos.pr2",    0x0020, 0x0100, 0x730ba7fb ) /* characters */
ROM_LOAD( "motos.pr3",    0x0120, 0x0100, 0x7721275d ) /* sprites */

ROM_REGION( 0x0100, REGION_SOUND1 )	/* sound prom */
ROM_LOAD( "motos.spr",    0x0000, 0x0100, 0x2accdfb4 )
ROM_END

ROM_START( todruaga )
ROM_REGION( 0x10000, REGION_CPU1 )     /* 64k for code for the first CPU  */
ROM_LOAD( "druaga3.bin",  0x8000, 0x4000, 0x7ab4f5b2 )
ROM_LOAD( "druaga1.bin",  0xc000, 0x4000, 0x8c20ef10 )

ROM_REGION( 0x10000, REGION_CPU2 )     /* 64k for the second CPU */
ROM_LOAD( "druaga4.bin",  0xe000, 0x2000, 0xae9d06d9 )

ROM_REGION( 0x1000, REGION_GFX1 | REGIONFLAG_DISPOSE )
ROM_LOAD( "druaga3b.bin", 0x0000, 0x1000, 0xd32b249f )

ROM_REGION( 0x4000, REGION_GFX2 | REGIONFLAG_DISPOSE )
ROM_LOAD( "druaga3m.bin", 0x0000, 0x2000, 0xe827e787 )
ROM_LOAD( "druaga3n.bin", 0x2000, 0x2000, 0x962bd060 )

ROM_REGION( 0x0520, REGION_PROMS )
ROM_LOAD( "todruaga.pr1", 0x0000, 0x0020, 0x122cc395 ) /* palette */
ROM_LOAD( "todruaga.pr2", 0x0020, 0x0100, 0x8c661d6a ) /* characters */
ROM_LOAD( "todruaga.pr3", 0x0120, 0x0100, 0x5bcec186 ) /* sprites */
ROM_LOAD( "todruaga.pr4", 0x0220, 0x0100, 0xf029e5f5 ) /* sprites */
ROM_LOAD( "todruaga.pr5", 0x0320, 0x0100, 0xecdc206c ) /* sprites */
ROM_LOAD( "todruaga.pr6", 0x0420, 0x0100, 0x57b5ad6d ) /* sprites */

ROM_REGION( 0x0100, REGION_SOUND1 )	/* sound prom */
ROM_LOAD( "todruaga.spr", 0x0000, 0x0100, 0x07104c40 )
ROM_END

ROM_START( todruagb )
ROM_REGION( 0x10000, REGION_CPU1 )     /* 64k for code for the first CPU  */
ROM_LOAD( "druaga3a.bin", 0x8000, 0x4000, 0xfbf16299 )
ROM_LOAD( "druaga1a.bin", 0xc000, 0x4000, 0xb238d723 )

ROM_REGION( 0x10000, REGION_CPU2 )     /* 64k for the second CPU */
ROM_LOAD( "druaga4.bin",  0xe000, 0x2000, 0xae9d06d9 )

ROM_REGION( 0x1000, REGION_GFX1 | REGIONFLAG_DISPOSE )
ROM_LOAD( "druaga3b.bin", 0x0000, 0x1000, 0xd32b249f )

ROM_REGION( 0x4000, REGION_GFX2 | REGIONFLAG_DISPOSE )
ROM_LOAD( "druaga3m.bin", 0x0000, 0x2000, 0xe827e787 )
ROM_LOAD( "druaga3n.bin", 0x2000, 0x2000, 0x962bd060 )

ROM_REGION( 0x0520, REGION_PROMS )
ROM_LOAD( "todruaga.pr1", 0x0000, 0x0020, 0x122cc395 ) /* palette */
ROM_LOAD( "todruaga.pr2", 0x0020, 0x0100, 0x8c661d6a ) /* characters */
ROM_LOAD( "todruaga.pr3", 0x0120, 0x0100, 0x5bcec186 ) /* sprites */
ROM_LOAD( "todruaga.pr4", 0x0220, 0x0100, 0xf029e5f5 ) /* sprites */
ROM_LOAD( "todruaga.pr5", 0x0320, 0x0100, 0xecdc206c ) /* sprites */
ROM_LOAD( "todruaga.pr6", 0x0420, 0x0100, 0x57b5ad6d ) /* sprites */

ROM_REGION( 0x0100, REGION_SOUND1 )	/* sound prom */
ROM_LOAD( "todruaga.spr", 0x0000, 0x0100, 0x07104c40 )
ROM_END



GAME( 1983, mappy,    0,        mappy,    mappy,    0, ROT90, "Namco", "Mappy (US)" )
GAME( 1983, mappyjp,  mappy,    mappy,    mappy,    0, ROT90, "Namco", "Mappy (Japan)" )
GAME( 1985, digdug2,  0,        digdug2,  digdug2,  0, ROT90, "Namco", "Dig Dug II (set 1)" )
GAME( 1985, digdug2a, digdug2,  digdug2,  digdug2,  0, ROT90, "Namco", "Dig Dug II (set 2)" )
GAME( 1985, motos,    0,        motos,    motos,    0, ROT90, "Namco", "Motos" )
GAME( 1984, todruaga, 0,        todruaga, todruaga, 0, ROT90, "Namco", "Tower of Druaga (set 1)" )
GAME( 1984, todruagb, todruaga, todruaga, todruaga, 0, ROT90, "Namco", "Tower of Druaga (set 2)" )