/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

        HIDUSAGE.H

Abstract:

   Public Definitions of HID USAGES.

Environment:

    Kernel & user mode

--*/

#ifndef   __HIDUSAGE_H__
#define   __HIDUSAGE_H__

//
// Usage Pages
//

typedef USHORT USAGE, *PUSAGE;

#define HID_USAGE_PAGE_UNDEFINED      ((USAGE) 0x00)
#define HID_USAGE_PAGE_GENERIC        ((USAGE) 0x01)
#define HID_USAGE_PAGE_SIMULATION     ((USAGE) 0x02)
#define HID_USAGE_PAGE_VR             ((USAGE) 0x03)
#define HID_USAGE_PAGE_SPORT          ((USAGE) 0x04)
#define HID_USAGE_PAGE_GAME           ((USAGE) 0x05)
#define HID_USAGE_PAGE_KEYBOARD       ((USAGE) 0x07)
#define HID_USAGE_PAGE_LED            ((USAGE) 0x08)
#define HID_USAGE_PAGE_BUTTON         ((USAGE) 0x09)
#define HID_USAGE_PAGE_ORDINAL        ((USAGE) 0x0A)
#define HID_USAGE_PAGE_TELEPHONY      ((USAGE) 0x0B)
#define HID_USAGE_PAGE_CONSUMER       ((USAGE) 0x0C)
#define HID_USAGE_PAGE_DIGITIZER      ((USAGE) 0x0D)
#define HID_USAGE_PAGE_UNICODE        ((USAGE) 0x10)
#define HID_USAGE_PAGE_ALPHANUMERIC   ((USAGE) 0x14)


//
// Usages from Generic Desktop Page (0x01)
//

#define HID_USAGE_GENERIC_POINTER      ((USAGE) 0x01)
#define HID_USAGE_GENERIC_MOUSE        ((USAGE) 0x02)
#define HID_USAGE_GENERIC_JOYSTICK     ((USAGE) 0x04)
#define HID_USAGE_GENERIC_GAMEPAD      ((USAGE) 0x05)
#define HID_USAGE_GENERIC_KEYBOARD     ((USAGE) 0x06)
#define HID_USAGE_GENERIC_KEYPAD       ((USAGE) 0x07)
#define HID_USAGE_GENERIC_SYSTEM_CTL   ((USAGE) 0x80)

#define HID_USAGE_GENERIC_X                        ((USAGE) 0x30)
#define HID_USAGE_GENERIC_Y                        ((USAGE) 0x31)
#define HID_USAGE_GENERIC_Z                        ((USAGE) 0x32)
#define HID_USAGE_GENERIC_RX                       ((USAGE) 0x33)
#define HID_USAGE_GENERIC_RY                       ((USAGE) 0x34)
#define HID_USAGE_GENERIC_RZ                       ((USAGE) 0x35)
#define HID_USAGE_GENERIC_SLIDER                   ((USAGE) 0x36)
#define HID_USAGE_GENERIC_DIAL                     ((USAGE) 0x37)
#define HID_USAGE_GENERIC_WHEEL                    ((USAGE) 0x38)
#define HID_USAGE_GENERIC_HATSWITCH                ((USAGE) 0x39)
#define HID_USAGE_GENERIC_COUNTED_BUFFER           ((USAGE) 0x3A)
#define HID_USAGE_GENERIC_BYTE_COUNT               ((USAGE) 0x3B)
#define HID_USAGE_GENERIC_MOTION_WAKEUP            ((USAGE) 0x3C)
#define HID_USAGE_GENERIC_VX                       ((USAGE) 0x40)
#define HID_USAGE_GENERIC_VY                       ((USAGE) 0x41)
#define HID_USAGE_GENERIC_VZ                       ((USAGE) 0x42)
#define HID_USAGE_GENERIC_VBRX                     ((USAGE) 0x43)
#define HID_USAGE_GENERIC_VBRY                     ((USAGE) 0x44)
#define HID_USAGE_GENERIC_VBRZ                     ((USAGE) 0x45)
#define HID_USAGE_GENERIC_VNO                      ((USAGE) 0x46)
#define HID_USAGE_GENERIC_SYSCTL_POWER             ((USAGE) 0x81)
#define HID_USAGE_GENERIC_SYSCTL_SLEEP             ((USAGE) 0x82)
#define HID_USAGE_GENERIC_SYSCTL_WAKE              ((USAGE) 0x83)
#define HID_USAGE_GENERIC_SYSCTL_CONTEXT_MENU      ((USAGE) 0x84)
#define HID_USAGE_GENERIC_SYSCTL_MAIN_MENU         ((USAGE) 0x85)
#define HID_USAGE_GENERIC_SYSCTL_APP_MENU          ((USAGE) 0x86)
#define HID_USAGE_GENERIC_SYSCTL_HELP_MENU         ((USAGE) 0x87)
#define HID_USAGE_GENERIC_SYSCTL_MENU_EXIT         ((USAGE) 0x88)
#define HID_USAGE_GENERIC_SYSCTL_MENU_SELECT       ((USAGE) 0x89)
#define HID_USAGE_GENERIC_SYSCTL_MENU_RIGHT        ((USAGE) 0x8A)
#define HID_USAGE_GENERIC_SYSCTL_MENU_LEFT         ((USAGE) 0x8B)
#define HID_USAGE_GENERIC_SYSCTL_MENU_UP           ((USAGE) 0x8C)
#define HID_USAGE_GENERIC_SYSCTL_MENU_DOWN         ((USAGE) 0x8D)

//
// Usages from Simulation Controls Page (0x02)
//

#define HID_USAGE_SIMULATION_RUDDER                ((USAGE) 0xBA)
#define HID_USAGE_SIMULATION_THROTTLE              ((USAGE) 0xBB)

//
// Virtual Reality Controls Page (0x03)
//


//
// Sport Controls Page (0x04)
//


//
// Game Controls Page (0x05)
//


//
// Keyboard/Keypad Page (0x07)
//

        // Error "keys"
#define HID_USAGE_KEYBOARD_NOEVENT     ((USAGE) 0x00)
#define HID_USAGE_KEYBOARD_ROLLOVER    ((USAGE) 0x01)
#define HID_USAGE_KEYBOARD_POSTFAIL    ((USAGE) 0x02)
#define HID_USAGE_KEYBOARD_UNDEFINED   ((USAGE) 0x03)

        // Letters
#define HID_USAGE_KEYBOARD_aA          ((USAGE) 0x04)
#define HID_USAGE_KEYBOARD_zZ          ((USAGE) 0x1D)
        // Numbers
#define HID_USAGE_KEYBOARD_ONE         ((USAGE) 0x1E)
#define HID_USAGE_KEYBOARD_ZERO        ((USAGE) 0x27)
        // Modifier Keys
#define HID_USAGE_KEYBOARD_LCTRL       ((USAGE) 0xE0)
#define HID_USAGE_KEYBOARD_LSHFT       ((USAGE) 0xE1)
#define HID_USAGE_KEYBOARD_LALT        ((USAGE) 0xE2)
#define HID_USAGE_KEYBOARD_LGUI        ((USAGE) 0xE3)
#define HID_USAGE_KEYBOARD_RCTRL       ((USAGE) 0xE4)
#define HID_USAGE_KEYBOARD_RSHFT       ((USAGE) 0xE5)
#define HID_USAGE_KEYBOARD_RALT        ((USAGE) 0xE6)
#define HID_USAGE_KEYBOARD_RGUI        ((USAGE) 0xE7)
#define HID_USAGE_KEYBOARD_SCROLL_LOCK ((USAGE) 0x47)
#define HID_USAGE_KEYBOARD_NUM_LOCK    ((USAGE) 0x53)
#define HID_USAGE_KEYBOARD_CAPS_LOCK   ((USAGE) 0x39)
        // Funtion keys
#define HID_USAGE_KEYBOARD_F1          ((USAGE) 0x3A)
#define HID_USAGE_KEYBOARD_F12         ((USAGE) 0x45)

#define HID_USAGE_KEYBOARD_RETURN      ((USAGE) 0x28)
#define HID_USAGE_KEYBOARD_ESCAPE      ((USAGE) 0x29)
#define HID_USAGE_KEYBOARD_DELETE      ((USAGE) 0x2A)

#define HID_USAGE_KEYBOARD_PRINT_SCREEN ((USAGE) 0x46)

// and hundreds more...

//
// LED Page (0x08)
//

#define HID_USAGE_LED_NUM_LOCK               ((USAGE) 0x01)
#define HID_USAGE_LED_CAPS_LOCK              ((USAGE) 0x02)
#define HID_USAGE_LED_SCROLL_LOCK            ((USAGE) 0x03)
#define HID_USAGE_LED_COMPOSE                ((USAGE) 0x04)
#define HID_USAGE_LED_KANA                   ((USAGE) 0x05)
#define HID_USAGE_LED_POWER                  ((USAGE) 0x06)
#define HID_USAGE_LED_SHIFT                  ((USAGE) 0x07)
#define HID_USAGE_LED_DO_NOT_DISTURB         ((USAGE) 0x08)
#define HID_USAGE_LED_MUTE                   ((USAGE) 0x09)
#define HID_USAGE_LED_TONE_ENABLE            ((USAGE) 0x0A)
#define HID_USAGE_LED_HIGH_CUT_FILTER        ((USAGE) 0x0B)
#define HID_USAGE_LED_LOW_CUT_FILTER         ((USAGE) 0x0C)
#define HID_USAGE_LED_EQUALIZER_ENABLE       ((USAGE) 0x0D)
#define HID_USAGE_LED_SOUND_FIELD_ON         ((USAGE) 0x0E)
#define HID_USAGE_LED_SURROUND_FIELD_ON      ((USAGE) 0x0F)
#define HID_USAGE_LED_REPEAT                 ((USAGE) 0x10)
#define HID_USAGE_LED_STEREO                 ((USAGE) 0x11)
#define HID_USAGE_LED_SAMPLING_RATE_DETECT   ((USAGE) 0x12)
#define HID_USAGE_LED_SPINNING               ((USAGE) 0x13)
#define HID_USAGE_LED_CAV                    ((USAGE) 0x14)
#define HID_USAGE_LED_CLV                    ((USAGE) 0x15)
#define HID_USAGE_LED_RECORDING_FORMAT_DET   ((USAGE) 0x16)
#define HID_USAGE_LED_OFF_HOOK               ((USAGE) 0x17)
#define HID_USAGE_LED_RING                   ((USAGE) 0x18)
#define HID_USAGE_LED_MESSAGE_WAITING        ((USAGE) 0x19)
#define HID_USAGE_LED_DATA_MODE              ((USAGE) 0x1A)
#define HID_USAGE_LED_BATTERY_OPERATION      ((USAGE) 0x1B)
#define HID_USAGE_LED_BATTERY_OK             ((USAGE) 0x1C)
#define HID_USAGE_LED_BATTERY_LOW            ((USAGE) 0x1D)
#define HID_USAGE_LED_SPEAKER                ((USAGE) 0x1E)
#define HID_USAGE_LED_HEAD_SET               ((USAGE) 0x1F)
#define HID_USAGE_LED_HOLD                   ((USAGE) 0x20)
#define HID_USAGE_LED_MICROPHONE             ((USAGE) 0x21)
#define HID_USAGE_LED_COVERAGE               ((USAGE) 0x22)
#define HID_USAGE_LED_NIGHT_MODE             ((USAGE) 0x23)
#define HID_USAGE_LED_SEND_CALLS             ((USAGE) 0x24)
#define HID_USAGE_LED_CALL_PICKUP            ((USAGE) 0x25)
#define HID_USAGE_LED_CONFERENCE             ((USAGE) 0x26)
#define HID_USAGE_LED_STAND_BY               ((USAGE) 0x27)
#define HID_USAGE_LED_CAMERA_ON              ((USAGE) 0x28)
#define HID_USAGE_LED_CAMERA_OFF             ((USAGE) 0x29)
#define HID_USAGE_LED_ON_LINE                ((USAGE) 0x2A)
#define HID_USAGE_LED_OFF_LINE               ((USAGE) 0x2B)
#define HID_USAGE_LED_BUSY                   ((USAGE) 0x2C)
#define HID_USAGE_LED_READY                  ((USAGE) 0x2D)
#define HID_USAGE_LED_PAPER_OUT              ((USAGE) 0x2E)
#define HID_USAGE_LED_PAPER_JAM              ((USAGE) 0x2F)
#define HID_USAGE_LED_REMOTE                 ((USAGE) 0x30)
#define HID_USAGE_LED_FORWARD                ((USAGE) 0x31)
#define HID_USAGE_LED_REVERSE                ((USAGE) 0x32)
#define HID_USAGE_LED_STOP                   ((USAGE) 0x33)
#define HID_USAGE_LED_REWIND                 ((USAGE) 0x34)
#define HID_USAGE_LED_FAST_FORWARD           ((USAGE) 0x35)
#define HID_USAGE_LED_PLAY                   ((USAGE) 0x36)
#define HID_USAGE_LED_PAUSE                  ((USAGE) 0x37)
#define HID_USAGE_LED_RECORD                 ((USAGE) 0x38)
#define HID_USAGE_LED_ERROR                  ((USAGE) 0x39)
#define HID_USAGE_LED_SELECTED_INDICATOR     ((USAGE) 0x3A)
#define HID_USAGE_LED_IN_USE_INDICATOR       ((USAGE) 0x3B)
#define HID_USAGE_LED_MULTI_MODE_INDICATOR   ((USAGE) 0x3C)
#define HID_USAGE_LED_INDICATOR_ON           ((USAGE) 0x3D)
#define HID_USAGE_LED_INDICATOR_FLASH        ((USAGE) 0x3E)
#define HID_USAGE_LED_INDICATOR_SLOW_BLINK   ((USAGE) 0x3F)
#define HID_USAGE_LED_INDICATOR_FAST_BLINK   ((USAGE) 0x40)
#define HID_USAGE_LED_INDICATOR_OFF          ((USAGE) 0x41)
#define HID_USAGE_LED_FLASH_ON_TIME          ((USAGE) 0x42)
#define HID_USAGE_LED_SLOW_BLINK_ON_TIME     ((USAGE) 0x43)
#define HID_USAGE_LED_SLOW_BLINK_OFF_TIME    ((USAGE) 0x44)
#define HID_USAGE_LED_FAST_BLINK_ON_TIME     ((USAGE) 0x45)
#define HID_USAGE_LED_FAST_BLINK_OFF_TIME    ((USAGE) 0x46)
#define HID_USAGE_LED_INDICATOR_COLOR        ((USAGE) 0x47)
#define HID_USAGE_LED_RED                    ((USAGE) 0x48)
#define HID_USAGE_LED_GREEN                  ((USAGE) 0x49)
#define HID_USAGE_LED_AMBER                  ((USAGE) 0x4A)
#define HID_USAGE_LED_GENERIC_INDICATOR      ((USAGE) 0x4B)

//
//  Button Page (0x09)
//
//  There is no need to label these usages.
//


//
//  Ordinal Page (0x0A)
//
//  There is no need to label these usages.
//


//
//  Telephony Device Page (0x0B)
//

#define HID_USAGE_TELEPHONY_PHONE                  ((USAGE) 0x01)
#define HID_USAGE_TELEPHONY_ANSWERING_MACHINE      ((USAGE) 0x02)
#define HID_USAGE_TELEPHONY_MESSAGE_CONTROLS       ((USAGE) 0x03)
#define HID_USAGE_TELEPHONY_HANDSET                ((USAGE) 0x04)
#define HID_USAGE_TELEPHONY_HEADSET                ((USAGE) 0x05)
#define HID_USAGE_TELEPHONY_KEYPAD                 ((USAGE) 0x06)
#define HID_USAGE_TELEPHONY_PROGRAMMABLE_BUTTON    ((USAGE) 0x07)

//
// and others...
//

#define HID_USAGE_CONSUMERCTRL          ((USAGE)0x01)
#define HID_USAGE_DIGITIZER_PEN         ((USAGE)0x02)
#define HID_USAGE_DIGITIZER_IN_RANGE    ((USAGE)0x32)
#define HID_USAGE_DIGITIZER_TIP_SWITCH  ((USAGE)0x42)
#define HID_USAGE_DIGITIZER_BARREL_SWITCH ((USAGE)0x44)

#endif


