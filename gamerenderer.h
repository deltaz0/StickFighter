/// \file gamerenderer.h 
/// Definition of the renderer class CGameRenderer.
/// Copyright Ian Parberry, 2010.
/// Last updated September 27, 2010.

#pragma once

#include "renderer.h"
#include "defines.h"

/// The game renderer handles all the nasty Direct3D details for the game

class CGameRenderer: public CRenderer{
private: 
  LPDIRECT3DVERTEXBUFFER9 m_pBackgroundVB;  ///< Vertex buffer for background.
  LPDIRECT3DVERTEXBUFFER9 m_pHealthVB; ///< Vertex buffer for health.
  LPDIRECT3DVERTEXBUFFER9 m_pHealthVB2; ///< Vertex Buffer for player 2 health.
  LPDIRECT3DVERTEXBUFFER9 m_pSuperBarVB;
  LPDIRECT3DVERTEXBUFFER9 m_pSuperBarVB2;
  LPDIRECT3DTEXTURE9 m_pBackgroundTexture; ///< Texture for background.
  LPDIRECT3DTEXTURE9 m_pFloorTexture; ///< Texture for floor.
  LPDIRECT3DTEXTURE9 m_pWallTexture; //wall texture
  LPDIRECT3DTEXTURE9 m_pWireframeTexture; ///< Texture for showing wireframe, all black.
  LPDIRECT3DTEXTURE9 m_pIntroTexture; ///< Texture for showing intro sequence.
  LPDIRECT3DTEXTURE9 m_pMenuTexture; ///< Texture for showing menu.
  LPDIRECT3DTEXTURE9 m_pDeviceMenuTexture; ///< Texture for showing device menu.
  LPDIRECT3DTEXTURE9 m_pHUDTexture; ///< Texture for heads-up display.
  LPDIRECT3DTEXTURE9 m_pSuccessTexture; ///< Texture for showing success screen.
  LPDIRECT3DTEXTURE9 m_pHealthTexture; ///< Texture for full health bars.
  LPDIRECT3DTEXTURE9 m_pHealthTexture2;
  LPDIRECT3DTEXTURE9 m_pMediumHealthTexture; ///< Texture for medium health bars.
  LPDIRECT3DTEXTURE9 m_pMediumHealthTexture2;
  LPDIRECT3DTEXTURE9 m_pLowHealthTexture; ///< Texture for low health bars.
  LPDIRECT3DTEXTURE9 m_pLowHealthTexture2;
  LPDIRECT3DTEXTURE9 m_pSuperBarTexture;
  LPDIRECT3DTEXTURE9 m_pSuperBarTexture2;
  BOOL m_bCameraDefaultMode;
  BOOL DisplayScreen(char *filename);
  void DisplayScreen(LPDIRECT3DTEXTURE9 texture);
public:
  CGameRenderer::CGameRenderer(); //constructor
  void InitBackgroundVertexBuffer();
  void DrawBackground(float x);
  void HealthVertexBuffer(float x, float x2);
  void HealthVertexBuffer2(float x, float x2);

  void LoadTextures(); //create textures for image storage
  void Release(); //release offscreen images 

  BOOL ComposeMenuFrame(LPDIRECT3DTEXTURE9 menutexture);
  BOOL ComposeDeviceFrame(LPDIRECT3DTEXTURE9 menutexture);
  void ProcessDeviceMenuFrame();
  void ProcessMenuFrame();
  void ComposeFrame();
  void ProcessFrame();
  void ProcessSuccessFrame();


  void FlipCameraMode();

  void DisplayIntroScreen(GameStateType state);
  void PerspectiveRendering(BOOL on);
  void DisplaySuccessScreen();

  void DrawHitSpark(int left, int right, int bottom, int top);

  int second;
  int timeStart;

  int successArrowPos;
  int successOptionSelected;

  int menuArrowPos;
  int menuOptionSelected;

  int dMenuArrowPos;
  int dMenuOptionSelected;

  D3DXVECTOR3 sparkLoc[6];

  int hitSparkOn[6];
  int sparkTimer;

  BOOL roundNotchInc;
  BOOL soundTemp;

  int p1ComboSlide[12];
  int p1ComboSlideTimer;

  int p2ComboSlide[12];
  int p2ComboSlideTimer;

  int announceTimer;
  float announceSize;

  int winner;
};
