/// \file gamerenderer.cpp
/// Direct3D rendering.
/// DirectX stuff that won't change much is hidden away in this file
/// so you won't have to keep looking at it.
/// Copyright Ian Parberry, 2010.
/// Last updated September 27, 2010.

#include <d3d9.h>

#include "gamerenderer.h"
#include "defines.h" 
#include "abort.h"
#include "tinyxml.h"
#include "imagefilenamelist.h"
#include "sprite.h"
#include "object.h"
#include "objman.h"
#include "spriteman.h"
#include "input.h"
#include "HeaderManager.h"
#include "debug.h"
#include "InputBuffer.h"
#include "Timer.h"
#include "Sndlist.h"

extern int p1SuperCurrent;
extern int p2SuperCurrent;
extern int p1ComboCounter;
extern int p2ComboCounter;
extern int PauseGame;
extern CTimer g_cTimer;
extern TiXmlElement* g_xmlSettings;
extern BOOL g_bWindowed;
extern int g_nScreenWidth;
extern int g_nScreenHeight;
extern CImageFileNameList g_cImageFileName;
extern BOOL g_bImagesLoaded;
extern CGameObject g_cPlayer; 
extern CObjectManager g_cObjectManager;
extern CSpriteManager g_cSpriteManager;
extern BOOL g_bWireFrame;
extern CInputManager* g_pInputManager; 
extern CHeaderManager HeaderManager; 
extern InputBuffer p1InputBuffer;
extern InputBuffer p2InputBuffer;
extern int p1Health;
extern int p2Health;
extern int p1RoundsWon;
extern int p2RoundsWon;


CGameRenderer::CGameRenderer(): m_bCameraDefaultMode(TRUE), m_pMenuTexture(NULL), 
  m_pDeviceMenuTexture(NULL), m_pHUDTexture(NULL), m_pSuccessTexture(NULL), second(102), timeStart(-1), 
  successArrowPos(0), successOptionSelected(-1), menuArrowPos(0), menuOptionSelected(-1), dMenuArrowPos(0), dMenuOptionSelected(-1), sparkTimer(-1), roundNotchInc(FALSE), soundTemp(TRUE) {
      hitSparkOn[0] = 0;
      hitSparkOn[1] = 0;
      hitSparkOn[2] = 0;
      hitSparkOn[3] = 0;
      hitSparkOn[4] = 0;
      hitSparkOn[5] = 0;
      p1ComboSlide[0] = 0;
      p1ComboSlide[1] = 0;
      p1ComboSlide[2] = 0;
      p1ComboSlide[3] = 0;
      p1ComboSlide[4] = 0;
      p1ComboSlide[5] = 0;
      p1ComboSlide[6] = 0;
      p1ComboSlide[7] = 0;
      p1ComboSlide[8] = 0;
      p1ComboSlide[9] = 0;
      p1ComboSlide[10] = 0;
      p1ComboSlide[11] = 0;
      p1ComboSlideTimer = -1;
      p2ComboSlide[0] = 0;
      p2ComboSlide[1] = 0;
      p2ComboSlide[2] = 0;
      p2ComboSlide[3] = 0;
      p2ComboSlide[4] = 0;
      p2ComboSlide[5] = 0;
      p2ComboSlide[6] = 0;
      p2ComboSlide[7] = 0;
      p2ComboSlide[8] = 0;
      p2ComboSlide[9] = 0;
      p2ComboSlide[10] = 0;
      p2ComboSlide[11] = 0;
      p2ComboSlideTimer = -1;
}

void CGameRenderer::InitBackgroundVertexBuffer(){
  //create vertex buffer for background
  m_d3ddevice->CreateVertexBuffer( 
    13*sizeof(BILLBOARDVERTEX), D3DUSAGE_WRITEONLY, BILLBOARDVERTEX::FVF,  //changed size of mesh to hold 13 vertices
    D3DPOOL_MANAGED, &m_pBackgroundVB, NULL);
  
  //load vertex buffer
  BILLBOARDVERTEX* v; //vertex buffer data
  float w = 2.0f * g_nScreenWidth;
  float h = 2.0f * g_nScreenHeight; 

  if(SUCCEEDED(m_pBackgroundVB->Lock(0, 0, (void**)&v, 0))){ //lock buffer
    //vertex information, first triangle in clockwise order
    v[0].p = D3DXVECTOR3(w, 0, 0); 
    v[0].tu = 1.0f; v[0].tv = 0.0f;

    v[1].p = D3DXVECTOR3(0, 0, 0);
    v[1].tu = 0.0f; v[1].tv=0.0f;

    v[2].p = D3DXVECTOR3(w, 0, 1500);
    v[2].tu = 1.0f; v[2].tv = (float)g_nScreenHeight/g_nScreenWidth;

    v[3].p = D3DXVECTOR3(0, 0, 1500);
    v[3].tu = 0.0f; v[3].tv = (float)g_nScreenHeight/g_nScreenWidth;

    v[4].p = D3DXVECTOR3(w, h, 1500);
    v[4].tu = 1.0f; v[4].tv = 0.0f;

    v[5].p = D3DXVECTOR3(0, h, 1500);
    v[5].tu = 0.0f; v[5].tv = 0.0f;

    //extended the mesh to provide a left and right wall (left:5-8, right:9-12)

    v[6].p = D3DXVECTOR3(0, 0, 1500);
    v[6].tu = 0.0f; v[6].tv = 1.0f;

    v[7].p = D3DXVECTOR3(0, h, 0);
    v[7].tu = 1.0f; v[7].tv = 0.0f;

    v[8].p = D3DXVECTOR3(0, 0, 0);
    v[8].tu = 1.0f; v[8].tv = 1.0f;

    v[9].p = D3DXVECTOR3(w, 0, 0);
    v[9].tu = 1.0f; v[9].tv = 1.0f;
    
    v[10].p = D3DXVECTOR3(w, 0, 1500);
    v[10].tu = 0.0f; v[10].tv = 1.0f;

    v[11].p = D3DXVECTOR3(w, h, 0);
    v[11].tu = 1.0f; v[11].tv = 0.0f;

    v[12].p = D3DXVECTOR3(w, h, 1500);
    v[12].tu = 0.0f; v[12].tv = 0.0f;

    m_pBackgroundVB->Unlock();
  }
  //init p1 super bar
   m_d3ddevice->CreateVertexBuffer( 
    4*sizeof(BILLBOARDVERTEX), D3DUSAGE_WRITEONLY, BILLBOARDVERTEX::FVF,
    D3DPOOL_MANAGED, &m_pSuperBarVB, NULL);

	 if(SUCCEEDED(m_pSuperBarVB->Lock(0,0,(void**)&v, 0))){
    float tempf = ((float)p1SuperCurrent/100.0f);
   // int tempX = 930 - (450 * tempf);
	int tempG = 409 + (450*tempf); 
	if(tempG >= 849){
		tempG = 849;
	}
	if(tempG <= 409){
		tempG = 409;
	}

	  v[0].p = D3DXVECTOR3(tempG, 343, 1000);
	  v[0].tu = 1.0f; v[0].tv = 1.0f;

	  v[1].p = D3DXVECTOR3(409, 343, 1000);
	  v[1].tu = 0.0f; v[1].tv = 1.0f;

	  v[2].p = D3DXVECTOR3(tempG, 390, 1000);
	  v[2].tu = 1.0f; v[2].tv = 0.0f;

	  v[3].p = D3DXVECTOR3(409, 390, 1000);
	  v[3].tu = 0.0f; v[3].tv = 0.0f;

	  m_pSuperBarVB->Unlock();

 }

	 m_d3ddevice->CreateVertexBuffer( 
    4*sizeof(BILLBOARDVERTEX), D3DUSAGE_WRITEONLY, BILLBOARDVERTEX::FVF,
    D3DPOOL_MANAGED, &m_pHealthVB, NULL);
  

if(SUCCEEDED(m_pHealthVB->Lock(0, 0, (void**)&v, 0))){ //lock buffer
    //vertex information, first triangle in clockwise order
    float tempf = ((float)p1Health/100.0f);
    int tempX = 930 - (450 * tempf);

    v[0].p = D3DXVECTOR3(930, 1160, 1000);
    v[0].tu = 1.0f; v[0].tv = 1.0f;

    v[1].p = D3DXVECTOR3(tempX, 1160, 1000);
    v[1].tu = 0.0f; v[1].tv = 1.0f;

    v[2].p = D3DXVECTOR3(930, 1222, 1000);
    v[2].tu = 1.0f; v[2].tv = 0.0f;

    v[3].p = D3DXVECTOR3(tempX, 1222, 1000);
    v[3].tu = 0.0f; v[3].tv = 0.0f;
 
    m_pHealthVB->Unlock();
  }

 m_d3ddevice->CreateVertexBuffer( 
    4*sizeof(BILLBOARDVERTEX), D3DUSAGE_WRITEONLY, BILLBOARDVERTEX::FVF,
    D3DPOOL_MANAGED, &m_pHealthVB2, NULL);
  
  //load vertex buffer
  BILLBOARDVERTEX* v2; //vertex buffer data
  //GameRenderer.HealthVertexBuffer2(1120, 1570);
  if(SUCCEEDED(m_pHealthVB2->Lock(0, 0, (void**)&v2, 0))){ //lock buffer
    //vertex information, first triangle in clockwise order
    float tempF = ((float)p2Health/100.0f);
    int tempx = 1120 + (450 * tempF);

    v2[0].p = D3DXVECTOR3(tempx, 1160, 1000);
    v2[0].tu = 1.0f; v2[0].tv = 1.0f;

    v2[1].p = D3DXVECTOR3(1120, 1160, 1000);
    v2[1].tu = 0.0f; v2[1].tv = 1.0f;

    v2[2].p = D3DXVECTOR3(tempx, 1222, 1000);
    v2[2].tu = 1.0f; v2[2].tv = 0.0f;

    v2[3].p = D3DXVECTOR3(1120, 1222, 1000);
    v2[3].tu = 0.0f; v2[3].tv = 0.0f;
 
    m_pHealthVB2->Unlock();
  }

  BILLBOARDVERTEX* v3; //vertex buffer data
  m_d3ddevice->CreateVertexBuffer( 
    4*sizeof(BILLBOARDVERTEX), D3DUSAGE_WRITEONLY, BILLBOARDVERTEX::FVF,
    D3DPOOL_MANAGED, &m_pSuperBarVB2, NULL);
    float tempF = ((float)p2SuperCurrent/100.0f);
	int tempG = 1638 - (440*tempF); 
	if(tempG <= 1198){
		tempG = 1198;
	}
	if(tempG >= 1638){
		tempG = 1638;
	}


 if(SUCCEEDED(m_pSuperBarVB2->Lock(0,0,(void**)&v3, 0))){
    float tempF = ((float)p2Health/100.0f);
    int tempx = 1120 + (450 * tempF);

    v3[0].p = D3DXVECTOR3(1638, 343, 1000);
    v3[0].tu = 1.0f; v3[0].tv = 1.0f;

    v3[1].p = D3DXVECTOR3(tempG, 343, 1000);
    v3[1].tu = 0.0f; v3[1].tv = 1.0f;

    v3[2].p = D3DXVECTOR3(1638, 390, 1000);
    v3[2].tu = 1.0f; v3[2].tv = 0.0f;

    v3[3].p = D3DXVECTOR3(tempG, 390, 1000);
    v3[3].tu = 0.0f; v3[3].tv = 0.0f;

	  m_pSuperBarVB2->Unlock();

 }
} //InitBackgroundVertexBuffer




void CGameRenderer::HealthVertexBuffer(float x, float x2)
{
  ////create vertex buffer for background
  //m_d3ddevice->CreateVertexBuffer( 
  //  4*sizeof(BILLBOARDVERTEX), D3DUSAGE_WRITEONLY, BILLBOARDVERTEX::FVF,
  //  D3DPOOL_MANAGED, &m_pHealthVB, NULL);
  //
  ////load vertex buffer
  //BILLBOARDVERTEX* v; //vertex buffer data
  //float w = 2.0f * g_nScreenWidth;
  //float h = 2.0f * g_nScreenHeight; 

  //if(SUCCEEDED(m_pHealthVB->Lock(0, 0, (void**)&v, 0))){ //lock buffer
  //  //vertex information, first triangle in clockwise order

  //  int tempX = 480 * (p1Health / 100);

  //  v[0].p = D3DXVECTOR3(tempX, 1160, 1000);
  //  v[0].tu = 1.0f; v[0].tv = 1.0;

  //  v[1].p = D3DXVECTOR3(x, 1160, 1000);
  //  v[1].tu = 0.0f; v[1].tv = 1.0;

  //  v[2].p = D3DXVECTOR3(tempX, 1222, 1000);
  //  v[2].tu = 1.0f; v[2].tv = 0.0f;

  //  v[3].p = D3DXVECTOR3(x, 1222, 1000);
  //  v[3].tu = 0.0f; v[3].tv = 0.0f;
 
  //  m_pHealthVB->Unlock();
  //}
}


void CGameRenderer::HealthVertexBuffer2(float x, float x2)
{
  ////create vertex buffer for background
  //m_d3ddevice->CreateVertexBuffer( 
  //  4*sizeof(BILLBOARDVERTEX), D3DUSAGE_WRITEONLY, BILLBOARDVERTEX::FVF,
  //  D3DPOOL_MANAGED, &m_pHealthVB2, NULL);
  //
  ////load vertex buffer
  //BILLBOARDVERTEX* v; //vertex buffer data
  //float w = 2.0f * g_nScreenWidth;
  //float h = 2.0f * g_nScreenHeight; 

  //if(SUCCEEDED(m_pHealthVB2->Lock(0, 0, (void**)&v, 0))){ //lock buffer
  //  //vertex information, first triangle in clockwise order

  //  v[0].p = D3DXVECTOR3(x2, 1160, 1000);
  //  v[0].tu = 1.0f; v[0].tv = 1.0;

  //  v[1].p = D3DXVECTOR3(x, 1160, 1000);
  //  v[1].tu = 0.0f; v[1].tv = 1.0;

  //  v[2].p = D3DXVECTOR3(x2, 1222, 1000);
  //  v[2].tu = 1.0f; v[2].tv = 0.0f;

  //  v[3].p = D3DXVECTOR3(x, 1222, 1000);
  //  v[3].tu = 0.0f; v[3].tv = 0.0f;
 
  //  m_pHealthVB2->Unlock();
  //}
}




/// Draw the game background.
/// \param x Camera x offset

void CGameRenderer::DrawBackground(float x){
  
  //(x is unused at this point, maybe take out in future)
  //const float delta = 2.0f * g_nScreenWidth; //no longer necessary
  //float fQuantizeX = delta * (int)(x/delta - 1.0f) + g_nScreenWidth; ///< old Quantized x coordinate (no longer necessary)
  //float fQuantizeX = (int)((2.0f*x)/delta); ///< new Quantized x coordinate // No longer necessary

  //set vertex buffer to background
  m_d3ddevice->SetStreamSource(0, m_pBackgroundVB, 0, sizeof(BILLBOARDVERTEX));
  m_d3ddevice->SetFVF(BILLBOARDVERTEX::FVF); //flexible vertex format

  //draw floor
  if(g_bWireFrame)m_d3ddevice->SetTexture(0, m_pWireframeTexture); //set wireframe texture
  else m_d3ddevice->SetTexture(0, m_pFloorTexture); //set floor texture
  SetWorldMatrix(g_nScreenWidth/2);
  m_d3ddevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2); //no need to quantize any further

  //draw backdrop
  if(!g_bWireFrame)
    m_d3ddevice->SetTexture(0, m_pBackgroundTexture); //set cloud texture
  SetWorldMatrix(g_nScreenWidth/2);
  m_d3ddevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 2, 2); //no need to quantize further

  //draw wall
  m_d3ddevice->SetTexture(0, m_pWallTexture); //set wall texture
  SetWorldMatrix(g_nScreenWidth/2);
  m_d3ddevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 5, 2); //left wall
  m_d3ddevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 9, 2); //right wall

} //DrawBackground

/// Draw intro screen.

void CGameRenderer::DisplayScreen(LPDIRECT3DTEXTURE9 texture){
  //set vertex buffer to background
  m_d3ddevice->SetStreamSource(0, m_pBackgroundVB, 0, sizeof(BILLBOARDVERTEX));
  m_d3ddevice->SetFVF(BILLBOARDVERTEX::FVF); //flexible vertex format

  //draw backdrop
  m_d3ddevice->SetTexture(0, texture); //set intro texture
  SetWorldMatrix(0.0f);
  m_d3ddevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 2, 2);
} //DisplayScreen



/// Load the textures required for the game. 
/// Calls function LoadTexture to load in the textures needed for the game.
/// The texture names are hard-coded.

void CGameRenderer::LoadTextures(){ //create textures for image storage
  LoadTexture(m_pBackgroundTexture, g_cImageFileName[BACKGROUND_IMAGEFILE]);
  LoadTexture(m_pFloorTexture, g_cImageFileName[GROUND_IMAGEFILE]);
  LoadTexture(m_pWallTexture, g_cImageFileName[WALL_IMAGEFILE]);
  LoadTexture(m_pWireframeTexture, g_cImageFileName[BLACK_IMAGEFILE]);
  LoadTexture(m_pMenuTexture, g_cImageFileName[MENU_IMAGEFILE]);
  LoadTexture(m_pDeviceMenuTexture, g_cImageFileName[DEVICEMENU_IMAGEFILE]);
  LoadTexture(m_pHUDTexture, g_cImageFileName[HUD_IMAGEFILE]);
  LoadTexture(m_pSuccessTexture, g_cImageFileName[SUCCESS_IMAGEFILE]);
  LoadTexture(m_pHealthTexture, g_cImageFileName[HEALTH_IMAGEFILE]);
  LoadTexture(m_pHealthTexture2, g_cImageFileName[HEALTH2_IMAGEFILE]);

  LoadTexture(m_pMediumHealthTexture, g_cImageFileName[MEDIUMHEALTH_IMAGEFILE]);
  LoadTexture(m_pMediumHealthTexture2, g_cImageFileName[MEDIUMHEALTH2_IMAGEFILE]);
  LoadTexture(m_pLowHealthTexture, g_cImageFileName[LOWHEALTH_IMAGEFILE]);
  LoadTexture(m_pLowHealthTexture2, g_cImageFileName[LOWHEALTH2_IMAGEFILE]);

  LoadTexture(m_pSuperBarTexture, g_cImageFileName[SUPERBAR_IMAGEFILE]);
  LoadTexture(m_pSuperBarTexture2,g_cImageFileName[SUPERBAR2_IMAGEFILE]);

  //sprites
  g_cSpriteManager.Load(RUSSIAN_OBJECT,"russian");
  g_cSpriteManager.Load(PLAYER1_OBJECT,"player");
  g_cSpriteManager.Load(NPC_OBJECT,"npc");
  g_cSpriteManager.Load(BUTTON_OBJECT,"button");
  g_cSpriteManager.Load(COLLISION_BOX, "collisionbox");
  g_cSpriteManager.Load(HIT_BOX, "hitbox");
  g_cSpriteManager.Load(HURT_BOX, "hurtbox");
  g_cSpriteManager.Load(READY, "ready");
  g_cSpriteManager.Load(FIGHT, "fight");
  g_cSpriteManager.Load(NINE, "nine");
  g_cSpriteManager.Load(EIGHT, "eight");
  g_cSpriteManager.Load(SEVEN, "seven");
  g_cSpriteManager.Load(SIX, "six");
  g_cSpriteManager.Load(FIVE, "five");
  g_cSpriteManager.Load(FOUR, "four");
  g_cSpriteManager.Load(THREE, "three");
  g_cSpriteManager.Load(TWO, "two");
  g_cSpriteManager.Load(ONE, "one");
  g_cSpriteManager.Load(ZERO, "zero");
  g_cSpriteManager.Load(ROUNDONE, "roundone");
  g_cSpriteManager.Load(ROUNDTWO, "roundtwo");
  g_cSpriteManager.Load(ROUNDTHREE, "roundthree");
  g_cSpriteManager.Load(P1WIN, "p1win");
  g_cSpriteManager.Load(P2WIN, "p2win");
  g_cSpriteManager.Load(ARROW, "arrow");
  g_cSpriteManager.Load(ROUNDNOTCHOFF, "roundnotchoff");
  g_cSpriteManager.Load(ROUNDNOTCHON, "roundnotchon");
  g_cSpriteManager.Load(HITSPARK, "hitspark");
  g_cSpriteManager.Load(COMBO, "combo");
  g_cSpriteManager.Load(WNINE, "wnine");
  g_cSpriteManager.Load(WEIGHT, "weight");
  g_cSpriteManager.Load(WSEVEN, "wseven");
  g_cSpriteManager.Load(WSIX, "wsix");
  g_cSpriteManager.Load(WFIVE, "wfive");
  g_cSpriteManager.Load(WFOUR, "wfour");
  g_cSpriteManager.Load(WTHREE, "wthree");
  g_cSpriteManager.Load(WTWO, "wtwo");
  g_cSpriteManager.Load(WONE, "wone");
  g_cSpriteManager.Load(WZERO, "wzero");
  //g_cSpriteManager.Load(CIRCLE, "circle");	//ZSPRITE
  
} //LoadTextures


/// Release game textures.
/// All textures used in the game are released - the release function is kind
/// of like a destructor for DirectX entities, which are COM objects.

void CGameRenderer::Release(){ //release D3D textures and stuff, then D3D itself 
  if(m_pSuccessTexture)m_pSuccessTexture->Release();
  if(m_pHUDTexture)m_pHUDTexture->Release();
  if(m_pBackgroundTexture)m_pBackgroundTexture->Release();
  if(m_pFloorTexture)m_pFloorTexture->Release();
  if(m_pWallTexture)m_pWallTexture->Release();
  if(m_pWireframeTexture)m_pWireframeTexture->Release();
  if(m_pMenuTexture)m_pMenuTexture->Release();
  if(m_pDeviceMenuTexture)m_pDeviceMenuTexture->Release();
  if(m_pBackgroundVB)m_pBackgroundVB->Release(); //release background vertex buffer
  if(m_pHealthTexture)m_pHealthTexture->Release();
  if(m_pHealthTexture2)m_pHealthTexture2->Release();
  if(m_pMediumHealthTexture)m_pMediumHealthTexture->Release();
  if(m_pMediumHealthTexture2)m_pMediumHealthTexture2->Release();
  if(m_pLowHealthTexture)m_pLowHealthTexture->Release();
  if(m_pLowHealthTexture2)m_pLowHealthTexture2->Release();
  if(m_pSuperBarVB)m_pSuperBarVB->Release();
  if(m_pSuperBarVB2)m_pSuperBarVB2->Release();
  if(m_pHealthVB)m_pHealthVB->Release();
  if(m_pHealthVB2)m_pHealthVB2->Release();
  g_cSpriteManager.Release(); 
  
  ReleaseD3D(); //release Direct3D
} //Release


/// Compose a frame of animation.
/// Draws the next frame of animation to the back buffer and flips it to the front.
/// \return TRUE if it succeeded

void CGameRenderer::ComposeFrame(){  
  m_d3ddevice->Clear(0L, NULL, D3DCLEAR_TARGET, 0xFFFFFF, 1.0f, 0L); //clear render buffer
  if(SUCCEEDED(m_d3ddevice->BeginScene())){ //can start rendering
    //move objects
    g_cObjectManager.move();
    //Write to input buffer ADD P2 LATER
    p1InputBuffer.write(g_pInputManager->checkDirection(1));
    p2InputBuffer.write(g_pInputManager->checkDirection(2));
    if(p1InputBuffer.dashTimer6 > 0)  //Basically, dashTimer for a given direction (6 is forward, 4 is back)
    {   //gets set whenever you press forwards or backwards, and it decreases by one value every frame.  The
      p1InputBuffer.dashTimer6--; //initial value it's set to determines the leniency of the dash input. In
    }   //order for a dash to occur, you must press forwards or backwards twice quickly enough that the 
    if(p1InputBuffer.dashTimer4 > 0)  //variable doesn't reach 0 before the second press.  Right now, the
    {   //timer is defaulted to 12 frames in the input.cpp file under the keyboard handler.
      p1InputBuffer.dashTimer4--;
    }
    if(p2InputBuffer.dashTimer6 > 0)
    {
      p2InputBuffer.dashTimer6--;
    }
    if(p2InputBuffer.dashTimer4 > 0)
    {
      p2InputBuffer.dashTimer4--;
    }
    //finished input buffer stuff
    //set camera location
    float x1, x2, y1, y2, xAvg, yAvg, xDif; //player's current location
    g_cObjectManager.GetPlayerLocations(x1, y1, x2, y2); //get player's lcoation
	xAvg = (x1+x2)/2.0f;
	yAvg = (y1+y2)/2.0f;
  xDif = abs(x2-x1);
    if(yAvg > g_nScreenHeight/2.0f) 
      yAvg = g_nScreenHeight/2.0f;
    if(yAvg < g_nScreenHeight/4.0f) 
      yAvg = g_nScreenHeight/4.0f;
    if(xAvg > 1.7f*g_nScreenWidth)
        xAvg = 1.7f*g_nScreenWidth;
    if(xAvg < .3f*g_nScreenWidth)
        xAvg = .3f*g_nScreenWidth;
    if(xDif<g_nScreenWidth/2.0f)
        xDif = g_nScreenWidth/2.0f;
    if(m_bCameraDefaultMode) 
       /*
        Completed design for dynamic camera follow and zoom w/ the following parameters
        X param: xAvg moves camera with player position average, offset half a screenwidth to the right for correction
        Y param: yAvg follows jumps, xDif/5 to vertically center based on z-zoom, screenheight/5 offset for correction
        Z param: -xDif/2 creates z-zoom based on distance, offset by 200 for correction
        */
      SetViewMatrix(xAvg + g_nScreenWidth/2.0f, yAvg-(g_nScreenHeight/5)+(xDif/5), (-xDif*.6f)+350);
    else SetViewMatrix(xAvg + g_nScreenWidth/2.0f, 1000.0f, -5000.0f);
    //draw background 
    DrawBackground(xAvg);
    //draw objects
	 /*D3DXVECTOR3 loc; //ZSPRITE
	 loc.x = 500;
	 loc.y = 100;
	 loc.z = 800;
	 g_cSpriteManager.Draw(CIRCLE, loc, 0.0f, 1.0f, 0);*/

	if(PauseGame == 1)
		timeStart = g_cTimer.time();
    if(timeStart==-1)
        timeStart = g_cTimer.time();

    if(g_cTimer.elapsed(timeStart, 1000))
        second = second - 1;


    if(g_cTimer.elapsed(sparkTimer, 30))
    {
        for(int i=5; i>=0; i--)
        {
            if(hitSparkOn[i]==1)
            {
                if(i!=5)
                {
                hitSparkOn[i+1] = hitSparkOn[i];
                sparkLoc[i+1] = sparkLoc[i];
                }
                hitSparkOn[i] = 0;
                sparkLoc[i].x = 0;
                sparkLoc[i].y = 0;
                sparkLoc[i].z = 0;
            }
        }
    }






    D3DXVECTOR3 loct1;
    loct1.x = xAvg+(g_nScreenWidth/50);
	loct1.y = yAvg+(g_nScreenHeight*.125f)+(xDif/5);
	loct1.z = 1150+(-xDif*.6f);

    if((second<=99)&&(second>=0))
    {
    switch(second%10){
    case 9:
        g_cSpriteManager.Draw(NINE, loct1, 0.0f, 1.0f, 0);
        break;
    case 8:
        g_cSpriteManager.Draw(EIGHT, loct1, 0.0f, 1.0f, 0);
        break;
    case 7:
        g_cSpriteManager.Draw(SEVEN, loct1, 0.0f, 1.0f, 0);
        break;
    case 6:
        g_cSpriteManager.Draw(SIX, loct1, 0.0f, 1.0f, 0);
        break;
    case 5:
        g_cSpriteManager.Draw(FIVE, loct1, 0.0f, 1.0f, 0);
        break;
    case 4:
        g_cSpriteManager.Draw(FOUR, loct1, 0.0f, 1.0f, 0);
        break;
    case 3:
        g_cSpriteManager.Draw(THREE, loct1, 0.0f, 1.0f, 0);
        break;
    case 2:
        g_cSpriteManager.Draw(TWO, loct1, 0.0f, 1.0f, 0);
        break;
    case 1:
        g_cSpriteManager.Draw(ONE, loct1, 0.0f, 1.0f, 0);
        break;
    case 0:
        g_cSpriteManager.Draw(ZERO, loct1, 0.0f, 1.0f, 0);
        break;
    default:
        break;
    }

    D3DXVECTOR3 loct2;
	loct2.x = xAvg-(g_nScreenWidth/50);
	loct2.y = yAvg+(g_nScreenHeight*.125f)+(xDif/5);
	loct2.z = 1150+(-xDif*.6f);

    switch((second-(second%10))/10){
    case 9:
        g_cSpriteManager.Draw(NINE, loct2, 0.0f, 1.0f, 0);
        break;
    case 8:
        g_cSpriteManager.Draw(EIGHT, loct2, 0.0f, 1.0f, 0);
        break;
    case 7:
        g_cSpriteManager.Draw(SEVEN, loct2, 0.0f, 1.0f, 0);
        break;
    case 6:
        g_cSpriteManager.Draw(SIX, loct2, 0.0f, 1.0f, 0);
        break;
    case 5:
        g_cSpriteManager.Draw(FIVE, loct2, 0.0f, 1.0f, 0);
        break;
    case 4:
        g_cSpriteManager.Draw(FOUR, loct2, 0.0f, 1.0f, 0);
        break;
    case 3:
        g_cSpriteManager.Draw(THREE, loct2, 0.0f, 1.0f, 0);
        break;
    case 2:
        g_cSpriteManager.Draw(TWO, loct2, 0.0f, 1.0f, 0);
        break;
    case 1:
        g_cSpriteManager.Draw(ONE, loct2, 0.0f, 1.0f, 0);
        break;
    case 0:
        g_cSpriteManager.Draw(ZERO, loct2, 0.0f, 1.0f, 0);
        break;
    default:
        break;
    }
    }

    float cxa = xAvg;
    float cxs = g_nScreenWidth;
    int cy = yAvg+(g_nScreenHeight*.114f)+(xDif/5);
	int cz = 1150+(-xDif*.6f);

    if(p1ComboCounter>0)
    {

    float p1scale;

    if(p1ComboCounter<15)
        p1scale = 0.25f + p1ComboCounter*0.05f;
    else
        p1scale = 1.0f;

    D3DXVECTOR3 comb11;
    comb11.x = (cxa + cxs*(-0.365f + (p1scale*(0.012f))));
	comb11.y = cy;
	comb11.z = cz;

    D3DXVECTOR3 comb12;
	comb12.x = (cxa + cxs*(-0.375f - (p1scale*(0.012f))));
	comb12.y = cy;
	comb12.z = cz;

    D3DXVECTOR3 comb13;
    comb13.x = (cxa + cxs*(-0.38f + (p1scale*(0.0595f))));
	comb13.y = (cy - (g_nScreenHeight*(0.04f + (p1scale*(0.04f)))));
	comb13.z = cz;




    switch(p1ComboCounter%10){
    case 9:
        g_cSpriteManager.Draw(WNINE, comb11, 0.0f, p1scale, 0);
        break;
    case 8:
        g_cSpriteManager.Draw(WEIGHT, comb11, 0.0f, p1scale, 0);
        break;
    case 7:
        g_cSpriteManager.Draw(WSEVEN, comb11, 0.0f, p1scale, 0);
        break;
    case 6:
        g_cSpriteManager.Draw(WSIX, comb11, 0.0f, p1scale, 0);
        break;
    case 5:
        g_cSpriteManager.Draw(WFIVE, comb11, 0.0f, p1scale, 0);
        break;
    case 4:
        g_cSpriteManager.Draw(WFOUR, comb11, 0.0f, p1scale, 0);
        break;
    case 3:
        g_cSpriteManager.Draw(WTHREE, comb11, 0.0f, p1scale, 0);
        break;
    case 2:
        g_cSpriteManager.Draw(WTWO, comb11, 0.0f, p1scale, 0);
        break;
    case 1:
        g_cSpriteManager.Draw(WONE, comb11, 0.0f, p1scale, 0);
        break;
    case 0:
        g_cSpriteManager.Draw(WZERO, comb11, 0.0f, p1scale, 0);
        break;
    default:
        break;
    }

    

    switch((p1ComboCounter-(p1ComboCounter%10))/10){
    case 9:
        g_cSpriteManager.Draw(WNINE, comb12, 0.0f, p1scale, 0);
        break;
    case 8:
        g_cSpriteManager.Draw(WEIGHT, comb12, 0.0f, p1scale, 0);
        break;
    case 7:
        g_cSpriteManager.Draw(WSEVEN, comb12, 0.0f, p1scale, 0);
        break;
    case 6:
        g_cSpriteManager.Draw(WSIX, comb12, 0.0f, p1scale, 0);
        break;
    case 5:
        g_cSpriteManager.Draw(WFIVE, comb12, 0.0f, p1scale, 0);
        break;
    case 4:
        g_cSpriteManager.Draw(WFOUR, comb12, 0.0f, p1scale, 0);
        break;
    case 3:
        g_cSpriteManager.Draw(WTHREE, comb12, 0.0f, p1scale, 0);
        break;
    case 2:
        g_cSpriteManager.Draw(WTWO, comb12, 0.0f, p1scale, 0);
        break;
    case 1:
        g_cSpriteManager.Draw(WONE, comb12, 0.0f, p1scale, 0);
        break;
    //case 0:
    //    g_cSpriteManager.Draw(WZERO, comb12, 0.0f, p1scale, 0);
    //    break;
    default:
        break;
    }

    g_cSpriteManager.Draw(COMBO, comb13, 0.0f, p1scale, 0);

    }


    if(p2ComboCounter>0)
    {

    float p2scale;

    if(p2ComboCounter<15)
        p2scale = 0.25f + p2ComboCounter*0.05f;
    else
        p2scale = 1.0f;

    D3DXVECTOR3 comb22;
    comb22.x = (cxa + cxs*(+0.365f - (p2scale*(0.012f))));
	comb22.y = cy;
	comb22.z = cz;

    D3DXVECTOR3 comb21;
	comb21.x = (cxa + cxs*(+0.375f + (p2scale*(0.012f))));
	comb21.y = cy;
	comb21.z = cz;

    D3DXVECTOR3 comb23;
    comb23.x = (cxa + cxs*(+0.38f - (p2scale*(0.0595f))));
	comb23.y = (cy - (g_nScreenHeight*(0.04f + (p2scale*(0.04f)))));
	comb23.z = cz;




    switch(p2ComboCounter%10){
    case 9:
        g_cSpriteManager.Draw(WNINE, comb21, 0.0f, p2scale, 0);
        break;
    case 8:
        g_cSpriteManager.Draw(WEIGHT, comb21, 0.0f, p2scale, 0);
        break;
    case 7:
        g_cSpriteManager.Draw(WSEVEN, comb21, 0.0f, p2scale, 0);
        break;
    case 6:
        g_cSpriteManager.Draw(WSIX, comb21, 0.0f, p2scale, 0);
        break;
    case 5:
        g_cSpriteManager.Draw(WFIVE, comb21, 0.0f, p2scale, 0);
        break;
    case 4:
        g_cSpriteManager.Draw(WFOUR, comb21, 0.0f, p2scale, 0);
        break;
    case 3:
        g_cSpriteManager.Draw(WTHREE, comb21, 0.0f, p2scale, 0);
        break;
    case 2:
        g_cSpriteManager.Draw(WTWO, comb21, 0.0f, p2scale, 0);
        break;
    case 1:
        g_cSpriteManager.Draw(WONE, comb21, 0.0f, p2scale, 0);
        break;
    case 0:
        g_cSpriteManager.Draw(WZERO, comb21, 0.0f, p2scale, 0);
        break;
    default:
        break;
    }

    

    switch((p2ComboCounter-(p2ComboCounter%10))/10){
    case 9:
        g_cSpriteManager.Draw(WNINE, comb22, 0.0f, p2scale, 0);
        break;
    case 8:
        g_cSpriteManager.Draw(WEIGHT, comb22, 0.0f, p2scale, 0);
        break;
    case 7:
        g_cSpriteManager.Draw(WSEVEN, comb22, 0.0f, p2scale, 0);
        break;
    case 6:
        g_cSpriteManager.Draw(WSIX, comb22, 0.0f, p2scale, 0);
        break;
    case 5:
        g_cSpriteManager.Draw(WFIVE, comb22, 0.0f, p2scale, 0);
        break;
    case 4:
        g_cSpriteManager.Draw(WFOUR, comb22, 0.0f, p2scale, 0);
        break;
    case 3:
        g_cSpriteManager.Draw(WTHREE, comb22, 0.0f, p2scale, 0);
        break;
    case 2:
        g_cSpriteManager.Draw(WTWO, comb22, 0.0f, p2scale, 0);
        break;
    case 1:
        g_cSpriteManager.Draw(WONE, comb22, 0.0f, p2scale, 0);
        break;
    //case 0:
    //    g_cSpriteManager.Draw(WZERO, comb12, 0.0f, p2scale, 0);
    //    break;
    default:
        break;
    }

    g_cSpriteManager.Draw(COMBO, comb23, 0.0f, p2scale, 0);

    }

    //----------------------------------------------------------------------
    // - - - - - - - - - - PLAYER 1 COMBO SLIDER - - - - - - - - - - - - - -
    //----------------------------------------------------------------------
    if((p1ComboSlide[0]>0)&&(p1ComboSlideTimer==-1))
        p1ComboSlideTimer = g_cTimer.time();


    if(p1ComboSlideTimer!=-1)
    {
        for(int i=11; i>=0; i--)
        {
            if(p1ComboSlide[i]>0)
            {


                float s1scale;
                if(p1ComboSlide[i]<15)
                    s1scale = 0.25f + p1ComboSlide[i]*0.05f;
                else
                    s1scale = 1.0f;

                D3DXVECTOR3 comb11;
                comb11.x = (cxa + cxs*(-0.365f + (s1scale*0.012f) - (i*.0075f)));
	            comb11.y = cy;
	            comb11.z = cz;

                D3DXVECTOR3 comb12;
	            comb12.x = (cxa + cxs*(-0.375f - (s1scale*0.012f) - (i*.0075f)));
	            comb12.y = cy;
	            comb12.z = cz;

                D3DXVECTOR3 comb13;
                comb13.x = (cxa + cxs*(-0.38f + (s1scale*0.0595f) - (i*.0075f)));
	            comb13.y = (cy - (g_nScreenHeight*(0.04f + (s1scale*(0.04f)))));
	            comb13.z = cz;




                switch(p1ComboSlide[i]%10){
                case 9:
                    g_cSpriteManager.Draw(WNINE, comb11, 0.0f, s1scale, 0);
                    break;
                case 8:
                    g_cSpriteManager.Draw(WEIGHT, comb11, 0.0f, s1scale, 0);
                    break;
                case 7:
                    g_cSpriteManager.Draw(WSEVEN, comb11, 0.0f, s1scale, 0);
                    break;
                case 6:
                    g_cSpriteManager.Draw(WSIX, comb11, 0.0f, s1scale, 0);
                    break;
                case 5:
                    g_cSpriteManager.Draw(WFIVE, comb11, 0.0f, s1scale, 0);
                    break;
                case 4:
                    g_cSpriteManager.Draw(WFOUR, comb11, 0.0f, s1scale, 0);
                    break;
                case 3:
                    g_cSpriteManager.Draw(WTHREE, comb11, 0.0f, s1scale, 0);
                    break;
                case 2:
                    g_cSpriteManager.Draw(WTWO, comb11, 0.0f, s1scale, 0);
                    break;
                case 1:
                    g_cSpriteManager.Draw(WONE, comb11, 0.0f, s1scale, 0);
                    break;
                case 0:
                    g_cSpriteManager.Draw(WZERO, comb11, 0.0f, s1scale, 0);
                    break;
                default:
                    break;
                }

    

                switch((p1ComboSlide[i]-(p1ComboSlide[i]%10))/10){
                case 9:
                    g_cSpriteManager.Draw(WNINE, comb12, 0.0f, s1scale, 0);
                    break;
                case 8:
                    g_cSpriteManager.Draw(WEIGHT, comb12, 0.0f, s1scale, 0);
                    break;
                case 7:
                    g_cSpriteManager.Draw(WSEVEN, comb12, 0.0f, s1scale, 0);
                    break;
                case 6:
                    g_cSpriteManager.Draw(WSIX, comb12, 0.0f, s1scale, 0);
                    break;
                case 5:
                    g_cSpriteManager.Draw(WFIVE, comb12, 0.0f, s1scale, 0);
                    break;
                case 4:
                    g_cSpriteManager.Draw(WFOUR, comb12, 0.0f, s1scale, 0);
                    break;
                case 3:
                    g_cSpriteManager.Draw(WTHREE, comb12, 0.0f, s1scale, 0);
                    break;
                case 2:
                    g_cSpriteManager.Draw(WTWO, comb12, 0.0f, s1scale, 0);
                    break;
                case 1:
                    g_cSpriteManager.Draw(WONE, comb12, 0.0f, s1scale, 0);
                    break;
                //case 0:
                //    g_cSpriteManager.Draw(WZERO, comb12, 0.0f, s1scale, 0);
                //    break;
                default:
                    break;
                }

                g_cSpriteManager.Draw(COMBO, comb13, 0.0f, s1scale, 0);

            }
        }
    }


    if((p1ComboSlideTimer!=-1)&&(g_cTimer.elapsed(p1ComboSlideTimer, 4)))
    {
        for(int i=11; i>=0; i--)
        {
            if(p1ComboSlide[i]>0)
            {                
                if(i!=11)
                    p1ComboSlide[i+1] = p1ComboSlide[i];

                p1ComboSlide[i] = 0;
            }
        }
    }
    //----------------------------------------------------------------------
    // - - - - - - - - - END PLAYER 1 COMBO SLIDER - - - - - - - - - - - - -
    //----------------------------------------------------------------------


    //----------------------------------------------------------------------
    // - - - - - - - - - - PLAYER 2 COMBO SLIDER - - - - - - - - - - - - - -
    //----------------------------------------------------------------------
    if((p2ComboSlide[0]>0)&&(p2ComboSlideTimer==-1))
        p2ComboSlideTimer = g_cTimer.time();


    if(p2ComboSlideTimer!=-1)
    {
        for(int i=11; i>=0; i--)
        {
            if(p2ComboSlide[i]>0)
            {


                float s2scale;
                if(p2ComboSlide[i]<15)
                    s2scale = 0.25f + p2ComboSlide[i]*0.05f;
                else
                    s2scale = 1.0f;

                D3DXVECTOR3 comb22;
                comb22.x = (cxa + cxs*(0.365f - (s2scale*0.012f) + (i*.0075f)));
	            comb22.y = cy;
	            comb22.z = cz;

                D3DXVECTOR3 comb21;
	            comb21.x = (cxa + cxs*(0.375f + (s2scale*0.012f) + (i*.0075f)));
	            comb21.y = cy;
	            comb21.z = cz;

                D3DXVECTOR3 comb23;
                comb23.x = (cxa + cxs*(0.38f - (s2scale*0.0595f) + (i*.0075f)));
	            comb23.y = (cy - (g_nScreenHeight*(0.04f + (s2scale*(0.04f)))));
	            comb23.z = cz;




                switch(p2ComboSlide[i]%10){
                case 9:
                    g_cSpriteManager.Draw(WNINE, comb21, 0.0f, s2scale, 0);
                    break;
                case 8:
                    g_cSpriteManager.Draw(WEIGHT, comb21, 0.0f, s2scale, 0);
                    break;
                case 7:
                    g_cSpriteManager.Draw(WSEVEN, comb21, 0.0f, s2scale, 0);
                    break;
                case 6:
                    g_cSpriteManager.Draw(WSIX, comb21, 0.0f, s2scale, 0);
                    break;
                case 5:
                    g_cSpriteManager.Draw(WFIVE, comb21, 0.0f, s2scale, 0);
                    break;
                case 4:
                    g_cSpriteManager.Draw(WFOUR, comb21, 0.0f, s2scale, 0);
                    break;
                case 3:
                    g_cSpriteManager.Draw(WTHREE, comb21, 0.0f, s2scale, 0);
                    break;
                case 2:
                    g_cSpriteManager.Draw(WTWO, comb21, 0.0f, s2scale, 0);
                    break;
                case 1:
                    g_cSpriteManager.Draw(WONE, comb21, 0.0f, s2scale, 0);
                    break;
                case 0:
                    g_cSpriteManager.Draw(WZERO, comb21, 0.0f, s2scale, 0);
                    break;
                default:
                    break;
                }

    

                switch((p2ComboSlide[i]-(p2ComboSlide[i]%10))/10){
                case 9:
                    g_cSpriteManager.Draw(WNINE, comb22, 0.0f, s2scale, 0);
                    break;
                case 8:
                    g_cSpriteManager.Draw(WEIGHT, comb22, 0.0f, s2scale, 0);
                    break;
                case 7:
                    g_cSpriteManager.Draw(WSEVEN, comb22, 0.0f, s2scale, 0);
                    break;
                case 6:
                    g_cSpriteManager.Draw(WSIX, comb22, 0.0f, s2scale, 0);
                    break;
                case 5:
                    g_cSpriteManager.Draw(WFIVE, comb22, 0.0f, s2scale, 0);
                    break;
                case 4:
                    g_cSpriteManager.Draw(WFOUR, comb22, 0.0f, s2scale, 0);
                    break;
                case 3:
                    g_cSpriteManager.Draw(WTHREE, comb22, 0.0f, s2scale, 0);
                    break;
                case 2:
                    g_cSpriteManager.Draw(WTWO, comb22, 0.0f, s2scale, 0);
                    break;
                case 1:
                    g_cSpriteManager.Draw(WONE, comb22, 0.0f, s2scale, 0);
                    break;
                //case 0:
                //    g_cSpriteManager.Draw(WZERO, comb22, 0.0f, s2scale, 0);
                //    break;
                default:
                    break;
                }

                g_cSpriteManager.Draw(COMBO, comb23, 0.0f, s2scale, 0);

            }
        }
    }


    if((p2ComboSlideTimer!=-1)&&(g_cTimer.elapsed(p2ComboSlideTimer, 4)))
    {
        for(int i=11; i>=0; i--)
        {
            if(p2ComboSlide[i]>0)
            {                
                if(i!=11)
                    p2ComboSlide[i+1] = p2ComboSlide[i];

                p2ComboSlide[i] = 0;
            }
        }
    }
    //----------------------------------------------------------------------
    // - - - - - - - - - END PLAYER 2 COMBO SLIDER - - - - - - - - - - - - -
    //----------------------------------------------------------------------




      //create vertex buffer for background

  
  //load vertex buffer
  BILLBOARDVERTEX* v;
 if(SUCCEEDED(m_pSuperBarVB->Lock(0,0,(void**)&v, 0))){
    float tempf = ((float)p1SuperCurrent/100.0f);
   // int tempX = 930 - (450 * tempf);
	int tempG = 409 + (450*tempf); 
	if(tempG >= 849){
		tempG = 849;
	}
	if(tempG <= 409){
		tempG = 409;
	}

	  v[0].p = D3DXVECTOR3(tempG, 343, 1000);
	  v[0].tu = 1.0f; v[0].tv = 1.0f;

	  v[1].p = D3DXVECTOR3(409, 343, 1000);
	  v[1].tu = 0.0f; v[1].tv = 1.0f;

	  v[2].p = D3DXVECTOR3(tempG, 390, 1000);
	  v[2].tu = 1.0f; v[2].tv = 0.0f;

	  v[3].p = D3DXVECTOR3(409, 390, 1000);
	  v[3].tu = 0.0f; v[3].tv = 0.0f;

	  m_pSuperBarVB->Unlock();

 }
  
 BILLBOARDVERTEX* v2;
if(SUCCEEDED(m_pHealthVB->Lock(0, 0, (void**)&v2, 0))){ //lock buffer
    //vertex information, first triangle in clockwise order
    float tempf = ((float)p1Health/100.0f);
    int tempX = 930 - (450 * tempf);

    v2[0].p = D3DXVECTOR3(930, 1160, 1000);
    v2[0].tu = 1.0f; v2[0].tv = 1.0f;

    v2[1].p = D3DXVECTOR3(tempX, 1160, 1000);
    v2[1].tu = 0.0f; v2[1].tv = 1.0f;

    v2[2].p = D3DXVECTOR3(930, 1222, 1000);
    v2[2].tu = 1.0f; v2[2].tv = 0.0f;

    v2[3].p = D3DXVECTOR3(tempX, 1222, 1000);
    v2[3].tu = 0.0f; v2[3].tv = 0.0f;
 
    m_pHealthVB->Unlock();
  }

  //create vertex buffer for background
  
  //load vertex buffer
  //GameRenderer.HealthVertexBuffer2(1120, 1570);
  if(SUCCEEDED(m_pHealthVB2->Lock(0, 0, (void**)&v2, 0))){ //lock buffer
    //vertex information, first triangle in clockwise order
    float tempF = ((float)p2Health/100.0f);
    int tempx = 1120 + (450 * tempF);

    v2[0].p = D3DXVECTOR3(tempx, 1160, 1000);
    v2[0].tu = 1.0f; v2[0].tv = 1.0f;

    v2[1].p = D3DXVECTOR3(1120, 1160, 1000);
    v2[1].tu = 0.0f; v2[1].tv = 1.0f;

    v2[2].p = D3DXVECTOR3(tempx, 1222, 1000);
    v2[2].tu = 1.0f; v2[2].tv = 0.0f;

    v2[3].p = D3DXVECTOR3(1120, 1222, 1000);
    v2[3].tu = 0.0f; v2[3].tv = 0.0f;
 
    m_pHealthVB2->Unlock();
  }
  
 BILLBOARDVERTEX* v3; //vertex buffer data
 float tempF = ((float)p2SuperCurrent/100.0f);
	int tempG = 1638 - (440*tempF); 
	if(tempG <= 1198){
		tempG = 1198;
	}
	if(tempG >= 1638){
		tempG = 1638;
	}

 if(SUCCEEDED(m_pSuperBarVB2->Lock(0,0,(void**)&v3, 0))){
    float tempF = ((float)p2Health/100.0f);
    int tempx = 1120 + (450 * tempF);

    v3[0].p = D3DXVECTOR3(1638, 343, 1000);
    v3[0].tu = 1.0f; v3[0].tv = 1.0f;

    v3[1].p = D3DXVECTOR3(tempG, 343, 1000);
    v3[1].tu = 0.0f; v3[1].tv = 1.0f;

    v3[2].p = D3DXVECTOR3(1638, 390, 1000);
    v3[2].tu = 1.0f; v3[2].tv = 0.0f;

    v3[3].p = D3DXVECTOR3(tempG, 390, 1000);
    v3[3].tu = 0.0f; v3[3].tv = 0.0f;

	  m_pSuperBarVB2->Unlock();

 }



  //    //create vertex buffer for background
  //m_d3ddevice->CreateVertexBuffer( 
  //  4*sizeof(BILLBOARDVERTEX), D3DUSAGE_WRITEONLY, BILLBOARDVERTEX::FVF,
  //  D3DPOOL_MANAGED, &m_pHealthVB, NULL);
  //
  ////load vertex buffer
  //BILLBOARDVERTEX* v; //vertex buffer data

  //if(SUCCEEDED(m_pHealthVB->Lock(0, 0, (void**)&v, 0))){ //lock buffer
  //  //vertex information, first triangle in clockwise order
  //  float tempf = ((float)p1Health/100.0f);
  //  int tempX = 480 + (450 * tempf);

  //  v[0].p = D3DXVECTOR3(tempX, 1160, 1000);
  //  v[0].tu = 1.0f; v[0].tv = 1.0f;

  //  v[1].p = D3DXVECTOR3(480, 1160, 1000);
  //  v[1].tu = 0.0f; v[1].tv = 1.0f;

  //  v[2].p = D3DXVECTOR3(tempX, 1222, 1000);
  //  v[2].tu = 1.0f; v[2].tv = 0.0f;

  //  v[3].p = D3DXVECTOR3(480, 1222, 1000);
  //  v[3].tu = 0.0f; v[3].tv = 0.0f;
 
  //  m_pHealthVB->Unlock();
  //}

  ////create vertex buffer for background
  //m_d3ddevice->CreateVertexBuffer( 
  //  4*sizeof(BILLBOARDVERTEX), D3DUSAGE_WRITEONLY, BILLBOARDVERTEX::FVF,
  //  D3DPOOL_MANAGED, &m_pHealthVB2, NULL);
  //
  ////load vertex buffer
  //BILLBOARDVERTEX* v2; //vertex buffer data

  //if(SUCCEEDED(m_pHealthVB2->Lock(0, 0, (void**)&v2, 0))){ //lock buffer
  //  //vertex information, first triangle in clockwise order
  //  float tempF = ((float)p2Health/100.0f);
  //  int tempx = 1570 - (450 * tempF);

  //  v2[0].p = D3DXVECTOR3(1570, 1160, 1000);
  //  v2[0].tu = 1.0f; v2[0].tv = 1.0f;

  //  v2[1].p = D3DXVECTOR3(tempx, 1160, 1000);
  //  v2[1].tu = 0.0f; v2[1].tv = 1.0f;

  //  v2[2].p = D3DXVECTOR3(1570, 1222, 1000);
  //  v2[2].tu = 1.0f; v2[2].tv = 0.0f;

  //  v2[3].p = D3DXVECTOR3(tempx, 1222, 1000);
  //  v2[3].tu = 0.0f; v2[3].tv = 0.0f;
 
  //  m_pHealthVB2->Unlock();
  //}


    D3DXVECTOR3 announcementSplash;
	announcementSplash.x = xAvg+(g_nScreenWidth/25);
	announcementSplash.y = yAvg-(g_nScreenHeight/10)+(xDif/5);
	announcementSplash.z = 1150+(-xDif*.6f);

    if(second>101)
    {
        announceTimer = g_cTimer.time();
        announceSize = 10.0f;
    }

    if((second<=101)&&(second>99))
    {
        if((g_cTimer.elapsed(announceTimer, 5))&&(announceSize>1.0f))
        {
            announceSize = announceSize - 0.6f;
            if(announceSize<1.0f)
                announceSize = 1.0f;
        }
        if(soundTemp==TRUE)
        {
            g_pInputManager->playSound(NOW_SOUND);
            soundTemp=FALSE;
        }
        if((p1RoundsWon+p2RoundsWon)==0){
            g_cSpriteManager.Draw(ROUNDONE, announcementSplash, 0.0f, announceSize, 0);
			p1SuperCurrent = 0;
			p2SuperCurrent = 0;
		}
        else if((p1RoundsWon+p2RoundsWon)==1)
            g_cSpriteManager.Draw(ROUNDTWO, announcementSplash, 0.0f, announceSize, 0);
        else if((p1RoundsWon+p2RoundsWon)>=2)
            g_cSpriteManager.Draw(ROUNDTHREE, announcementSplash, 0.0f, announceSize, 0);
        g_pInputManager->p1AtkOK = FALSE;
        g_pInputManager->p2AtkOK = FALSE;
        g_pInputManager->p1MoveOK = TRUE;
        g_pInputManager->p2MoveOK = TRUE;
    }

    if((98<second)&&(second<=99))
    {
        if((g_cTimer.elapsed(announceTimer, 5))&&(announceSize<10.0f))
        {
            announceSize = announceSize + 0.85f;
            if(announceSize>10.0f)
                announceSize = 10.0f;
        }
        if(soundTemp==FALSE)
        {
            g_pInputManager->playSound(FIGHT_SOUND);
            soundTemp = TRUE;
        }
	    g_cSpriteManager.Draw(FIGHT, announcementSplash, 0.0f, (11.0f-announceSize), 0);
        g_pInputManager->p1AtkOK = TRUE;
        g_pInputManager->p2AtkOK = TRUE;
    }

    if((p1Health<=0)||(p2Health<=0))
    {
        if(second>=0)
            second =-1;
    }

    if((second<=-1)&&(second>-2))
    {
        g_pInputManager->p1AtkOK = FALSE;
        g_pInputManager->p2AtkOK = FALSE;
        g_pInputManager->p1MoveOK = FALSE;
        g_pInputManager->p2MoveOK = FALSE;
		roundNotchInc = TRUE;
        soundTemp = FALSE;
    }

    if((second<=-2)&&(second>-5))
    {
        if(soundTemp==FALSE)
        {
            g_pInputManager->playSound(LAUGH_SOUND);
            soundTemp = TRUE;
        }
		if(roundNotchInc==TRUE)
		{
			if(p1Health>p2Health)
			{
				p1RoundsWon++;
				roundNotchInc=FALSE;
			}
			else
			{
				p2RoundsWon++;
				roundNotchInc=FALSE;
			}
		}
        if((g_cTimer.elapsed(announceTimer, 5))&&(announceSize>1.0f))
        {
            announceSize = announceSize - 0.5f;
            if(announceSize<1.0f)
                announceSize = 1.0f;
        }
        if(p1Health>p2Health)
        {
            g_cSpriteManager.Draw(P1WIN, announcementSplash, 0.0f, announceSize, 0);
        }
        else
        {
            g_cSpriteManager.Draw(P2WIN, announcementSplash, 0.0f, announceSize, 0);
        }
    }

    if(second==-5)
    {
        p1Health = 100;
        p2Health = 100;
        second = 102;
    }

    D3DXVECTOR3 roundnotchp11;
    roundnotchp11.x = xAvg+(g_nScreenWidth/25)-(g_nScreenWidth*0.16f);
	roundnotchp11.y = yAvg-(g_nScreenHeight/10)+(xDif/5)+(g_nScreenHeight*0.17f);
	roundnotchp11.z = 1150+(-xDif*.6f);

    D3DXVECTOR3 roundnotchp12;
    roundnotchp12.x = xAvg+(g_nScreenWidth/25)-(g_nScreenWidth*0.12f);
	roundnotchp12.y = yAvg-(g_nScreenHeight/10)+(xDif/5)+(g_nScreenHeight*0.17f);
	roundnotchp12.z = 1150+(-xDif*.6f);

    D3DXVECTOR3 roundnotchp21;
    roundnotchp21.x = xAvg+(g_nScreenWidth/25)+(g_nScreenWidth*0.04f);
	roundnotchp21.y = yAvg-(g_nScreenHeight/10)+(xDif/5)+(g_nScreenHeight*0.17f);
	roundnotchp21.z = 1150+(-xDif*.6f);

    D3DXVECTOR3 roundnotchp22;
    roundnotchp22.x = xAvg+(g_nScreenWidth/25)+(g_nScreenWidth*0.08f);
	roundnotchp22.y = yAvg-(g_nScreenHeight/10)+(xDif/5)+(g_nScreenHeight*0.17f);
	roundnotchp22.z = 1150+(-xDif*.6f);

    if(p1RoundsWon>=2)
        g_cSpriteManager.Draw(ROUNDNOTCHON, roundnotchp11, 0.0f, 1.0f, 0);
    else
        g_cSpriteManager.Draw(ROUNDNOTCHOFF, roundnotchp11, 0.0f, 1.0f, 0);

    if(p1RoundsWon>=1)
        g_cSpriteManager.Draw(ROUNDNOTCHON, roundnotchp12, 0.0f, 1.0f, 0);
    else
        g_cSpriteManager.Draw(ROUNDNOTCHOFF, roundnotchp12, 0.0f, 1.0f, 0);

    if(p2RoundsWon>=2)
        g_cSpriteManager.Draw(ROUNDNOTCHON, roundnotchp22, 0.0f, 1.0f, 0);
    else
        g_cSpriteManager.Draw(ROUNDNOTCHOFF, roundnotchp22, 0.0f, 1.0f, 0);

    if(p2RoundsWon>=1)
        g_cSpriteManager.Draw(ROUNDNOTCHON, roundnotchp21, 0.0f, 1.0f, 0);
    else
        g_cSpriteManager.Draw(ROUNDNOTCHOFF, roundnotchp21, 0.0f, 1.0f, 0);



    g_cObjectManager.draw();
    
	    for(int i=5; i>=0; i--)
    {
        if(hitSparkOn[i]==1)
        {
             g_cSpriteManager.Draw(HITSPARK, sparkLoc[i], 0, 0.6f, i);
        }
    }
    

    //heads-up display 
    m_d3ddevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_d3ddevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_d3ddevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    m_d3ddevice->SetStreamSource(0, m_pBackgroundVB, 0, sizeof(BILLBOARDVERTEX));
    m_d3ddevice->SetFVF(BILLBOARDVERTEX::FVF); //flexible vertex format
    m_d3ddevice->SetTexture(0, m_pHUDTexture); //set menutexture
    SetWorldMatrix(0.0f);
    SetViewMatrix((float)g_nScreenWidth, (float)g_nScreenHeight, -350.0f);
    m_d3ddevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 2, 2);

    
     //test hitbox VB UNCOMMENT NEXT 3 LINES TO ENABLE TEST HITBOX DRAWING
	 m_d3ddevice->SetStreamSource(0, m_pSuperBarVB, 0, sizeof(BILLBOARDVERTEX));
	 m_d3ddevice->SetTexture(0, m_pSuperBarTexture);
     m_d3ddevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	 m_d3ddevice->SetStreamSource(0, m_pSuperBarVB2, 0, sizeof(BILLBOARDVERTEX));
	 m_d3ddevice->SetTexture(0, m_pSuperBarTexture2);
     m_d3ddevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);


     if(p1Health >= 100)
     {
        m_d3ddevice->SetStreamSource(0, m_pHealthVB, 0, sizeof(BILLBOARDVERTEX));
        m_d3ddevice->SetTexture(0, m_pHealthTexture);
        m_d3ddevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
     }
     else if(p1Health < 100 && p1Health > 35)
     {
        m_d3ddevice->SetStreamSource(0, m_pHealthVB, 0, sizeof(BILLBOARDVERTEX));
        m_d3ddevice->SetTexture(0, m_pMediumHealthTexture);
        m_d3ddevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
     }
     else if(p1Health <= 35)
     {
        m_d3ddevice->SetStreamSource(0, m_pHealthVB, 0, sizeof(BILLBOARDVERTEX));
        m_d3ddevice->SetTexture(0, m_pLowHealthTexture);
        m_d3ddevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
     }
     if(p2Health >= 100)
     {
        m_d3ddevice->SetStreamSource(0, m_pHealthVB2, 0, sizeof(BILLBOARDVERTEX));
	      m_d3ddevice->SetTexture(0, m_pHealthTexture2);
	      m_d3ddevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
     }
     else if(p2Health < 100 && p2Health > 35)
     { 
        m_d3ddevice->SetStreamSource(0, m_pHealthVB2, 0, sizeof(BILLBOARDVERTEX));
	      m_d3ddevice->SetTexture(0, m_pMediumHealthTexture2);
	      m_d3ddevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
     }
     else if(p2Health <= 35)
     {
        m_d3ddevice->SetStreamSource(0, m_pHealthVB2, 0, sizeof(BILLBOARDVERTEX));
	      m_d3ddevice->SetTexture(0, m_pLowHealthTexture2);
	      m_d3ddevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
     }





    /*
	D3DXMATRIXA16 matWorld;   
  D3DXMatrixTranslation(&matWorld, dx, 0.0, 0.0);
  m_d3ddevice->SetTransform(D3DTS_WORLD, &matWorld );
    */
    //-------------

    //draw text on the HUD
    HeaderManager.DrawTextHeader();
   
    m_d3ddevice->EndScene(); //done rendering
  }
} //ComposeFrame

/// Compose menu frame.
/// Compose a frame of menu screen animation. This is just the buttons right
/// now, but we could add a glitzy animation if we wished.

BOOL CGameRenderer::ComposeMenuFrame(LPDIRECT3DTEXTURE9 menutexture){ //display menu screen
  m_d3ddevice->BeginScene();  //set vertex buffer to background
    m_d3ddevice->SetStreamSource(0, m_pBackgroundVB, 0, sizeof(BILLBOARDVERTEX));
    m_d3ddevice->SetFVF(BILLBOARDVERTEX::FVF); //flexible vertex format
    m_d3ddevice->SetTexture(0, menutexture); //set menutexture
    SetWorldMatrix(0.0f);
    SetViewMatrix((float)g_nScreenWidth, (float)g_nScreenHeight, -350.0f);
    m_d3ddevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 2, 2);
//    g_pInputManager->DrawButtons(); //draw buttons on screen
    D3DXVECTOR3 loc;
        loc.x = g_nScreenWidth*0.25f;
        loc.z = 100;

        if(menuArrowPos==0)
            loc.y = g_nScreenHeight - (g_nScreenHeight*0.2f);
        else if(menuArrowPos==1)
            loc.y = g_nScreenHeight - (g_nScreenHeight*0.35f);
        else if(menuArrowPos==2)
            loc.y = g_nScreenHeight - (g_nScreenHeight*0.5f);
    
        g_cSpriteManager.Draw(ARROW, loc, 0.0f, 2.0f, 0);
  m_d3ddevice->EndScene();
  return TRUE;
} //ComposeMenuFrame

/// Process a frame of animation of menu screen. 
/// Compose a frame, present it, then check for 
/// lost device and restore it if necessary.

void CGameRenderer::ProcessMenuFrame(){ //process a frame of animation
  static BOOL loading = FALSE;
  if(g_bImagesLoaded){ //wait for the image loading thread to finish
    //check for lost graphics device
    if(DeviceLost())RestoreDevice(); //if device lost, restore it
    //process a frame of animation
    ComposeMenuFrame(m_pMenuTexture); //compose a frame of animation
    PageFlip(); //flip to front
  }
  else{ //still loading, so display loading screen
    if(!loading){
      DisplayScreen(g_cImageFileName[LOADING_IMAGEFILE]);
      loading = TRUE;
    }
  }
} //ProcessMenuFrame

/// Process a frame of animation of menu screen. 
/// Compose a frame, present it, then check for 
/// lost device and restore it if necessary.

void CGameRenderer::ProcessDeviceMenuFrame(){ //process a frame of animation
  //check for lost graphics device
  if(DeviceLost())RestoreDevice(); //if device lost, restore it
  //process a frame of animation
  ComposeDeviceFrame(m_pDeviceMenuTexture); //compose a frame of animation
  PageFlip(); //flip to front
} //ProcessMenuFrame

BOOL CGameRenderer::ComposeDeviceFrame(LPDIRECT3DTEXTURE9 menutexture){ //display menu screen
    


  m_d3ddevice->BeginScene();  //set vertex buffer to background
    m_d3ddevice->SetStreamSource(0, m_pBackgroundVB, 0, sizeof(BILLBOARDVERTEX));
    m_d3ddevice->SetFVF(BILLBOARDVERTEX::FVF); //flexible vertex format
    m_d3ddevice->SetTexture(0, menutexture); //set menutexture
    SetWorldMatrix(0.0f);
    SetViewMatrix((float)g_nScreenWidth, (float)g_nScreenHeight, -350.0f);
    m_d3ddevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 2, 2);
//    g_pInputManager->DrawButtons(); //draw buttons on screen
    D3DXVECTOR3 loc;
        
        loc.z = 400;

        if(dMenuArrowPos<=6)
        {
            loc.x = -g_nScreenWidth*0.1f;
            loc.y = 1.7f*g_nScreenHeight - (g_nScreenHeight*0.2f*dMenuArrowPos);
        }
        else if(dMenuArrowPos<=13)
        {
            loc.x = g_nScreenWidth*0.7f;
            loc.y = 1.7f*g_nScreenHeight - (g_nScreenHeight*0.2f*(dMenuArrowPos-7));
        }
        else if(dMenuArrowPos==14)
        {
            loc.x = g_nScreenWidth*0.3f;
            loc.y = 1.7f*g_nScreenHeight - (g_nScreenHeight*0.2f*7);
        }


        if((dMenuOptionSelected>=0)&&(dMenuOptionSelected<14))
            loc.x = loc.x + (g_nScreenWidth*0.25f);
        

        g_cSpriteManager.Draw(ARROW, loc, 0.0f, 2.0f, 0);



        HeaderManager.DrawTextDMenu();



  m_d3ddevice->EndScene();

  return TRUE;
} //ComposeMenuFrame

/// Process a frame of animation. 
/// Compose a frame, present it, then check for 
/// lost device and restore it if necessary.

void CGameRenderer::ProcessFrame(){ //process a frame of animation
  if(g_bImagesLoaded){ //wait for the image loading thread to finish
    //check for lost graphics device
    if(DeviceLost())RestoreDevice(); //if device lost, restore it
    //process a frame of animation
    ComposeFrame(); //compose a frame of animation
    PageFlip(); //flip to front
  }
} //ProcessFrame

/// Toggle camera mode.
/// Toggle between eagle-eye camera (camera pulled back far enough to see
/// backdrop) and the normal game camera.

void CGameRenderer::FlipCameraMode(){ //pull the camera back
  m_bCameraDefaultMode = !m_bCameraDefaultMode; 
}


/// Display intro screen.
/// Display the intro screen for the current state.
/// \param state New game state

void CGameRenderer::DisplayIntroScreen(GameStateType state){ 
  switch(state){
    case LOGO_GAMESTATE: 
      DisplayScreen(g_cImageFileName[LOGO_IMAGEFILE]); break;
    case TITLE_GAMESTATE: 
      DisplayScreen(g_cImageFileName[TITLE_IMAGEFILE]); break;
    case CREDITS_GAMESTATE: 
      DisplayScreen(g_cImageFileName[CREDITS_IMAGEFILE]); break;
  }
} //DisplayIntroScreen


/// Display screen image.
/// Display an image from a file to the screen.
/// \param filename name of image file
/// \return TRUE if the load from file succeeded

BOOL CGameRenderer::DisplayScreen(char *filename){ //display image file
  //get surface for image
  LoadTexture(m_pIntroTexture, filename);

  //draw image 
  if(SUCCEEDED(m_d3ddevice->BeginScene())){ //can start rendering
    SetViewMatrix((float)g_nScreenWidth, (float)g_nScreenHeight, -350.0f); //350.0 is a kluge
    DisplayScreen(m_pIntroTexture);
    m_d3ddevice->EndScene(); 
  }
  PageFlip(); //flip to front

  //release image surface and return
  if(m_pIntroTexture)m_pIntroTexture->Release(); //release
  return TRUE;
} //DisplayScreen

void CGameRenderer::DisplaySuccessScreen(){ //display success screen
  //draw image 
    /*
  if(SUCCEEDED(m_d3ddevice->BeginScene())){ //can start rendering
    SetViewMatrix((float)g_nScreenWidth, (float)g_nScreenHeight, -350.0f); //350.0 is a kluge
    DisplayScreen(m_pSuccessTexture);


    m_d3ddevice->EndScene(); 
  }
  PageFlip(); //flip to front
  */
} //DisplayScreen


void CGameRenderer::PerspectiveRendering(BOOL on){
  SetD3dStates(on); //perspective projection
}

void CGameRenderer::ProcessSuccessFrame(){
    m_d3ddevice->Clear(0L, NULL, D3DCLEAR_TARGET, 0xFFFFFF, 1.0f, 0L);

    if(SUCCEEDED(m_d3ddevice->BeginScene())){

        SetViewMatrix((float)g_nScreenWidth, (float)g_nScreenHeight, -350.0f); //350.0 is a kluge
        DisplayScreen(m_pSuccessTexture);

        D3DXVECTOR3 splash;
        splash.x = g_nScreenWidth*0.4f;
        splash.y = g_nScreenHeight*0.95f;
        splash.z = 400;

        if(winner==1)
            g_cSpriteManager.Draw(P1WIN, splash, 0.0f, 1.5f, 0);
        else if(winner==2)
            g_cSpriteManager.Draw(P2WIN, splash, 0.0f, 1.5f, 0);

        D3DXVECTOR3 loc;
        loc.x = g_nScreenWidth*0.62f;
        loc.z = 400;

        if(successArrowPos==0)
            loc.y = g_nScreenHeight + (g_nScreenHeight*0.11f);
        else if(successArrowPos==1)
            loc.y = g_nScreenHeight + (g_nScreenHeight*0.02f);
        else if(successArrowPos==2)
            loc.y = g_nScreenHeight - (g_nScreenHeight*0.07f);
    
        g_cSpriteManager.Draw(ARROW, loc, 0.0f, 1.0f, 0);

        m_d3ddevice->EndScene();
    }

    PageFlip();
}




void CGameRenderer::DrawHitSpark(int left, int right, int bottom, int top)
{
    sparkLoc[0].x = (left+right)/2;
    sparkLoc[0].y = (bottom+top)/2;
    sparkLoc[0].z = 800;
    hitSparkOn[0] = 1;
    if(sparkTimer==-1)
        sparkTimer = g_cTimer.time();
}