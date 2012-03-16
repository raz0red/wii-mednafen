#ifndef CONTROLMACROS_H
#define CONTROLMACROS_H

#define READ_CONTROLS_STATE                                                   \
  /* Check the state of the controllers */                                    \
  u32 pressed = WPAD_ButtonsDown( c );                                        \
  u32 held = WPAD_ButtonsHeld( c );                                           \
  u32 gcPressed = PAD_ButtonsDown( c );                                       \
  u32 gcHeld = PAD_ButtonsHeld( c );                                          \
                                                                              \
  /* Classic or Nunchuck? */                                                  \
  expansion_t exp;                                                            \
  WPAD_Expansion( c, &exp );                                                  \
                                                                              \
  BOOL isClassic = ( exp.type == WPAD_EXP_CLASSIC );                          \
  BOOL isNunchuk = ( exp.type == WPAD_EXP_NUNCHUK );                          \
                                                                              \
  /* Mask off the Wiimote d-pad depending on whether a nunchuk */             \
  /* is connected. (Wiimote d-pad is left when nunchuk is not  */             \
  /* connected, right when it is). */                                         \
  u32 heldLeft = ( isNunchuk ? ( held & ~0x0F00 ) : held );                   \
  u32 heldRight = ( !isNunchuk ? ( held & ~0x0F00 ) : held );                 \
                                                                              \
  /* Analog for Wii controls */                                               \
  float expX = wii_exp_analog_val( &exp, TRUE, FALSE );                       \
  float expY = wii_exp_analog_val( &exp, FALSE, FALSE );                      \
  float expRX = isClassic ? wii_exp_analog_val( &exp, TRUE, TRUE ) : 0;       \
  float expRY = isClassic ? wii_exp_analog_val( &exp, FALSE, TRUE ) : 0;      \
                                                                              \
  /* Analog for Gamecube controls */                                          \
  s8 gcX = PAD_StickX( c );                                                   \
  s8 gcY = PAD_StickY( c );                                                   \
  s8 gcRX = PAD_SubStickX( c );                                               \
  s8 gcRY = PAD_SubStickY( c );                                               \
                                                                              \
  if( c == 0 )                                                                \
  {                                                                           \
    /* Check for home */                                                      \
    if( ( pressed & WII_BUTTON_HOME ) ||                                      \
      ( gcPressed & GC_BUTTON_HOME ) ||                                       \
      wii_hw_button )                                                         \
    {                                                                         \
      GameThreadRun = 0;                                                      \
    }                                                                         \
  }                                                                           \

#define BEGIN_IF_BUTTON_HELD(p)                            \
  if( ( held &                                             \
        ( ( isClassic ?                                    \
              entry->appliedButtonMap[p][                  \
                WII_CONTROLLER_CLASSIC ][ i ] : 0 ) |      \
          ( isNunchuk ?                                    \
              entry->appliedButtonMap[p][                  \
                WII_CONTROLLER_CHUK ][ i ] :               \
              entry->appliedButtonMap[p][                  \
                WII_CONTROLLER_MOTE ][ i ] ) ) ) ||        \
      ( gcHeld &                                           \
          entry->appliedButtonMap[p][                      \
            WII_CONTROLLER_CUBE ][ i ] ) )                 \
  {                                                        \

#define END_IF_BUTTON_HELD }

#define IF_RIGHT                                              \
  if( wii_digital_right( !isNunchuk, isClassic, heldLeft ) || \
      ( gcHeld & GC_BUTTON_RIGHT ) ||                         \
      wii_analog_right( expX, gcX ) )                         \

#define IF_LEFT                                               \
  if( wii_digital_left( !isNunchuk, isClassic, heldLeft ) ||  \
      ( gcHeld & GC_BUTTON_LEFT ) ||                          \
      wii_analog_left( expX, gcX ) )                          \

#define IF_UP                                                 \
  if( wii_digital_up( !isNunchuk, isClassic, heldLeft ) ||    \
      ( gcHeld & GC_BUTTON_UP ) ||                            \
      wii_analog_up( expY, gcY ) )                            \

#define IF_DOWN                                               \
  if( wii_digital_down( !isNunchuk, isClassic, heldLeft ) ||  \
      ( gcHeld & GC_BUTTON_DOWN ) ||                          \
      wii_analog_down( expY, gcY ) )                          \

#define IF_R_RIGHT                                            \
  if( wii_digital_right( !isNunchuk, 0, heldRight ) ||        \
      wii_analog_right( expRX, gcRX ) )                       \

#define IF_R_LEFT                                             \
  if( wii_digital_left( !isNunchuk, 0, heldRight ) ||         \
      wii_analog_left( expRX, gcRX ) )                        \

#define IF_R_UP                                               \
  if( wii_digital_up( !isNunchuk, 0, heldRight ) ||           \
      wii_analog_up( expRY, gcRY ) )                          \

#define IF_R_DOWN                                             \
  if( wii_digital_down( !isNunchuk, 0, heldRight ) ||         \
      wii_analog_down( expRY, gcRY ) )                        \

#endif