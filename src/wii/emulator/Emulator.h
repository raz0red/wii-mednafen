#ifndef EMULATOR_H
#define EMULATOR_H

#include <gctypes.h>
#include <stdio.h>
#include <string>
using namespace std;

// Mednafen externs
extern volatile int GameThreadRun;

class ConfigManager;
class DatabaseManager;
class MenuManager;

/**
 * Rectangle (used to specify width and height)
 */
typedef struct Rect {
    /** The width */
    u16 w;
    /** The height */
    u16 h;
} Rect;

/**
 * Represents a specific screen size
 */
typedef struct ScreenSize {
    /** The screen size */
    Rect r;
    /** The name of the specific screen size (2x, etc.) */
    const char* name;
} ScreenSize;

#define BTN_RAPID 0x80000000
#define BTN_REWIND 0x40000000

#define DOUBLE_STRIKE_DISABLED 0
#define DOUBLE_STRIKE_ENABLED 1
#define DOUBLE_STRIKE_DEFAULT 2

#define VOLUME_DEFAULT 160

/**
 * Base class for the various emulators supported by WiiMednafen.
 * This class and its sub-classes attempt to encapsulate the functionality that
 * is specific to each emulator.
 */
class Emulator {
private:
    /** The key associated with the specific emulator in the map of emulators */
    const char* m_key;
    /** The name of the emulator */
    const char* m_name;
    /** Whether frame skip is enabled */
    bool m_frameSkip;

protected:
    /** Status of the various game pads */
    u16 m_padData[4];
    /** The size of the emulator screen (size of the backing surface) */
    Rect m_emulatorScreenSize;
    /** The user-specified screen size */
    Rect m_screenSize;
    /** The user-specified screen size when rotated */
    Rect m_rotatedScreenSize;
    /** Current double strike mode (enabled, disabled, default, etc.) */
    int m_doubleStrike;
    /** The volume */
    int m_volume;

    /**
     * Constructs the emulator
     *
     * @param   key The key associated with the specific emulator in the map of
     *          emulators
     * @param   name The name of the emulator
     */
    Emulator(const char* key, const char* name);

    /**
     * The screen size of the emulator in double strike mode
     *
     * @return  The screen size of the emulator in double strike mode
     */
    virtual const ScreenSize* getDoubleStrikeScreenSize();

    /**
     * The screen size of the emulator in double strike mode when rotated
     *
     * @return  The screen size of the emulator in double strike mode when
     *          rotated
     */
    virtual const ScreenSize* getDoubleStrikeRotatedScreenSize();

public:
    /**
     * Returns the key associated with the specific emulator in the map of
     * emulators
     *
     * @return  The key associated with the specific emulator in the map of
     *          emulators
     */
    const char* getKey();

    /**
     * Returns the name of the emulator 
     *
     * @return  The name of the emulator 
     */
    const char* getName();

    /**
     * Returns the {@class ConfigManager} for the emulator
     * 
     * @return  The {@class ConfigManager} for the emulator
     */
    virtual ConfigManager& getConfigManager() = 0;

    /**
     * Returns the {@class DatabaseManager} for the emulator
     * 
     * @return  The {@class DatabaseManager} for the emulator
     */
    virtual DatabaseManager& getDbManager() = 0;

    /**
     * Returns the {@class MenuManager} for the emulator
     * 
     * @return  The {@class MenuManager} for the emulator
     */
    virtual MenuManager& getMenuManager() = 0;

    /**
     * Updates the state of the controllers
     * 
     * @param   isRapid Whether rapid fire is occurring
     */
    virtual void updateControls(bool isRapid) = 0;

    /**
     * Invoked after the current game is loaded
     */
    virtual void onPostLoad();

    /**
     * Gives emulator an opportunity to show a "controls" screen. displays 
     * applicable controllers, etc.
     * 
     * @return  Whether to start emulation after the screen was displayed
     */
    virtual bool onShowControlsScreen();

    /**
     * Invoked prior to the emulator loop starting
     */
    virtual void onPreLoop();

    /**
     * Provides the emulator an opportunity to update the debug text
     * 
     * @param   output The final debug text
     * @param   defaultOutput the default debug text
     * @param   len The length of the output text
     * @return  Whether the text was updated
     */
    virtual bool updateDebugText(char* output,
                                 const char* defaultOutput,
                                 int len);

    /**
     * Whether the emulator supports screen rotation
     * 
     * @return    Whether the emulator supports screen rotation
     */
    virtual bool isRotationSupported();

    /**
     * Returns the current rotation state of the emulator (MDFN_ROTATE0, etc.)
     *
     * @return    The current rotation state of the emulator (MDFN_ROTATE0,
     *            etc.)
     */
    virtual int getRotation();
    
    /**
     * Returns the bits per pixel (BPP) for the emulator
     * 
     * @return    the bits per pixel (BPP) for the emulator
     */
    virtual u8 getBpp();

    /**
     * Resizes the screen to match the screen size set by the user
     */
    virtual void resizeScreen();

    /**
     * Specifies how the screen should be resized
     *
     * @param   rect (out) The rect that contains how the screen should be
     *          resized
     */
    void getResizeScreenRect(Rect* rect);

    /**
     * Returns the screen size ratio
     * 
     * @param   ratiox (out) The x ratio
     * @param   ratioy (out) The y ratio
     */
    void getCurrentScreenSizeRatio(float* ratiox, float* ratioy);

    /**
     * Returns an array of default screen sizes
     * 
     * @return  The array of default screen sizes
     */
    virtual const ScreenSize* getDefaultScreenSizes();

    /**
     * Returns the count of default screen sizes
     * 
     * @return  The count of default screen sizes
     */
    virtual int getDefaultScreenSizesCount();

    /**
     * Returns the name of the current screen size
     * 
     * @return  The name of the current screen size
     */
    const char* getScreenSizeName();

    /**
     * Returns the name of the screen size with the specified dimensions
     *
     * @return    The name of the screen size with the specified dimensions
     */
    const char* getScreenSizeName(int w, int h);

    /**
     * Returns an array of default rotated screen sizes
     * 
     * @return  The array of default roteated screen sizes
     */
    virtual const ScreenSize* getDefaultRotatedScreenSizes();

    /**
     * Returns the count of default rotated screen sizes
     * 
     * @return  The count of default rotated screen sizes
     */
    virtual int getDefaultRotatedScreenSizesCount();

    /**
     * Returns the name of the current rotated screen size
     * 
     * @return  The name of the current rotated screen size
     */
    const char* getRotatedScreenSizeName();

    /**
     * Returns the name of the rotated screen size with the specified dimensions
     *
     * @return    The name of the rotated screen size with the specified
     *            dimensions
     */
    const char* getRotatedScreenSizeName(int w, int h);

    /**
     * Returns whether the emulator supports double strike mode
     * 
     * @return  Whether the emulator supports double strike mode
     */
    virtual bool isDoubleStrikeSupported();

    /**
     * Returns whether double strike mode is enabled for the emulator
     *
     * @return  Whether double strike mode is enabled for the emulator
     */
    virtual bool isDoubleStrikeEnabled();

    /**
     * Sets the double strike mode for the emulator (enabled, disabled, default)
     *
     * @param   mode The double strike mode for the emulator (enabled, disabled,
     *          default)
     */
    void setDoubleStrikeMode(int mode);

    /**
     * Returns the double strike mode for the emulator (enabled, disabled,
     * default)
     *
     * @return  The double strike mode for the emulator (enabled, disabled,
     *          default)
     */
    int getDoubleStrikeMode();

    /**
     * Returns the size of the emulator screen (size of the backing surface)
     *
     * @return  The size of the emulator screen (size of the backing surface)
     */
    Rect* getEmulatorScreenSize();

    /**
     * Returns the user-specified screen size 
     * 
     * @return  The user-specified screen size 
     */
    Rect* getScreenSize();

    /**
     * Returns the user-specified rotated screen size 
     * 
     * @return  The user-specified rotated screen size 
     */
    Rect* getRotatedScreenSize();

    /**
     * Returns whether rewind is supported by the emulator
     * 
     * @return  Whether rewind is supported by the emulator
     */
    virtual bool isRewindSupported();

    /**
     * Returns the volume for the emulator
     * 
     * @return  The volume for the emulator
     */
    int getVolume();

    /**
     * Sets the volume for the emulator
     * 
     * @param   volume The volume for the emulator
     */
    void setVolume(int volume);

    /**
     * Returns the applied volume for the emulator (considers default)
     *
     * @return  The applied volume for the emulator (considers default)
     */
    int getAppliedVolume();

    /**
     * Invoked by Mednafen to update the input device data 
     *
     * @param   device The device being updated
     * @param   data The input device data
     * @param   size The size of the input device data
     */
    virtual void updateInputDeviceData(int device, u8* data, int size);

    /**
     * Returns an array of strings indicating the current set of input devices
     * for the emulator
     *
     * @return  An array of strings indicating the current set of input devices
     *          for the emulator
     */
    virtual const char** getInputDevices();

    /**
     * Whether frame skip is enabled
     * 
     * @return  Whether frame skip is enabled
     */
    bool getFrameSkip();

    /**
     * Sets whether frame skip is enabled
     * 
     * @param   skip Whether frame skip is enabled
     */
    void setFrameSkip(bool skip);

    /**
     * Whether frame skip is enabled (considers setting for game from db)
     *
     * @return  Whether frame skip is enabled (considers setting for game from
     *          db)
     */
    bool getAppliedFrameSkip();
};

#endif