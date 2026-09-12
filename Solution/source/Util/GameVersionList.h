#pragma once

// Single source of truth for eGameVersion. X() = sequential enumerator,
// XV() = enumerator with an explicit value. Second arg is the display
// string used by VersionString() in FileLogger.cpp.
#define GAME_VERSION_LIST \
	X(VER_1_0_335_2_STEAM,      "b335") \
	X(VER_1_0_335_2_NOSTEAM,    "b335") \
	X(VER_1_0_350_1_STEAM,      "b350") \
	X(VER_1_0_350_2_NOSTEAM,    "b350") \
	X(VER_1_0_372_2_STEAM,      "b372") \
	X(VER_1_0_372_2_NOSTEAM,    "b372") \
	X(VER_1_0_393_2_STEAM,      "b393") \
	X(VER_1_0_393_2_NOSTEAM,    "b393") \
	X(VER_1_0_393_4_STEAM,      "b393") \
	X(VER_1_0_393_4_NOSTEAM,    "b393") \
	X(VER_1_0_463_1_STEAM,      "b463") \
	X(VER_1_0_463_1_NOSTEAM,    "b463") \
	X(VER_1_0_505_2_STEAM,      "b505") \
	X(VER_1_0_505_2_NOSTEAM,    "b505") \
	X(VER_1_0_573_1_STEAM,      "b573") \
	X(VER_1_0_573_1_NOSTEAM,    "b573") \
	X(VER_1_0_617_1_STEAM,      "b617") \
	X(VER_1_0_617_1_NOSTEAM,    "b617") \
	X(VER_1_0_678_1_STEAM,      "b678") \
	X(VER_1_0_678_1_NOSTEAM,    "b678") \
	X(VER_1_0_757_2_STEAM,      "b757") \
	X(VER_1_0_757_2_NOSTEAM,    "b757") \
	X(VER_1_0_757_4_STEAM,      "b757") \
	X(VER_1_0_757_4_NOSTEAM,    "b757") \
	X(VER_1_0_791_2_STEAM,      "b791") \
	X(VER_1_0_791_2_NOSTEAM,    "b791") \
	X(VER_1_0_877_1_STEAM,      "b877") \
	X(VER_1_0_877_1_NOSTEAM,    "b877") \
	X(VER_1_0_944_2_STEAM,      "b944") \
	X(VER_1_0_944_2_NOSTEAM,    "b944") \
	X(VER_1_0_1011_1_STEAM,     "b1011") \
	X(VER_1_0_1011_1_NOSTEAM,   "b1011") \
	X(VER_1_0_1032_1_STEAM,     "b1032") \
	X(VER_1_0_1032_1_NOSTEAM,   "b1032") \
	X(VER_1_0_1103_2_STEAM,     "b1103") \
	X(VER_1_0_1103_2_NOSTEAM,   "b1103") \
	X(VER_1_0_1180_2_STEAM,     "b1180") \
	X(VER_1_0_1180_2_NOSTEAM,   "b1180") \
	X(VER_1_0_1290_1_STEAM,     "b1290") \
	X(VER_1_0_1290_1_NOSTEAM,   "b1290") \
	X(VER_1_0_1365_1_STEAM,     "b1365") \
	X(VER_1_0_1365_1_NOSTEAM,   "b1365") \
	X(VER_1_0_1493_0_STEAM,     "b1493") \
	X(VER_1_0_1493_0_NOSTEAM,   "b1493") \
	X(VER_1_0_1493_1_STEAM,     "b1493") \
	X(VER_1_0_1493_1_NOSTEAM,   "b1493") \
	X(VER_1_0_1604_0_STEAM,     "b1604") \
	X(VER_1_0_1604_0_NOSTEAM,   "b1604") \
	X(VER_1_0_1604_1_STEAM,     "b1604") \
	X(VER_1_0_1604_1_NOSTEAM,   "b1604") \
	X(VER_1_0_1737_0_STEAM,     "b1737") \
	X(VER_1_0_1737_0_NOSTEAM,   "b1737") \
	X(VER_1_0_1737_6_STEAM,     "b1737") \
	X(VER_1_0_1737_6_NOSTEAM,   "b1737") \
	X(VER_1_0_1868_0_STEAM,     "b1868") \
	X(VER_1_0_1868_0_NOSTEAM,   "b1868") \
	X(VER_1_0_1868_1_STEAM,     "b1868") \
	X(VER_1_0_1868_1_NOSTEAM,   "b1868") \
	X(VER_1_0_1868_4_EGS,       "b1868") \
	X(VER_1_0_2060_0_STEAM,     "b2060") \
	X(VER_1_0_2060_0_NOSTEAM,   "b2060") \
	X(VER_1_0_2060_1_STEAM,     "b2060") \
	X(VER_1_0_2060_1_NOSTEAM,   "b2060") \
	X(VER_1_0_2189_0_STEAM,     "b2189") \
	X(VER_1_0_2189_0_NOSTEAM,   "b2189") \
	X(VER_1_0_2215_0_STEAM,     "b2215") \
	X(VER_1_0_2215_0_NOSTEAM,   "b2215") \
	X(VER_1_0_2245_0_STEAM,     "b2245") \
	X(VER_1_0_2245_0_NOSTEAM,   "b2245") \
	X(VER_1_0_2372_0_STEAM,     "b2372") \
	X(VER_1_0_2372_0_NOSTEAM,   "b2372") \
	X(VER_1_0_2545_0_STEAM,     "b2545") \
	X(VER_1_0_2545_0_NOSTEAM,   "b2545") \
	X(VER_1_0_2612_1_STEAM,     "b2612") \
	X(VER_1_0_2612_1_NOSTEAM,   "b2612") \
	X(VER_1_0_2628_2_STEAM,     "b2628") \
	X(VER_1_0_2628_2_NOSTEAM,   "b2628") \
	X(VER_1_0_2699_0_STEAM,     "b2699") \
	X(VER_1_0_2699_0_NOSTEAM,   "b2699") \
	X(VER_1_0_2699_16,          "b2699.16") \
	X(VER_1_0_2802_0,           "b2802") \
	X(VER_1_0_2824_0,           "b2824") \
	X(VER_1_0_2845_0,           "b2845") \
	X(VER_1_0_2944_0,           "b2944") \
	X(VER_1_0_3028_0,           "b3028") \
	X(VER_1_0_3095_0,           "b3095") \
	X(VER_1_0_3179_0,           "b3179") \
	X(VER_1_0_3258_0,           "b3258") \
	X(VER_1_0_3274_0,           "b3274") \
	X(VER_1_0_3323_0,           "b3323") \
	X(VER_1_0_3337_0,           "b3337") \
	X(VER_1_0_3351_0,           "b3351") \
	X(VER_1_0_3407_0,           "b3407") \
	X(VER_1_0_3411_0,           "b3411") \
	X(VER_1_0_3442_0,           "b3442") \
	X(VER_1_0_3504_0,           "b3504") \
	X(VER_1_0_3521_0,           "b3521") \
	X(VER_1_0_3570_0,           "b3570") \
	X(VER_1_0_3586_0,           "b3586") \
	X(VER_1_0_3717_0,           "b3717") \
	X(VER_1_0_3751_0,           "b3751") \
	X(VER_1_0_3788_0,           "b3788") \
	X(VER_1_0_3889_0,           "b3889") \
	XV(VER_1_0_811_8,   1001,   "b811.8") \
	XV(VER_1_0_812_8,   1002,   "b812.8") \
	XV(VER_1_0_813_11,  1003,   "b813.11") \
	XV(VER_1_0_814_9,   1004,   "b814.9") \
	XV(VER_1_0_889_15,  1005,   "b889.15") \
	XV(VER_1_0_889_19,  1006,   "b889.19") \
	XV(VER_1_0_889_22,  1007,   "b889.22") \
	XV(VER_1_0_1013_17, 1008,   "b1013.17") \
	XV(VER_1_0_1013_29, 1009,   "b1013.29") \
	XV(VER_1_0_1013_33, 1010,   "b1013.33") \
	XV(VER_1_0_1013_34, 1011,   "b1013.34") \
	XV(VER_1_0_1158_13, 1012,   "b1158.13")
