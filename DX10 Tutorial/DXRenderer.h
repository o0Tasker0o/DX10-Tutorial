#ifndef DXRENDERER_H
#define DXRENDERER_H

//Platform includes
#include <Windows.h>

//Graphics library includes
#include <d3d10.h>
#include <d3dx10.h>

class DXRenderer
{
  public:
	DXRenderer(void);
	~DXRenderer(void);
	int Initialise(HWND hwnd, unsigned int width, unsigned int height);
	int Resize(unsigned int width, unsigned int height);
	void Render(void);
  private:
    ID3D10Device *m_pD3DDevice;
    IDXGISwapChain *m_pSwapChain;
    ID3D10RenderTargetView *m_pRenderTargetView;
	unsigned int m_width, m_height;
};

#endif