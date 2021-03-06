/* -*-c++-*- OpenSceneGraph - Copyright (C) 1998-2006 Robert Osfield 
 *
 * This library is open source and may be redistributed and/or modified under  
 * the terms of the OpenSceneGraph Public License (OSGPL) version 0.0 or 
 * (at your option) any later version.  The full license is in LICENSE file
 * included with this distribution, and on the openscenegraph.org website.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 * OpenSceneGraph Public License for more details.
*/
#ifndef CRPRODUCER_GUIEVENTADAPTER
#define CRPRODUCER_GUIEVENTADAPTER 1 

#include <CRCore/Referenced.h>
#include <CRProducer/crExport.h>

namespace CRProducer{

/**
Pure virtual base class for adapting platform specific events into
generic keyboard and mouse events. 

Used as GUI toolkit-independent input into GUIEventAdapters. Viewer
writers should subclass this base class to implement the functionality
to translate one of their GUI events, e.g. a Qt Event or an MFC Event,
as appropriate.
*/
class CRPRODUCER_EXPORT crGUIEventAdapter : public CRCore::Referenced
{
public:

        enum MouseButtonMask 
		{ 
            LEFT_MOUSE_BUTTON=1,
            MIDDLE_MOUSE_BUTTON=2,
            RIGHT_MOUSE_BUTTON=4
        };

        enum EventType 
		{
            NONE=0,
            PUSH,
            RELEASE,
            DOUBLECLICK,
            DRAG,
            MOVE,
            KEYDOWN,
            KEYUP,
            FRAME,
            RESIZE,
            SCROLLUP,
            SCROLLDOWN,
            SCROLLLEFT,
            SCROLLRIGHT,
			//WINFOCUS,
			INPUTCHAR,
			WINRESTORED,
        };

        enum KeySymbol
        {
            KEY_Space           = 0x20,
            
            KEY_BackSpace       = 0xFF08,        /* back space, back char */
            KEY_Tab             = 0xFF09,
            KEY_Linefeed        = 0xFF0A,        /* Linefeed, LF */
            KEY_Clear           = 0xFF0B,
            KEY_Return          = 0xFF0D,        /* Return, enter */
            KEY_Pause           = 0xFF13,        /* Pause, hold */
            KEY_Scroll_Lock     = 0xFF14,
            KEY_Sys_Req         = 0xFF15,
            KEY_Escape          = 0xFF1B,
            KEY_Delete          = 0xFFFF,        /* Delete, rubout */


            /* Cursor control & motion */

            KEY_Home            = 0xFF50,
            KEY_Left            = 0xFF51,        /* Move left, left arrow */
            KEY_Up              = 0xFF52,        /* Move up, up arrow */
            KEY_Right           = 0xFF53,        /* Move right, right arrow */
            KEY_Down            = 0xFF54,        /* Move down, down arrow */
            KEY_Prior           = 0xFF55,        /* Prior, previous */
            KEY_Page_Up         = 0xFF55,
            KEY_Next            = 0xFF56,        /* Next */
            KEY_Page_Down       = 0xFF56,
            KEY_End             = 0xFF57,        /* EOL */
            KEY_Begin           = 0xFF58,        /* BOL */


            /* Misc Functions */

            KEY_Select          = 0xFF60,        /* Select, mark */
            KEY_Print           = 0xFF61,
            KEY_Execute         = 0xFF62,        /* Execute, run, do */
            KEY_Insert          = 0xFF63,        /* Insert, insert here */
            KEY_Undo            = 0xFF65,        /* Undo, oops */
            KEY_Redo            = 0xFF66,        /* redo, again */
            KEY_Menu            = 0xFF67,
            KEY_Find            = 0xFF68,        /* Find, search */
            KEY_Cancel          = 0xFF69,        /* Cancel, stop, abort, exit */
            KEY_Help            = 0xFF6A,        /* Help */
            KEY_Break           = 0xFF6B,
            KEY_Mode_switch     = 0xFF7E,        /* Character set switch */
            KEY_Script_switch   = 0xFF7E,        /* Alias for mode_switch */
            KEY_Num_Lock        = 0xFF7F,

            /* Keypad Functions, keypad numbers cleverly chosen to map to ascii */

            KEY_KP_Space        = 0xFF80,        /* space */
            KEY_KP_Tab          = 0xFF89,
            KEY_KP_Enter        = 0xFF8D,        /* enter */
            KEY_KP_F1           = 0xFF91,        /* PF1, KP_A, ... */
            KEY_KP_F2           = 0xFF92,
            KEY_KP_F3           = 0xFF93,
            KEY_KP_F4           = 0xFF94,
            KEY_KP_Home         = 0xFF95,
            KEY_KP_Left         = 0xFF96,
            KEY_KP_Up           = 0xFF97,
            KEY_KP_Right        = 0xFF98,
            KEY_KP_Down         = 0xFF99,
            KEY_KP_Prior        = 0xFF9A,
            KEY_KP_Page_Up      = 0xFF9A,
            KEY_KP_Next         = 0xFF9B,
            KEY_KP_Page_Down    = 0xFF9B,
            KEY_KP_End          = 0xFF9C,
            KEY_KP_Begin        = 0xFF9D,
            KEY_KP_Insert       = 0xFF9E,
            KEY_KP_Delete       = 0xFF9F,
            KEY_KP_Equal        = 0xFFBD,        /* equals */
            KEY_KP_Multiply     = 0xFFAA,
            KEY_KP_Add          = 0xFFAB,
            KEY_KP_Separator    = 0xFFAC,       /* separator, often comma */
            KEY_KP_Subtract     = 0xFFAD,
            KEY_KP_Decimal      = 0xFFAE,
            KEY_KP_Divide       = 0xFFAF,

            KEY_KP_0            = 0xFFB0,
            KEY_KP_1            = 0xFFB1,
            KEY_KP_2            = 0xFFB2,
            KEY_KP_3            = 0xFFB3,
            KEY_KP_4            = 0xFFB4,
            KEY_KP_5            = 0xFFB5,
            KEY_KP_6            = 0xFFB6,
            KEY_KP_7            = 0xFFB7,
            KEY_KP_8            = 0xFFB8,
            KEY_KP_9            = 0xFFB9,
            
            /*
             * Auxilliary Functions; note the duplicate definitions for left and right
             * function keys;  Sun keyboards and a few other manufactures have such
             * function key groups on the left and/or right sides of the keyboard.
             * We've not found a keyboard with more than 35 function keys total.
             */

            KEY_F1              = 0xFFBE,
            KEY_F2              = 0xFFBF,
            KEY_F3              = 0xFFC0,
            KEY_F4              = 0xFFC1,
            KEY_F5              = 0xFFC2,
            KEY_F6              = 0xFFC3,
            KEY_F7              = 0xFFC4,
            KEY_F8              = 0xFFC5,
            KEY_F9              = 0xFFC6,
            KEY_F10             = 0xFFC7,
            KEY_F11             = 0xFFC8,
            KEY_F12             = 0xFFC9,
            KEY_F13             = 0xFFCA,
            KEY_F14             = 0xFFCB,
            KEY_F15             = 0xFFCC,
            KEY_F16             = 0xFFCD,
            KEY_F17             = 0xFFCE,
            KEY_F18             = 0xFFCF,
            KEY_F19             = 0xFFD0,
            KEY_F20             = 0xFFD1,
            KEY_F21             = 0xFFD2,
            KEY_F22             = 0xFFD3,
            KEY_F23             = 0xFFD4,
            KEY_F24             = 0xFFD5,
            KEY_F25             = 0xFFD6,
            KEY_F26             = 0xFFD7,
            KEY_F27             = 0xFFD8,
            KEY_F28             = 0xFFD9,
            KEY_F29             = 0xFFDA,
            KEY_F30             = 0xFFDB,
            KEY_F31             = 0xFFDC,
            KEY_F32             = 0xFFDD,
            KEY_F33             = 0xFFDE,
            KEY_F34             = 0xFFDF,
            KEY_F35             = 0xFFE0,

            /* Modifiers */

            KEY_Shift_L         = 0xFFE1,        /* Left shift */
            KEY_Shift_R         = 0xFFE2,        /* Right shift */
            KEY_Control_L       = 0xFFE3,        /* Left control */
            KEY_Control_R       = 0xFFE4,        /* Right control */
            KEY_Caps_Lock       = 0xFFE5,        /* Caps lock */
            KEY_Shift_Lock      = 0xFFE6,        /* Shift lock */

            KEY_Meta_L          = 0xFFE7,        /* Left meta */
            KEY_Meta_R          = 0xFFE8,        /* Right meta */
            KEY_Alt_L           = 0xFFE9,        /* Left alt */
            KEY_Alt_R           = 0xFFEA,        /* Right alt */
            KEY_Super_L         = 0xFFEB,        /* Left super */
            KEY_Super_R         = 0xFFEC,        /* Right super */
            KEY_Hyper_L         = 0xFFED,        /* Left hyper */
            KEY_Hyper_R         = 0xFFEE,        /* Right hyper */
        };


        enum ModKeyMask
        {
            MODKEY_LEFT_SHIFT  = 0x0001,
            MODKEY_RIGHT_SHIFT = 0x0002,
            MODKEY_LEFT_CTRL   = 0x0040,
            MODKEY_RIGHT_CTRL  = 0x0080,
            MODKEY_LEFT_ALT    = 0x0100,
            MODKEY_RIGHT_ALT   = 0x0200,
            MODKEY_LEFT_META   = 0x0400,
            MODKEY_RIGHT_META  = 0x0800,
            MODKEY_NUM_LOCK    = 0x1000,
            MODKEY_CAPS_LOCK   = 0x2000,
            MODKEY_CTRL        = (MODKEY_LEFT_CTRL|MODKEY_RIGHT_CTRL),
            MODKEY_SHIFT       = (MODKEY_LEFT_SHIFT|MODKEY_RIGHT_SHIFT),
            MODKEY_ALT         = (MODKEY_LEFT_ALT|MODKEY_RIGHT_ALT),
            MODKEY_META        = (MODKEY_LEFT_META|MODKEY_RIGHT_META)
        };

        /** Get the EventType of the GUI event.*/
        virtual EventType getEventType() const = 0;

        /** key pressed, return -1 if inappr   opriate for this event. */
        virtual int getKey() const = 0;

		virtual wchar_t getChar() const = 0;

        /** button pressed/released, return -1 if inappropriate for this event.*/
        virtual int getButton() const = 0;


        enum MouseYOrientation
        {
            Y_INCREASING_UPWARDS,
            Y_INCREASING_DOWNWARDS
        };

        void setMouseYOrientation(MouseYOrientation myo) { m_mouseYOrientation = myo; }
        MouseYOrientation getMouseYOrientation() const { return m_mouseYOrientation; }

        /** manimum x mouse position. */
        virtual float getXmin() const = 0;

        /** maximum x mouse position. */
        virtual float getXmax() const = 0;

        /** minimum y mouse position. */
        virtual float getYmin() const = 0;

        /** maximum y mouse position. */   
        virtual float getYmax() const = 0;

        /** current mouse x position.*/
        virtual float getX() const = 0;

        /** current mouse y position.*/
        virtual float getY() const = 0;
        
        /** current mouse button state */
        virtual unsigned int getButtonMask() const = 0;

        /** current modkey state */
        virtual unsigned int getModKeyMask() const = 0;

        /** time in seconds of event. */
        virtual double time() const = 0;
        
        /** return the getX() value normalised to the range of -1 to 1.
          * -1 would be the left hand side of the window.
          * 0.0 would be the middle of the window. 
          * +1 would be the right hand side of the window.*/
        inline float getXnormalized() const { return 2.0f*(getX()-getXmin())/(getXmax()-getXmin())-1.0f; }

        /** return the getY() value normalised to the range of -1 to 1.
          * -1 would be the bottom of the window.
          * 0.0 would be the middle of the window. 
          * +1 would be the top of the window.*/
        inline float getYnormalized() const
        {///+0.00166947
            if (m_mouseYOrientation==Y_INCREASING_UPWARDS) return 2.0f*(getY()-getYmin())/(getYmax()-getYmin())-1.0f;
            else return -(2.0f*(getY()-getYmin())/(getYmax()-getYmin())-1.0f);
        }

		//inline virtual int getFocus() const = 0;
		//inline virtual __int32 getTransferwin() const = 0;

protected:

        crGUIEventAdapter(MouseYOrientation myo=Y_INCREASING_DOWNWARDS):m_mouseYOrientation(myo) {}

        /** Force users to create on heap, so that multiple referencing is safe.*/
        virtual ~crGUIEventAdapter() {}

        MouseYOrientation m_mouseYOrientation;

};

}

#endif
