#include "../vidhrdw/spy.cpp"

/***************************************************************************

S.P.Y. (c) 1989 Konami

Similar to Bottom of the Ninth

driver by Nicola Salmoria

***************************************************************************/

#include "driver.h"
#include "vidhrdw/generic.h"
#include "cpu/m6809/m6809.h"
#include "vidhrdw/konamiic.h"


int spy_vh_start(void);
void spy_vh_stop(void);
void spy_vh_screenrefresh(struct osd_bitmap *bitmap,int full_refresh);

static data8_t *pmcram;
static int rambank,pmcbank;;
static unsigned char *ram;

static int spy_interrupt(void)
{
  if (K052109_is_IRQ_enabled())
  {
    return interrupt();
  }
  else return ignore_interrupt();
}


static READ_HANDLER( spy_bankedram1_r )
{
  if (rambank & 1)
  {
    return paletteram_r(offset);
  }
  else if (rambank & 2)
  {
    if (pmcbank)
    {
      return pmcram[offset];
    }
    else
    {
      return 0;
    }
  }
  else
    return ram[offset];
}

static WRITE_HANDLER( spy_bankedram1_w )
{
  if (rambank & 1)
  {
    paletteram_xBBBBBGGGGGRRRRR_swap_w(offset,data);
  }
  else if (rambank & 2)
  {
    if (pmcbank)
    {
      pmcram[offset] = data;
    }
  }
  else
    ram[offset] = data;
}

static WRITE_HANDLER( bankswitch_w )
{
  unsigned char *rom = memory_region(REGION_CPU1);
  int offs;

  /* bit 0 = RAM bank? */
  //if ((data & 1) == 0) usrintf_showmessage("bankswitch RAM bank 0");

  /* bit 1-4 = ROM bank */
  if (data & 0x10) offs = 0x20000 + (data & 0x06) * 0x1000;
  else offs = 0x10000 + (data & 0x0e) * 0x1000;
  cpu_setbank(1,&rom[offs]);
}


void spy_collision(void)
{
#define MAX_SPRITES 64
#define DEF_NEAR_PLANE 0x6400
#define NEAR_PLANE_ZOOM 0x0100
#define FAR_PLANE_ZOOM 0x0000

  int op1, x1, w1, z1, d1, y1, h1;
  int op2, x2, w2, z2, d2, y2, h2;
  int mode, i, loopend, nearplane;

  mode = pmcram[0x1];
  op1 = pmcram[0x2];
  if (op1 == 1)
  {
    x1 = (pmcram[0x3]<<8) + pmcram[0x4];
    w1 = (pmcram[0x5]<<8) + pmcram[0x6];
    z1 = (pmcram[0x7]<<8) + pmcram[0x8];
    d1 = (pmcram[0x9]<<8) + pmcram[0xa];
    y1 = (pmcram[0xb]<<8) + pmcram[0xc];
    h1 = (pmcram[0xd]<<8) + pmcram[0xe];

    for (i=16; i<14*MAX_SPRITES + 2; i+=14)
    {
      op2 = pmcram[i];
      if (op2 || mode==0x0c)
      {
        x2 = (pmcram[i+0x1]<<8) + pmcram[i+0x2];
        w2 = (pmcram[i+0x3]<<8) + pmcram[i+0x4];
        z2 = (pmcram[i+0x5]<<8) + pmcram[i+0x6];
        d2 = (pmcram[i+0x7]<<8) + pmcram[i+0x8];
        y2 = (pmcram[i+0x9]<<8) + pmcram[i+0xa];
        h2 = (pmcram[i+0xb]<<8) + pmcram[i+0xc];
        /*
        	The mad scientist's laser truck has both a high sprite center and a small height value.
        	It has to be measured from the ground to detect correctly.
        */
        if (w2==0x58 && d2==0x04 && h2==0x10 && y2==0x30) h2 = y2;

        // what other sprites fall into:
        if ( (abs(x1-x2)<w1+w2) && (abs(z1-z2)<d1+d2) && (abs(y1-y2)<h1+h2) )
        {
          pmcram[0xf] = 0;
          pmcram[i+0xd] = 0;
        }
        else
          pmcram[i+0xd] = 1;
      }
    }
  }
  else if (op1 > 1)
  {
    /*
    	The PMCU also projects geometries to screen coordinates. Unfortunately I'm unable to figure
    	the scale factors from the PMCU code. Plugging 0 and 0x100 to the far and near planes seems
    	to do the trick though.
    */
    loopend = (pmcram[0]<<8) + pmcram[1];
    nearplane = (pmcram[2]<<8) + pmcram[3];

    // fail safe
    if (loopend > MAX_SPRITES) loopend = MAX_SPRITES;
    if (!nearplane) nearplane = DEF_NEAR_PLANE;

    loopend = (loopend<<1) + 4;

    for (i=4; i<loopend; i+=2)
    {
      op2 = (pmcram[i]<<8) + pmcram[i+1];
      op2 = (op2 * (NEAR_PLANE_ZOOM - FAR_PLANE_ZOOM)) / nearplane + FAR_PLANE_ZOOM;
      pmcram[i] = op2 >> 8;
      pmcram[i+1] = op2 & 0xff;
    }

    memset(pmcram+loopend, 0, 0x800-loopend); // clean up for next frame
  }
}

static WRITE_HANDLER( spy_3f90_w )
{
  extern int spy_video_enable;
  static int old;

  /* bits 0/1 = coin counters */
  coin_counter_w(0,data & 0x01);
  coin_counter_w(1,data & 0x02);

  /* bit 2 = enable char ROM reading through the video RAM */
  K052109_set_RMRD_line((data & 0x04) ? ASSERT_LINE : CLEAR_LINE);

  /* bit 3 = disable video */
  spy_video_enable = ~(data & 0x08);

  /* bit 4 = read RAM at 0000 (if set) else read color palette RAM */
  /* bit 5 = PMCBK */
  rambank = (data & 0x30) >> 4;
  /* bit 7 = PMC-BK */
  pmcbank = (data & 0x80) >> 7;

  /* bit 6 = PMC-START */
  if ((data & 0x40) && !(old & 0x40))
  {
    spy_collision();
    cpu_set_irq_line(0,M6809_FIRQ_LINE,HOLD_LINE);
  }

  old = data;
}


static WRITE_HANDLER( spy_sh_irqtrigger_w )
{
  cpu_cause_interrupt(1,0xff);
}

static WRITE_HANDLER( sound_bank_w )
{
  unsigned char *rom;
  int bank_A,bank_B;

  rom = memory_region(REGION_SOUND1);
  bank_A = 0x20000 * ((data >> 0) & 0x03);
  bank_B = 0x20000 * ((data >> 2) & 0x03);
  K007232_bankswitch(0,rom + bank_A,rom + bank_B);
  rom = memory_region(REGION_SOUND2);
  bank_A = 0x20000 * ((data >> 4) & 0x03);
  bank_B = 0x20000 * ((data >> 6) & 0x03);
  K007232_bankswitch(1,rom + bank_A,rom + bank_B);
}



static struct MemoryReadAddress spy_readmem[] =
{
  { 0x0000, 0x07ff, spy_bankedram1_r },
  { 0x0800, 0x1aff, MRA_RAM },
  { 0x3fd0, 0x3fd0, input_port_4_r },
  { 0x3fd1, 0x3fd1, input_port_0_r },
  { 0x3fd2, 0x3fd2, input_port_1_r },
  { 0x3fd3, 0x3fd3, input_port_2_r },
  { 0x3fe0, 0x3fe0, input_port_3_r },
  { 0x2000, 0x5fff, K052109_051960_r },
  { 0x6000, 0x7fff, MRA_BANK1 },
  { 0x8000, 0xffff, MRA_ROM },
  { -1 }	/* end of table */
};

static struct MemoryWriteAddress spy_writemem[] =
{
  { 0x0000, 0x07ff, spy_bankedram1_w, &ram },
  { 0x0800, 0x1aff, MWA_RAM },
  { 0x3f80, 0x3f80, bankswitch_w },
  { 0x3f90, 0x3f90, spy_3f90_w },
  { 0x3fa0, 0x3fa0, watchdog_reset_w },
  { 0x3fb0, 0x3fb0, soundlatch_w },
  { 0x3fc0, 0x3fc0, spy_sh_irqtrigger_w },
  { 0x2000, 0x5fff, K052109_051960_w },
  { 0x6000, 0x7fff, MWA_ROM },
  { 0x8000, 0xffff, MWA_ROM },
  { -1 }	/* end of table */
};

static struct MemoryReadAddress spy_sound_readmem[] =
{
  { 0x0000, 0x7fff, MRA_ROM },
  { 0x8000, 0x87ff, MRA_RAM },
  { 0xa000, 0xa00d, K007232_read_port_0_r },
  { 0xb000, 0xb00d, K007232_read_port_1_r },
  { 0xc000, 0xc000, YM3812_status_port_0_r },
  { 0xd000, 0xd000, soundlatch_r },
  { -1 }	/* end of table */
};

static struct MemoryWriteAddress spy_sound_writemem[] =
{
  { 0x0000, 0x7fff, MWA_ROM },
  { 0x8000, 0x87ff, MWA_RAM },
  { 0x9000, 0x9000, sound_bank_w },
  { 0xa000, 0xa00d, K007232_write_port_0_w },
  { 0xb000, 0xb00d, K007232_write_port_1_w },
  { 0xc000, 0xc000, YM3812_control_port_0_w },
  { 0xc001, 0xc001, YM3812_write_port_0_w },
  { -1 }	/* end of table */
};



INPUT_PORTS_START( spy )
PORT_START
PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_START1 )
PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_UP    | IPF_8WAY | IPF_PLAYER1 )
PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN  | IPF_8WAY | IPF_PLAYER1 )
PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT  | IPF_8WAY | IPF_PLAYER1 )
PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT | IPF_8WAY | IPF_PLAYER1 )
PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON1 | IPF_PLAYER1 )
PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_BUTTON2 | IPF_PLAYER1 )
PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

PORT_START
PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_START2 )
PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_UP    | IPF_8WAY | IPF_PLAYER2 )
PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN  | IPF_8WAY | IPF_PLAYER2 )
PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT  | IPF_8WAY | IPF_PLAYER2 )
PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT | IPF_8WAY | IPF_PLAYER2 )
PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON1 | IPF_PLAYER2 )
PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_BUTTON2 | IPF_PLAYER2 )
PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

PORT_START
PORT_DIPNAME( 0x0f, 0x0f, DEF_STR( Coin_A ) )
PORT_DIPSETTING(    0x02, DEF_STR( 4C_1C ) )
PORT_DIPSETTING(    0x05, DEF_STR( 3C_1C ) )
PORT_DIPSETTING(    0x08, DEF_STR( 2C_1C ) )
PORT_DIPSETTING(    0x04, DEF_STR( 3C_2C ) )
PORT_DIPSETTING(    0x01, DEF_STR( 4C_3C ) )
PORT_DIPSETTING(    0x0f, DEF_STR( 1C_1C ) )
PORT_DIPSETTING(    0x03, DEF_STR( 3C_4C ) )
PORT_DIPSETTING(    0x07, DEF_STR( 2C_3C ) )
PORT_DIPSETTING(    0x0e, DEF_STR( 1C_2C ) )
PORT_DIPSETTING(    0x06, DEF_STR( 2C_5C ) )
PORT_DIPSETTING(    0x0d, DEF_STR( 1C_3C ) )
PORT_DIPSETTING(    0x0c, DEF_STR( 1C_4C ) )
PORT_DIPSETTING(    0x0b, DEF_STR( 1C_5C ) )
PORT_DIPSETTING(    0x0a, DEF_STR( 1C_6C ) )
PORT_DIPSETTING(    0x09, DEF_STR( 1C_7C ) )
PORT_DIPSETTING(    0x00, DEF_STR( Free_Play ) )
PORT_DIPNAME( 0xf0, 0xf0, DEF_STR( Coin_B ) )
PORT_DIPSETTING(    0x20, DEF_STR( 4C_1C ) )
PORT_DIPSETTING(    0x50, DEF_STR( 3C_1C ) )
PORT_DIPSETTING(    0x80, DEF_STR( 2C_1C ) )
PORT_DIPSETTING(    0x40, DEF_STR( 3C_2C ) )
PORT_DIPSETTING(    0x10, DEF_STR( 4C_3C ) )
PORT_DIPSETTING(    0xf0, DEF_STR( 1C_1C ) )
PORT_DIPSETTING(    0x30, DEF_STR( 3C_4C ) )
PORT_DIPSETTING(    0x70, DEF_STR( 2C_3C ) )
PORT_DIPSETTING(    0xe0, DEF_STR( 1C_2C ) )
PORT_DIPSETTING(    0x60, DEF_STR( 2C_5C ) )
PORT_DIPSETTING(    0xd0, DEF_STR( 1C_3C ) )
PORT_DIPSETTING(    0xc0, DEF_STR( 1C_4C ) )
PORT_DIPSETTING(    0xb0, DEF_STR( 1C_5C ) )
PORT_DIPSETTING(    0xa0, DEF_STR( 1C_6C ) )
PORT_DIPSETTING(    0x90, DEF_STR( 1C_7C ) )
//	PORT_DIPSETTING(    0x00, "Invalid" )

PORT_START
PORT_DIPNAME( 0x03, 0x02, DEF_STR( Lives ) )
PORT_DIPSETTING(    0x03, "2" )
PORT_DIPSETTING(    0x02, "3" )
PORT_DIPSETTING(    0x01, "5" )
PORT_DIPSETTING(    0x00, "7" )
PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) )
PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
PORT_DIPSETTING(    0x00, DEF_STR( On ) )
PORT_DIPNAME( 0x18, 0x08, DEF_STR( Bonus_Life ) )
PORT_DIPSETTING(    0x18, "10k and every 20k" )
PORT_DIPSETTING(    0x10, "20k and every 30k" )
PORT_DIPSETTING(    0x08, "20k" )
PORT_DIPSETTING(    0x00, "30k" )
PORT_DIPNAME( 0x60, 0x40, DEF_STR( Difficulty ) )
PORT_DIPSETTING(	0x60, "Easy" )
PORT_DIPSETTING(	0x40, "Normal" )
PORT_DIPSETTING(	0x20, "Difficult" )
PORT_DIPSETTING(	0x00, "Very Difficult" )
PORT_DIPNAME( 0x80, 0x00, DEF_STR( Demo_Sounds ) )
PORT_DIPSETTING(	0x80, DEF_STR( Off ) )
PORT_DIPSETTING(	0x00, DEF_STR( On ) )

PORT_START
PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_COIN1 )
PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_COIN2 )
PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_COIN3 )
PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNUSED )
PORT_DIPNAME( 0x10, 0x10, DEF_STR( Flip_Screen ) )
PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
PORT_DIPSETTING(    0x00, DEF_STR( On ) )
PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
PORT_DIPSETTING(    0x00, DEF_STR( On ) )
PORT_SERVICE( 0x40, IP_ACTIVE_LOW )
PORT_DIPNAME( 0x80, 0x80, "Continues" )
PORT_DIPSETTING(    0x80, "Unlimited" )
PORT_DIPSETTING(    0x00, "5 Times" )
INPUT_PORTS_END



static void volume_callback0(int v)
{
  K007232_set_volume(0,0,(v >> 4) * 0x11,0);
  K007232_set_volume(0,1,0,(v & 0x0f) * 0x11);
}

static void volume_callback1(int v)
{
  K007232_set_volume(1,0,(v >> 4) * 0x11,0);
  K007232_set_volume(1,1,0,(v & 0x0f) * 0x11);
}

static struct K007232_interface k007232_interface =
{
  2,			/* number of chips */
  { REGION_SOUND1, REGION_SOUND2 },	/* memory regions */
  {
    K007232_VOL(40,MIXER_PAN_CENTER,40,MIXER_PAN_CENTER),
    K007232_VOL(40,MIXER_PAN_CENTER,40,MIXER_PAN_CENTER)
  },	/* volume */
  { volume_callback0, volume_callback1 }	/* external port callback */
};


static void irqhandler(int linestate)
{
  cpu_set_nmi_line(1,linestate);
}

static struct YM3812interface ym3812_interface =
{
  1,			/* 1 chip */
  3579545,	/* ??? */
  { 100 },	/* volume */
  { irqhandler },
};



static struct MachineDriver machine_driver_spy =
{
  {
    {
      CPU_M6809,
      3000000, /* ? */
      spy_readmem,spy_writemem,0,0,
      spy_interrupt,1
    },
    {
      CPU_Z80 | CPU_AUDIO_CPU,
      3579545,
      spy_sound_readmem, spy_sound_writemem,0,0,
      ignore_interrupt,0	/* irq is triggered by the main CPU */
      /* nmi by the sound chip */
    }
  },
  60, DEFAULT_60HZ_VBLANK_DURATION,	/* frames per second, vblank duration */
  1,	/* 1 CPU slice per frame - interleaving is forced when a sound command is written */
  0,

  /* video hardware */
  64*8, 32*8, { 13*8, (64-13)*8-1, 2*8, 30*8-1 },
  0,	/* gfx decoded by konamiic.c */
  1024, 1024,
  0,

  VIDEO_TYPE_RASTER | VIDEO_MODIFIES_PALETTE,
  0,
  spy_vh_start,
  spy_vh_stop,
  spy_vh_screenrefresh,

  /* sound hardware */
  0,0,0,0,
  {
    {
      SOUND_YM3812,
      &ym3812_interface
    },
    {
      SOUND_K007232,
      &k007232_interface
    }
  }
};


/***************************************************************************

  Game ROMs

***************************************************************************/

ROM_START( spy )
ROM_REGION( 0x29000, REGION_CPU1 ) /* code + banked roms + space for banked ram */
ROM_LOAD( "857m03.bin",   0x10000, 0x10000, 0x3bd87fa4 )
ROM_LOAD( "857m02.bin",   0x20000, 0x08000, 0x306cc659 )
ROM_CONTINUE(             0x08000, 0x08000 )

ROM_REGION( 0x10000, REGION_CPU2 ) /* Z80 code */
ROM_LOAD( "857d01.bin",   0x0000, 0x8000, 0xaad4210f )

ROM_REGION( 0x080000, REGION_GFX1 ) /* graphics ( dont dispose as the program can read them ) */
ROM_LOAD( "857b09.bin",   0x00000, 0x40000, 0xb8780966 )	/* characters */
ROM_LOAD( "857b08.bin",   0x40000, 0x40000, 0x3e4d8d50 )

ROM_REGION( 0x100000, REGION_GFX2 ) /* graphics ( dont dispose as the program can read them ) */
ROM_LOAD( "857b06.bin",   0x00000, 0x80000, 0x7b515fb1 )	/* sprites */
ROM_LOAD( "857b05.bin",   0x80000, 0x80000, 0x27b0f73b )

ROM_REGION( 0x0200, REGION_PROMS )
ROM_LOAD( "857a10.bin",   0x0000, 0x0100, 0x32758507 )	/* priority encoder (not used) */

ROM_REGION( 0x40000, REGION_SOUND1 ) /* samples for 007232 #0 */
ROM_LOAD( "857b07.bin",   0x00000, 0x40000, 0xce3512d4 )

ROM_REGION( 0x40000, REGION_SOUND2 ) /* samples for 007232 #1 */
ROM_LOAD( "857b04.bin",   0x00000, 0x40000, 0x20b83c13 )
ROM_END



static void gfx_untangle(void)
{
  konami_rom_deinterleave_2(REGION_GFX1);
  konami_rom_deinterleave_2(REGION_GFX2);
}

static void init_spy(void)
{
  paletteram = &memory_region(REGION_CPU1)[0x28000];
  pmcram =     &memory_region(REGION_CPU1)[0x28800];
  gfx_untangle();
}



GAME ( 1989, spy, 0, spy, spy, spy, ROT0, "Konami", "S.P.Y. - Special Project Y (US)" )
