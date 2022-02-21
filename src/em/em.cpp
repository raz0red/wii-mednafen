#include "config.h"
#include "main.h"
#include "sound.h"
#include <trio/trio.h>
#include <emscripten.h>

// Mednafen external references
extern volatile Uint32 MainThreadID;
extern MDFNSetting DriverSettings[];
extern int DriverSettingsSize;
extern std::vector<MDFNSetting> NeoDriverSettings;
extern char* DrBaseDirectory;
extern volatile MDFN_Surface* VTReady;
extern volatile MDFN_Rect* VTLWReady;
extern volatile MDFN_Rect* VTDRReady;
extern MDFN_Rect VTDisplayRects[2];
extern volatile MDFN_Surface* VTBuffer[2];
extern MDFN_Rect* VTLineWidths[2];
extern volatile int NeedVideoChange;
extern MDFNGI* MDFNGameInfo;
extern int RewindState;

extern int LoadGame(const char* force_module, const char* path);
extern int GameLoop(void* arg);
extern char* GetBaseDirectory(void);
extern void KillVideo(void);
extern void FPS_Init(void);
extern void MakeMednafenArgsStruct(void);
extern bool CreateDirs(void);
extern void MakeVideoSettings(std::vector<MDFNSetting>& settings);
extern void MakeInputSettings(std::vector<MDFNSetting>& settings);
extern void DeleteInternalArgs(void);
extern void KillInputSettings(void);
extern void CalcFramerates(char* virtfps,
                           char* drawnfps,
                           char* blitfps,
                           size_t maxlen);
extern void FPS_IncBlitted(void);
extern void BlitScreen(MDFN_Surface* msurface,
                       const MDFN_Rect* DisplayRect,
                       const MDFN_Rect* LineWidths);

extern void KillPortInfo();
extern void BuildPortInfo(MDFNGI* gi);
extern SDL_Surface *screen;

// The back surface
SDL_Surface *back_surface;

// The original 8bpp palette
SDL_Palette orig_8bpp_palette;
SDL_Color orig_8bpp_colors[512];

/**
 * Initializes SDL
 */
static int initSdl() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        return 0;
    }

#if 0
    if (SDL_InitSubSystem(SDL_INIT_VIDEO) < 0) {
        return 0;
    }

    back_surface = SDL_SetVideoMode(
        640, 480, 8, SDL_HWSURFACE | SDL_HWPALETTE | SDL_FULLSCREEN);

    if (!back_surface) {
        return 0;
    }

    int ncolors = back_surface->format->palette->ncolors;
    memcpy(orig_8bpp_colors, back_surface->format->palette->colors,
           ncolors * sizeof(SDL_Color));
    orig_8bpp_palette.colors = orig_8bpp_colors;
    orig_8bpp_palette.ncolors = ncolors;
#endif    

    return 1;
}

/**
 * Resets the current video mode
 */
static void resetVideo() {    
#ifndef WRC
    free_video();
#else
    uint8 bpp =  32;
#endif    

    back_surface = SDL_SetVideoMode(
        MDFNGameInfo->fb_width, MDFNGameInfo->fb_height, 
        bpp, SDL_HWSURFACE);

    MDFN_PixelFormat nf;
    nf.bpp = bpp;
    nf.colorspace = MDFN_COLORSPACE_RGB;

    nf.Rshift = 0;
    nf.Gshift = 8;
    nf.Bshift = 16;
    nf.Ashift = 24;
    nf.Rprec = 8;
    nf.Gprec = 8;
    nf.Bprec = 8;
    nf.Aprec = 8;

    MDFN_printf(_("BPP: %d\n"), nf.bpp);

    VTReady = NULL;
    VTDRReady = NULL;
    VTLWReady = NULL;

    VTBuffer[0] =
        new MDFN_Surface(NULL, MDFNGameInfo->fb_width, MDFNGameInfo->fb_height,
                         MDFNGameInfo->fb_width, nf);
    VTBuffer[1] =
        new MDFN_Surface(NULL, MDFNGameInfo->fb_width, MDFNGameInfo->fb_height,
                         MDFNGameInfo->fb_width, nf);
    VTLineWidths[0] =
        (MDFN_Rect*)calloc(MDFNGameInfo->fb_height, sizeof(MDFN_Rect));
    VTLineWidths[1] =
        (MDFN_Rect*)calloc(MDFNGameInfo->fb_height, sizeof(MDFN_Rect));

    screen = back_surface;
}

static void preLoop() {
#if 0    
    Emulator* emu = emuRegistry.getCurrentEmulator();
    update_ports();
    wii_mednafen_enable_rewind(wii_rewind);  // Update rewind
#endif    

    resetVideo();

#if 0
    emu->getDbManager().applyButtonMap();  // Apply the button map
    emu->onPreLoop();

    if (MDFN_GetSettingB("cheats") != wii_cheats) {
        MDFNI_SetSettingB("cheats", wii_cheats);
    }
#endif    

    for (int i = 0; i < 2; i++)
        ((MDFN_Surface*)VTBuffer[i])->Fill(0, 0, 0, 0);

#if 0
    WII_SetRotation(emu->getRotation() * 90);
#endif    

    ClearSound();
    PauseSound(0);

    GameThreadRun = 1;
    NeedVideoChange = 0;

#if 0
    GameLoop(NULL);
    PauseSound(1);
#endif        
}


/**
 * Initializes the emulator
 */
extern "C" void emInit() {
    // Initialize SDL
    initSdl();

    std::vector<MDFNGI*> ExternalSystems;
    MainThreadID = SDL_ThreadID();

    DrBaseDirectory = GetBaseDirectory();

    MDFNI_printf(_("Starting Mednafen %s\n"), MEDNAFEN_VERSION);
    MDFN_indent(1);
    MDFN_printf(_("Base directory: %s\n"), DrBaseDirectory);

    // Look for external emulation modules here.
    if (!MDFNI_InitializeModules(ExternalSystems)) {
        MDFN_PrintError("Unable to initialize external modules");
        exit(0);
    }

    for (unsigned int x = 0; x < DriverSettingsSize / sizeof(MDFNSetting); x++)
        NeoDriverSettings.push_back(DriverSettings[x]);

    MakeVideoSettings(NeoDriverSettings);
    MakeInputSettings(NeoDriverSettings);

    if (!(MDFNI_Initialize(DrBaseDirectory, NeoDriverSettings))) {
        MDFN_PrintError("Error during initialization");
        exit(0);
    }

    MakeMednafenArgsStruct();
    CreateDirs();

    FPS_Init();        
}

// Mednafen extern
namespace PCE_Fast
{
  extern bool AVPad6Enabled[5];
}

extern "C" void emPceSet6PadEnabled(int index, bool enabled) {
    printf("PCE 6 pad enabled: %d = %d\n", index, enabled);
    PCE_Fast::AVPad6Enabled[index] = enabled;
}

static int firstLoop = true;

static int lastWidth = 0;
static int lastHeight = 0;

extern "C" void emStep() {
    if (firstLoop) {
        preLoop();
        firstLoop = false;                        
        PauseSound(0);
    }

    PauseSound(0);
    GameLoop(NULL);

    if (VTReady && GameThreadRun) {
        if (lastWidth != VTDRReady->w || lastHeight != VTDRReady->h) {
            if (lastWidth != VTDRReady->w) {
                SDL_FillRect(back_surface, NULL,
                             SDL_MapRGB(back_surface->format, 0x0, 0x0, 0x0));
            }

            // Store last rect values
            lastWidth = VTDRReady->w;
            lastHeight = VTDRReady->h;

            EM_ASM({
                    const canvas = document.getElementById('canvas');
                    canvas.width = $0;
                    canvas.height = $1;
                    console.log("Screen size: " + $0 + "x" + $1);
            }, lastWidth, lastHeight);
        }

        BlitScreen((MDFN_Surface*)VTReady, (MDFN_Rect*)VTDRReady,
                   (MDFN_Rect*)VTLWReady);
        FPS_IncBlitted();
        VTReady = NULL;
    }
}

extern "C" int Ngp_FlashSave();
extern "C" int Pce_SramSave();
extern "C" int VB_SramSave();
extern "C" int WSwan_SramSave();

extern "C" int emSramSave() {
    const char* systemName = 
        MDFNGameInfo && MDFNGameInfo->shortname ?
            MDFNGameInfo->shortname : NULL;
    //printf("## systemName: %s\n", systemName);
    if (systemName) {
        const char* saveName = "sram.sav";        
        if (!strcmp(systemName, "vb")) {
            return VB_SramSave();
        } else if (!strcmp(systemName, "wswan")) {
            return WSwan_SramSave();
        } else if (!strcmp(systemName, "ngp")) {
            return Ngp_FlashSave();
        } else if (!strcmp(systemName, "pce_fast")) {
            return Pce_SramSave();
        }
    }

    return 0;
}