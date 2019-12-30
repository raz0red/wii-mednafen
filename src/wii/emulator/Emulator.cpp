#include "main.h"
#include "Emulator.h"
#include "DatabaseManager.h"

#include "wii_mednafen.h"
#include "wii_mednafen_main.h"

#ifdef WII_NETTRACE
#include <network.h>
#include "net_print.h"
#endif

// External Mednafen references
extern volatile MDFN_Rect* VTDRReady;
extern volatile MDFN_Surface* VTReady;
extern MDFNGI* MDFNGameInfo;

extern uint32 round_up_pow2(uint32 v);

// External SDL references
extern "C" {
void WII_SetRotation(int rot);
void WII_ChangeSquare(int xscale, int yscale, int xshift, int yshift);
}


/**
 * Constructs the emulator
 *
 * @param   key The key associated with the specific emulator in the map of
 *          emulators
 * @param   name The name of the emulator
 */
Emulator::Emulator(const char* key, const char* name)
    : m_key(key), m_name(name), m_frameSkip(true) {
    memset(m_padData, 0, sizeof(u32) << 2);
    m_doubleStrike = DOUBLE_STRIKE_DEFAULT;
    m_volume = VOLUME_DEFAULT;
}

/** {@inheritDoc} */
Rect* Emulator::getScreenSize() {
    return &m_screenSize;
}

/** {@inheritDoc} */
Rect* Emulator::getRotatedScreenSize() {
    return &m_rotatedScreenSize;
}

/** {@inheritDoc} */
Rect* Emulator::getEmulatorScreenSize() {
    return &m_emulatorScreenSize;
}

/** {@inheritDoc} */
const char* Emulator::getKey() {
    return m_key;
}

/** {@inheritDoc} */
const char* Emulator::getName() {
    return m_name;
}

/** {@inheritDoc} */
void Emulator::onPostLoad() {}

/** {@inheritDoc} */
bool Emulator::onShowControlsScreen() {
    dbEntry* entry = getDbManager().getEntry();
    if (entry->wiimoteSupported) {
        // Wiimote is supported
        return true;
    }

    return wii_mednafen_show_controls_screen();
}

/** {@inheritDoc} */
void Emulator::onPreLoop() {}

/** {@inheritDoc} */
bool Emulator::updateDebugText(char* output,
                               const char* defaultOutput,
                               int len) {
    return false;
}

/** {@inheritDoc} */
bool Emulator::isRotationSupported() {
    return false;
}

/** {@inheritDoc} */
int Emulator::getRotation() {
    return MDFN_ROTATE0;
}

/** {@inheritDoc} */
void Emulator::getCurrentScreenSizeRatio(float* ratiox, float* ratioy) {
    *ratiox = (float)MDFNGameInfo->nominal_width / (float)VTDRReady->w;
    *ratioy = (float)MDFNGameInfo->nominal_height / (float)VTDRReady->h;
}

/** {@inheritDoc} */
void Emulator::getResizeScreenRect(Rect* rect) {
    float ratiox, ratioy;
    getCurrentScreenSizeRatio(&ratiox, &ratioy);

    if (isDoubleStrikeEnabled()) {
        const ScreenSize* size = getRotation()
                                     ? getDoubleStrikeRotatedScreenSize()
                                     : getDoubleStrikeScreenSize();

        rect->w = (float)size->r.w * ratiox;
        rect->h = (float)size->r.h * ratioy;
    } else {
        if (getRotation()) {
            rect->w = (float)m_rotatedScreenSize.w * ratiox;
            rect->h = (float)m_rotatedScreenSize.h * ratioy;
        } else {
            rect->w = (float)m_screenSize.w * ratiox;
            rect->h = (float)m_screenSize.h * ratioy;
        }
    }
#ifdef WII_NETTRACE
    net_print_string(
        NULL, 0, "resizing to: w:%dx%d, ratio:%f,%f, rect->w:%d, rect->h:%d\n",
        VTDRReady->w, VTDRReady->h, ratiox, ratioy, rect->w, rect->h);
#endif

    if (wii_16_9_correction == WS_AUTO ? is_widescreen : wii_16_9_correction) {
        if (getRotation()) {
            rect->h = (rect->h * 3) / 4;
        } else {
            rect->w = (rect->w * 3) / 4;
        }
    }

    rect->w = ((rect->w + 1) & ~1);
    rect->h = ((rect->h + 1) & ~1);
}

/** {@inheritDoc} */
void Emulator::resizeScreen() {
    Rect r;
    getResizeScreenRect(&r);
    WII_ChangeSquare(r.w, r.h, 0, 0);
#ifdef WII_NETTRACE
    net_print_string(NULL, 0, "resizeScreen, change square: %dx%d\n", r.w, r.h);
#endif
}

/** {@inheritDoc} */
bool Emulator::getFrameSkip() {
    return m_frameSkip;
}

/** {@inheritDoc} */
void Emulator::setFrameSkip(bool skip) {
    m_frameSkip = skip;
}

/** {@inheritDoc} */
bool Emulator::getAppliedFrameSkip() {
    dbEntry* entry = getDbManager().getEntry();
    if (entry->frameSkip == FRAME_SKIP_DEFAULT) {
        return getFrameSkip();
    }

    return entry->frameSkip == FRAME_SKIP_ENABLED;
}

/** {@inheritDoc} */
bool Emulator::isDoubleStrikeSupported() {
    return true;
}

/** {@inheritDoc} */
u8 Emulator::getBpp() {
    return 32;
}

/** {@inheritDoc} */
const ScreenSize* Emulator::getDefaultScreenSizes() {
    return NULL;
}

/** {@inheritDoc} */
int Emulator::getDefaultScreenSizesCount() {
    return 0;
}

/** {@inheritDoc} */
const char* Emulator::getScreenSizeName() {
    return getScreenSizeName(m_screenSize.w, m_screenSize.h);
}

/** {@inheritDoc} */
const char* Emulator::getScreenSizeName(int w, int h) {
    const ScreenSize* sizes = getDefaultScreenSizes();
    for (int i = 0; i < getDefaultScreenSizesCount(); i++) {
        if (sizes[i].r.w == w && sizes[i].r.h == h) {
            return sizes[i].name;
        }
    }

    return "Custom";
}

/** {@inheritDoc} */
const ScreenSize* Emulator::getDefaultRotatedScreenSizes() {
    return NULL;
}

/** {@inheritDoc} */
int Emulator::getDefaultRotatedScreenSizesCount() {
    return 0;
}

/** {@inheritDoc} */
const char* Emulator::getRotatedScreenSizeName() {
    return getRotatedScreenSizeName(m_rotatedScreenSize.w,
                                    m_rotatedScreenSize.h);
}

/** {@inheritDoc} */
const char* Emulator::getRotatedScreenSizeName(int w, int h) {
    const ScreenSize* sizes = getDefaultRotatedScreenSizes();
    for (int i = 0; i < getDefaultRotatedScreenSizesCount(); i++) {
        if (sizes[i].r.w == w && sizes[i].r.h == h) {
            return sizes[i].name;
        }
    }

    return "Custom";
}

/** {@inheritDoc} */
const ScreenSize* Emulator::getDoubleStrikeScreenSize() {
    return NULL;
}

/** {@inheritDoc} */
const ScreenSize* Emulator::getDoubleStrikeRotatedScreenSize() {
    return NULL;
}

/** {@inheritDoc} */
void Emulator::setDoubleStrikeMode(int mode) {
    m_doubleStrike = mode;
}

/** {@inheritDoc} */
int Emulator::getDoubleStrikeMode() {
    return m_doubleStrike;
}

/** {@inheritDoc} */
bool Emulator::isDoubleStrikeEnabled() {
    return isDoubleStrikeSupported() &&
           ((m_doubleStrike == DOUBLE_STRIKE_ENABLED) ||
            ((m_doubleStrike == DOUBLE_STRIKE_DEFAULT) &&
             wii_double_strike_mode));
}

/** {@inheritDoc} */
int Emulator::getVolume() {
    return m_volume;
}

/** {@inheritDoc} */
void Emulator::setVolume(int volume) {
    m_volume = volume;
}

/** {@inheritDoc} */
int Emulator::getAppliedVolume() {
    if (m_volume == VOLUME_DEFAULT) {
        return wii_volume;
    } else {
        return m_volume;
    }
}

/** {@inheritDoc} */
bool Emulator::isRewindSupported() {
    return false;
}

/** {@inheritDoc} */
const char** Emulator::getInputDevices() {
    return NULL;
}

/** {@inheritDoc} */
void Emulator::updateInputDeviceData(int device, u8* data, int size) {
    if (device < 4) {
        u16 padData = m_padData[device];
        if (size >= 1) {
            data[0] = padData & 0xFF;
        }
        if (size >= 2) {
            data[1] = (padData >> 8) & 0xFF;
        }
    }
}