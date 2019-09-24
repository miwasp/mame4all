typedef int (*miread8_func)(unsigned int);

unsigned miread8_offs[FAME_N_ENTRIES*MAX_CONTEXTS];
miread8_func miread8_funcs[FAME_N_ENTRIES*MAX_CONTEXTS];

#define MIREAD8_FUNC(NAME) \
  static int miread8_##NAME (unsigned int address) \
  { \
    /* int data, shift = ((address ^ 1) & 1) << 3; */ \
    /* address &= ~1; */ \
    /* data = miread8_funcs[(NAME)](address - miread8_offs[(NAME)]); */ \
    /* return (data >> shift) & 0xff; */ \
    return ((miread8_funcs[(NAME)])((address & ~1) - miread8_offs[(NAME)]) >> (((address & 1) << 3) ^ 8)) & 0xff; \
  }

MIREAD8_FUNC(0)
MIREAD8_FUNC(1)
MIREAD8_FUNC(2)
MIREAD8_FUNC(3)
MIREAD8_FUNC(4)
MIREAD8_FUNC(5)
MIREAD8_FUNC(6)
MIREAD8_FUNC(7)
MIREAD8_FUNC(8)
MIREAD8_FUNC(9)
MIREAD8_FUNC(10)
MIREAD8_FUNC(11)
MIREAD8_FUNC(12)
MIREAD8_FUNC(13)
MIREAD8_FUNC(14)
MIREAD8_FUNC(15)
MIREAD8_FUNC(16)
MIREAD8_FUNC(17)
MIREAD8_FUNC(18)
MIREAD8_FUNC(19)
MIREAD8_FUNC(20)
MIREAD8_FUNC(21)
MIREAD8_FUNC(22)
MIREAD8_FUNC(23)
MIREAD8_FUNC(24)
MIREAD8_FUNC(25)
MIREAD8_FUNC(26)
MIREAD8_FUNC(27)
MIREAD8_FUNC(28)
MIREAD8_FUNC(29)
MIREAD8_FUNC(30)
MIREAD8_FUNC(31)
#if ( FAME_N_ENTRIES > 32 || MAX_CONTEXTS > 1)
  MIREAD8_FUNC(32)
  MIREAD8_FUNC(33)
  MIREAD8_FUNC(34)
  MIREAD8_FUNC(35)
  MIREAD8_FUNC(36)
  MIREAD8_FUNC(37)
  MIREAD8_FUNC(38)
  MIREAD8_FUNC(39)
  MIREAD8_FUNC(40)
  MIREAD8_FUNC(41)
  MIREAD8_FUNC(42)
  MIREAD8_FUNC(43)
  MIREAD8_FUNC(44)
  MIREAD8_FUNC(45)
  MIREAD8_FUNC(46)
  MIREAD8_FUNC(47)
  MIREAD8_FUNC(48)
  MIREAD8_FUNC(49)
  MIREAD8_FUNC(50)
  MIREAD8_FUNC(51)
  MIREAD8_FUNC(52)
  MIREAD8_FUNC(53)
  MIREAD8_FUNC(54)
  MIREAD8_FUNC(55)
  MIREAD8_FUNC(56)
  MIREAD8_FUNC(57)
  MIREAD8_FUNC(58)
  MIREAD8_FUNC(59)
  MIREAD8_FUNC(60)
  MIREAD8_FUNC(61)
  MIREAD8_FUNC(62)
  MIREAD8_FUNC(63)
#endif
#if (MAX_CONTEXTS > 1 && FAME_N_ENTRIES > 32)
  MIREAD8_FUNC(64)
  MIREAD8_FUNC(65)
  MIREAD8_FUNC(66)
  MIREAD8_FUNC(67)
  MIREAD8_FUNC(68)
  MIREAD8_FUNC(69)
  MIREAD8_FUNC(70)
  MIREAD8_FUNC(71)
  MIREAD8_FUNC(72)
  MIREAD8_FUNC(73)
  MIREAD8_FUNC(74)
  MIREAD8_FUNC(75)
  MIREAD8_FUNC(76)
  MIREAD8_FUNC(77)
  MIREAD8_FUNC(78)
  MIREAD8_FUNC(79)
  MIREAD8_FUNC(80)
  MIREAD8_FUNC(81)
  MIREAD8_FUNC(82)
  MIREAD8_FUNC(83)
  MIREAD8_FUNC(84)
  MIREAD8_FUNC(85)
  MIREAD8_FUNC(86)
  MIREAD8_FUNC(87)
  MIREAD8_FUNC(88)
  MIREAD8_FUNC(89)
  MIREAD8_FUNC(90)
  MIREAD8_FUNC(91)
  MIREAD8_FUNC(92)
  MIREAD8_FUNC(93)
  MIREAD8_FUNC(94)
  MIREAD8_FUNC(95)
  MIREAD8_FUNC(96)
  MIREAD8_FUNC(97)
  MIREAD8_FUNC(98)
  MIREAD8_FUNC(99)
  MIREAD8_FUNC(100)
  MIREAD8_FUNC(101)
  MIREAD8_FUNC(102)
  MIREAD8_FUNC(103)
  MIREAD8_FUNC(104)
  MIREAD8_FUNC(105)
  MIREAD8_FUNC(106)
  MIREAD8_FUNC(107)
  MIREAD8_FUNC(108)
  MIREAD8_FUNC(109)
  MIREAD8_FUNC(110)
  MIREAD8_FUNC(111)
  MIREAD8_FUNC(112)
  MIREAD8_FUNC(113)
  MIREAD8_FUNC(114)
  MIREAD8_FUNC(115)
  MIREAD8_FUNC(116)
  MIREAD8_FUNC(117)
  MIREAD8_FUNC(118)
  MIREAD8_FUNC(119)
  MIREAD8_FUNC(120)
  MIREAD8_FUNC(121)
  MIREAD8_FUNC(122)
  MIREAD8_FUNC(123)
  MIREAD8_FUNC(124)
  MIREAD8_FUNC(125)
  MIREAD8_FUNC(126)
  MIREAD8_FUNC(127)
#endif

miread8_func miread8_funcs_indirect[]=
{
  miread8_0, miread8_1, miread8_2, miread8_3, miread8_4, miread8_5, miread8_6, miread8_7,
  miread8_8, miread8_9, miread8_10, miread8_11, miread8_12, miread8_13, miread8_14, miread8_15,
  miread8_16, miread8_17, miread8_18, miread8_19, miread8_20, miread8_21, miread8_22, miread8_23,
  miread8_24, miread8_25, miread8_26, miread8_27, miread8_28, miread8_29, miread8_30, miread8_31,
#if ( FAME_N_ENTRIES > 32 || MAX_CONTEXTS > 1)
  miread8_32, miread8_33, miread8_34, miread8_35, miread8_36, miread8_37, miread8_38, miread8_39,
  miread8_40, miread8_41, miread8_42, miread8_43, miread8_44, miread8_45, miread8_46, miread8_47,
  miread8_48, miread8_49, miread8_50, miread8_51, miread8_52, miread8_53, miread8_54, miread8_55,
  miread8_56, miread8_57, miread8_58, miread8_59, miread8_60, miread8_61, miread8_62, miread8_63,
#endif
#if (MAX_CONTEXTS > 1 && FAME_N_ENTRIES > 32)
  miread8_64, miread8_65, miread8_66, miread8_67, miread8_68, miread8_69, miread8_70, miread8_71,
  miread8_72, miread8_73, miread8_74, miread8_75, miread8_76, miread8_77, miread8_78, miread8_79,
  miread8_80, miread8_81, miread8_82, miread8_83, miread8_84, miread8_85, miread8_86, miread8_87,
  miread8_88, miread8_89, miread8_90, miread8_91, miread8_92, miread8_93, miread8_94, miread8_95,
  miread8_96, miread8_97, miread8_98, miread8_99, miread8_100, miread8_101, miread8_102, miread8_103,
  miread8_104, miread8_105, miread8_106, miread8_107, miread8_108, miread8_109, miread8_110, miread8_111,
  miread8_112, miread8_113, miread8_114, miread8_115, miread8_116, miread8_117, miread8_118, miread8_119,
  miread8_120, miread8_121, miread8_122, miread8_123, miread8_124, miread8_125, miread8_126, miread8_127
#endif
};


typedef int (*miread16_func)(unsigned int);

unsigned miread16_offs[FAME_N_ENTRIES*MAX_CONTEXTS];
miread16_func miread16_funcs[FAME_N_ENTRIES*MAX_CONTEXTS];

#define MIREAD16_FUNC(NAME) \
  static int miread16_##NAME (unsigned int address) \
  { \
    return miread16_funcs[(NAME)](address - miread16_offs[(NAME)]); \
  }


MIREAD16_FUNC(0)
MIREAD16_FUNC(1)
MIREAD16_FUNC(2)
MIREAD16_FUNC(3)
MIREAD16_FUNC(4)
MIREAD16_FUNC(5)
MIREAD16_FUNC(6)
MIREAD16_FUNC(7)
MIREAD16_FUNC(8)
MIREAD16_FUNC(9)
MIREAD16_FUNC(10)
MIREAD16_FUNC(11)
MIREAD16_FUNC(12)
MIREAD16_FUNC(13)
MIREAD16_FUNC(14)
MIREAD16_FUNC(15)
MIREAD16_FUNC(16)
MIREAD16_FUNC(17)
MIREAD16_FUNC(18)
MIREAD16_FUNC(19)
MIREAD16_FUNC(20)
MIREAD16_FUNC(21)
MIREAD16_FUNC(22)
MIREAD16_FUNC(23)
MIREAD16_FUNC(24)
MIREAD16_FUNC(25)
MIREAD16_FUNC(26)
MIREAD16_FUNC(27)
MIREAD16_FUNC(28)
MIREAD16_FUNC(29)
MIREAD16_FUNC(30)
MIREAD16_FUNC(31)
#if ( FAME_N_ENTRIES > 32 || MAX_CONTEXTS > 1)
  MIREAD16_FUNC(32)
  MIREAD16_FUNC(33)
  MIREAD16_FUNC(34)
  MIREAD16_FUNC(35)
  MIREAD16_FUNC(36)
  MIREAD16_FUNC(37)
  MIREAD16_FUNC(38)
  MIREAD16_FUNC(39)
  MIREAD16_FUNC(40)
  MIREAD16_FUNC(41)
  MIREAD16_FUNC(42)
  MIREAD16_FUNC(43)
  MIREAD16_FUNC(44)
  MIREAD16_FUNC(45)
  MIREAD16_FUNC(46)
  MIREAD16_FUNC(47)
  MIREAD16_FUNC(48)
  MIREAD16_FUNC(49)
  MIREAD16_FUNC(50)
  MIREAD16_FUNC(51)
  MIREAD16_FUNC(52)
  MIREAD16_FUNC(53)
  MIREAD16_FUNC(54)
  MIREAD16_FUNC(55)
  MIREAD16_FUNC(56)
  MIREAD16_FUNC(57)
  MIREAD16_FUNC(58)
  MIREAD16_FUNC(59)
  MIREAD16_FUNC(60)
  MIREAD16_FUNC(61)
  MIREAD16_FUNC(62)
  MIREAD16_FUNC(63)
#endif
#if (MAX_CONTEXTS > 1 && FAME_N_ENTRIES > 32)
  MIREAD16_FUNC(64)
  MIREAD16_FUNC(65)
  MIREAD16_FUNC(66)
  MIREAD16_FUNC(67)
  MIREAD16_FUNC(68)
  MIREAD16_FUNC(69)
  MIREAD16_FUNC(70)
  MIREAD16_FUNC(71)
  MIREAD16_FUNC(72)
  MIREAD16_FUNC(73)
  MIREAD16_FUNC(74)
  MIREAD16_FUNC(75)
  MIREAD16_FUNC(76)
  MIREAD16_FUNC(77)
  MIREAD16_FUNC(78)
  MIREAD16_FUNC(79)
  MIREAD16_FUNC(80)
  MIREAD16_FUNC(81)
  MIREAD16_FUNC(82)
  MIREAD16_FUNC(83)
  MIREAD16_FUNC(84)
  MIREAD16_FUNC(85)
  MIREAD16_FUNC(86)
  MIREAD16_FUNC(87)
  MIREAD16_FUNC(88)
  MIREAD16_FUNC(89)
  MIREAD16_FUNC(90)
  MIREAD16_FUNC(91)
  MIREAD16_FUNC(92)
  MIREAD16_FUNC(93)
  MIREAD16_FUNC(94)
  MIREAD16_FUNC(95)
  MIREAD16_FUNC(96)
  MIREAD16_FUNC(97)
  MIREAD16_FUNC(98)
  MIREAD16_FUNC(99)
  MIREAD16_FUNC(100)
  MIREAD16_FUNC(101)
  MIREAD16_FUNC(102)
  MIREAD16_FUNC(103)
  MIREAD16_FUNC(104)
  MIREAD16_FUNC(105)
  MIREAD16_FUNC(106)
  MIREAD16_FUNC(107)
  MIREAD16_FUNC(108)
  MIREAD16_FUNC(109)
  MIREAD16_FUNC(110)
  MIREAD16_FUNC(111)
  MIREAD16_FUNC(112)
  MIREAD16_FUNC(113)
  MIREAD16_FUNC(114)
  MIREAD16_FUNC(115)
  MIREAD16_FUNC(116)
  MIREAD16_FUNC(117)
  MIREAD16_FUNC(118)
  MIREAD16_FUNC(119)
  MIREAD16_FUNC(120)
  MIREAD16_FUNC(121)
  MIREAD16_FUNC(122)
  MIREAD16_FUNC(123)
  MIREAD16_FUNC(124)
  MIREAD16_FUNC(125)
  MIREAD16_FUNC(126)
  MIREAD16_FUNC(127)
#endif

miread16_func miread16_funcs_indirect[]=
{
  miread16_0, miread16_1, miread16_2, miread16_3, miread16_4, miread16_5, miread16_6, miread16_7,
  miread16_8, miread16_9, miread16_10, miread16_11, miread16_12, miread16_13, miread16_14, miread16_15,
  miread16_16, miread16_17, miread16_18, miread16_19, miread16_20, miread16_21, miread16_22, miread16_23,
  miread16_24, miread16_25, miread16_26, miread16_27, miread16_28, miread16_29, miread16_30, miread16_31,
#if ( FAME_N_ENTRIES > 32 || MAX_CONTEXTS > 1)
  miread16_32, miread16_33, miread16_34, miread16_35, miread16_36, miread16_37, miread16_38, miread16_39,
  miread16_40, miread16_41, miread16_42, miread16_43, miread16_44, miread16_45, miread16_46, miread16_47,
  miread16_48, miread16_49, miread16_50, miread16_51, miread16_52, miread16_53, miread16_54, miread16_55,
  miread16_56, miread16_57, miread16_58, miread16_59, miread16_60, miread16_61, miread16_62, miread16_63,
#endif
#if (MAX_CONTEXTS > 1 && FAME_N_ENTRIES > 32)
  miread16_64, miread16_65, miread16_66, miread16_67, miread16_68, miread16_69, miread16_70, miread16_71,
  miread16_72, miread16_73, miread16_74, miread16_75, miread16_76, miread16_77, miread16_78, miread16_79,
  miread16_80, miread16_81, miread16_82, miread16_83, miread16_84, miread16_85, miread16_86, miread16_87,
  miread16_88, miread16_89, miread16_90, miread16_91, miread16_92, miread16_93, miread16_94, miread16_95,
  miread16_96, miread16_97, miread16_98, miread16_99, miread16_100, miread16_101, miread16_102, miread16_103,
  miread16_104, miread16_105, miread16_106, miread16_107, miread16_108, miread16_109, miread16_110, miread16_111,
  miread16_112, miread16_113, miread16_114, miread16_115, miread16_116, miread16_117, miread16_118, miread16_119,
  miread16_120, miread16_121, miread16_122, miread16_123, miread16_124, miread16_125, miread16_126, miread16_127
#endif
};




typedef void (*miwrite8_func)(unsigned int, int);

unsigned miwrite8_offs[FAME_N_ENTRIES*MAX_CONTEXTS];
miwrite8_func miwrite8_funcs[FAME_N_ENTRIES*MAX_CONTEXTS];

#define MIWRITE8_FUNC(NAME) \
  static void miwrite8_##NAME (unsigned int address, int data) \
  { \
    /* int shift = ((address ^ 1) & 1) << 3; */ \
    /* address &= ~1; */ \
    /* miwrite8_funcs[(NAME)](address - miwrite8_offs[(NAME)],(0xff000000 >> shift) | ((data & 0xff) << shift)); */ \
    int shift = ((address & 1) << 3)^8;  \
    data = (0xff000000 >> shift) | ((data & 0xff) << shift); \
    address &= ~1; \
    miwrite8_funcs[(NAME)](address - miwrite8_offs[(NAME)], data); \
  }


MIWRITE8_FUNC(0)
MIWRITE8_FUNC(1)
MIWRITE8_FUNC(2)
MIWRITE8_FUNC(3)
MIWRITE8_FUNC(4)
MIWRITE8_FUNC(5)
MIWRITE8_FUNC(6)
MIWRITE8_FUNC(7)
MIWRITE8_FUNC(8)
MIWRITE8_FUNC(9)
MIWRITE8_FUNC(10)
MIWRITE8_FUNC(11)
MIWRITE8_FUNC(12)
MIWRITE8_FUNC(13)
MIWRITE8_FUNC(14)
MIWRITE8_FUNC(15)
MIWRITE8_FUNC(16)
MIWRITE8_FUNC(17)
MIWRITE8_FUNC(18)
MIWRITE8_FUNC(19)
MIWRITE8_FUNC(20)
MIWRITE8_FUNC(21)
MIWRITE8_FUNC(22)
MIWRITE8_FUNC(23)
MIWRITE8_FUNC(24)
MIWRITE8_FUNC(25)
MIWRITE8_FUNC(26)
MIWRITE8_FUNC(27)
MIWRITE8_FUNC(28)
MIWRITE8_FUNC(29)
MIWRITE8_FUNC(30)
MIWRITE8_FUNC(31)
#if ( FAME_N_ENTRIES > 32 || MAX_CONTEXTS > 1)
  MIWRITE8_FUNC(32)
  MIWRITE8_FUNC(33)
  MIWRITE8_FUNC(34)
  MIWRITE8_FUNC(35)
  MIWRITE8_FUNC(36)
  MIWRITE8_FUNC(37)
  MIWRITE8_FUNC(38)
  MIWRITE8_FUNC(39)
  MIWRITE8_FUNC(40)
  MIWRITE8_FUNC(41)
  MIWRITE8_FUNC(42)
  MIWRITE8_FUNC(43)
  MIWRITE8_FUNC(44)
  MIWRITE8_FUNC(45)
  MIWRITE8_FUNC(46)
  MIWRITE8_FUNC(47)
  MIWRITE8_FUNC(48)
  MIWRITE8_FUNC(49)
  MIWRITE8_FUNC(50)
  MIWRITE8_FUNC(51)
  MIWRITE8_FUNC(52)
  MIWRITE8_FUNC(53)
  MIWRITE8_FUNC(54)
  MIWRITE8_FUNC(55)
  MIWRITE8_FUNC(56)
  MIWRITE8_FUNC(57)
  MIWRITE8_FUNC(58)
  MIWRITE8_FUNC(59)
  MIWRITE8_FUNC(60)
  MIWRITE8_FUNC(61)
  MIWRITE8_FUNC(62)
  MIWRITE8_FUNC(63)
#endif
#if (MAX_CONTEXTS > 1 && FAME_N_ENTRIES > 32)
  MIWRITE8_FUNC(64)
  MIWRITE8_FUNC(65)
  MIWRITE8_FUNC(66)
  MIWRITE8_FUNC(67)
  MIWRITE8_FUNC(68)
  MIWRITE8_FUNC(69)
  MIWRITE8_FUNC(70)
  MIWRITE8_FUNC(71)
  MIWRITE8_FUNC(72)
  MIWRITE8_FUNC(73)
  MIWRITE8_FUNC(74)
  MIWRITE8_FUNC(75)
  MIWRITE8_FUNC(76)
  MIWRITE8_FUNC(77)
  MIWRITE8_FUNC(78)
  MIWRITE8_FUNC(79)
  MIWRITE8_FUNC(80)
  MIWRITE8_FUNC(81)
  MIWRITE8_FUNC(82)
  MIWRITE8_FUNC(83)
  MIWRITE8_FUNC(84)
  MIWRITE8_FUNC(85)
  MIWRITE8_FUNC(86)
  MIWRITE8_FUNC(87)
  MIWRITE8_FUNC(88)
  MIWRITE8_FUNC(89)
  MIWRITE8_FUNC(90)
  MIWRITE8_FUNC(91)
  MIWRITE8_FUNC(92)
  MIWRITE8_FUNC(93)
  MIWRITE8_FUNC(94)
  MIWRITE8_FUNC(95)
  MIWRITE8_FUNC(96)
  MIWRITE8_FUNC(97)
  MIWRITE8_FUNC(98)
  MIWRITE8_FUNC(99)
  MIWRITE8_FUNC(100)
  MIWRITE8_FUNC(101)
  MIWRITE8_FUNC(102)
  MIWRITE8_FUNC(103)
  MIWRITE8_FUNC(104)
  MIWRITE8_FUNC(105)
  MIWRITE8_FUNC(106)
  MIWRITE8_FUNC(107)
  MIWRITE8_FUNC(108)
  MIWRITE8_FUNC(109)
  MIWRITE8_FUNC(110)
  MIWRITE8_FUNC(111)
  MIWRITE8_FUNC(112)
  MIWRITE8_FUNC(113)
  MIWRITE8_FUNC(114)
  MIWRITE8_FUNC(115)
  MIWRITE8_FUNC(116)
  MIWRITE8_FUNC(117)
  MIWRITE8_FUNC(118)
  MIWRITE8_FUNC(119)
  MIWRITE8_FUNC(120)
  MIWRITE8_FUNC(121)
  MIWRITE8_FUNC(122)
  MIWRITE8_FUNC(123)
  MIWRITE8_FUNC(124)
  MIWRITE8_FUNC(125)
  MIWRITE8_FUNC(126)
  MIWRITE8_FUNC(127)
#endif

miwrite8_func miwrite8_funcs_indirect[]=
{
  miwrite8_0, miwrite8_1, miwrite8_2, miwrite8_3, miwrite8_4, miwrite8_5, miwrite8_6, miwrite8_7,
  miwrite8_8, miwrite8_9, miwrite8_10, miwrite8_11, miwrite8_12, miwrite8_13, miwrite8_14, miwrite8_15,
  miwrite8_16, miwrite8_17, miwrite8_18, miwrite8_19, miwrite8_20, miwrite8_21, miwrite8_22, miwrite8_23,
  miwrite8_24, miwrite8_25, miwrite8_26, miwrite8_27, miwrite8_28, miwrite8_29, miwrite8_30, miwrite8_31,
#if ( FAME_N_ENTRIES > 32 || MAX_CONTEXTS > 1)
  miwrite8_32, miwrite8_33, miwrite8_34, miwrite8_35, miwrite8_36, miwrite8_37, miwrite8_38, miwrite8_39,
  miwrite8_40, miwrite8_41, miwrite8_42, miwrite8_43, miwrite8_44, miwrite8_45, miwrite8_46, miwrite8_47,
  miwrite8_48, miwrite8_49, miwrite8_50, miwrite8_51, miwrite8_52, miwrite8_53, miwrite8_54, miwrite8_55,
  miwrite8_56, miwrite8_57, miwrite8_58, miwrite8_59, miwrite8_60, miwrite8_61, miwrite8_62, miwrite8_63,
#endif
#if (MAX_CONTEXTS > 1 && FAME_N_ENTRIES > 32)
  miwrite8_64, miwrite8_65, miwrite8_66, miwrite8_67, miwrite8_68, miwrite8_69, miwrite8_70, miwrite8_71,
  miwrite8_72, miwrite8_73, miwrite8_74, miwrite8_75, miwrite8_76, miwrite8_77, miwrite8_78, miwrite8_79,
  miwrite8_80, miwrite8_81, miwrite8_82, miwrite8_83, miwrite8_84, miwrite8_85, miwrite8_86, miwrite8_87,
  miwrite8_88, miwrite8_89, miwrite8_90, miwrite8_91, miwrite8_92, miwrite8_93, miwrite8_94, miwrite8_95,
  miwrite8_96, miwrite8_97, miwrite8_98, miwrite8_99, miwrite8_100, miwrite8_101, miwrite8_102, miwrite8_103,
  miwrite8_104, miwrite8_105, miwrite8_106, miwrite8_107, miwrite8_108, miwrite8_109, miwrite8_110, miwrite8_111,
  miwrite8_112, miwrite8_113, miwrite8_114, miwrite8_115, miwrite8_116, miwrite8_117, miwrite8_118, miwrite8_119,
  miwrite8_120, miwrite8_121, miwrite8_122, miwrite8_123, miwrite8_124, miwrite8_125, miwrite8_126, miwrite8_127
#endif
};



typedef void (*miwrite16_func)(unsigned int, int);

unsigned miwrite16_offs[FAME_N_ENTRIES*MAX_CONTEXTS];
miwrite16_func miwrite16_funcs[FAME_N_ENTRIES*MAX_CONTEXTS];

#define MIWRITE16_FUNC(NAME) \
  static void miwrite16_##NAME (unsigned int address, int data) \
  { \
    miwrite16_funcs[(NAME)](address - miwrite16_offs[(NAME)], data&0xffff ); \
  }

MIWRITE16_FUNC(0)
MIWRITE16_FUNC(1)
MIWRITE16_FUNC(2)
MIWRITE16_FUNC(3)
MIWRITE16_FUNC(4)
MIWRITE16_FUNC(5)
MIWRITE16_FUNC(6)
MIWRITE16_FUNC(7)
MIWRITE16_FUNC(8)
MIWRITE16_FUNC(9)
MIWRITE16_FUNC(10)
MIWRITE16_FUNC(11)
MIWRITE16_FUNC(12)
MIWRITE16_FUNC(13)
MIWRITE16_FUNC(14)
MIWRITE16_FUNC(15)
MIWRITE16_FUNC(16)
MIWRITE16_FUNC(17)
MIWRITE16_FUNC(18)
MIWRITE16_FUNC(19)
MIWRITE16_FUNC(20)
MIWRITE16_FUNC(21)
MIWRITE16_FUNC(22)
MIWRITE16_FUNC(23)
MIWRITE16_FUNC(24)
MIWRITE16_FUNC(25)
MIWRITE16_FUNC(26)
MIWRITE16_FUNC(27)
MIWRITE16_FUNC(28)
MIWRITE16_FUNC(29)
MIWRITE16_FUNC(30)
MIWRITE16_FUNC(31)
#if ( FAME_N_ENTRIES > 32 || MAX_CONTEXTS > 1)
  MIWRITE16_FUNC(32)
  MIWRITE16_FUNC(33)
  MIWRITE16_FUNC(34)
  MIWRITE16_FUNC(35)
  MIWRITE16_FUNC(36)
  MIWRITE16_FUNC(37)
  MIWRITE16_FUNC(38)
  MIWRITE16_FUNC(39)
  MIWRITE16_FUNC(40)
  MIWRITE16_FUNC(41)
  MIWRITE16_FUNC(42)
  MIWRITE16_FUNC(43)
  MIWRITE16_FUNC(44)
  MIWRITE16_FUNC(45)
  MIWRITE16_FUNC(46)
  MIWRITE16_FUNC(47)
  MIWRITE16_FUNC(48)
  MIWRITE16_FUNC(49)
  MIWRITE16_FUNC(50)
  MIWRITE16_FUNC(51)
  MIWRITE16_FUNC(52)
  MIWRITE16_FUNC(53)
  MIWRITE16_FUNC(54)
  MIWRITE16_FUNC(55)
  MIWRITE16_FUNC(56)
  MIWRITE16_FUNC(57)
  MIWRITE16_FUNC(58)
  MIWRITE16_FUNC(59)
  MIWRITE16_FUNC(60)
  MIWRITE16_FUNC(61)
  MIWRITE16_FUNC(62)
  MIWRITE16_FUNC(63)
#endif
#if (MAX_CONTEXTS > 1 && FAME_N_ENTRIES > 32)
  MIWRITE16_FUNC(64)
  MIWRITE16_FUNC(65)
  MIWRITE16_FUNC(66)
  MIWRITE16_FUNC(67)
  MIWRITE16_FUNC(68)
  MIWRITE16_FUNC(69)
  MIWRITE16_FUNC(70)
  MIWRITE16_FUNC(71)
  MIWRITE16_FUNC(72)
  MIWRITE16_FUNC(73)
  MIWRITE16_FUNC(74)
  MIWRITE16_FUNC(75)
  MIWRITE16_FUNC(76)
  MIWRITE16_FUNC(77)
  MIWRITE16_FUNC(78)
  MIWRITE16_FUNC(79)
  MIWRITE16_FUNC(80)
  MIWRITE16_FUNC(81)
  MIWRITE16_FUNC(82)
  MIWRITE16_FUNC(83)
  MIWRITE16_FUNC(84)
  MIWRITE16_FUNC(85)
  MIWRITE16_FUNC(86)
  MIWRITE16_FUNC(87)
  MIWRITE16_FUNC(88)
  MIWRITE16_FUNC(89)
  MIWRITE16_FUNC(90)
  MIWRITE16_FUNC(91)
  MIWRITE16_FUNC(92)
  MIWRITE16_FUNC(93)
  MIWRITE16_FUNC(94)
  MIWRITE16_FUNC(95)
  MIWRITE16_FUNC(96)
  MIWRITE16_FUNC(97)
  MIWRITE16_FUNC(98)
  MIWRITE16_FUNC(99)
  MIWRITE16_FUNC(100)
  MIWRITE16_FUNC(101)
  MIWRITE16_FUNC(102)
  MIWRITE16_FUNC(103)
  MIWRITE16_FUNC(104)
  MIWRITE16_FUNC(105)
  MIWRITE16_FUNC(106)
  MIWRITE16_FUNC(107)
  MIWRITE16_FUNC(108)
  MIWRITE16_FUNC(109)
  MIWRITE16_FUNC(110)
  MIWRITE16_FUNC(111)
  MIWRITE16_FUNC(112)
  MIWRITE16_FUNC(113)
  MIWRITE16_FUNC(114)
  MIWRITE16_FUNC(115)
  MIWRITE16_FUNC(116)
  MIWRITE16_FUNC(117)
  MIWRITE16_FUNC(118)
  MIWRITE16_FUNC(119)
  MIWRITE16_FUNC(120)
  MIWRITE16_FUNC(121)
  MIWRITE16_FUNC(122)
  MIWRITE16_FUNC(123)
  MIWRITE16_FUNC(124)
  MIWRITE16_FUNC(125)
  MIWRITE16_FUNC(126)
  MIWRITE16_FUNC(127)
#endif

miwrite16_func miwrite16_funcs_indirect[]=
{
  miwrite16_0, miwrite16_1, miwrite16_2, miwrite16_3, miwrite16_4, miwrite16_5, miwrite16_6, miwrite16_7,
  miwrite16_8, miwrite16_9, miwrite16_10, miwrite16_11, miwrite16_12, miwrite16_13, miwrite16_14, miwrite16_15,
  miwrite16_16, miwrite16_17, miwrite16_18, miwrite16_19, miwrite16_20, miwrite16_21, miwrite16_22, miwrite16_23,
  miwrite16_24, miwrite16_25, miwrite16_26, miwrite16_27, miwrite16_28, miwrite16_29, miwrite16_30, miwrite16_31,
#if ( FAME_N_ENTRIES > 32 || MAX_CONTEXTS > 1)
  miwrite16_32, miwrite16_33, miwrite16_34, miwrite16_35, miwrite16_36, miwrite16_37, miwrite16_38, miwrite16_39,
  miwrite16_40, miwrite16_41, miwrite16_42, miwrite16_43, miwrite16_44, miwrite16_45, miwrite16_46, miwrite16_47,
  miwrite16_48, miwrite16_49, miwrite16_50, miwrite16_51, miwrite16_52, miwrite16_53, miwrite16_54, miwrite16_55,
  miwrite16_56, miwrite16_57, miwrite16_58, miwrite16_59, miwrite16_60, miwrite16_61, miwrite16_62, miwrite16_63,
#endif
#if (MAX_CONTEXTS > 1 && FAME_N_ENTRIES > 32)
  miwrite16_64, miwrite16_65, miwrite16_66, miwrite16_67, miwrite16_68, miwrite16_69, miwrite16_70, miwrite16_71,
  miwrite16_72, miwrite16_73, miwrite16_74, miwrite16_75, miwrite16_76, miwrite16_77, miwrite16_78, miwrite16_79,
  miwrite16_80, miwrite16_81, miwrite16_82, miwrite16_83, miwrite16_84, miwrite16_85, miwrite16_86, miwrite16_87,
  miwrite16_88, miwrite16_89, miwrite16_90, miwrite16_91, miwrite16_92, miwrite16_93, miwrite16_94, miwrite16_95,
  miwrite16_96, miwrite16_97, miwrite16_98, miwrite16_99, miwrite16_100, miwrite16_101, miwrite16_102, miwrite16_103,
  miwrite16_104, miwrite16_105, miwrite16_106, miwrite16_107, miwrite16_108, miwrite16_109, miwrite16_110, miwrite16_111,
  miwrite16_112, miwrite16_113, miwrite16_114, miwrite16_115, miwrite16_116, miwrite16_117, miwrite16_118, miwrite16_119,
  miwrite16_120, miwrite16_121, miwrite16_122, miwrite16_123, miwrite16_124, miwrite16_125, miwrite16_126, miwrite16_127
#endif
};
