/// \file sprite.cpp
/// Code for the base sprite class C3DSprite.
/// Copyright Ian Parberry, 2004.
/// Last updated September 24, 2010.

#include "sprite.h"
#include "defines.h"
#include "gamerenderer.h"

extern CGameRenderer GameRenderer;
extern int g_nScreenWidth;
extern BOOL g_bWireFrame;

C3DSprite::C3DSprite(int framecount){ //constructor
  m_nFrameCount = framecount; //number of frames 
  m_lpTexture = new LPDIRECT3DTEXTURE9[framecount]; ///texture array
  for(int i=0; i<framecount; i++) m_lpTexture[i] = NULL; //null it out
  ZeroMemory(&m_structImageInfo, sizeof(D3DXIMAGE_INFO)); //dimensions
  m_pVertexBuffer = NULL; //vertex buffer
  D3DXMatrixIdentity(&m_matWorld); //world matrix
}

C3DSprite::~C3DSprite(){ //destructor
  //deallocate memory allocated in constructor
  delete [] m_lpTexture;
}

/// The Load function loads the sprite image into a texture from a given file name.
/// It also creates a vertex buffer for the billboard image containing 4 corner vertices
/// spaced apart the appropriate width and height.
/// \param filename the name of the image file
/// \param frame frame number

BOOL C3DSprite::Load(char* filename, int frame){

  HRESULT hr; //D3D result

  //create texture to store image
  hr=D3DXCreateTextureFromFileEx(GameRenderer.m_d3ddevice, filename,
    0, 0, 0, 0,D3DFMT_A8R8G8B8, D3DPOOL_MANAGED,
    D3DX_FILTER_NONE, D3DX_DEFAULT, 0, &m_structImageInfo,
    NULL, &m_lpTexture[frame]);

  if(FAILED(hr)){ //fail and bail
    m_lpTexture[frame] = NULL; return FALSE;
  }


  //create vertex buffer
  if(m_pVertexBuffer == NULL){ //create only when first frame is loaded
    hr=GameRenderer.m_d3ddevice->CreateVertexBuffer( 
      4*sizeof(BILLBOARDVERTEX), D3DUSAGE_WRITEONLY, BILLBOARDVERTEX::FVF,
      D3DPOOL_MANAGED, &m_pVertexBuffer, NULL);
    if(FAILED(hr))return FALSE; //bail if failed
  }

  //load vertex buffer
  BILLBOARDVERTEX* v; //vertex buffer data
  float w = (float)m_structImageInfo.Width/2.0f; //width
  float h = (float)m_structImageInfo.Height/2.0f; //height
  
  if(SUCCEEDED(m_pVertexBuffer->Lock(0, 0, (void**)&v, 0))){ //lock buffer

    //vertex information, first triangle in clockwise order
    v[0].p = D3DXVECTOR3(w, h, 0.0f);
    v[0].tu = 1.0f; v[0].tv = 0.0f;

    v[1].p = D3DXVECTOR3(w, -h, 0.0f);
    v[1].tu = 1.0f; v[1].tv = 1.0f;

    v[2].p = D3DXVECTOR3(-w, h, 0.0f);
    v[2].tu = 0.0f; v[2].tv = 0.0f;

    v[3].p = D3DXVECTOR3(-w, -h, 0.0f);
    v[3].tu = 0.0f; v[3].tv = 1.0f;

    m_pVertexBuffer->Unlock(); //unlock buffer
  }



  //else return FALSE;
  //THIS COULD END VERY BADLY
  //while(!SUCCEEDED(m_pVertexBuffer->Lock(0, 0, (void**)&v, 0)))
  //{
  //  v[0].p = D3DXVECTOR3(w, h, 0.0f);
  //  v[0].tu = 1.0f; v[0].tv = 0.0f;

  //  v[1].p = D3DXVECTOR3(w, -h, 0.0f);
  //  v[1].tu = 1.0f; v[1].tv = 1.0f;

  //  v[2].p = D3DXVECTOR3(-w, h, 0.0f);
  //  v[2].tu = 0.0f; v[2].tv = 0.0f;

  //  v[3].p = D3DXVECTOR3(-w, -h, 0.0f);
  //  v[3].tu = 0.0f; v[3].tv = 1.0f;

  //  m_pVertexBuffer->Unlock(); //unlock buffer
  //}



  return TRUE; //successful
}

/// Draws the sprite image at a given point in 3D space relative to the center of the screen.
/// \param p point in 3D space at which to draw the sprite
/// \param angle Angle to rotate sprite
/// \param frame frame number

void C3DSprite::Draw(D3DXVECTOR3 p, float angle, float scale, int frame){

  D3DXMATRIXA16 rotationmatrix, translationmatrix, scalematrix, tempmatrix; ///< Rotation and translation matrices.

  //set diffuse blending for alpha set in vertices
  GameRenderer.m_d3ddevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
  GameRenderer.m_d3ddevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
  GameRenderer.m_d3ddevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

  //set translation matrix
  D3DXMatrixTranslation(&translationmatrix, p.x + g_nScreenWidth/2, p.y, p.z);

  //set rotation matrix  
  D3DXMatrixRotationZ(&rotationmatrix, angle);
  D3DXMatrixScaling(&scalematrix, scale, scale, 1.0f);
  D3DXMatrixMultiply(&tempmatrix, &scalematrix, &rotationmatrix);
  D3DXMatrixMultiply(&m_matWorld, &tempmatrix, &translationmatrix);
  GameRenderer.m_d3ddevice->SetTransform(D3DTS_WORLD, &m_matWorld);

  //set vertex buffer
  GameRenderer.m_d3ddevice->SetStreamSource(0, m_pVertexBuffer, 0, sizeof(BILLBOARDVERTEX));
  GameRenderer.m_d3ddevice->SetFVF(BILLBOARDVERTEX::FVF); //flexible vertex format

  //set texture
  if( !g_bWireFrame && frame>=0 && frame<m_nFrameCount && m_lpTexture[frame] ) //if ok
    GameRenderer.m_d3ddevice->SetTexture(0, m_lpTexture[frame]); //set sprite texture

  //draw sprite (finally)
  GameRenderer.m_d3ddevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

  //restore alpha blend state
  GameRenderer.m_d3ddevice->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
}




/// Draws the sprite image at a given point in 3D space relative to the center of the screen.
/// \param p point in 3D space at which to draw the sprite
/// \param angle Angle to rotate sprite
/// \param frame frame number

void C3DSprite::Draw(D3DXVECTOR3 p, float angle, float scale, int frame, int facing){

  D3DXMATRIXA16 rotationmatrix, translationmatrix, scalematrix, tempmatrix; ///< Rotation and translation matrices.

  //set diffuse blending for alpha set in vertices
  GameRenderer.m_d3ddevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
  GameRenderer.m_d3ddevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
  GameRenderer.m_d3ddevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
  //GameRenderer.m_d3ddevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

  

  //set translation matrix
  //D3DXMatrixTranslation(&translationmatrix, p.x + g_nScreenWidth/2, p.y, p.z);

  //set rotation matrix  
  D3DXMatrixRotationZ(&rotationmatrix, angle);
  if(facing == 4)
  {
    
    D3DXMatrixTranslation(&translationmatrix, p.x + g_nScreenWidth/2, p.y, p.z);
    D3DXMatrixScaling(&scalematrix, scale, scale, 1.0f);
    D3DXMatrixMultiply(&tempmatrix, &scalematrix, &rotationmatrix);
    D3DXMatrixMultiply(&m_matWorld, &tempmatrix, &translationmatrix);
  }
  else if(facing == 6)
  {
    
    //D3DXMatrixScaling(&scalematrix, -scale, scale, 1.0f);
    GameRenderer.m_d3ddevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
    //scalematrix._41 = p.x*2;
    D3DXMatrixTranslation(&translationmatrix, p.x + g_nScreenWidth/2, p.y, p.z);
    D3DXMatrixScaling(&scalematrix, -scale, scale, 1.0f);
    /*scalematrix._11 = -scale;
    scalematrix._12 = 0;
    scalematrix._13 = 0;
    scalematrix._14 = 0;
    scalematrix._21 = 0;
    scalematrix._22 = scale;
    scalematrix._23 = 0;
    scalematrix._24 = 0;
    scalematrix._31 = 0;
    scalematrix._32 = 0;
    scalematrix._33 = 1.0f;
    scalematrix._34 = 0;
    scalematrix._41 = 0;
    scalematrix._42 = 0;
    scalematrix._43 = 0;
    scalematrix._44 = 1;*/
    D3DXMatrixMultiply(&tempmatrix, &scalematrix, &rotationmatrix);
    
    //tempmatrix._41 = 2*tempmatrix._41;// its adding this...
    D3DXMatrixMultiply(&m_matWorld, &tempmatrix, &translationmatrix);
    //BREAKINGGGG
  }
  /*D3DXMatrixMultiply(&tempmatrix, &scalematrix, &rotationmatrix);
  D3DXMatrixMultiply(&m_matWorld, &tempmatrix, &translationmatrix);*/
  GameRenderer.m_d3ddevice->SetTransform(D3DTS_WORLD, &m_matWorld);

  //set vertex buffer
  GameRenderer.m_d3ddevice->SetStreamSource(0, m_pVertexBuffer, 0, sizeof(BILLBOARDVERTEX));
  GameRenderer.m_d3ddevice->SetFVF(BILLBOARDVERTEX::FVF); //flexible vertex format

  //set texture
  if( !g_bWireFrame && frame>=0 && frame<m_nFrameCount && m_lpTexture[frame] ) //if ok
    GameRenderer.m_d3ddevice->SetTexture(0, m_lpTexture[frame]); //set sprite texture

  //draw sprite (finally)
  GameRenderer.m_d3ddevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

  //restore alpha blend state
  GameRenderer.m_d3ddevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
  GameRenderer.m_d3ddevice->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
}

void C3DSprite::DrawDetectionBox(D3DXVECTOR3 p, float scaleX, float scaleY, float xOffset, float yOffset){

  D3DXMATRIXA16 rotationmatrix, translationmatrix, scalematrix, tempmatrix; ///< Rotation and translation matrices.

  //set diffuse blending for alpha set in vertices
  GameRenderer.m_d3ddevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
  GameRenderer.m_d3ddevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
  GameRenderer.m_d3ddevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

  //set translation matrix
  D3DXMatrixTranslation(&translationmatrix, p.x + g_nScreenWidth/2 + xOffset, p.y + yOffset, p.z);

  //set rotation matrix  
  //D3DXMatrixRotationZ(&rotationmatrix, angle);
  D3DXMatrixScaling(&scalematrix, scaleX, scaleY, 1.0f);
  //D3DXMatrixMultiply(&tempmatrix, &scalematrix, &rotationmatrix);
  D3DXMatrixMultiply(&m_matWorld, &scalematrix, &translationmatrix);
  GameRenderer.m_d3ddevice->SetTransform(D3DTS_WORLD, &m_matWorld);

  //set vertex buffer
  GameRenderer.m_d3ddevice->SetStreamSource(0, m_pVertexBuffer, 0, sizeof(BILLBOARDVERTEX));
  GameRenderer.m_d3ddevice->SetFVF(BILLBOARDVERTEX::FVF); //flexible vertex format

  //set texture
  if( !g_bWireFrame && m_lpTexture[0] ) //if ok
    GameRenderer.m_d3ddevice->SetTexture(0, m_lpTexture[0]); //set sprite texture

  //draw sprite (finally)
  GameRenderer.m_d3ddevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

  //restore alpha blend state
  GameRenderer.m_d3ddevice->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
}

void C3DSprite::Release(){ //release sprite
  if(m_pVertexBuffer)m_pVertexBuffer->Release(); //release vertex buffer
  for(int i=0; i<m_nFrameCount; i++) //for each frame
    if(m_lpTexture[i]) m_lpTexture[i]->Release(); //release texture
}

