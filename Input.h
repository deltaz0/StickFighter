/// \file input.h
/// Interface for the input manager class CInputManager.
/// Copyright Ian Parberry, 2004
/// Last updated September 7, 2010

#pragma once

#define DIRECTINPUT_VERSION 0x0800 //put before #include dinput.h

#include <windows.h>
#include <dinput.h> //DirectInput
#include <XInput.h>

#include "defines.h" //essential defines
#include "buttons.h" //for button manager

#define DI_BUFSIZE 16 ///< Buffer size for DirectInput events from devices
#pragma comment(lib, "XInput.lib")
  
/// Input manager class.
/// Manages keyboard and mouse input using DirectInput.

class CInputManager{

  private:
    static LPDIRECTINPUT8 m_lpDirectInput; ///< DirectInput object.
    LPDIRECTINPUTDEVICE8 m_pKeyboard; ///< Keyboard device.
    DIDEVICEOBJECTDATA m_DIod[DI_BUFSIZE]; ///< Input device buffer.
    LPDIRECTINPUTDEVICE8 m_pMouse; ///< Mouse device.
    HANDLE m_hMouseEvent; ///< Mouse interrupt event.
    POINT m_pointMouse; ///< Mouse point.  
    BOOL m_bCursorShowing; ///< TRUE if mouse cursor is showing.
    POINT m_ptMouseRelative; ///< Mouse point relative to window boundaries.
    CButtonManager *m_pButtonManager; ///< Button manager for  menu buttons.
    BOOL m_bCursorOutsideWindow; ///< Is mouse cursor outside the window?
    static LPDIRECTINPUTDEVICE8 m_pJoystick; ///< Joystick device.
    InputDeviceType m_nInputDevice; ///< Current input device.
    static BOOL m_bJoystickHasSlider; ///< TRUE if the joystick has a slider

    //private helper functions
    BOOL KeyboardHandler(DWORD keystroke); ///< Keyboard handler.
    void IntroKeyboardHandler(DWORD keystroke); ///< Keyboard handler for intro.
    BOOL MenuKeyboardHandler(DWORD keystroke); ///< Keyboard handler for menu.
	BOOL XboxMenuHandler();

    BOOL SuccessKeyboardHandler(DWORD keystroke); //success keyboard handler

    void DeviceMenuKeyboardHandler(DWORD keystroke); ///< Keyboard handler for device menu.
    void GameKeyboardHandler(DWORD keystroke, BOOL Override=FALSE); ///< Keyboard handler for game.
    void SetPlayerSpeed(POINT point, SIZE extent); ///< Set player speed from mouse.
    void MouseAbsoluteToRelative(POINT abs, POINT& rel); ///< Convert mouse coords to window relative
    
    //joystick callback functions
    static BOOL CALLBACK EnumJoysticksCallback(const DIDEVICEINSTANCE* pdidInstance, VOID* pContext);
    static BOOL CALLBACK EnumObjectsCallback(const DIDEVICEOBJECTINSTANCE* pdidoi,VOID* pContext);
    //setup functions
    BOOL InitKeyboardInput(HWND hwnd); ///< Set up keyboard input.
    BOOL InitMouseInput(HWND hwnd); ///< set up mouse input.   
    BOOL InitJoystickInput(HWND hwnd); ///< Set up joystick input.
	BOOL InitXboxController();
    void SetupMenuButtons(int count,int x,int y,int ydelta); ///< Set up button manager.
    void SetupMainMenuButtons(); ///< Set up main menu button manager.  
    void SetupDeviceMenuButtons(); ///< Set up device menu button manager.

    //mouse handlers
    void MouseMove(int x,int y); ///< Handle mouse motion.

    //left mouse button down handlers
    void MainMenuLMouseDown(); ///< Mouse down handler for main menu.
    void DeviceMenuLMouseDown(); ///< Mouse down handler for device menu.
    void GameLMouseDown(); ///< Mouse down handler for game. 

    //left mouse button up handlers
    void IntroLMouseUp(); ///< Mouse up handler for intro.
    BOOL MainMenuLMouseUp(); ///< Mouse up handler for main menu.
    void DeviceMenuLMouseUp(); ///< Mouse up handler for device menu.
    void LMouseDown(); ///< Main mouse down handler.
    BOOL LMouseUp(); ///< Main mouse up handler.

	XINPUT_STATE state; //State for controller


  public:
    CInputManager(HINSTANCE hInstance, HWND hwnd); ///< Constructor.
    ~CInputManager(); ///< Destructor.
    BOOL ProcessKeyboardInput(); ///< Process buffered keyboard events.
    BOOL ProcessMouseInput(); ///< Process buffered mouse events.
    BOOL ProcessJoystickInput(); ///< Process polled joystick events.
	BOOL ProcessXboxController(int player); ///<Process polled xbox controller events.
    void SetupButtons(GameStateType phase); ///< Set up menu buttons.
    void DrawButtons(); ///< Draw main menu buttons.
    void ChangeState(GameStateType state); ///< Change game state.
    int checkDirection(int player);
    void playSound(int index, BOOL looping = FALSE); // use to play a sound

    int CInputManager::keymap(DWORD keystroke);

    int p1DownHeld;
    int p1UpHeld;
    int p1RightHeld;
    int p1LeftHeld;
    
    int p2DownHeld;
    int p2UpHeld;
    int p2RightHeld;
    int p2LeftHeld;

	BOOL X1UP;
	BOOL X1DOWN;
	BOOL X1LEFT;
	BOOL X1RIGHT;

	BOOL X1LSUP;
	BOOL X1LSDOWN;
	BOOL X1LSLEFT;
	BOOL X1LSRIGHT;
	
	BOOL X2LSUP;
	BOOL X2LSDOWN;
	BOOL X2LSLEFT;
	BOOL X2LSRIGHT;

	BOOL X2UP;
	BOOL X2DOWN;
	BOOL X2LEFT;
	BOOL X2RIGHT;

	BOOL X1A;
	BOOL X1B;
	BOOL X1C;

	BOOL X2A;
	BOOL X2B;
	BOOL X2C;


    bool p1AtkOK;
    bool p2AtkOK;

    bool p1MoveOK;
    bool p2MoveOK;

    DWORD p1D;
    DWORD p1L;
    DWORD p1R;
    DWORD p1U;
    DWORD p1A;
    DWORD p1B;
    DWORD p1C;


    DWORD p2D;
    DWORD p2L;
    DWORD p2R;
    DWORD p2U;
    DWORD p2A;
    DWORD p2B;
    DWORD p2C;

	int totalcontrollers;

    DWORD p0Start;

    int getKeyFlag;
};