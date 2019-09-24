/************************************************
 * Result table for Z8000 DAB instruction
 *
 * bits    description
 * ----------------------------------------------
 * 0..7    destination value
 * 8       carry flag before
 * 9       half carry flag before
 * 10      D flag (0 add/adc, 1 sub/sbc)
 *
 * result  description
 * ----------------------------------------------
 * 0..7    result value
 * 8       carry flag after
 ************************************************/
static UINT16 Z8000_dab[0x800] =
{
  /* add/adc results */
  0x000,0x001,0x002,0x003,0x004,0x005,0x006,0x007,
  0x008,0x009,0x010,0x011,0x012,0x013,0x014,0x015,
  0x010,0x011,0x012,0x013,0x014,0x015,0x016,0x017,
  0x018,0x019,0x020,0x021,0x022,0x023,0x024,0x025,
  0x020,0x021,0x022,0x023,0x024,0x025,0x026,0x027,
  0x028,0x029,0x030,0x031,0x032,0x033,0x034,0x035,
  0x030,0x031,0x032,0x033,0x034,0x035,0x036,0x037,
  0x038,0x039,0x040,0x041,0x042,0x043,0x044,0x045,
  0x040,0x041,0x042,0x043,0x044,0x045,0x046,0x047,
  0x048,0x049,0x050,0x051,0x052,0x053,0x054,0x055,
  0x050,0x051,0x052,0x053,0x054,0x055,0x056,0x057,
  0x058,0x059,0x060,0x061,0x062,0x063,0x064,0x065,
  0x060,0x061,0x062,0x063,0x064,0x065,0x066,0x067,
  0x068,0x069,0x070,0x071,0x072,0x073,0x074,0x075,
  0x070,0x071,0x072,0x073,0x074,0x075,0x076,0x077,
  0x078,0x079,0x080,0x081,0x082,0x083,0x084,0x085,
  0x080,0x081,0x082,0x083,0x084,0x085,0x086,0x087,
  0x088,0x089,0x090,0x091,0x092,0x093,0x094,0x095,
  0x090,0x091,0x092,0x093,0x094,0x095,0x096,0x097,
  0x098,0x099,0x100,0x101,0x102,0x103,0x104,0x105,
  0x100,0x101,0x102,0x103,0x104,0x105,0x106,0x107,
  0x108,0x109,0x110,0x111,0x112,0x113,0x114,0x115,
  0x110,0x111,0x112,0x113,0x114,0x115,0x116,0x117,
  0x118,0x119,0x120,0x121,0x122,0x123,0x124,0x125,
  0x120,0x121,0x122,0x123,0x124,0x125,0x126,0x127,
  0x128,0x129,0x130,0x131,0x132,0x133,0x134,0x135,
  0x130,0x131,0x132,0x133,0x134,0x135,0x136,0x137,
  0x138,0x139,0x140,0x141,0x142,0x143,0x144,0x145,
  0x140,0x141,0x142,0x143,0x144,0x145,0x146,0x147,
  0x148,0x149,0x150,0x151,0x152,0x153,0x154,0x155,
  0x150,0x151,0x152,0x153,0x154,0x155,0x156,0x157,
  0x158,0x159,0x160,0x161,0x162,0x163,0x164,0x165,
  0x166,0x167,0x168,0x169,0x16a,0x16b,0x16c,0x16d,
  0x16e,0x16f,0x170,0x171,0x172,0x173,0x174,0x175,
  0x176,0x177,0x178,0x179,0x17a,0x17b,0x17c,0x17d,
  0x17e,0x17f,0x180,0x181,0x182,0x183,0x184,0x185,
  0x186,0x187,0x188,0x189,0x18a,0x18b,0x18c,0x18d,
  0x18e,0x18f,0x190,0x191,0x192,0x193,0x194,0x195,
  0x196,0x197,0x198,0x199,0x19a,0x19b,0x19c,0x19d,
  0x19e,0x19f,0x1a0,0x1a1,0x1a2,0x1a3,0x1a4,0x1a5,
  0x1a6,0x1a7,0x1a8,0x1a9,0x1aa,0x1ab,0x1ac,0x1ad,
  0x1ae,0x1af,0x1b0,0x1b1,0x1b2,0x1b3,0x1b4,0x1b5,
  0x1b6,0x1b7,0x1b8,0x1b9,0x1ba,0x1bb,0x1bc,0x1bd,
  0x1be,0x1bf,0x1c0,0x1c1,0x1c2,0x1c3,0x1c4,0x1c5,
  0x1c6,0x1c7,0x1c8,0x1c9,0x1ca,0x1cb,0x1cc,0x1cd,
  0x1ce,0x1cf,0x1d0,0x1d1,0x1d2,0x1d3,0x1d4,0x1d5,
  0x1d6,0x1d7,0x1d8,0x1d9,0x1da,0x1db,0x1dc,0x1dd,
  0x1de,0x1df,0x1e0,0x1e1,0x1e2,0x1e3,0x1e4,0x1e5,
  0x1e6,0x1e7,0x1e8,0x1e9,0x1ea,0x1eb,0x1ec,0x1ed,
  0x1ee,0x1ef,0x1f0,0x1f1,0x1f2,0x1f3,0x1f4,0x1f5,
  0x1f6,0x1f7,0x1f8,0x1f9,0x1fa,0x1fb,0x1fc,0x1fd,
  0x1fe,0x1ff,0x100,0x101,0x102,0x103,0x104,0x105,
  0x106,0x107,0x108,0x109,0x10a,0x10b,0x10c,0x10d,
  0x10e,0x10f,0x110,0x111,0x112,0x113,0x114,0x115,
  0x116,0x117,0x118,0x119,0x11a,0x11b,0x11c,0x11d,
  0x11e,0x11f,0x120,0x121,0x122,0x123,0x124,0x125,
  0x126,0x127,0x128,0x129,0x12a,0x12b,0x12c,0x12d,
  0x12e,0x12f,0x130,0x131,0x132,0x133,0x134,0x135,
  0x136,0x137,0x138,0x139,0x13a,0x13b,0x13c,0x13d,
  0x13e,0x13f,0x140,0x141,0x142,0x143,0x144,0x145,
  0x146,0x147,0x148,0x149,0x14a,0x14b,0x14c,0x14d,
  0x14e,0x14f,0x150,0x151,0x152,0x153,0x154,0x155,
  0x156,0x157,0x158,0x159,0x15a,0x15b,0x15c,0x15d,
  0x15e,0x15f,0x160,0x161,0x162,0x163,0x164,0x165,
  0x166,0x167,0x168,0x169,0x16a,0x16b,0x16c,0x16d,
  0x16e,0x16f,0x170,0x171,0x172,0x173,0x174,0x175,
  0x176,0x177,0x178,0x179,0x17a,0x17b,0x17c,0x17d,
  0x17e,0x17f,0x180,0x181,0x182,0x183,0x184,0x185,
  0x186,0x187,0x188,0x189,0x18a,0x18b,0x18c,0x18d,
  0x18e,0x18f,0x190,0x191,0x192,0x193,0x194,0x195,
  0x196,0x197,0x198,0x199,0x19a,0x19b,0x19c,0x19d,
  0x19e,0x19f,0x1a0,0x1a1,0x1a2,0x1a3,0x1a4,0x1a5,
  0x1a6,0x1a7,0x1a8,0x1a9,0x1aa,0x1ab,0x1ac,0x1ad,
  0x1ae,0x1af,0x1b0,0x1b1,0x1b2,0x1b3,0x1b4,0x1b5,
  0x1b6,0x1b7,0x1b8,0x1b9,0x1ba,0x1bb,0x1bc,0x1bd,
  0x1be,0x1bf,0x1c0,0x1c1,0x1c2,0x1c3,0x1c4,0x1c5,
  0x1c6,0x1c7,0x1c8,0x1c9,0x1ca,0x1cb,0x1cc,0x1cd,
  0x1ce,0x1cf,0x1d0,0x1d1,0x1d2,0x1d3,0x1d4,0x1d5,
  0x1d6,0x1d7,0x1d8,0x1d9,0x1da,0x1db,0x1dc,0x1dd,
  0x1de,0x1df,0x1e0,0x1e1,0x1e2,0x1e3,0x1e4,0x1e5,
  0x1e6,0x1e7,0x1e8,0x1e9,0x1ea,0x1eb,0x1ec,0x1ed,
  0x1ee,0x1ef,0x1f0,0x1f1,0x1f2,0x1f3,0x1f4,0x1f5,
  0x1f6,0x1f7,0x1f8,0x1f9,0x1fa,0x1fb,0x1fc,0x1fd,
  0x1fe,0x1ff,0x100,0x101,0x102,0x103,0x104,0x105,
  0x106,0x107,0x108,0x109,0x10a,0x10b,0x10c,0x10d,
  0x10e,0x10f,0x110,0x111,0x112,0x113,0x114,0x115,
  0x116,0x117,0x118,0x119,0x11a,0x11b,0x11c,0x11d,
  0x11e,0x11f,0x120,0x121,0x122,0x123,0x124,0x125,
  0x126,0x127,0x128,0x129,0x12a,0x12b,0x12c,0x12d,
  0x12e,0x12f,0x130,0x131,0x132,0x133,0x134,0x135,
  0x136,0x137,0x138,0x139,0x13a,0x13b,0x13c,0x13d,
  0x13e,0x13f,0x140,0x141,0x142,0x143,0x144,0x145,
  0x146,0x147,0x148,0x149,0x14a,0x14b,0x14c,0x14d,
  0x14e,0x14f,0x150,0x151,0x152,0x153,0x154,0x155,
  0x156,0x157,0x158,0x159,0x15a,0x15b,0x15c,0x15d,
  0x15e,0x15f,0x160,0x161,0x162,0x163,0x164,0x165,
  0x166,0x167,0x168,0x169,0x16a,0x16b,0x16c,0x16d,
  0x16e,0x16f,0x170,0x171,0x172,0x173,0x174,0x175,
  0x176,0x177,0x178,0x179,0x17a,0x17b,0x17c,0x17d,
  0x17e,0x17f,0x180,0x181,0x182,0x183,0x184,0x185,
  0x186,0x187,0x188,0x189,0x18a,0x18b,0x18c,0x18d,
  0x18e,0x18f,0x190,0x191,0x192,0x193,0x194,0x195,
  0x196,0x197,0x198,0x199,0x19a,0x19b,0x19c,0x19d,
  0x19e,0x19f,0x1a0,0x1a1,0x1a2,0x1a3,0x1a4,0x1a5,
  0x1a6,0x1a7,0x1a8,0x1a9,0x1aa,0x1ab,0x1ac,0x1ad,
  0x1ae,0x1af,0x1b0,0x1b1,0x1b2,0x1b3,0x1b4,0x1b5,
  0x1b6,0x1b7,0x1b8,0x1b9,0x1ba,0x1bb,0x1bc,0x1bd,
  0x1be,0x1bf,0x1c0,0x1c1,0x1c2,0x1c3,0x1c4,0x1c5,
  0x1c6,0x1c7,0x1c8,0x1c9,0x1ca,0x1cb,0x1cc,0x1cd,
  0x1ce,0x1cf,0x1d0,0x1d1,0x1d2,0x1d3,0x1d4,0x1d5,
  0x1d6,0x1d7,0x1d8,0x1d9,0x1da,0x1db,0x1dc,0x1dd,
  0x1de,0x1df,0x1e0,0x1e1,0x1e2,0x1e3,0x1e4,0x1e5,
  0x1e6,0x1e7,0x1e8,0x1e9,0x1ea,0x1eb,0x1ec,0x1ed,
  0x1ee,0x1ef,0x1f0,0x1f1,0x1f2,0x1f3,0x1f4,0x1f5,
  0x1f6,0x1f7,0x1f8,0x1f9,0x1fa,0x1fb,0x1fc,0x1fd,
  0x1fe,0x1ff,0x100,0x101,0x102,0x103,0x104,0x105,
  0x106,0x107,0x108,0x109,0x10a,0x10b,0x10c,0x10d,
  0x10e,0x10f,0x110,0x111,0x112,0x113,0x114,0x115,
  0x116,0x117,0x118,0x119,0x11a,0x11b,0x11c,0x11d,
  0x11e,0x11f,0x120,0x121,0x122,0x123,0x124,0x125,
  0x126,0x127,0x128,0x129,0x12a,0x12b,0x12c,0x12d,
  0x12e,0x12f,0x130,0x131,0x132,0x133,0x134,0x135,
  0x136,0x137,0x138,0x139,0x13a,0x13b,0x13c,0x13d,
  0x13e,0x13f,0x140,0x141,0x142,0x143,0x144,0x145,
  0x146,0x147,0x148,0x149,0x14a,0x14b,0x14c,0x14d,
  0x14e,0x14f,0x150,0x151,0x152,0x153,0x154,0x155,
  0x156,0x157,0x158,0x159,0x15a,0x15b,0x15c,0x15d,
  0x15e,0x15f,0x160,0x161,0x162,0x163,0x164,0x165,
  /* sub/sbc results */
  0x000,0x001,0x002,0x003,0x004,0x005,0x006,0x007,
  0x008,0x009,0x00a,0x00b,0x00c,0x00d,0x00e,0x00f,
  0x010,0x011,0x012,0x013,0x014,0x015,0x016,0x017,
  0x018,0x019,0x01a,0x01b,0x01c,0x01d,0x01e,0x01f,
  0x020,0x021,0x022,0x023,0x024,0x025,0x026,0x027,
  0x028,0x029,0x02a,0x02b,0x02c,0x02d,0x02e,0x02f,
  0x030,0x031,0x032,0x033,0x034,0x035,0x036,0x037,
  0x038,0x039,0x03a,0x03b,0x03c,0x03d,0x03e,0x03f,
  0x040,0x041,0x042,0x043,0x044,0x045,0x046,0x047,
  0x048,0x049,0x04a,0x04b,0x04c,0x04d,0x04e,0x04f,
  0x050,0x051,0x052,0x053,0x054,0x055,0x056,0x057,
  0x058,0x059,0x05a,0x05b,0x05c,0x05d,0x05e,0x05f,
  0x060,0x061,0x062,0x063,0x064,0x065,0x066,0x067,
  0x068,0x069,0x06a,0x06b,0x06c,0x06d,0x06e,0x06f,
  0x070,0x071,0x072,0x073,0x074,0x075,0x076,0x077,
  0x078,0x079,0x07a,0x07b,0x07c,0x07d,0x07e,0x07f,
  0x080,0x081,0x082,0x083,0x084,0x085,0x086,0x087,
  0x088,0x089,0x08a,0x08b,0x08c,0x08d,0x08e,0x08f,
  0x090,0x091,0x092,0x093,0x094,0x095,0x096,0x097,
  0x098,0x099,0x09a,0x09b,0x09c,0x09d,0x09e,0x09f,
  0x0a0,0x0a1,0x0a2,0x0a3,0x0a4,0x0a5,0x0a6,0x0a7,
  0x0a8,0x0a9,0x0aa,0x0ab,0x0ac,0x0ad,0x0ae,0x0af,
  0x0b0,0x0b1,0x0b2,0x0b3,0x0b4,0x0b5,0x0b6,0x0b7,
  0x0b8,0x0b9,0x0ba,0x0bb,0x0bc,0x0bd,0x0be,0x0bf,
  0x0c0,0x0c1,0x0c2,0x0c3,0x0c4,0x0c5,0x0c6,0x0c7,
  0x0c8,0x0c9,0x0ca,0x0cb,0x0cc,0x0cd,0x0ce,0x0cf,
  0x0d0,0x0d1,0x0d2,0x0d3,0x0d4,0x0d5,0x0d6,0x0d7,
  0x0d8,0x0d9,0x0da,0x0db,0x0dc,0x0dd,0x0de,0x0df,
  0x0e0,0x0e1,0x0e2,0x0e3,0x0e4,0x0e5,0x0e6,0x0e7,
  0x0e8,0x0e9,0x0ea,0x0eb,0x0ec,0x0ed,0x0ee,0x0ef,
  0x0f0,0x0f1,0x0f2,0x0f3,0x0f4,0x0f5,0x0f6,0x0f7,
  0x0f8,0x0f9,0x0fa,0x0fb,0x0fc,0x0fd,0x0fe,0x0ff,
  0x1a0,0x1a1,0x1a2,0x1a3,0x1a4,0x1a5,0x1a6,0x1a7,
  0x1a8,0x1a9,0x1aa,0x1ab,0x1ac,0x1ad,0x1ae,0x1af,
  0x1b0,0x1b1,0x1b2,0x1b3,0x1b4,0x1b5,0x1b6,0x1b7,
  0x1b8,0x1b9,0x1ba,0x1bb,0x1bc,0x1bd,0x1be,0x1bf,
  0x1c0,0x1c1,0x1c2,0x1c3,0x1c4,0x1c5,0x1c6,0x1c7,
  0x1c8,0x1c9,0x1ca,0x1cb,0x1cc,0x1cd,0x1ce,0x1cf,
  0x1d0,0x1d1,0x1d2,0x1d3,0x1d4,0x1d5,0x1d6,0x1d7,
  0x1d8,0x1d9,0x1da,0x1db,0x1dc,0x1dd,0x1de,0x1df,
  0x1e0,0x1e1,0x1e2,0x1e3,0x1e4,0x1e5,0x1e6,0x1e7,
  0x1e8,0x1e9,0x1ea,0x1eb,0x1ec,0x1ed,0x1ee,0x1ef,
  0x1f0,0x1f1,0x1f2,0x1f3,0x1f4,0x1f5,0x1f6,0x1f7,
  0x1f8,0x1f9,0x1fa,0x1fb,0x1fc,0x1fd,0x1fe,0x1ff,
  0x100,0x101,0x102,0x103,0x104,0x105,0x106,0x107,
  0x108,0x109,0x10a,0x10b,0x10c,0x10d,0x10e,0x10f,
  0x110,0x111,0x112,0x113,0x114,0x115,0x116,0x117,
  0x118,0x119,0x11a,0x11b,0x11c,0x11d,0x11e,0x11f,
  0x120,0x121,0x122,0x123,0x124,0x125,0x126,0x127,
  0x128,0x129,0x12a,0x12b,0x12c,0x12d,0x12e,0x12f,
  0x130,0x131,0x132,0x133,0x134,0x135,0x136,0x137,
  0x138,0x139,0x13a,0x13b,0x13c,0x13d,0x13e,0x13f,
  0x140,0x141,0x142,0x143,0x144,0x145,0x146,0x147,
  0x148,0x149,0x14a,0x14b,0x14c,0x14d,0x14e,0x14f,
  0x150,0x151,0x152,0x153,0x154,0x155,0x156,0x157,
  0x158,0x159,0x15a,0x15b,0x15c,0x15d,0x15e,0x15f,
  0x160,0x161,0x162,0x163,0x164,0x165,0x166,0x167,
  0x168,0x169,0x16a,0x16b,0x16c,0x16d,0x16e,0x16f,
  0x170,0x171,0x172,0x173,0x174,0x175,0x176,0x177,
  0x178,0x179,0x17a,0x17b,0x17c,0x17d,0x17e,0x17f,
  0x180,0x181,0x182,0x183,0x184,0x185,0x186,0x187,
  0x188,0x189,0x18a,0x18b,0x18c,0x18d,0x18e,0x18f,
  0x190,0x191,0x192,0x193,0x194,0x195,0x196,0x197,
  0x198,0x199,0x19a,0x19b,0x19c,0x19d,0x19e,0x19f,
  0x1fa,0x1fb,0x1fc,0x1fd,0x1fe,0x1ff,0x100,0x101,
  0x102,0x103,0x104,0x105,0x106,0x107,0x108,0x109,
  0x10a,0x10b,0x10c,0x10d,0x10e,0x10f,0x110,0x111,
  0x112,0x113,0x114,0x115,0x116,0x117,0x118,0x119,
  0x11a,0x11b,0x11c,0x11d,0x11e,0x11f,0x120,0x121,
  0x122,0x123,0x124,0x125,0x126,0x127,0x128,0x129,
  0x12a,0x12b,0x12c,0x12d,0x12e,0x12f,0x130,0x131,
  0x132,0x133,0x134,0x135,0x136,0x137,0x138,0x139,
  0x13a,0x13b,0x13c,0x13d,0x13e,0x13f,0x140,0x141,
  0x142,0x143,0x144,0x145,0x146,0x147,0x148,0x149,
  0x14a,0x14b,0x14c,0x14d,0x14e,0x14f,0x150,0x151,
  0x152,0x153,0x154,0x155,0x156,0x157,0x158,0x159,
  0x15a,0x15b,0x15c,0x15d,0x15e,0x15f,0x160,0x161,
  0x162,0x163,0x164,0x165,0x166,0x167,0x168,0x169,
  0x16a,0x16b,0x16c,0x16d,0x16e,0x16f,0x170,0x171,
  0x172,0x173,0x174,0x175,0x176,0x177,0x178,0x179,
  0x17a,0x17b,0x17c,0x17d,0x17e,0x17f,0x180,0x181,
  0x182,0x183,0x184,0x185,0x186,0x187,0x188,0x189,
  0x18a,0x18b,0x18c,0x18d,0x18e,0x18f,0x190,0x191,
  0x192,0x193,0x194,0x195,0x196,0x197,0x198,0x199,
  0x19a,0x19b,0x19c,0x19d,0x19e,0x19f,0x1a0,0x1a1,
  0x1a2,0x1a3,0x1a4,0x1a5,0x1a6,0x1a7,0x1a8,0x1a9,
  0x1aa,0x1ab,0x1ac,0x1ad,0x1ae,0x1af,0x1b0,0x1b1,
  0x1b2,0x1b3,0x1b4,0x1b5,0x1b6,0x1b7,0x1b8,0x1b9,
  0x1ba,0x1bb,0x1bc,0x1bd,0x1be,0x1bf,0x1c0,0x1c1,
  0x1c2,0x1c3,0x1c4,0x1c5,0x1c6,0x1c7,0x1c8,0x1c9,
  0x1ca,0x1cb,0x1cc,0x1cd,0x1ce,0x1cf,0x1d0,0x1d1,
  0x1d2,0x1d3,0x1d4,0x1d5,0x1d6,0x1d7,0x1d8,0x1d9,
  0x1da,0x1db,0x1dc,0x1dd,0x1de,0x1df,0x1e0,0x1e1,
  0x1e2,0x1e3,0x1e4,0x1e5,0x1e6,0x1e7,0x1e8,0x1e9,
  0x1ea,0x1eb,0x1ec,0x1ed,0x1ee,0x1ef,0x1f0,0x1f1,
  0x1f2,0x1f3,0x1f4,0x1f5,0x1f6,0x1f7,0x1f8,0x1f9,
  0x19a,0x19b,0x19c,0x19d,0x19e,0x19f,0x1a0,0x1a1,
  0x1a2,0x1a3,0x1a4,0x1a5,0x1a6,0x1a7,0x1a8,0x1a9,
  0x1aa,0x1ab,0x1ac,0x1ad,0x1ae,0x1af,0x1b0,0x1b1,
  0x1b2,0x1b3,0x1b4,0x1b5,0x1b6,0x1b7,0x1b8,0x1b9,
  0x1ba,0x1bb,0x1bc,0x1bd,0x1be,0x1bf,0x1c0,0x1c1,
  0x1c2,0x1c3,0x1c4,0x1c5,0x1c6,0x1c7,0x1c8,0x1c9,
  0x1ca,0x1cb,0x1cc,0x1cd,0x1ce,0x1cf,0x1d0,0x1d1,
  0x1d2,0x1d3,0x1d4,0x1d5,0x1d6,0x1d7,0x1d8,0x1d9,
  0x1da,0x1db,0x1dc,0x1dd,0x1de,0x1df,0x1e0,0x1e1,
  0x1e2,0x1e3,0x1e4,0x1e5,0x1e6,0x1e7,0x1e8,0x1e9,
  0x1ea,0x1eb,0x1ec,0x1ed,0x1ee,0x1ef,0x1f0,0x1f1,
  0x1f2,0x1f3,0x1f4,0x1f5,0x1f6,0x1f7,0x1f8,0x1f9,
  0x1fa,0x1fb,0x1fc,0x1fd,0x1fe,0x1ff,0x100,0x101,
  0x102,0x103,0x104,0x105,0x106,0x107,0x108,0x109,
  0x10a,0x10b,0x10c,0x10d,0x10e,0x10f,0x110,0x111,
  0x112,0x113,0x114,0x115,0x116,0x117,0x118,0x119,
  0x11a,0x11b,0x11c,0x11d,0x11e,0x11f,0x120,0x121,
  0x122,0x123,0x124,0x125,0x126,0x127,0x128,0x129,
  0x12a,0x12b,0x12c,0x12d,0x12e,0x12f,0x130,0x131,
  0x132,0x133,0x134,0x135,0x136,0x137,0x138,0x139,
  0x13a,0x13b,0x13c,0x13d,0x13e,0x13f,0x140,0x141,
  0x142,0x143,0x144,0x145,0x146,0x147,0x148,0x149,
  0x14a,0x14b,0x14c,0x14d,0x14e,0x14f,0x150,0x151,
  0x152,0x153,0x154,0x155,0x156,0x157,0x158,0x159,
  0x15a,0x15b,0x15c,0x15d,0x15e,0x15f,0x160,0x161,
  0x162,0x163,0x164,0x165,0x166,0x167,0x168,0x169,
  0x16a,0x16b,0x16c,0x16d,0x16e,0x16f,0x170,0x171,
  0x172,0x173,0x174,0x175,0x176,0x177,0x178,0x179,
  0x17a,0x17b,0x17c,0x17d,0x17e,0x17f,0x180,0x181,
  0x182,0x183,0x184,0x185,0x186,0x187,0x188,0x189,
  0x18a,0x18b,0x18c,0x18d,0x18e,0x18f,0x190,0x191,
  0x192,0x193,0x194,0x195,0x196,0x197,0x198,0x199,
};
