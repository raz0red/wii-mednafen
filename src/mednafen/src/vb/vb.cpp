/* Mednafen - Multi-system Emulator
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "vb.h"
#include "timer.h"
#include "vsu.h"
#include "vip.h"
#include "input.h"
#include "../general.h"
#include "../md5.h"
#include "../mempatcher.h"
#include <iconv.h>

#ifdef MEM2
#include "mem2.h"
#endif

#ifdef WII
#include "wii_mednafen.h"
#endif
//#define VB_SUPPORT_BIN_EXT		// Even with this enabled, any *.bin file loaded must be in the internal database to be recognized as a VB game.

namespace MDFN_IEN_VB
{

enum
{
 ANAGLYPH_PRESET_DISABLED = 0,
 ANAGLYPH_PRESET_RED_BLUE,
 ANAGLYPH_PRESET_RED_CYAN,
 ANAGLYPH_PRESET_RED_ELECTRICCYAN,
 ANAGLYPH_PRESET_RED_GREEN,
 ANAGLYPH_PRESET_GREEN_MAGENTA,
 ANAGLYPH_PRESET_YELLOW_BLUE,
};

static const uint32 AnaglyphPreset_Colors[][2] =
{
 { 0, 0 },
 { 0xFF0000, 0x0000FF },
 { 0xFF0000, 0x00B7EB },
 { 0xFF0000, 0x00FFFF },
 { 0xFF0000, 0x00FF00 },
 { 0x00FF00, 0xFF00FF },
 { 0xFFFF00, 0x0000FF },
};


int32 VB_InDebugPeek;

static uint32 VB3DMode;

static Blip_Buffer sbuf[2];

static uint8 *WRAM = NULL;

static uint8 *GPRAM = NULL;
static uint32 GPRAM_Mask;

static uint8 *GPROM = NULL;
static uint32 GPROM_Mask;
static uint8 *GPROM_NonPatched = NULL;

static V810VB VB_V810 MDFN_SECTION("v810_core");

VSU *VB_VSU = NULL;
static uint32 VSU_CycleFix;

static uint8 WCR;

static int32 next_vip_ts, next_timer_ts, next_input_ts;


static uint32 IRQ_Asserted;

static INLINE void RecalcIntLevel(void)
{
 int ilevel = -1;

 for(int i = 4; i >= 0; i--)
 {
  if(IRQ_Asserted & (1 << i))
  {
   ilevel = i;
   break;
  }
 }

 VB_V810.SetInt(ilevel);
}

void VBIRQ_Assert(int source, bool assert)
{
 assert(source >= 0 && source <= 4);

 IRQ_Asserted &= ~(1 << source);

 if(assert)
  IRQ_Asserted |= 1 << source;
 
 RecalcIntLevel();
}



static uint8 HWCTRL_Read(v810_timestamp_t timestamp, uint32 A)
{
 uint8 ret = 0;

 if(A & 0x3)
 { 
#ifdef VB_DEBUG_MESSAGES
      MDFN_PrintError("HWCtrl Bogus Read?");
#endif
  return(ret);
 }

 switch(A & 0xFF)
 {
    default: 
#ifdef VB_DEBUG_MESSAGES
      MDFN_PrintError("Unknown HWCTRL Read: %08x\n", A);
#endif
	   break;

  case 0x18:
  case 0x1C:
  case 0x20: ret = TIMER_Read(timestamp, A);
	     break;

  case 0x24: ret = WCR | 0xFC;
	     break;

  case 0x10:
  case 0x14:
  case 0x28: ret = VBINPUT_Read(timestamp, A);
             break;

 }

 return(ret);
}

static void HWCTRL_Write(v810_timestamp_t timestamp, uint32 A, uint8 V)
{
 if(A & 0x3)
 {
#ifdef VB_DEBUG_MESSAGES
      MDFN_PrintError("HWCtrl Bogus Write?");
#endif
  return;
 }

 switch(A & 0xFF)
 {
    default: 
#ifdef VB_DEBUG_MESSAGES
      MDFN_PrintError("Unknown HWCTRL Write: %08x %02x\n", A, V);
#endif
           break;

  case 0x18:
  case 0x1C:
  case 0x20: TIMER_Write(timestamp, A, V);
	     break;

  case 0x24: WCR = V & 0x3;
	     break;

  case 0x10:
  case 0x14:
  case 0x28: VBINPUT_Write(timestamp, A, V);
	     break;
 }
}

static INLINE uint8 MemRead8(v810_timestamp_t timestamp, uint32 A)
{
 uint8 ret = 0;
 A &= (1 << 27) - 1;

 //if((A >> 24) <= 2)
 // printf("Read8: %d %08x\n", timestamp, A);

 switch(A >> 24)
 {
  case 0: ret = VIP_Read8(timestamp, A);
	  break;

  case 1: break;

  case 2: ret = HWCTRL_Read(timestamp, A);
	  break;

  case 3: break;
  case 4: break;

  case 5: ret = WRAM[A & 0xFFFF];
	  break;

  case 6: ret = GPRAM[A & GPRAM_Mask];
	  break;

  case 7: ret = GPROM_NonPatched[A & GPROM_Mask];
	  break;
 }
 return(ret);
}

static INLINE uint16 MemRead16(v810_timestamp_t timestamp, uint32 A)
{
 uint16 ret = 0;

 A &= (1 << 27) - 1;

 switch(A >> 24)
 {
  case 0: ret = VIP_Read16(timestamp, A);
	  break;

  case 1: break;

  case 2: ret = HWCTRL_Read(timestamp, A);
	  break;

  case 3: break;

  case 4: break;

  case 5: ret = le16toh(*(uint16 *)&WRAM[A & 0xFFFF]);
	  break;

  case 6: ret = le16toh(*(uint16 *)&GPRAM[A & GPRAM_Mask]);
	  break;

  case 7: ret = le16toh(*(uint16 *)&GPROM_NonPatched[A & GPROM_Mask]);
	  break;
 }
 return(ret);
}

static INLINE uint32 MemRead32(v810_timestamp_t timestamp, uint32 A)
{
 A &= (1 << 27) - 1;

 switch(A >> 24)
 {
  case 5: return(le32toh(*(uint32 *)&WRAM[A & 0xFFFF]));
  case 7: return(le32toh(*(uint32 *)&GPROM_NonPatched[A & GPROM_Mask]));

  case 0: return(VIP_Read16(timestamp, A) | (VIP_Read16(timestamp, A | 2) << 16));

  case 2: return(HWCTRL_Read(timestamp, A) | (HWCTRL_Read(timestamp, A | 2) << 16));

  case 6: return(le32toh(*(uint32 *)&GPRAM[A & GPRAM_Mask]));

  default:
  case 1:
  case 3:
  case 4: return(0);
 }
}

static INLINE void MemWrite8(v810_timestamp_t timestamp, uint32 A, uint8 V)
{
 A &= (1 << 27) - 1;

 switch(A >> 24)
 {
  case 0: VIP_Write8(timestamp, A, V);
          break;

  case 1: VB_VSU->Write((timestamp + VSU_CycleFix) >> 2, A, V);
          break;

  case 2: HWCTRL_Write(timestamp, A, V);
          break;

  case 3: break;

  case 4: break;

  case 5: WRAM[A & 0xFFFF] = V;
          break;

  case 6: GPRAM[A & GPRAM_Mask] = V;
          break;

  case 7: // ROM, no writing allowed!
          break;
 }
}

static INLINE void MemWrite16(v810_timestamp_t timestamp, uint32 A, uint16 V)
{
 A &= (1 << 27) - 1;

 switch(A >> 24)
 {
  case 5: *(uint16 *)&WRAM[A & 0xFFFF] = htole16(V);
          return;

  case 0: VIP_Write16(timestamp, A, V);
 	  break;

  case 1: VB_VSU->Write((timestamp + VSU_CycleFix) >> 2, A, V);
          break;

  case 2: HWCTRL_Write(timestamp, A, V);
 	  break;

  case 6: *(uint16 *)&GPRAM[A & GPRAM_Mask] = htole16(V);
          break;

  case 3:
  case 4:
  case 7:
          break;
 }
}

static INLINE void MemWrite32(v810_timestamp_t timestamp, uint32 A, uint32 V)
{
 A &= (1 << 27) - 1;

 switch(A >> 24)
 {
  case 5: *(uint32 *)&WRAM[A & 0xFFFF] = htole32(V);
          return;

  case 0: VIP_Write16(timestamp, A, V);
	  VIP_Write16(timestamp, A | 2, V >> 16);
          break;

  case 1: VB_VSU->Write((timestamp + VSU_CycleFix) >> 2, A, V);
	  VB_VSU->Write((timestamp + VSU_CycleFix) >> 2, A | 2, V >> 16);
          break;

  case 2: HWCTRL_Write(timestamp, A, V);
	  HWCTRL_Write(timestamp, A | 2, V >> 16);
          break;

  case 6: *(uint32 *)&GPRAM[A & GPRAM_Mask] = htole32(V);
          break;

  case 3:
  case 4:
  case 7:
          break;
 }
}


static void FixNonEvents(void)
{
 if(next_vip_ts & 0x40000000)
  next_vip_ts = VB_EVENT_NONONO;

 if(next_timer_ts & 0x40000000)
  next_timer_ts = VB_EVENT_NONONO;

 if(next_input_ts & 0x40000000)
  next_input_ts = VB_EVENT_NONONO;
}

static void EventReset(void)
{
 next_vip_ts = VB_EVENT_NONONO;
 next_timer_ts = VB_EVENT_NONONO;
 next_input_ts = VB_EVENT_NONONO;
}

static INLINE int32 CalcNextTS(void)
{
 int32 next_timestamp = next_vip_ts;

 if(next_timestamp > next_timer_ts)
  next_timestamp  = next_timer_ts;

 if(next_timestamp > next_input_ts)
  next_timestamp  = next_input_ts;

 return(next_timestamp);
}

static void RebaseTS(const v810_timestamp_t timestamp)
{
 //printf("Rebase: %08x %08x %08x\n", timestamp, next_vip_ts, next_timer_ts);

 assert(next_vip_ts > timestamp);
 assert(next_timer_ts > timestamp);
 assert(next_input_ts > timestamp);

 next_vip_ts -= timestamp;
 next_timer_ts -= timestamp;
 next_input_ts -= timestamp;
}

void VB_SetEvent(const int type, const v810_timestamp_t next_timestamp)
{
 //assert(next_timestamp > VB_V810->v810_timestamp);

 if(type == VB_EVENT_VIP)
  next_vip_ts = next_timestamp;
 else if(type == VB_EVENT_TIMER)
  next_timer_ts = next_timestamp;
 else if(type == VB_EVENT_INPUT)
  next_input_ts = next_timestamp;

 if(next_timestamp < VB_V810.GetEventNT())
  VB_V810.SetEventNT(next_timestamp);
}


static int32 MDFN_FASTCALL EventHandler(const v810_timestamp_t timestamp)
{
 if(timestamp >= next_vip_ts)
  next_vip_ts = VIP_Update(timestamp);

 if(timestamp >= next_timer_ts)
  next_timer_ts = TIMER_Update(timestamp);

 if(timestamp >= next_input_ts)
  next_input_ts = VBINPUT_Update(timestamp);

 return(CalcNextTS());
}

static void ForceEventUpdates(const v810_timestamp_t timestamp)
{
 next_vip_ts = VIP_Update(timestamp);
 next_timer_ts = TIMER_Update(timestamp);
 next_input_ts = VBINPUT_Update(timestamp);

 VB_V810.SetEventNT(CalcNextTS());
 //printf("FEU: %d %d %d\n", next_vip_ts, next_timer_ts, next_input_ts);
}

static void VB_Power(void)
{
 memset(WRAM, 0, 65536);

 VIP_Power();
 VB_VSU->Power();
 TIMER_Power();
 VBINPUT_Power();

 EventReset();
 IRQ_Asserted = 0;
 RecalcIntLevel();
 VB_V810.Reset();

 VSU_CycleFix = 0;
 WCR = 0;


 ForceEventUpdates(0);	//VB_V810->v810_timestamp);
}

static void SettingChanged(const char *name)
{
 if(!strcasecmp(name, "vb.3dmode"))
 {
  // FIXME, TODO (complicated)
  //VB3DMode = MDFN_GetSettingUI("vb.3dmode");
  //VIP_Set3DMode(VB3DMode);
 }
 else if(!strcasecmp(name, "vb.disable_parallax"))
 {
  VIP_SetParallaxDisable(MDFN_GetSettingB("vb.disable_parallax"));
 }
 else if(!strcasecmp(name, "vb.anaglyph.lcolor") || !strcasecmp(name, "vb.anaglyph.rcolor") ||
	!strcasecmp(name, "vb.anaglyph.preset") || !strcasecmp(name, "vb.default_color"))

 {
  uint32 lcolor = MDFN_GetSettingUI("vb.anaglyph.lcolor"), rcolor = MDFN_GetSettingUI("vb.anaglyph.rcolor");
  int preset = MDFN_GetSettingI("vb.anaglyph.preset");

  if(preset != ANAGLYPH_PRESET_DISABLED)
  {
   lcolor = AnaglyphPreset_Colors[preset][0];
   rcolor = AnaglyphPreset_Colors[preset][1];
  }
  VIP_SetAnaglyphColors(lcolor, rcolor);
  VIP_SetDefaultColor(MDFN_GetSettingUI("vb.default_color"));
 }
 else if(!strcasecmp(name, "vb.input.instant_read_hack"))
 {
  VBINPUT_SetInstantReadHack(MDFN_GetSettingB("vb.input.instant_read_hack"));
 }
 else if(!strcasecmp(name, "vb.instant_display_hack"))
  VIP_SetInstantDisplayHack(MDFN_GetSettingB("vb.instant_display_hack"));
 else if(!strcasecmp(name, "vb.allow_draw_skip"))
  VIP_SetAllowDrawSkip(MDFN_GetSettingB("vb.allow_draw_skip"));
 else
  abort();


}

struct VB_HeaderInfo
{
 char game_title[256];
 uint32 game_code;
 uint16 manf_code;
 uint8 version;
};

static void ReadHeader(MDFNFILE *fp, VB_HeaderInfo *hi)
{
#ifndef WII
 iconv_t sjis_ict = iconv_open("UTF-8", "shift_jis");

 if(sjis_ict != (iconv_t)-1)
 {
  char *in_ptr, *out_ptr;
  size_t ibl, obl;

  ibl = 20;
  obl = sizeof(hi->game_title) - 1;

  in_ptr = (char*)fp->data + (0xFFFFFDE0 & (fp->size - 1));
  out_ptr = hi->game_title;

  iconv(sjis_ict, (ICONV_CONST char **)&in_ptr, &ibl, &out_ptr, &obl);
  iconv_close(sjis_ict);

  *out_ptr = 0;

  MDFN_trim(hi->game_title);
 }
 else
#endif
  hi->game_title[0] = 0;

 hi->game_code = MDFN_de32lsb(fp->data + (0xFFFFFDFB & (fp->size - 1)));
 hi->manf_code = MDFN_de16lsb(fp->data + (0xFFFFFDF9 & (fp->size - 1)));
 hi->version = fp->data[0xFFFFFDFF & (fp->size - 1)];
}

#ifdef VB_SUPPORT_BIN_EXT
static bool FindGame(MDFNFILE *fp)
{
 static const char *GameNames[] =
 {
  "POLYGOBLOCK",	// 3D Tetris
  "BOUND HIGH",
  "GALACTIC PINBALL",
  "GOLF",
  "ｲﾝｽﾏｳｽ ﾉ ﾔｶﾀ",
  "JACK BROS",
  "MARIO CLASH",
  "Mario's Tennis",
  "NESTER'S FUNKY BOWLI",
  "Panic Bomber",
  "REDALARM",
  "VB SDｶﾞﾝﾀﾞﾑ",
  "SPACE INVADERS V.C.",
  "SPACE SQUASH",
  "TELEROBOXER",
  "T&EVIRTUALGOLF",
  "ﾄﾋﾞﾀﾞｾ!ﾊﾟﾆﾎﾞﾝ",
  "VERTICAL FORCE",
  "VIRTUAL BOWLING",
  "VB ﾜﾘｵﾗﾝﾄﾞ",
  "VIRTUAL FISHING JPN",
  "ﾊﾞ-ﾁｬﾙLAB",
  "VIRTUAL LEAGUE BB",
  "ﾊﾞｰﾁｬﾙﾌﾟﾛﾔｷｭｳ'95",
  "V･TETRIS",
  "WATERWORLD",
  NULL
 };
 VB_HeaderInfo hinfo;

 ReadHeader(fp, &hinfo);

 const char **gni = GameNames;
 while(*gni)
 {
  if(!strcmp(hinfo.game_title, *gni))
   return(true);
  gni++;
 }
 return(false);
}
#endif

static bool TestMagic(const char *name, MDFNFILE *fp)
{
 if(!strcasecmp(fp->ext, "vb") || !strcasecmp(fp->ext, "vboy"))
  return(true);

 #ifdef VB_SUPPORT_BIN_EXT
 if(!strcasecmp(fp->ext, "bin") && FindGame(fp))
  return(true);
 #endif

 return(false);
}

struct VBGameEntry
{
 uint32 checksums[16];
 const char *title;
 uint32 patch_address[512];
};

static const struct VBGameEntry VBGames[] =
{
 { { 0xbb71b522 } , "3D Tetris (US)", { 0xFFF1E740,
       0xfff1e75a,
       0xfff5c958,
       0xfff5c9a4,
       0xfff5c9b6,
       0xfff677a6,
       0xfff677f0,
       0xfff6c72a,
       0xfffeffc2,
       0xfff6df22,
       0xfff6e01a,
       0xfff6e20a,
       0xfff6e302,
       0xfff6e5e4,
       0xfff6eb34,
       0xfff6eb8a,
       0xfff00cd0,
       0xfffd9508,
       0xfffdad90,
       0xfffd9f1c,
       0xfffca7a2,
       0xfffca986,
       0xfffcaad4,
       0xfffcacb0,
       0xfff3dbc6,
       0xfff3dc58,
       0xfff3ca1a,
       0xfff3effe,
       0xfff3f06c,
       0xfff3f122,
       0xfff3f2da,
       0xfff3c9d8,
       0xfff01892,
       0xfff017f4,
       0xfff016c8,
       0xfff4677c,
       0xfff4620a,
       0xfff3503e,
       0xfff3f97a,
       0xfff3fae0,
       0xfff01270,
       0xfff473c8,
       0xfff472dc,
       0xfff0160a,
       0xfff6e112,
       0xfff6e7d2,
       0xfffc1730,
       0xfff3f1e6,
       0xfffc22da,
       0xfffc20a2,
       0xfffc2378,
       0xfff36152,
       0xfff37120,
       0xfff378b4,
       0xfffc1b44,
       0xfffc1b8e,
       0xfff3f258,
       0xfff3655c,
       0xfffd9902,
       0xfff49b60,
       0xfff86ef0,
       0xfff3cf08,
       0xfff5894c,
       0xfff3cec2,
       0xfff5a73e,
       0xfff400ce,
       0xfff3c2ec,
       0xfff5b5c2,
       0xfff5b64a,
       0xfffd2968,
       0xfffd2ca0,
       0xfffd2be0,
       0xfffd2b40,
       0xfffd2d10,
       0xfffd2d26,
       0xfff5a6a0,
       0xfff5a564,
       0xfffd9800,
       0xfffd9a04,
       0xfffd9b30,
       0xfffcd8d0,
       0xfffcd830,
       0xfffc15a4,
       0xfff4b5f2,
       0xfff3f346,
       0xfff374ae,
       0xfff3fa42,
       0xfff3720c,
       0xfff38298,
       0xfff38370,
       0xfff2b8a2,
       0xfff2bf52,
       0xfff0199a,
       0xfff01dc0,
       0xfffd169e,
       0xfffd19d6,
       0xfffd1876,
       0xfffd1a46,
       0xfffd1a5c,
       0xfff676b6,
       0xfff675fc,
       0xfff37b48,
       0xfffc246e,
       0xfffc24b8,
       0xfff365e8,
       0xfff2dfa0,
       0xfff36674,
       0xfff36700,
       0xfff598f4,
       0xfff59992,
       0xfff59c2a,
       0xfff59c74,
       0xfff4709a,
       0xfff46eec,
       0xfff4714c,
       0xfff4b808,
       0xfff3f3b2,
       0xfffdaeda,
       0xfffdb044,
       0xfffdae36,
       0xfff4b698,
       0xfff4b73c,
       0xfff49c06,
       0xfffdb322,
       0xfffdb18c,
       0xfff4ba68,
       0xfff4b8d2,
       0xfff474aa,
       0xfffd1916,
       0xfff3d0ac,
       0xfff477ac,
       0xfff3d2ca,
       0xfff476c0,
       0xfff5a41e,
       0xfff5a04c,
       0xfff4788e,
       0xfffc1392,
       0xfffc14f8,
 } },

 { { 0xe81a3703 }, "Bound High!", { 0x0703181A , 0x070229BE, 0x07002CA6,
       0x07024f20,
       0x070317ae,
       0x07030986,
       0x070309ca,
       0x07031968,
       0x07030ad2,
       0x07030b16,
       0x070319be,
       0x07031a20,
       0x070296b4,
       0x0702984e,
       0x07029888,
       0x070298d0,
       0x07029910,
       0x070299ce,
       0x07030c3a,
       0x07030dfc,
       0x07030e52,
       0x07030eb4,
       0x070250fa,
       0x07025148,
       0x07025776,
       0x070257c6,
       0x07025828,
       0x07021dc0,
       0x07021e50,
       0x070008e6,
       0x0700155c,
       0x070005f4,
       0x07022a08,
       0x07031e2e,
       0x07032308,
       0x0703234a,
       0x070323ac,
       0x0703a27a,
       0x0703a754,
       0x0703a77c,
       0x0703a7b4,
       0x0703a816,
       0x07002104,
       0x070028e2,
       0x0700299e,
       0x070029d8,
       0x07001782,
       0x07001912,
       0x070024da,
       0x070347e6,
       0x07035ec4,
       0x07035f16,
       0x07035f78,
       0x0702d152,
       0x0702d19a,
       0x0702d548,
       0x0702d5cc,
       0x07001c30,
       0x0702e97e,
       0x0702e9bc,
       0x070009b2,
       0x07030422,
       0x070305fc,
       0x0703064c,
       0x070306ae,
       0x07026c80,
       0x07027618,
       0x07027656,
       0x07001a92,
       0x0702eff8,
       0x0702f4e6,
       0x0702f54e,
       0x0702f5b0,
       0x07025df6,
       0x07025fb2,
       0x07026036,
       0x0702e01a,
       0x0702e090,
       0x0702e114,
       0x0702e8ea,
       0x07001440,
       0x0702c46e,
       0x0702c4b6,
       0x0702c9e4,
       0x0702ca6e,
       0x0702a532,
       0x0702a5bc,
       0x0702aba0,
       0x0702b4ce,
       0x0702b536,
 } },

 { { 0xc9710a36} , "Galactic Pinball", { 0xFFF4018A,
       0xfff40114,
       0xfff51886,
       0xfff51abe,
       0xfff4b704,
       0xfff4ec7a,
       0xfff53708,
       0xfff53a3c,
 }},

 { { 0x2199af41 }, "Golf (US)", { 0xFFE0141A, 0xFFE097C4, 0xFFE1F47A, 0xFFE0027E, 0xFFE05CA0, 0xFFE014A0,
       0xffe00286,
       0xffe013d0,
       0xffe013f0,
       0xffe01482,
       0xffe01004,
       0xffe01024,
       0xffe00d30,
       0xffe00d50,
       0xffe00db4,
       0xffe00dd4,
       0xffe13efe,
       0xffe13f62,
       0xffe13fd2,
       0xffe01744,
       0xffe01764,
       0xffe05c90,
       0xffe017b2,
       0xffe017d2,
       0xffe01930,
       0xffe01950,
       0xffe01c4a,
       0xffe01c6a,
       0xffe01cb8,
       0xffe01cd8,
       0xffe01dce,
       0xffe01de8,
       0xffe01e5a,
       0xffe01e7a,
       0xffe1409e,
       0xffe02450,
       0xffe02470,
       0xffe024b0,
       0xffe024d0,
       0xffe02530,
       0xffe02550,
       0xffe0257e,
       0xffe0259e,
       0xffe1429c,
       0xffe027c4,
       0xffe027e4,
       0xffe13f2a,
       0xffe097a4,
       0xffe181bc,
       0xffe1ce40,
       0xffe1ce60,
       0xffe07e80,
       0xffe07ea0,
       0xffe07ec0,
       0xffe07ee0,
       0xffe0810e,
       0xffe08158,
       0xffe1e468,
       0xffe1e488,
       0xffe2198c,
       0xffe09a96,
       0xffe09ab6,
       0xffe1f7be,
       0xffe1f7de,
       0xffe1f8ae,
       0xffe1f9d6,
       0xffe1fa18,
       0xffe1fa38,
       0xffe05c68,
       0xffe05c78,
       0xffe14344,
       0xffe09e06,
       0xffe16d8a,
       0xffe16daa,
       0xffe21712,
       0xffe1f37a,
       0xffe1f39a,
       0xffe1f624,
       0xffe1f666,
       0xffe1f686,
       0xffe141f8,
       0xffe151d2,
       0xffe17622,
 } },

 { { 0xefd0ac36 }, "Innsmouth House (Japan)", { 0x070082F4,
       0x07017546,
       0x07016f9a,
       0x07017558,
       0x07016fac,
       0x07017692,
       0x070176a4,
 }},

 { { 0xa44de03c, 0x81af4d6d /*[b1]*/,  0xb15d706e /*(Enable Cheats Hack)*/, 0x79a99f3c /*[T+Ger1.0_KR155E]*/ }, "Jack Bros (US)", {  0x070001A6 }},
 { { 0xcab61e8b }, "Jack Bros (Japan)", {
       0x07000192,
 } },

 // Is 0xbf0d0ab0 the bad dump?
 { { 0xa47de78c, 0xbf0d0ab0 }, "Mario Clash", { 0xFFFF5BB4, 0xFFFE6FF0, 0xFFFE039E,
       0xfffdd786,
       0xfffdd76c,
       0xffff5bc6,
       0xffff73a4,
       0xffff73b6,
       0xfffdd836,
       0xfffdd848,
       0xffff916c,
       0xffff917e,
       0xfffe7002,
       0xfffe7c36,
       0xfffe7c48,
       0xffff44f4,
       0xffff4506,
       0xfffe03b0,
       0xffff2514,
       0xffff2526,
       0xffff9be2,
       0xffff9bf4,
 } },

 { { 0x7ce7460d }, "Mario's Tennis", { 0xFFF8017C, 0xFFF90FEA, 0xFFF86DFA, 0xFFF873D6, 0xFFF9301C,
       0xfff801a2,
       0xfff90f98,
       0xfff90fae,
       0xfff90fc8,
       0xfff914c4,
       0xfff9150e,
       0xfff82a00,
       0xfff82a1a,
       0xfff82a38,
       0xfff86b98,
       0xfff86e66,
       0xfff871a2,
       0xfff8d9b6,
       0xfff8da5a,
       0xfff8dcf8,
       0xfff87aa8,
       0xfff93c86,
       0xfff93ca8,
       0xfff93ce0,
       0xfff94338,
       0xfff93158,
 } },

 { { 0xdf4d56b4 }, "Nester's Funky Bowling (US)", { 0xFFE00B78, 0xFFE00D82, 0xFFE0105A, 0xFFE00FCE,
       0xffe00f6a,
       0xffe00f72,
       0xffe00ddc,
       0xffe0089e,
       0xffe00fba,
       0xffe03702,
       0xffe01064,
       0xffe0c024,
       0xffe00bba,
       0xffe0d86c,
       0xffe0d51c,
 } },

 { { 0x19bb2dfb, 0x25fb89bb /*[b1]*/, 0x21d224af /*[h1]*/, 0xc767fe4b /*[h2]*/ }, "Panic Bomber (US)", { 0x07001FE8,
       0x07001f34,
       0x07001fc6,
 } },

 // Japan and US versions probably use the same code.
 { { 0xaa10a7b4, 0x7e85c45d }, "Red Alarm", { 0x202CE, 0x070202B4,
       0x070202e2,
       0x07020642,
       0x0702074c,
       0x07001f50,
       0x0703ca0a,
       0x07045c9e,
       0x0703326a,
       0x07031aae,
       0x070328a4,
       0x07032e5e,
       0x0703748e,
       0x07035868,
       0x07035948,
       0x07031c88,
       0x0703786a,
       0x0703628e,
       0x07035b20,
       0x07035bc0,
       0x07033856,
       0x07037370,
       0x07031a2e,
       0x070373c4,
       0x0703cfd8,
       0x0703d03c,
       0x0703d0a4,
       0x0702eae2,
       0x0703ce00,
       0x07044f02,
       0x07041922,
       0x07041df0,
       0x0701ba38,
       0x07045176,
       0x07044f8c,
       0x0703c926,
       0x0703c940,
       0x0703c712,
       0x0703c73c,
       0x07023158,
       0x070231d6,
       0x0702320c,
       0x07020a60,
       0x0700489c,
       0x07032174,
       0x070324ca,
       0x0702ef56,
       0x070414b0,
       0x0702052e,
       0x070321a6,
       0x070321fe,
       0x07032224,
       0x07021866,
       0x07021aa4,
       0x07021b36,
       0x07021b5e,
       0x07021b88,
       0x0703c6ba,
       0x0703cb3c,
       0x0703c67a,
       0x070412d2,
       0x0704132a,
       0x07041428,
       0x07020d2e,
       0x07020d48,
       0x0703755a,
       0x070375ac,
       0x070375da,
       0x07020aaa,
       0x0701ba8a,
       0x07041b0c,
       0x070355d8,
       0x07051ce0,
       0x070049e0,
       0x070049fa,
       0x07004a94,
       0x07020fb8,
       0x07021046,
       0x070210dc,
       0x07004bc2,
       0x07004ac0,
       0x07041640,
       0x07041698,
       0x07041744,
       0x07020eb0,
       0x070208bc,
       0x07020db6,
       0x07020e40,
       0x07004b28,
       0x070417c0,
       0x07041d3c,
       0x07035f18,

       0x07022bd2,
       0x07020504,
       0x070419a4,
       0x07041a78,
 } },

 { { 0x44788197 }, "SD Gundam Dimension War (Japan)", {
       0xfff296de,
       0xfff2970a,
       0xfff29730,
       0xfff298b0,
       0xfff298fa,
       0xfff29aac,
       0xfff29af2,
       0xfff29b1a,
       0xfff065ba,
       0xfff29b56,
       0xfff29cd6,
       0xfff4b7aa,
       0xfff4b8e4,
       0xfff4b9e2,
       0xfff4ba2e,
       0xfff4bbba,
       0xfff2824c,
       0xfff2835a,
       0xfff2848e,
       0xfff28aa4,
       0xfff28b20,
       0xfff28d66,
       0xfff28e5c,
       0xfff28e76,
       0xfff065e6,
       0xfff29504,
       0xfff5869e,
       0xfff58134,
       0xfff563e4,
       0xfff5687e,
       0xfff19686,
       0xfff196e6,
       0xfff19726,
       0xfff1974e,
       0xfff1a3a2,
       0xfff1083e,
       0xfff140b2,
       0xfff1372c,
       0xfff13c0e,
       0xfff13c70,
       0xfff166f2,
       0xfff18010,
       0xfff18070,
       0xfff18604,
       0xfff10654,
       0xfff44a7a,
       0xfff44a90,
       0xfff44ab4,
       0xfff44af8,
       0xfff4bd2e,
       0xfff4517a,
       0xfff439e6,
       0xfff43e92,
       0xfff43ea8,
       0xfff43ecc,
       0xfff43ef8,
       0xfff442ee,
       0xfff444c8,
       0xfff1995c,
       0xfff191ce,
       0xfff264d4,
       0xfff267ae,
       0xfff489aa,
       0xfff489ee,
       0xfff48a32,
       0xfff48a76,
       0xfff48aba,
       0xfff48ccc,
       0xfff48dae,
       0xfff493f2,
       0xfff49422,
       0xfff494c0,
       0xfff49620,
       0xfff4a822,
       0xfff4a940,
       0xfff4aa46,
       0xfff4ab2a,
       0xfff1b3b6,
       0xfff1b3fc,
       0xfff1b456,
       0xfff1b496,
       0xfff1b4d6,
       0xfff1b516,
       0xfff1b556,
       0xfff1bc6e,
       0xfff1bc94,
       0xfff1bcaa,
       0xfff1bcce,
       0xfff1bcf6,
       0xfff1bd0c,
       0xfff1bd48,
       0xfff1bdca,
       0xfff1be2a,
       0xfff1bed6,
       0xfff26c88,
       0xfff4ac00,
       0xfff4ad68,
       0xfff4b0f8,
       0xfff26a70,
       0xfff26b18,
       0xfff1bf5e,
       0xfff26dbe,
       0xfff26e54,
       0xfff441c4,
       0xfff2d548,
       0xfff2d630,
       0xfff2d67c,
       0xfff2d6c0,
       0xfff2d750,
       0xfff2d7de,
       0xfff2d872,
       0xfff2d90e,
       0xfff2d9bc,
       0xfff2da64,
       0xfff2daec,
       0xfff2db74,
       0xfff2dc1c,
       0xfff2dc94,
       0xfff2dd32,
       0xfff2ddf4,
       0xfff2df4c,
       0xfff3017a,
       0xfff3091c,
       0xfff271f2,
       0xfff272a0,
       0xfff4be52,
       0xfff4bf06,
       0xfff4bf36,
       0xfff4bf5e,
       0xfff4c4fe,
       0xfff19852,
       0xfff44b38,
       0xfff44e4a,
       0xfff44f4c,
       0xfff0edc0,
       0xfff0f142,
 } },

 { { 0xfa44402d }, "Space Invaders VC (Japan)", {
       0xfff80154,
       0xfff87e04,
       0xfff87e18,
 } },

 // Is 0xc2211fcc a bad dump?
 { {0x60895693, 0xc2211fcc, 0x7cb69b3a /*[T+Eng]*/ }, "Space Squash (Japan)", {
       0x0701a97e,
 } },

 { { 0x6ba07915, 0x41fb63bf /*[b1]*/ }, "T&E Virtual Golf (Japan)", {
       0x0700027e,
       0x07000286,
       0x070013d0,
       0x070013f0,
       0x0700141a,
       0x07001004,
       0x07001024,
       0x07000d30,
       0x07000d50,
       0x07000db4,
       0x07000dd4,
       0x07013c4e,
       0x07013cb2,
       0x07013d22,
       0x07001718,
       0x07001738,
       0x07001a32,
       0x07001a52,
       0x07005988,
       0x07005998,
       0x07001aa0,
       0x07001ac0,
       0x07001bb6,
       0x07001bd0,
       0x07001c36,
       0x07001c56,
       0x07013dee,
       0x0700223c,
       0x0700225c,
       0x0700229c,
       0x070022bc,
       0x07013fec,
       0x070024ee,
       0x0700250e,
       0x07013c7a,
       0x0700947c,
       0x0700949c,
       0x07009ade,
       0x0702162c,
       0x07017e2c,
       0x0701cae0,
       0x0701cb00,
       0x07007b70,
       0x07007b90,
       0x07007bb0,
       0x07007bd0,
       0x07007dfe,
       0x07007e48,
       0x0701e108,
       0x0701e128,
       0x07017280,
       0x0700976e,
       0x0700978e,
       0x0701f45e,
       0x0701f47e,
       0x0701f54e,
       0x0701f676,
       0x0701f6b8,
       0x0701f6d8,
       0x07005960,
       0x07005970,
       0x07014094,
       0x070169e6,
       0x07016a06,
       0x07013f48,
       0x0701f01a,
       0x0701f03a,
       0x0701f11a,
       0x0701f2c4,
       0x0701f306,
       0x0701f326,
       0x07014f22,
       0x07007982,
       0x070079a2,
 } },

 { { 0x36103000, 0xa6e0d6db /*[T+Ger.4b_KR155E]*/, 0x126123ad /*[T+Ger1.0_KR155E]*/ }, "Teleroboxer", {
       0xfff2c408,
       0xfff2c3f2,
       0xfff2b626,
       0xfff2c2ee,
       0xfff2c2ae,
       0xfff2b71c,
       0xfff2b736,
       0xfff2c1da,
       0xfff2c21a,
       0xfff2c36c,
       0xfff2b876,
       0xfff2b996,
       0xfff2b9b0,
       0xfff2b970,
       0xfff2baf4,
       0xfff2bb50,
       0xfff2bc7e,
       0xfff2bc9c,
       0xfff6c2a2,
       0xfff6c386,
       0xfff6c39c,
       0xfff6ad22,
       0xfff0304c,
       0xfff03b8e,
       0xfff042ce,
       0xfff04782,
       0xfff04c5c,
       0xfff04d90,
       0xfff11242,
       0xfff12f4a,
       0xfff141b8,
       0xfff04192,
       0xfff0414c,
       0xfff0616c,
       0xfff069d0,
       0xfff07912,
       0xfff1a980,
       0xfff1dc7e,
       0xfff1f060,
       0xfff22c92,
       0xfff2ae50,
       0xfff2af42,
       0xfff2af60,
       0xfff2e08c,
       0xfff32112,
       0xfff3213a,
       0xfff2e9aa,

       0xfff2e66c,
       0xfff2eb50,
       0xfff2eb6e,
       0xfff29f8c,
       0xfff2a1e4,
       0xfff2a36e,
       0xfff2a38c,
       0xfff339b0,
       0xfff339c6,
       0xfff25802,
       0xfff25a1e,
       0xfff2637c,
       0xfff2745c,
       0xfff27a74,
       0xfff29010,
       0xfff04014,

       0xfff32368,
       0xfff32382,
       0xfff32398,
       0xfff325ea,
       0xfff325a4,
       0xfff32668,
       0xfff326c0,
       0xfff326fc,
       0xfff32730,
       0xfff32764,
       0xfff32798,
       0xfff327c2,
       0xfff327f0,
       0xfff0811c,
       0xfff083ac,
       0xfff098e4,

       0xfff0ed12,
       0xfff0f114,
       0xfff0f7aa,
       0xfff101f0,
       0xfff102ae,
       0xfff108be,
       0xfff0a568,
       0xfff0aa78,
       0xfff0be3c,
       0xfff043a4,
       0xfff0435e,
       0xfff0dcf8,
       0xfff0dcb2,

       0xfff054a4,
       0xfff05706,
       0xfff05934,
       0xfff23ad2,
       0xfff23b76,
       0xfff2420e,

       0xfff5d454,
       0xfff5d51a,
       0xfff5d550,
       0xfff30eac,
 } },

 { { 0x40498f5e }, "Tobidase! Panibomb (Japan)", {
       0x07001fe4,
       0x07001f30,
       0x07001fc2,
 } },

 { { 0x133e9372 }, "VB Wario Land", { 0x1c232e, 0x1BFB98, 0x1C215C, 0x1C27C6,
       0xfffc2814,
       0xfffbf49a,
       0xfffbf48c,
       0xfffc45bc,
       0xfffc2956,
 } },

 // Good grief(probably not even all of them!):
 { { 0x3ccb67ae }, "V-Tetris (Japan)", { 0xFFFA2ED4, 0xFFFA9FDC, 0xFFFA776A, 0xFFFA341C, 0xFFFABAB2, 0xFFFACCAE, 0xFFF8B38A, 0xFFFA9C14, 0xFFF8F086, 0xFFF925FE,
       0xfffa2e58,
       0xfffa2e78,
       0xfffa2e98,
       0xfffa2f02,
       0xfffa2f32,
       0xfffa2fb6,
       0xfffa2fd4,
       0xfff965e4,
       0xfffa9f6c,
       0xfffa9f8c,
       0xfffa9fac,
       0xfffaa026,
       0xfffaa044,
       0xfffa76c6,
       0xfffa7704,
       0xfffa7724,
       0xfffa7a2a,
       0xfffa7aba,
       0xfffa30d4,
       0xfffa30f2,
       0xfffa3142,
       0xfffa3158,
       0xfffa3268,
       0xfffa33c0,
       0xfffa33f4,
       0xfffa34b6,
       0xfffa35dc,
       0xfffa35f2,
       0xfffa3684,
       0xfffa369a,
       0xfffa376e,
       0xfffa3784,
       0xfffa3918,
       0xfffa3930,
       0xfffa3946,
       0xfffa39e4,
       0xfffa3a4e,
       0xfffa3ba6,
       0xfffa3bea,
       0xfffa3c00,
       0xfffa3cde,
       0xfffa3db4,
       0xfffa3de0,
       0xfffa3dfe,
       0xfffaba16,
       0xfffaba44,
       0xfffaba76,
       0xfffabc48,
       0xfffabf3e,
       0xfffac0f6,
       0xfffac45e,
       0xfffac59e,
       0xfffac904,
       0xfffaca44,
       0xfffaca94,
       0xfffacbc0,
       0xfffacbf0,
       0xfffacc2c,
       0xfffad262,
       0xfffacfe6,
       0xfffad01c,
       0xfffad296,
       0xfffad2f0,
       0xfff8b012,
       0xfff8b2de,
       0xfff8b2f2,
       0xfff8b31e,
       0xfff8b33e,
       0xfff8bf62,
       0xfff8c20e,
       0xfff8b98e,
       0xfff8bc4e,
       0xfff8c554,
       0xfff8c800,
       0xfff8c824,
       0xfff8c842,
       0xfff8d15e,
       0xfff8d188,
       0xfff8d1c0,
       0xfff8d1f6,
       0xfff8d5da,
       0xfff8d25e,
       0xfff8d27c,
       0xfffa9b98,
       0xfffa9bb8,
       0xfffa9bd8,
       0xfffa9f22,
       0xfffa9f40,
       0xfff9233c,
       0xfff923d0,
       0xfff923e4,
       0xfff9244a,
       0xfff92492,
       0xfff924b2,
       0xfff8f056,
       0xfff8f46a,
       0xfff9250e,
       0xfff925ba,
       0xfff925e0,
       0xfff94036,
       0xfff92f56,
       0xfff93d72,
       0xfff9409c,
       0xfff94270,
       0xfff928c0,
       0xfff92920,
       0xfff92940,
       0xfff9296e,
       0xfff929c8,
       0xfff92a74,
       0xfff92a9c,
       0xfff92aba,
       0xfff9265c,
       0xfff92688,
       0xfff926ba,
       0xfff926e6,
       0xfff92712,
       0xfff8f7b0,
       0xfff8f886,
       0xfff8f8a8,
       0xfff8f96a,
       0xfff8f998,
       0xfff8fae0,
       0xfff8faf6,
       0xfff8fb14,
       0xfff8fba4,
       0xfff8fcd0,
       0xfff8fd0c,
       0xfff8fd9c,
       0xfff8fe4c,
       0xfff8ff7a,
       0xfff8ffc8,
       0xfff90058,
       0xfff90108,
       0xfff90236,
       0xfff90284,
       0xfff90314,
       0xfff903c4,
       0xfff904f2,
       0xfff90540,
       0xfff905d0,
       0xfff90680,
       0xfff907ae,
       0xfff907fc,
       0xfff9088c,
       0xfff908ae,
       0xfff908de,
       0xfff909cc,
       0xfff8f4c6,
       0xfff8f586,
       0xfff8f59a,
       0xfff8f5bc,
       0xfff8f5ec,
       0xfff8f642,
       0xfff8f714,
       0xfff8f740,
       0xfff8f77c,
       0xfff940f4,
       0xfff9411a,
       0xfff94134,
       0xfffad91e,
       0xfff94152,
       0xfffad98a,
       0xfffad9f8,
       0xfffada66,
       0xfffadad8,
       0xfffadb4a,
       0xfffadbbc,
       0xfffadc2e,
       0xfffadc7e,
       0xfffb72e2,
       0xfff94172,
       0xfff941a6,
       0xfffa79b0,
       0xfffa7924,
       0xfffabd9e,
       0xfffabe32,
       0xfffabe54,
       0xfffabee8,
       0xfffabf18,
       0xfffac11a,
       0xfffac2c2,
       0xfffac41a,
       0xfffac5c0,
       0xfffac5d6,
       0xfffac768,
       0xfffac8c0,
       0xfffaca66,
       0xfffacc78,
       0xfffaddb2,
       0xfffade64,
       0xfffadf18,
       0xfffadfcc,
       0xfffae084,
       0xfffae13c,
       0xfffae1f4,
       0xfffae2ac,
       0xfffae2d8,
       0xfffa6dda,
       0xfffa6dfa,
       0xfffa6e24,
       0xfffa6e62,
       0xfffa6e82,
       0xfff9c9e2,
       0xfff9ca02,
       0xfff9ca2e,
       0xfff9cbd4,
       0xfff9cc14,
       0xfff9ccba,
       0xfff9cce6,
       0xfff9cd8c,
       0xfff9cdea,
       0xfff9ce86,
       0xfff9cea4,
       0xfffa6f46,
       0xfffa6f70,
       0xfffa6fae,
       0xfffa6fce,
       0xfff944fe,
       0xfff9453a,
       0xfff945bc,
       0xfff94604,
       0xfff946b0,
       0xfff946d6,
       0xfff946f4,
       0xfff9616a,
       0xfff95e7e,
       0xfff950be,
       0xfff961d0,
       0xfff964f8,
       0xfff948ca,
       0xfff948ea,
       0xfff94918,
       0xfff94972,
       0xfff94a1e,
       0xfff94a46,
       0xfff94a64,
       0xfff962d2,
       0xfff962f8,
       0xfff96312,
       0xfff96330,
       0xfffb7226,
       0xfff96350,
       0xfff96384,
       0xfff9652a,
       0xfff96554,
       0xfffa706a,
       0xffface1a,
       0xffface50,
       0xffface70,
       0xfff8c238,
       0xfff8c3a4,
       0xfff8c3ce,
       0xfff8c536,
       0xfff8de2a,
       0xfff8de54,
       0xfff8de8e,
       0xfff8dec6,
       0xfff8e284,
       0xfff8df1e,
       0xfff8df3c,
       0xfffa9dce,
       0xfffa9e56,
       0xfff943a2,
       0xfff94454,
       0xfff944e8,
       0xfff9486a,
       0xfff8cc76,
       0xfff8cca0,
       0xfff8ccda,
       0xfff8cd12,
       0xfff8d0d0,
       0xfff8cd6a,
       0xfff8cd88,
       0xfff8dda4,
       0xfff8d8bc,
       0xfff8ddc8,
       0xfff8ddf0,
       0xfff8c8de,
       0xfff8cbaa,
       0xfff8cbbe,
       0xfff8cbe8,
       0xfff8cc10,
       0xfff8d0f4,
       0xfff8d11c,
       0xfff8e2a8,
       0xfff8e2d0,
       0xfff8bc6c,
       0xfff8bf2c,
       0xfff8bf44,
       0xfff8f680,
       0xfff8e4f8,
       0xfff8e518,
       0xfff8e5ce,
       0xfff8e606,
       0xfff8ed2e,
       0xfff8eb30,
       0xfff8efa6,
       0xfff8efd0,
       0xfff8f012,
       0xfff9cdb2,
       0xfffa146a,
       0xfffa169c,
       0xfffa16ca,
       0xfffa16fa,
       0xfffa1766,
       0xfffa179e,
       0xfffa21d8,
       0xfffa2272,
       0xfffa2290,
       0xfffa22a6,
       0xfffa24d4,
       0xfff8df6e,
       0xfff8df98,
       0xfff8dfc6,
       0xfff8cdba,
       0xfff8cde4,
       0xfff8ce12,
       0xfff8d2ae,
       0xfff8d2d8,
       0xfff8d30e,
 } },

 { { 0x4c32ba5e, 0xdc8c0bf4 /*[h1]*/ }, "Vertical Force (US)", { 0x7000BF4 } },

 // Is 0x05d06377 a bad dump?
 { { 0x9e9b8b92, 0x05d06377 }, "Vertical Force (Japan)", { 0x7000BF4 } },

 { { 0x20688279, 0xddf9abe0 /*(Debug Menu Hack)*/ }, "Virtual Bowling (Japan)", {
       0xfff24eda,
       0xfff28bea,
       0xfff28c00,
       0xfff1fb9a,
       0xfff1c284,
       0xfff1ddc4,
       0xfff0b93e,
       0xfff249ac,
       0xfff0b9a4,
       0xfff258fc,
       0xfff172aa,
       0xfff2606e,
       0xfff1a0e6,
       0xfff1a0fc,
       0xfff17222,
       0xfff26058,
       0xfff0b984,
       0xfff21080,
       0xfff21096,
       0xfff1ddae,
       0xfff13288,
       0xfff132d8,
       0xfff13ec8,
       0xfff0b920,
       0xfff284e8,
       0xfff284fe,
       0xfff154ac,
       0xfff155f8,
       0xfff15706,
       0xfff2f51e,
       0xfff1fb84,
       0xfff27310,
       0xfff0c480,
       0xfff2d618,
       0xfff2d62e,
 } },

 { { 0x526cc969, 0x45471e40 /*[b1]*/ }, "Virtual Fishing (Japan)", {
       0x07000388,
       0x07000368,
 } },


 { { 0x8989fe0a }, "Virtual Lab (Japan)", {
       0x070000ae,
       0x0700395a,
       0x070051ac,
       0x070039cc,
       0x07003a96,
       0x07003b1c,
       0x07003bce,
       0x07003c4e,
       0x07003d42,
       0x07003e42,
       0x07003e9c,
       0x07003f30,
       0x07004098,
       0x070041ee,
       0x07003180,
       0x070050f4,
       0x07002fc4,
       0x07000960,
       0x070009a6,
       0x07000b60,
       0x07000bb4,
       0x07001764,
       0x070020ec,
       0x070075cc,
       0x07003112,
       0x070001b2,
       0x07001074,
       0x070010e4,
       0x070011d0,
       0x07001262,
       0x070012f6,
       0x070013f4,
       0x070014bc,
       0x070015a2,
       0x0700162e,
       0x070023b8,
       0x07002790,
       0x070033ca,
       0x070034c8,
       0x07003254,
       0x070035d0,
       0x0700369e,
       0x0700370a,
       0x07003796,
       0x070037fc,
       0x070032f6,
 } },

 // Is 0xcc62ab38 a bad dump?
 { { 0x736b40d6, 0xcc62ab38 }, "Virtual League Baseball (US)", {
       0x07000bbc,
       0x070011a4,
       0x07000cc4,
       0x07000c1c,
       0x07000c6a,
       0x07000be0,
       0x07000c40,
 } },

 { { 0x9ba8bb5e }, "Virtual Pro Yakyuu '95", {
       0x07000bbc,
       0x070011a4,
       0x07000cc4,
       0x07000c1c,
       0x07000c6a,
       0x07000be0,
       0x07000c40,
 } },

  { { 0x82a95e51, 0x742298d1 /*[b1]*/  }, "Waterworld (US)", { 	// Apparently has complex wait loop.
       0x070008fc,
       0x0700090e,
       0x0700209e,
       0x070020b4,
       0x070009da,
       0x0700222a,
       0x07002312,
       0x070023f8,
       0x07002680,
       0x07002c68,
       0x0700303c,
       0x07003052,
       0x0700397e,
       0x07003994,
       0x07000bb4,
       0x07000ac8,
 } },

 { { 0x44C2B723 } , "Space Pinball (Prototype)", { 
       0x0702EA7A
 }},
};

#ifndef WII
static void PatchROM(void)
#else
bool PatchROM(bool checkROM)
#endif
{
  bool patched = false;
 uint32 checksum = crc32(0, GPROM, GPROM_Mask + 1);

 MDFN_printf("0x%08x\n", checksum);

 GPROM_NonPatched = GPROM;

 if( checkROM )
 {
   for(unsigned int i = 0; i < sizeof(VBGames) / sizeof(VBGameEntry); i++)
   {
    for(int ci = 0; ci < 16 && VBGames[i].checksums[ci]; ci++)
     if(VBGames[i].checksums[ci] == checksum)
    {
     // Allocate and copy mem for GPROM_NonPatched, so we won't trigger any games potential anti-piracy checksum routines.
     // Comment out this code if you know it won't be a problem.
     if(VBGames[i].patch_address[0])
     {
      if(!(GPROM_NonPatched = (uint8 *)MDFN_calloc(1, GPROM_Mask + 1, _("GPROM"))))
      {
       // Soft fail:
       GPROM_NonPatched = GPROM;
       return false;
      }
      memcpy(GPROM_NonPatched, GPROM, GPROM_Mask + 1);
     }
     patched = true;
     //
     //
     //

     for(int j = 0; j < 512 && VBGames[i].patch_address[j]; j++)
     {
      uint32 addr = VBGames[i].patch_address[j] & GPROM_Mask;
      uint16 raw = MDFN_de16lsb(&GPROM[addr]);
      uint8 code_ex = raw >> 9;
      uint8 replace_code_ex = code_ex;

      MDFN_printf("%02x -- %04x\n", code_ex, raw);

      switch(code_ex)
      {
       default: MDFN_printf("Unhandled patch code: 0x%02x -- %08x\n", code_ex, VBGames[i].patch_address[j]);	//assert(0);
		   break;

       case 0x42:		// BE
	         replace_code_ex = 0x36;
	         break;

       case 0x4A:		// BNE
	         replace_code_ex = 0x37;
	         break;

       case 0x4C:		// BP
	         replace_code_ex = 0x64;
	         break;

       case 0x41:		// BL
	         replace_code_ex = 0x6C;
	         break;

       case 0x46:		// BLT
	         replace_code_ex = 0x65;
	         break;

       case 0x45:		// BR
		  replace_code_ex = 0x6D;
		  break;
      }

      MDFN_en16lsb(&GPROM[addr], (raw & 0x1FF) | (replace_code_ex << 9));
     }
     break;
    }
   }
 }

 return patched;
}

static int Load(const char *name, MDFNFILE *fp)
{
 V810_Emu_Mode cpu_mode;
 md5_context md5;


 VB_InDebugPeek = 0;

 cpu_mode = V810_EMU_MODE_FAST;

 if(fp->size != round_up_pow2(fp->size))
 {
      MDFN_PrintError("VB ROM image size is not a power of 2???");
  return(0);
 }

 if(fp->size < 256)
 {
      MDFN_PrintError("VB ROM image size is too small??");
  return(0);
 }

 if(fp->size > (1 << 24))
 {
      MDFN_PrintError("VB ROM image size is too large??");
  return(0);
 }

 md5.starts();
 md5.update(fp->data, fp->size);
 md5.finish(MDFNGameInfo->MD5);

 VB_HeaderInfo hinfo;

 ReadHeader(fp, &hinfo);

 MDFN_printf(_("Title:     %s\n"), hinfo.game_title);
 MDFN_printf(_("Game ID Code: %u\n"), hinfo.game_code);
 MDFN_printf(_("Manufacturer Code: %d\n"), hinfo.manf_code);
 MDFN_printf(_("Version:   %u\n"), hinfo.version);

 MDFN_printf(_("ROM:       %dKiB\n"), (int)(fp->size / 1024));
 MDFN_printf(_("ROM MD5:   0x%s\n"), md5_context::asciistr(MDFNGameInfo->MD5, 0).c_str());
 
 MDFN_printf("\n");
 
 #ifdef WII
    uint8 md5NoHeader[16];
    md5_context md5NoHeaderCtx;
    md5NoHeaderCtx.starts();
    md5NoHeaderCtx.update(fp->data, (0xFFFFFDE0 & (fp->size - 1)));
    md5NoHeaderCtx.finish(md5NoHeader);

    strcpy( 
      wii_cartridge_hash, 
      md5_context::asciistr(md5NoHeader, 0).c_str() );
#endif

 VB_V810.Init(cpu_mode, true);

 std::vector<uint32> Map_Addresses;

 for(uint64 A = 0; A < 1ULL << 32; A += (1 << 27))
 {
  for(uint64 sub_A = 5 << 24; sub_A < (6 << 24); sub_A += 65536)
  {
   Map_Addresses.push_back(A + sub_A);
  }
 }

 WRAM = VB_V810.SetFastMap(&Map_Addresses[0], 65536, Map_Addresses.size(), "WRAM");
 Map_Addresses.clear();


 // Round up the ROM size to 65536(we mirror it a little later)
 GPROM_Mask = (fp->size < 65536) ? (65536 - 1) : (fp->size - 1);

 for(uint64 A = 0; A < 1ULL << 32; A += (1 << 27))
 {
  for(uint64 sub_A = 7 << 24; sub_A < (8 << 24); sub_A += GPROM_Mask + 1)
  {
   Map_Addresses.push_back(A + sub_A);
   //printf("%08x\n", (uint32)(A + sub_A));
  }
 }


 GPROM = VB_V810.SetFastMap(&Map_Addresses[0], GPROM_Mask + 1, Map_Addresses.size(), "Cart ROM");
 Map_Addresses.clear();

 // Mirror ROM images < 64KiB to 64KiB
 for(uint64 i = 0; i < 65536; i += fp->size)
 {
  memcpy(GPROM + i, fp->data, fp->size);
 }

 GPRAM_Mask = 0xFFFF;

 for(uint64 A = 0; A < 1ULL << 32; A += (1 << 27))
 {
  for(uint64 sub_A = 6 << 24; sub_A < (7 << 24); sub_A += GPRAM_Mask + 1)
  {
   //printf("GPRAM: %08x\n", A + sub_A);
   Map_Addresses.push_back(A + sub_A);
  }
 }


 GPRAM = VB_V810.SetFastMap(&Map_Addresses[0], GPRAM_Mask + 1, Map_Addresses.size(), "Cart RAM");
 Map_Addresses.clear();

 memset(GPRAM, 0, GPRAM_Mask + 1);

 {
  gzFile gp = gzopen(MDFN_MakeFName(MDFNMKF_SAV, 0, "sav").c_str(), "rb");

  if(gp)
  {
   if(gzread(gp, GPRAM, 65536) != 65536)
          MDFN_PrintError("Error reading GPRAM");
   gzclose(gp);
  }
 }

 VIP_Init();
 VB_VSU = new VSU(&sbuf[0], &sbuf[1]);
 VBINPUT_Init();

 VB3DMode = MDFN_GetSettingUI("vb.3dmode");
 uint32 prescale = MDFN_GetSettingUI("vb.liprescale");
 uint32 sbs_separation = MDFN_GetSettingUI("vb.sidebyside.separation");

 VIP_Set3DMode(VB3DMode, MDFN_GetSettingUI("vb.3dreverse"), prescale, sbs_separation);


 //SettingChanged("vb.3dmode");
 SettingChanged("vb.disable_parallax");
 SettingChanged("vb.anaglyph.lcolor");
 SettingChanged("vb.anaglyph.rcolor");
 SettingChanged("vb.anaglyph.preset");
 SettingChanged("vb.default_color");

 SettingChanged("vb.instant_display_hack");
 SettingChanged("vb.allow_draw_skip");

 SettingChanged("vb.input.instant_read_hack");

 MDFNGameInfo->fps = (int64)20000000 * 65536 * 256 / (259 * 384 * 4);


 VB_Power();



 MDFNGameInfo->nominal_width = 384;
 MDFNGameInfo->nominal_height = 224;
 MDFNGameInfo->fb_width = 384;
 MDFNGameInfo->fb_height = 224;

 switch(VB3DMode)
 {
  default: break;

  case VB3DMODE_VLI:
        MDFNGameInfo->nominal_width = 768 * prescale;
        MDFNGameInfo->nominal_height = 224;
        MDFNGameInfo->fb_width = 768 * prescale;
        MDFNGameInfo->fb_height = 224;
        break;

  case VB3DMODE_HLI:
        MDFNGameInfo->nominal_width = 384;
        MDFNGameInfo->nominal_height = 448 * prescale;
        MDFNGameInfo->fb_width = 384;
        MDFNGameInfo->fb_height = 448 * prescale;
        break;

  case VB3DMODE_CSCOPE:
	MDFNGameInfo->nominal_width = 512;
	MDFNGameInfo->nominal_height = 384;
	MDFNGameInfo->fb_width = 512;
	MDFNGameInfo->fb_height = 384;
	break;

  case VB3DMODE_SIDEBYSIDE:
	MDFNGameInfo->nominal_width = 384 * 2 + sbs_separation;
  	MDFNGameInfo->nominal_height = 224;
  	MDFNGameInfo->fb_width = 384 * 2 + sbs_separation;
 	MDFNGameInfo->fb_height = 224;
	break;
 }
 MDFNGameInfo->lcm_width = MDFNGameInfo->fb_width;
 MDFNGameInfo->lcm_height = MDFNGameInfo->fb_height;


 MDFNMP_Init(32768, ((uint64)1 << 27) / 32768);
 MDFNMP_AddRAM(65536, 5 << 24, WRAM);
 if((GPRAM_Mask + 1) >= 32768)
  MDFNMP_AddRAM(GPRAM_Mask + 1, 6 << 24, GPRAM);

#ifndef WII
 PatchROM();
#endif

 return(1);
}

static void CloseGame(void)
{
 // Only save cart RAM if it has been modified.
 for(unsigned int i = 0; i < GPRAM_Mask + 1; i++)
 {
  if(GPRAM[i])
  {
   if(!MDFN_DumpToFile(MDFN_MakeFName(MDFNMKF_SAV, 0, "sav").c_str(), 6, GPRAM, 65536))
   {

   }
   break;
  }
 }
 //VIP_Kill();
 
 if(VB_VSU)
 {
  delete VB_VSU;
  VB_VSU = NULL;
 }

 /*
 if(GPRAM)
 {
  MDFN_free(GPRAM);
  GPRAM = NULL;
 }

 if(GPROM)
 {
  MDFN_free(GPROM);
  GPROM = NULL;
 }
 */

 if(GPROM_NonPatched && GPROM_NonPatched != GPROM)
 {
  MDFN_free(GPROM_NonPatched);
  GPROM_NonPatched = NULL;
 }

 VB_V810.Kill();
}

void VB_ExitLoop(void)
{
 VB_V810.Exit();
}

static void Emulate(EmulateSpecStruct *espec)
{
 v810_timestamp_t v810_timestamp;

 MDFNMP_ApplyPeriodicCheats();

 VBINPUT_Frame();

 if(espec->SoundFormatChanged)
 {
  for(int y = 0; y < 2; y++)
  {
   sbuf[y].set_sample_rate(espec->SoundRate ? espec->SoundRate : 44100, 50);
   sbuf[y].clock_rate((long)(VB_MASTER_CLOCK / 4));
   sbuf[y].bass_freq(20);
  }
 }

 VIP_StartFrame(espec);

 v810_timestamp = VB_V810.Run(EventHandler);

 FixNonEvents();
 ForceEventUpdates(v810_timestamp);

 VB_VSU->EndFrame((v810_timestamp + VSU_CycleFix) >> 2);

 if(espec->SoundBuf)
 {
  for(int y = 0; y < 2; y++)
  {
   sbuf[y].end_frame((v810_timestamp + VSU_CycleFix) >> 2);
   espec->SoundBufSize = sbuf[y].read_samples(espec->SoundBuf + y, espec->SoundBufMaxSize, 1);
  }
 }

 VSU_CycleFix = (v810_timestamp + VSU_CycleFix) & 3;

 espec->MasterCycles = v810_timestamp;

 TIMER_ResetTS();
 VBINPUT_ResetTS();
 VIP_ResetTS();

 RebaseTS(v810_timestamp);

 VB_V810.ResetTS();


#if 0
 if(espec->SoundRate)
 {
  unsigned long long crf = (unsigned long long)sbuf[0].clock_rate_factor(sbuf[0].clock_rate());
  double real_rate = (double)crf * sbuf[0].clock_rate() / (1ULL << BLIP_BUFFER_ACCURACY);

  printf("%f\n", real_rate);
 }
#endif
}

}

using namespace MDFN_IEN_VB;

static int StateAction(StateMem *sm, int load, int data_only)
{
 int ret = 1;
 SFORMAT StateRegs[] =
 {
  SFARRAY(WRAM, 65536),
  SFARRAY(GPRAM, GPRAM_Mask ? (GPRAM_Mask + 1) : 0),
  SFVAR(WCR),
  SFVAR(IRQ_Asserted),
  SFVAR(VSU_CycleFix),


  // TODO: Remove these(and recalc on state load)
  SFVAR(next_vip_ts), 
  SFVAR(next_timer_ts),
  SFVAR(next_input_ts),

  SFEND
 };

 ret &= MDFNSS_StateAction(sm, load, data_only, StateRegs, "MAIN");

 ret &= VB_V810.StateAction(sm, load, data_only);

 ret &= VB_VSU->StateAction(sm, load, data_only);
 ret &= TIMER_StateAction(sm, load, data_only);
 ret &= VBINPUT_StateAction(sm, load, data_only);
 ret &= VIP_StateAction(sm, load, data_only);

 if(load)
 {

 }
 return(ret);
}

static bool ToggleLayer(int which)
{
 return(1);
}

static void DoSimpleCommand(int cmd)
{
 switch(cmd)
 {
  case MDFN_MSC_POWER:
  case MDFN_MSC_RESET: VB_Power(); break;
 }
}

static const MDFNSetting_EnumList VB3DMode_List[] =
{
 { "anaglyph", VB3DMODE_ANAGLYPH, gettext_noop("Anaglyph"), gettext_noop("Used in conjuction with classic dual-lens-color glasses.") },
 { "cscope",  VB3DMODE_CSCOPE, gettext_noop("CyberScope"), gettext_noop("Intended for use with the CyberScope 3D device.") },
 { "sidebyside", VB3DMODE_SIDEBYSIDE, gettext_noop("Side-by-Side"), gettext_noop("The left-eye image is displayed on the left, and the right-eye image is displayed on the right.") },
// { "overunder", VB3DMODE_OVERUNDER },
 { "vli", VB3DMODE_VLI, gettext_noop("Vertical Line Interlaced"), gettext_noop("Vertical lines alternate between left view and right view.") },
 { "hli", VB3DMODE_HLI, gettext_noop("Horizontal Line Interlaced"), gettext_noop("Horizontal lines alternate between left view and right view.") },
 { NULL, 0 },
};

static const MDFNSetting_EnumList AnaglyphPreset_List[] =
{
 { "disabled", ANAGLYPH_PRESET_DISABLED, gettext_noop("Disabled"), gettext_noop("Forces usage of custom anaglyph colors.") },
 { "0", ANAGLYPH_PRESET_DISABLED },

 { "red_blue", ANAGLYPH_PRESET_RED_BLUE, gettext_noop("Red/Blue"), gettext_noop("Classic red/blue anaglyph.") },
 { "red_cyan", ANAGLYPH_PRESET_RED_CYAN, gettext_noop("Red/Cyan"), gettext_noop("Improved quality red/cyan anaglyph.") },
 { "red_electriccyan", ANAGLYPH_PRESET_RED_ELECTRICCYAN, gettext_noop("Red/Electric Cyan"), gettext_noop("Alternate version of red/cyan") },
 { "red_green", ANAGLYPH_PRESET_RED_GREEN, gettext_noop("Red/Green") },
 { "green_magenta", ANAGLYPH_PRESET_GREEN_MAGENTA, gettext_noop("Green/Magenta") },
 { "yellow_blue", ANAGLYPH_PRESET_YELLOW_BLUE, gettext_noop("Yellow/Blue") },

 { NULL, 0 },
};

static MDFNSetting VBSettings[] =
{
 { "vb.input.instant_read_hack", MDFNSF_EMU_STATE | MDFNSF_UNTRUSTED_SAFE, gettext_noop("Input latency reduction hack."), gettext_noop("Reduces latency in some games by 20ms by returning the current pad state, rather than latched state, on serial port data reads.  This hack may cause some homebrew software to malfunction, but it should be relatively safe for commercial official games."), MDFNST_BOOL, "1", NULL, NULL, NULL, SettingChanged },
 
 { "vb.instant_display_hack", MDFNSF_NOFLAGS, gettext_noop("Display latency reduction hack."), gettext_noop("Reduces latency in games by displaying the framebuffer 20ms earlier.  This hack has some potential of causing graphical glitches, so it is disabled by default."), MDFNST_BOOL, "1", NULL, NULL, NULL, SettingChanged },
 { "vb.allow_draw_skip", MDFNSF_EMU_STATE | MDFNSF_UNTRUSTED_SAFE, gettext_noop("Allow draw skipping."), gettext_noop("If vb.instant_display_hack is set to \"1\", and this setting is set to \"1\", then frame-skipping the drawing to the emulated framebuffer will be allowed.  THIS WILL CAUSE GRAPHICAL GLITCHES, AND THEORETICALLY(but unlikely) GAME CRASHES, ESPECIALLY WITH DIRECT FRAMEBUFFER DRAWING GAMES."), MDFNST_BOOL, "1", NULL, NULL, NULL, SettingChanged },

 // FIXME: We're going to have to set up some kind of video mode change notification for changing vb.3dmode while the game is running to work properly.
 { "vb.3dmode", MDFNSF_NOFLAGS, gettext_noop("3D mode."), NULL, MDFNST_ENUM, "anaglyph", NULL, NULL, NULL, /*SettingChanged*/NULL, VB3DMode_List },
 { "vb.liprescale", MDFNSF_NOFLAGS, gettext_noop("Line Interlaced prescale."), NULL, MDFNST_UINT, "2", "1", "10", NULL, NULL },

 { "vb.disable_parallax", MDFNSF_EMU_STATE | MDFNSF_UNTRUSTED_SAFE, gettext_noop("Disable parallax for BG and OBJ rendering."), NULL, MDFNST_BOOL, "0", NULL, NULL, NULL, SettingChanged },
 { "vb.default_color", MDFNSF_NOFLAGS, gettext_noop("Default maximum-brightness color to use in non-anaglyph 3D modes."), NULL, MDFNST_UINT, "0xF0F0F0", "0x000000", "0xFFFFFF", NULL, SettingChanged },

 { "vb.anaglyph.preset", MDFNSF_NOFLAGS, gettext_noop("Anaglyph preset colors."), NULL, MDFNST_ENUM, "red_blue", NULL, NULL, NULL, SettingChanged, AnaglyphPreset_List },
 { "vb.anaglyph.lcolor", MDFNSF_NOFLAGS, gettext_noop("Anaglyph maximum-brightness color for left view."), NULL, MDFNST_UINT, "0xffba00", "0x000000", "0xFFFFFF", NULL, SettingChanged },
 { "vb.anaglyph.rcolor", MDFNSF_NOFLAGS, gettext_noop("Anaglyph maximum-brightness color for right view."), NULL, MDFNST_UINT, "0x00baff", "0x000000", "0xFFFFFF", NULL, SettingChanged },

 { "vb.sidebyside.separation", MDFNSF_NOFLAGS, gettext_noop("Number of pixels to separate L/R views by."), gettext_noop("This setting refers to pixels before vb.xscale(fs) scaling is taken into consideration.  For example, a value of \"100\" here will result in a separation of 300 screen pixels if vb.xscale(fs) is set to \"3\"."), MDFNST_UINT, /*"96"*/"0", "0", "1024", NULL, NULL },

 { "vb.3dreverse", MDFNSF_NOFLAGS, gettext_noop("Reverse left/right 3D views."), NULL, MDFNST_BOOL, "0", NULL, NULL, NULL, SettingChanged },
 { NULL }
};


static const InputDeviceInputInfoStruct IDII[] =
{
 { "a", "A", 7, IDIT_BUTTON_CAN_RAPID,  NULL },
 { "b", "B", 6, IDIT_BUTTON_CAN_RAPID, NULL },
 { "rt", "Right-Back", 13, IDIT_BUTTON, NULL },
 { "lt", "Left-Back", 12, IDIT_BUTTON, NULL },

 { "up-r", "UP ↑ (Right D-Pad)", 8, IDIT_BUTTON, "down-r" },
 { "right-r", "RIGHT → (Right D-Pad)", 11, IDIT_BUTTON, "left-r" },

 { "right-l", "RIGHT → (Left D-Pad)", 3, IDIT_BUTTON, "left-l" },
 { "left-l", "LEFT ← (Left D-Pad)", 2, IDIT_BUTTON, "right-l" },
 { "down-l", "DOWN ↓ (Left D-Pad)", 1, IDIT_BUTTON, "up-l" },
 { "up-l", "UP ↑ (Left D-Pad)", 0, IDIT_BUTTON, "down-l" },

 { "start", "Start", 5, IDIT_BUTTON, NULL },
 { "select", "Select", 4, IDIT_BUTTON, NULL },

 { "left-r", "LEFT ← (Right D-Pad)", 10, IDIT_BUTTON, "right-r" },
 { "down-r", "DOWN ↓ (Right D-Pad)", 9, IDIT_BUTTON, "up-r" },
};

static InputDeviceInfoStruct InputDeviceInfo[] =
{
 {
  "gamepad",
  "Gamepad",
  NULL,
  sizeof(IDII) / sizeof(InputDeviceInputInfoStruct),
  IDII,
 }
};

static const InputPortInfoStruct PortInfo[] =
{
 { 0, "builtin", "Built-In", sizeof(InputDeviceInfo) / sizeof(InputDeviceInfoStruct), InputDeviceInfo, "gamepad" }
};

static InputInfoStruct InputInfo =
{
 sizeof(PortInfo) / sizeof(InputPortInfoStruct),
 PortInfo
};


static const FileExtensionSpecStruct KnownExtensions[] =
{
 { ".vb", gettext_noop("Nintendo Virtual Boy") },
 { ".vboy", gettext_noop("Nintendo Virtual Boy") },
 #ifdef VB_SUPPORT_BIN_EXT
 { ".bin", gettext_noop("Nintendo Virtual Boy (Deprecated)") },
 #endif
 { NULL, NULL }
};

MDFNGI EmulatedVB =
{
 "vb",
 "Virtual Boy",
 KnownExtensions,
 MODPRIO_INTERNAL_HIGH,
 NULL,		// Debug info
 &InputInfo,	//
 Load,
 TestMagic,
 NULL,
 NULL,
 CloseGame,
 ToggleLayer,
 NULL,		// Layer names, null-delimited
 NULL,
 NULL,
 NULL,
 StateAction,
 Emulate,
 VBINPUT_SetInput,
 DoSimpleCommand,
 VBSettings,
 MDFN_MASTERCLOCK_FIXED(VB_MASTER_CLOCK),
 0,
 false, // Multires possible?

 0,   // lcm_width
 0,   // lcm_height
 NULL,  // Dummy

 384,	// Nominal width
 224,	// Nominal height

 384,	// Framebuffer width
 256,	// Framebuffer height

 2,     // Number of output sound channels
};


/* V810 Emulator
 *
 * Copyright (C) 2006 David Tucker
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/* Alternatively, the V810 emulator code(and all V810 emulation header files) can be used/distributed under the following license(you can adopt either
   license exclusively for your changes by removing one of these license headers, but it's STRONGLY preferable
   to keep your changes dual-licensed as well):

This Reality Boy emulator is copyright (C) David Tucker 1997-2008, all rights
reserved.   You may use this code as long as you make no money from the use of
this code and you acknowledge the original author (Me).  I reserve the right to
dictate who can use this code and how (Just so you don't do something stupid
with it).
   Most Importantly, this code is swap ware.  If you use It send along your new
program (with code) or some other interesting tidbits you wrote, that I might be
interested in.
   This code is in beta, there are bugs!  I am not responsible for any damage
done to your computer, reputation, ego, dog, or family life due to the use of
this code.  All source is provided as is, I make no guaranties, and am not
responsible for anything you do with the code (legal or otherwise).
   Virtual Boy is a trademark of Nintendo, and V810 is a trademark of NEC.  I am
in no way affiliated with either party and all information contained hear was
found freely through public domain sources.
*/

//////////////////////////////////////////////////////////
// CPU routines

#include "../mednafen.h"
//#include "pcfx.h"
//#include "debug.h"

#include <string.h>
#include <errno.h>

#include "../hw_cpu/v810_vb/v810_opt.h"
#include "../hw_cpu/v810_vb/v810_cpu.h"
#include "../hw_cpu/v810_vb/v810_cpuD.h"

#if 0
#include <map>
static uint32 prev_old = 0;
static uint32 prev_new = 0;
void V810VB::IdleLoopTest(uint32 old_pc, uint32 new_pc)
{
 static std::map<uint32, bool> loop_addresses;
 static uint32 iter_counter = 0;
 static uint32 prev_P_REG[32];
 static uint32 prev_S_REG[32];

 if(prev_old == old_pc && prev_new == new_pc && !memcmp(prev_P_REG, P_REG, sizeof(P_REG)) &&
	!memcmp(prev_S_REG, S_REG, sizeof(S_REG)))
 {
  if(iter_counter < 1000)
   iter_counter++;
  else if(!loop_addresses[old_pc])
  {
   loop_addresses[old_pc] = 1;
   printf("       0x%08x,\n", old_pc);
  }
 }



 prev_old = old_pc;
 prev_new = new_pc;
 memcpy(prev_P_REG, P_REG, sizeof(P_REG));
 memcpy(prev_S_REG, S_REG, sizeof(S_REG));
}

void V810VB::IdleLoopMurder(void)
{
 prev_old = prev_new = 0xFFFFFFFF;
}

void V810VB::IdleLoopManslaughter(uint32 addr)
{
 if(((addr >> 24) & 0x7) == 0x2)
 {
  prev_old = prev_new = 0xFFFFFFFF;
 }
}

void V810VB::IdleLoopAssault(uint32 jump_pc)
{
 if(((jump_pc >> 24) & 0x7) != 0x7)
  prev_old = prev_new = 0xFFFFFFFF;
}


#else

INLINE void V810VB::IdleLoopTest(uint32 old_pc, uint32 new_pc)
{

}

INLINE void V810VB::IdleLoopMurder(void)
{

}

INLINE void V810VB::IdleLoopManslaughter(uint32 addr)
{


}

INLINE void V810VB::IdleLoopAssault(uint32 jump_pc)
{


}
#endif

INLINE void V810VB::RecalcIPendingCache(void)
{
 IPendingCache = 0;

 // Of course don't generate an interrupt if there's not one pending!
 if(ilevel < 0)
  return;

 // If CPU is halted because of a fatal exception, don't let an interrupt
 // take us out of this halted status.
 if(Halted == HALT_FATAL_EXCEPTION) 
  return;

 // If the NMI pending, exception pending, and/or interrupt disabled bit
 // is set, don't accept any interrupts.
 if(S_REG[PSW] & (PSW_NP | PSW_EP | PSW_ID))
  return;

 // If the interrupt level is lower than the interrupt enable level, don't
 // accept it.
 if(ilevel < (int)((S_REG[PSW] & PSW_IA) >> 16))
  return;

 IPendingCache = 0xFF;
}


namespace MDFN_IEN_VB
{
 uint8 MDFN_FASTCALL MemRead8(v810_timestamp_t timestamp, uint32 A);
 uint16 MDFN_FASTCALL MemRead16(v810_timestamp_t timestamp, uint32 A);
 uint32 MDFN_FASTCALL MemRead32(v810_timestamp_t timestamp, uint32 A);

 void MDFN_FASTCALL MemWrite8(v810_timestamp_t timestamp, uint32 A, uint8 V);
 void MDFN_FASTCALL MemWrite16(v810_timestamp_t timestamp, uint32 A, uint16 V);
 void MDFN_FASTCALL MemWrite32(v810_timestamp_t timestamp, uint32 A, uint32 V);
}

using namespace MDFN_IEN_VB;

//#include "fpu-new/softfloat.h"

V810VB::V810VB()
{
 memset(FastMap, 0, sizeof(FastMap));
}

V810VB::~V810VB()
{

}

// Reinitialize the defaults in the CPU
void V810VB::Reset() 
{
 v810_timestamp = 0;
 next_event_ts = 0x7FFFFFFF; // fixme

 memset(P_REG, 0, sizeof(P_REG));
 memset(S_REG, 0, sizeof(S_REG));

 P_REG[0]      =  0x00000000;
 SetPC(0xFFFFFFF0);

 S_REG[ECR]    =  0x0000FFF0;
 S_REG[PSW]    =  0x00008000;

 if(VBMode)
  S_REG[PIR]	= 0x00005346;
 else
  S_REG[PIR]    =  0x00008100;

 S_REG[TKCW]   =  0x000000E0;
 Halted = HALT_NONE;
 ilevel = -1;

 lastop = 0;

 in_bstr = FALSE;

 RecalcIPendingCache();
}

bool V810VB::Init(V810_Emu_Mode mode, bool vb_mode)
{
 EmuMode = mode;
 VBMode = vb_mode;

 in_bstr = FALSE;
 in_bstr_to = 0;

 if(mode == V810_EMU_MODE_FAST)
 {
  memset(DummyRegion, 0, V810_FAST_MAP_PSIZE);

  for(unsigned int i = V810_FAST_MAP_PSIZE; i < V810_FAST_MAP_PSIZE + V810_FAST_MAP_TRAMPOLINE_SIZE; i += 2)
  {
   DummyRegion[i + 0] = 0;
   DummyRegion[i + 1] = 0x36 << 2;
  }

  for(uint64 A = 0; A < (1ULL << 32); A += V810_FAST_MAP_PSIZE)
   FastMap[A / V810_FAST_MAP_PSIZE] = DummyRegion - A;
 }

 return(TRUE);
}

void V810VB::Kill(void)
{
#ifndef MEM2
 for(unsigned int i = 0; i < FastMapAllocList.size(); i++)
  MDFN_free(FastMapAllocList[i]);
#endif

 FastMapAllocList.clear();
}

void V810VB::SetInt(int level)
{
 assert(level >= -1 && level <= 15);

 ilevel = level;
 RecalcIPendingCache();
}

uint8 *V810VB::SetFastMap(uint32 addresses[], uint32 length, unsigned int num_addresses, const char *name)
{
 uint8 *ret = NULL;

 for(unsigned int i = 0; i < num_addresses; i++)
 {
  assert((addresses[i] & (V810_FAST_MAP_PSIZE - 1)) == 0);
 }
 assert((length & (V810_FAST_MAP_PSIZE - 1)) == 0);

#ifdef MEM2
 if(!(ret = (uint8 *)Mem2ManagerAlloc(length + V810_FAST_MAP_TRAMPOLINE_SIZE, name)))
#else
 if(!(ret = (uint8 *)MDFN_malloc(length + V810_FAST_MAP_TRAMPOLINE_SIZE, name)))
#endif
 {
  return(NULL);
 }

 for(unsigned int i = length; i < length + V810_FAST_MAP_TRAMPOLINE_SIZE; i += 2)
 {
  ret[i + 0] = 0;
  ret[i + 1] = 0x36 << 2;
 }

 for(unsigned int i = 0; i < num_addresses; i++)
 {  
  for(uint64 addr = addresses[i]; addr != (uint64)addresses[i] + length; addr += V810_FAST_MAP_PSIZE)
  {
   //printf("%08x, %d, %s\n", addr, length, name);

   FastMap[addr / V810_FAST_MAP_PSIZE] = ret - addresses[i];
  }
 }

 FastMapAllocList.push_back(ret);

 return(ret);
}


INLINE void V810VB::SetFlag(uint32 n, bool condition)
{
 S_REG[PSW] &= ~n;

 if(condition)
  S_REG[PSW] |= n;
}
	
INLINE void V810VB::SetSZ(uint32 value)
{
 SetFlag(PSW_Z, !value);
 SetFlag(PSW_S, value & 0x80000000);
}

#define SetPREG(n, val) { P_REG[n] = val; }

INLINE void V810VB::SetSREG(v810_timestamp_t &timestamp, unsigned int which, uint32 value)
{
	switch(which)
	{
	 default:	// Reserved
		break;

         case ECR:      // Read-only
                break;

         case PIR:      // Read-only (obviously)
                break;

         case TKCW:     // Read-only
                break;

	 case EIPSW:
	 case FEPSW:
              	S_REG[which] = value & 0xFF3FF;
		break;

	 case PSW:
              	S_REG[which] = value & 0xFF3FF;
		RecalcIPendingCache();
		break;

	 case EIPC:
	 case FEPC:
		S_REG[which] = value & 0xFFFFFFFE;
		break;

	 case ADDTRE:
  	        S_REG[ADDTRE] = value & 0xFFFFFFFE;
		break;

	 case CHCW:
              	S_REG[CHCW] = value & 0x2;
/*
              	switch(value & 0x31)
              	{
              	 default: printf("Undefined cache control bit combination: %08x\n", value);
                          break;

              	 case 0x00: break;

              	 case 0x01: CacheClear(timestamp, (value >> 20) & 0xFFF, (value >> 8) & 0xFFF);
                            break;

              	 case 0x10: CacheDump(timestamp, value & ~0xFF);
                            break;

              	 case 0x20: CacheRestore(timestamp, value & ~0xFF);
                            break;
               	}
		break;
*/
	}
}

INLINE uint32 V810VB::GetSREG(unsigned int which)
{
	uint32 ret;

	ret = S_REG[which];

	return(ret);
}

#define RB_SETPC(new_pc_raw) 										\
			  {										\
			   const uint32 new_pc = new_pc_raw;	/* So RB_SETPC(RB_GETPC()) won't mess up */	\
			   {										\
			    PC_ptr = &FastMap[(new_pc) >> V810_FAST_MAP_SHIFT][(new_pc)];		\
			    PC_base = PC_ptr - (new_pc);						\
			   }										\
			  }

#define RB_PCRELCHANGE(delta) { 				\
				{				\
				 uint32 PC_tmp = RB_GETPC();	\
				 PC_tmp += (delta);		\
				 RB_SETPC(PC_tmp);		\
				}					\
			      }

#define RB_INCPCBY2()	{ PC_ptr += 2; }
#define RB_INCPCBY4()   { PC_ptr += 4; }

#define RB_DECPCBY2()   { PC_ptr -= 2; }
#define RB_DECPCBY4()   { PC_ptr -= 4; }

//
// Define fast mode defines
//
#define RB_GETPC()      	((uint32)(PC_ptr - PC_base))

#ifdef MSB_FIRST
#define RB_RDOP(PC_offset, ...) (PC_ptr[PC_offset] | (PC_ptr[PC_offset + 1] << 8))
#else
#define RB_RDOP(PC_offset, ...) (*(uint16 *)&PC_ptr[PC_offset])
#endif

v810_timestamp_t V810VB::Run(int32 MDFN_FASTCALL (*event_handler)(const v810_timestamp_t timestamp))
{
 Running = true;

 #define RB_ADDBT(n)
 #define RB_CPUHOOK(n)

 #include "../hw_cpu/v810_vb/v810_oploop.inc"

 #undef RB_CPUHOOK
 #undef RB_ADDBT

 return(v810_timestamp);
}

void V810VB::Exit(void)
{
 Running = false;
}

uint32 V810VB::GetPC(void)
{
 return(PC_ptr - PC_base);
}

void V810VB::SetPC(uint32 new_pc)
{
 PC_ptr = &FastMap[new_pc >> V810_FAST_MAP_SHIFT][new_pc];
 PC_base = PC_ptr - new_pc;
}

uint32 V810VB::GetPR(const unsigned int which)
{
 assert(which <= 0x1F);

 return(which ? P_REG[which] : 0);
}

void V810VB::SetPR(const unsigned int which, uint32 value)
{
 assert(which <= 0x1F);

 if(which)
  P_REG[which] = value;
}

uint32 V810VB::GetSR(const unsigned int which)
{
 assert(which <= 0x1F);

 return(GetSREG(which));
}

void V810VB::SetSR(const unsigned int which, uint32 value)
{
 assert(which <= 0x1F);

// SetSREG(timestamp, which, value);
}


#define BSTR_OP_MOV dst_cache &= ~(1 << dstoff); dst_cache |= ((src_cache >> srcoff) & 1) << dstoff;
#define BSTR_OP_NOT dst_cache &= ~(1 << dstoff); dst_cache |= (((src_cache >> srcoff) & 1) ^ 1) << dstoff;

#define BSTR_OP_XOR dst_cache ^= ((src_cache >> srcoff) & 1) << dstoff;
#define BSTR_OP_OR dst_cache |= ((src_cache >> srcoff) & 1) << dstoff;
#define BSTR_OP_AND dst_cache &= ~((((src_cache >> srcoff) & 1) ^ 1) << dstoff);

#define BSTR_OP_XORN dst_cache ^= (((src_cache >> srcoff) & 1) ^ 1) << dstoff;
#define BSTR_OP_ORN dst_cache |= (((src_cache >> srcoff) & 1) ^ 1) << dstoff;
#define BSTR_OP_ANDN dst_cache &= ~(((src_cache >> srcoff) & 1) << dstoff);

INLINE uint32 V810VB::BSTR_RWORD(v810_timestamp_t &timestamp, uint32 A)
{
 {
  uint32 ret;

  timestamp += 2;
  ret = MemRead16(timestamp, A);
 
  timestamp += 2;
  ret |= MemRead16(timestamp, A | 2) << 16;
  return(ret);
 }
}

INLINE void V810VB::BSTR_WWORD(v810_timestamp_t &timestamp, uint32 A, uint32 V)
{
 {
  timestamp += 2;
  MemWrite16(timestamp, A, V & 0xFFFF);

  timestamp += 2;
  MemWrite16(timestamp, A | 2, V >> 16);
 }
}

#define DO_BSTR(op) { 						\
                while(len)					\
                {						\
                 if(!have_src_cache)                            \
                 {                                              \
		  have_src_cache = TRUE;			\
                  src_cache = BSTR_RWORD(timestamp, src);       \
                 }                                              \
								\
		 if(!have_dst_cache)				\
		 {						\
		  have_dst_cache = TRUE;			\
                  dst_cache = BSTR_RWORD(timestamp, dst);       \
                 }                                              \
								\
		 op;						\
                 srcoff = (srcoff + 1) & 0x1F;			\
                 dstoff = (dstoff + 1) & 0x1F;			\
		 len--;						\
								\
		 if(!srcoff)					\
		 {                                              \
		  src += 4;					\
		  have_src_cache = FALSE;			\
		 }                                              \
								\
                 if(!dstoff)                                    \
                 {                                              \
                  BSTR_WWORD(timestamp, dst, dst_cache);        \
                  dst += 4;                                     \
		  have_dst_cache = FALSE;			\
		  if(timestamp >= next_event_ts)		\
		   break;					\
                 }                                              \
                }						\
                if(have_dst_cache)				\
                 BSTR_WWORD(timestamp, dst, dst_cache);		\
		}

INLINE bool V810VB::Do_BSTR_Search(v810_timestamp_t &timestamp, const int inc_mul, unsigned int bit_test)
{
        uint32 srcoff = (P_REG[27] & 0x1F);
        uint32 len = P_REG[28];
        uint32 bits_skipped = P_REG[29];
        uint32 src = (P_REG[30] & 0xFFFFFFFC);
	bool found = false;

	#if 0
	// TODO: Better timing.
	if(!in_bstr)	// If we're just starting the execution of this instruction(kind of spaghetti-code), so FIXME if we change
			// bstr handling in v810_oploop.inc
	{
	 timestamp += 13 - 1;
	}
	#endif

	while(len)
	{
		if(!have_src_cache)
		{
		 have_src_cache = TRUE;
		 timestamp++;
		 src_cache = BSTR_RWORD(timestamp, src);
		}

		if(((src_cache >> srcoff) & 1) == bit_test)
		{
		 found = true;

		 /* Fix the bit offset and word address to "1 bit before" it was found */
		 srcoff -= inc_mul * 1;
		 if(srcoff & 0x20)		/* Handles 0x1F->0x20(0x00) and 0x00->0xFFFF... */
		 {
		  src -= inc_mul * 4;
		  srcoff &= 0x1F;
		 }
		 break;
		}
	        srcoff = (srcoff + inc_mul * 1) & 0x1F;
		bits_skipped++;
	        len--;

	        if(!srcoff)
		{
	         have_src_cache = FALSE;
		 src += inc_mul * 4;
		 if(timestamp >= next_event_ts)
		  break;
		}
	}

        P_REG[27] = srcoff;
        P_REG[28] = len;
        P_REG[29] = bits_skipped;
        P_REG[30] = src;


        if(found)               // Set Z flag to 0 if the bit was found
         SetFlag(PSW_Z, 0);
        else if(!len)           // ...and if the search is over, and the bit was not found, set it to 1
         SetFlag(PSW_Z, 1);

        if(found)               // Bit found, so don't continue the search.
         return(false);

        return((bool)len);      // Continue the search if any bits are left to search.
}

bool V810VB::bstr_subop(v810_timestamp_t &timestamp, int sub_op, int arg1)
{
 if((sub_op >= 0x10) || (!(sub_op & 0x8) && sub_op >= 0x4))
 {
  MDFN_printf("%08x\tBSR Error: %04x\n", PC,sub_op);

  SetPC(GetPC() - 2);
  Exception(INVALID_OP_HANDLER_ADDR, ECODE_INVALID_OP);

  return(false);
 }

// printf("BSTR: %02x, %02x %02x; src: %08x, dst: %08x, len: %08x\n", sub_op, P_REG[27], P_REG[26], P_REG[30], P_REG[29], P_REG[28]);

 if(sub_op & 0x08)
 {
	uint32 dstoff = (P_REG[26] & 0x1F);
	uint32 srcoff = (P_REG[27] & 0x1F);
	uint32 len =     P_REG[28];
	uint32 dst =    (P_REG[29] & 0xFFFFFFFC);
	uint32 src =    (P_REG[30] & 0xFFFFFFFC);

	switch(sub_op)
	{
	 case ORBSU: DO_BSTR(BSTR_OP_OR); break;

	 case ANDBSU: DO_BSTR(BSTR_OP_AND); break;

	 case XORBSU: DO_BSTR(BSTR_OP_XOR); break;

	 case MOVBSU: DO_BSTR(BSTR_OP_MOV); break;

	 case ORNBSU: DO_BSTR(BSTR_OP_ORN); break;

	 case ANDNBSU: DO_BSTR(BSTR_OP_ANDN); break;

	 case XORNBSU: DO_BSTR(BSTR_OP_XORN); break;

	 case NOTBSU: DO_BSTR(BSTR_OP_NOT); break;
	}

        P_REG[26] = dstoff; 
        P_REG[27] = srcoff;
        P_REG[28] = len;
        P_REG[29] = dst;
        P_REG[30] = src;

	return((bool)P_REG[28]);
 }
 else
 {
  MDFN_printf("BSTR Search: %02x\n", sub_op);
  return(Do_BSTR_Search(timestamp, ((sub_op & 1) ? -1 : 1), (sub_op & 0x2) >> 1));
 }
 assert(0);
 return(false);
}

INLINE void V810VB::SetFPUOPNonFPUFlags(uint32 result)
{
                 // Now, handle flag setting
                 SetFlag(PSW_OV, 0);

                 if(!(result & 0x7FFFFFFF)) // Check to see if exponent and mantissa are 0
		 {
		  // If Z flag is set, S and CY should be clear, even if it's negative 0(confirmed on real thing with subf.s, at least).
                  SetFlag(PSW_Z, 1);
                  SetFlag(PSW_S, 0);
                  SetFlag(PSW_CY, 0);
		 }
                 else
		 {
                  SetFlag(PSW_Z, 0);
                  SetFlag(PSW_S, result & 0x80000000);
                  SetFlag(PSW_CY, result & 0x80000000);
		 }
                 //printf("MEOW: %08x\n", S_REG[PSW] & (PSW_S | PSW_CY));
}

INLINE bool V810VB::CheckFPInputException(uint32 fpval)
{
 // Zero isn't a subnormal! (OR IS IT *DUN DUN DUNNN* ;b)
 if(!(fpval & 0x7FFFFFFF))
  return(false);

 switch((fpval >> 23) & 0xFF)
 {
  case 0x00: // Subnormal		
  case 0xFF: // NaN or infinity
	{
	 //puts("New FPU FRO");

	 S_REG[PSW] |= PSW_FRO;

	 SetPC(GetPC() - 4);
	 Exception(FPU_HANDLER_ADDR, ECODE_FRO);
	}
	return(true);	// Yes, exception occurred
 }
 return(false);	// No, no exception occurred.
}

bool V810VB::FPU_DoesExceptionKillResult(void)
{
 if(float_exception_flags & float_flag_invalid)
  return(true);

 if(float_exception_flags & float_flag_divbyzero)
  return(true);


 // Return false here, so that the result of this calculation IS put in the output register.
 // (Incidentally, to get the result of operations on overflow to match a real V810, required a bit of hacking of the SoftFloat code to "wrap" the exponent
 // on overflow,
 // rather than generating an infinity.  The wrapping behavior is specified in IEE 754 AFAIK, and is useful in cases where you divide a huge number
 // by another huge number, and fix the result afterwards based on the number of overflows that occurred.  Probably requires some custom assembly code,
 // though.  And it's the kind of thing you'd see in an engineering or physics program, not in a perverted video game :b).
 // Oh, and just a note to self, FPR is NOT set when an overflow occurs.  Or it is in certain cases?
 if(float_exception_flags & float_flag_overflow)
  return(false);

 return(false);
}

void V810VB::FPU_DoException(void)
{
 if(float_exception_flags & float_flag_invalid)
 {
  //puts("New FPU Invalid");

  S_REG[PSW] |= PSW_FIV;

  SetPC(GetPC() - 4);
  Exception(FPU_HANDLER_ADDR, ECODE_FIV);

  return;
 }

 if(float_exception_flags & float_flag_divbyzero)
 {
  //puts("New FPU Divide by Zero");

  S_REG[PSW] |= PSW_FZD;

  SetPC(GetPC() - 4);
  Exception(FPU_HANDLER_ADDR, ECODE_FZD);

  return;
 }

 if(float_exception_flags & float_flag_underflow)
 {
  //puts("New FPU Underflow");

  S_REG[PSW] |= PSW_FUD;
 }

 if(float_exception_flags & float_flag_inexact)
 {
  S_REG[PSW] |= PSW_FPR;
  //puts("New FPU Precision Degradation");
 }

 // FPR can be set along with overflow, so put the overflow exception handling at the end here(for Exception() messes with PSW).
 if(float_exception_flags & float_flag_overflow)
 {
  //puts("New FPU Overflow");

  S_REG[PSW] |= PSW_FOV;

  SetPC(GetPC() - 4);
  Exception(FPU_HANDLER_ADDR, ECODE_FOV);
 }
}

bool V810VB::IsSubnormal(uint32 fpval)
{
 if( ((fpval >> 23) & 0xFF) == 0 && (fpval & ((1 << 23) - 1)) )
  return(true);

 return(false);
}

INLINE void V810VB::FPU_Math_Template(float32 (*func)(float32, float32), uint32 arg1, uint32 arg2)
{
 if(CheckFPInputException(P_REG[arg1]) || CheckFPInputException(P_REG[arg2]))
 {

 }
 else
 {
  uint32 result;

  float_exception_flags = 0;
  result = func(P_REG[arg1], P_REG[arg2]);

  if(IsSubnormal(result))
  {
   float_exception_flags |= float_flag_underflow;
   float_exception_flags |= float_flag_inexact;
  }

  //printf("Result: %08x, %02x; %02x\n", result, (result >> 23) & 0xFF, float_exception_flags);

  if(!FPU_DoesExceptionKillResult())
  {
   // Force it to +/- zero before setting S/Z based off of it(confirmed with subf.s on real V810, at least).
   if(float_exception_flags & float_flag_underflow)
    result &= 0x80000000;

   SetFPUOPNonFPUFlags(result);
   SetPREG(arg1, result);
  }
  FPU_DoException();
 }
}

void V810VB::fpu_subop(v810_timestamp_t &timestamp, int sub_op, int arg1, int arg2)
{
 //printf("FPU: %02x\n", sub_op);
 if(VBMode)
 {
  switch(sub_op)
  {
   case XB: timestamp++;	// Unknown
	    P_REG[arg1] = (P_REG[arg1] & 0xFFFF0000) | ((P_REG[arg1] & 0xFF) << 8) | ((P_REG[arg1] & 0xFF00) >> 8);
	    return;

   case XH: timestamp++;	// Unknown
	    P_REG[arg1] = (P_REG[arg1] << 16) | (P_REG[arg1] >> 16);
	    return;

   // Does REV use arg1 or arg2 for the source register?
   case REV: timestamp++;	// Unknown
	     {
	      // Public-domain code snippet from: http://graphics.stanford.edu/~seander/bithacks.html#ReverseParallel
      	      uint32 v = P_REG[arg2]; // 32-bit word to reverse bit order

	      // swap odd and even bits
	      v = ((v >> 1) & 0x55555555) | ((v & 0x55555555) << 1);
	      // swap consecutive pairs
	      v = ((v >> 2) & 0x33333333) | ((v & 0x33333333) << 2);
	      // swap nibbles ... 
	      v = ((v >> 4) & 0x0F0F0F0F) | ((v & 0x0F0F0F0F) << 4);
	      // swap bytes
	      v = ((v >> 8) & 0x00FF00FF) | ((v & 0x00FF00FF) << 8);
	      // swap 2-byte long pairs
	      v = ( v >> 16             ) | ( v               << 16);

	      P_REG[arg1] = v;
	     }
	     return;

   case MPYHW: timestamp += 9 - 1;	// Unknown?
	       P_REG[arg1] = (int32)(int16)(P_REG[arg1] & 0xFFFF) * (int32)(int16)(P_REG[arg2] & 0xFFFF);
	       return;
  }
 }

 switch(sub_op) 
 {
        // Virtual-Boy specific(probably!)
	default:
		{
		 SetPC(GetPC() - 4);
                 Exception(INVALID_OP_HANDLER_ADDR, ECODE_INVALID_OP);
		}
		break;

	case CVT_WS: 
		timestamp += 5;
		{
		 uint32 result;

                 float_exception_flags = 0;
		 result = int32_to_float32((int32)P_REG[arg2]);

		 if(!FPU_DoesExceptionKillResult())
		 {
		  SetPREG(arg1, result);
		  SetFPUOPNonFPUFlags(result);
		 }
		 else
		 {
		  puts("Exception on CVT.WS?????");	// This shouldn't happen, but just in case there's a bug...
		 }
		 FPU_DoException();
		}
		break;	// End CVT.WS

	case CVT_SW:
		timestamp += 8;
                if(CheckFPInputException(P_REG[arg2]))
                {

                }
		else
		{
		 int32 result;

                 float_exception_flags = 0;
		 result = float32_to_int32(P_REG[arg2]);

		 if(!FPU_DoesExceptionKillResult())
		 {
		  SetPREG(arg1, result);
                  SetFlag(PSW_OV, 0);
                  SetSZ(result);
		 }
		 FPU_DoException();
		}
		break;	// End CVT.SW

	case ADDF_S: timestamp += 8;
		     FPU_Math_Template(float32_add, arg1, arg2);
		     break;

	case SUBF_S: timestamp += 11;
		     FPU_Math_Template(float32_sub, arg1, arg2);
		     break;

        case CMPF_S: timestamp += 6;
		     // Don't handle this like subf.s because the flags
		     // have slightly different semantics(mostly regarding underflow/subnormal results) (confirmed on real V810).
                     if(CheckFPInputException(P_REG[arg1]) || CheckFPInputException(P_REG[arg2]))
                     {

                     }
		     else
		     {
		      SetFlag(PSW_OV, 0);

		      if(float32_eq(P_REG[arg1], P_REG[arg2]))
		      {
		       SetFlag(PSW_Z, 1);
		       SetFlag(PSW_S, 0);
		       SetFlag(PSW_CY, 0);
		      }
		      else
		      {
		       SetFlag(PSW_Z, 0);

		       if(float32_lt(P_REG[arg1], P_REG[arg2]))
		       {
		        SetFlag(PSW_S, 1);
		        SetFlag(PSW_CY, 1);
		       }
		       else
		       {
		        SetFlag(PSW_S, 0);
		        SetFlag(PSW_CY, 0);
                       }
		      }
		     }	// else of if(CheckFP...
                     break;

	case MULF_S: timestamp += 7;
		     FPU_Math_Template(float32_mul, arg1, arg2);
		     break;

	case DIVF_S: timestamp += 43;
		     FPU_Math_Template(float32_div, arg1, arg2);
		     break;

	case TRNC_SW:
                timestamp += 7;

		if(CheckFPInputException(P_REG[arg2]))
		{

		}
		else
                {
                 int32 result;

		 float_exception_flags = 0;
                 result = float32_to_int32_round_to_zero(P_REG[arg2]);

                 if(!FPU_DoesExceptionKillResult())
                 {
                  SetPREG(arg1, result);
		  SetFlag(PSW_OV, 0);
		  SetSZ(result);
                 }
		 FPU_DoException();
                }
                break;	// end TRNC.SW
	}
}

// Generate exception
void V810VB::Exception(uint32 handler, uint16 eCode) 
{
 // Exception overhead is unknown.

    MDFN_printf("Exception: %08x %04x\n", handler, eCode);

    // Invalidate our bitstring state(forces the instruction to be re-read, and the r/w buffers reloaded).
    in_bstr = FALSE;
    have_src_cache = FALSE;
    have_dst_cache = FALSE;

    if(S_REG[PSW] & PSW_NP) // Fatal exception
    {
     MDFN_printf("Fatal exception; Code: %08x, ECR: %08x, PSW: %08x, PC: %08x\n", eCode, S_REG[ECR], S_REG[PSW], PC);
     Halted = HALT_FATAL_EXCEPTION;
     IPendingCache = 0;
     return;
    }
    else if(S_REG[PSW] & PSW_EP)  //Double Exception
    {
     S_REG[FEPC] = GetPC();
     S_REG[FEPSW] = S_REG[PSW];

     S_REG[ECR] = (S_REG[ECR] & 0xFFFF) | (eCode << 16);
     S_REG[PSW] |= PSW_NP;
     S_REG[PSW] |= PSW_ID;
     S_REG[PSW] &= ~PSW_AE;

     SetPC(0xFFFFFFD0);
     IPendingCache = 0;
     return;
    }
    else 	// Regular exception
    {
     S_REG[EIPC] = GetPC();
     S_REG[EIPSW] = S_REG[PSW];
     S_REG[ECR] = (S_REG[ECR] & 0xFFFF0000) | eCode;
     S_REG[PSW] |= PSW_EP;
     S_REG[PSW] |= PSW_ID;
     S_REG[PSW] &= ~PSW_AE;

     SetPC(handler);
     IPendingCache = 0;
     return;
    }
}

int V810VB::StateAction(StateMem *sm, int load, int data_only)
{
 uint32 PC_tmp = GetPC();
 SFORMAT StateRegs[] =
 {
  SFARRAY32(P_REG, 32),
  SFARRAY32(S_REG, 32),
  SFVARN(PC_tmp, "PC"),
  SFVAR(Halted),

  SFVAR(lastop),

  SFVAR(ilevel),	// Perhaps remove in future?
  SFVAR(next_event_ts),	// This too

  //SFVAR(tmp_timestamp),
  SFVAR(v810_timestamp),

  // Bitstring stuff:
  SFVAR(src_cache),
  SFVAR(dst_cache),
  SFVAR(have_src_cache),
  SFVAR(have_dst_cache),
  SFVAR(in_bstr),
  SFVAR(in_bstr_to),

  SFEND
 };

 int ret = MDFNSS_StateAction(sm, load, data_only, StateRegs, "V810");

 if(load)
 {
  RecalcIPendingCache();
  //clamp(&PCFX_V810.v810_timestamp, 0, 30 * 1000 * 1000);
  SetPC(PC_tmp);
 }

 return(ret);
}

