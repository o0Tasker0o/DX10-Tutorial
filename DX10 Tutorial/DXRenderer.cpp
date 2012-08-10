#include "DXRenderer.h"

DXRenderer::DXRenderer(void) : m_pD3DDevice(NULL),
							   m_pSwapChain(NULL),
							   m_pRenderTargetView(NULL),
							   m_width(1),
							   m_height(1)
{
}

DXRenderer::~DXRenderer(void)
{
	if(m_pD3DDevice)
	{
		m_pD3DDevice->Release();
	}
  
	if(m_pSwapChain)
	{
		m_pSwapChain->Release();
	}
  
	if(m_pRenderTargetView)
	{
		m_pRenderTargetView->Release();
	}
}

int DXRenderer::Initialise(HWND hwnd, unsigned int width, unsigned int height)
{
	//Create a description of the swap chain
	DXGI_SWAP_CHAIN_DESC swapDesc;

	//Initialise the swap chain's description to some 'sensible' values
	ZeroMemory(&swapDesc, sizeof(swapDesc));

	swapDesc.BufferCount = 2;								//Use double buffering
	swapDesc.BufferDesc.Width = width;						//Set swap chain to use window's width
	swapDesc.BufferDesc.Height = height;					//Set swap chain to use window's height
	swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;//Display graphics using 32bit RGBA colour
	swapDesc.BufferDesc.RefreshRate.Numerator = 60;			//Display at 60 frames/second
	swapDesc.BufferDesc.RefreshRate.Denominator = 1;		//
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	//Use this swap chain to render to
	swapDesc.OutputWindow = hwnd;							//The window that we're rendering to
	swapDesc.SampleDesc.Count = 1;							//Only one sample per pixel
	swapDesc.SampleDesc.Quality = 0;						//Ignored as we are not using multisampling
	swapDesc.Windowed = TRUE;								//Not drawing fullscreen

	//For now our application is single threaded, so use this flag to create the device
	unsigned int deviceFlags(D3D10_CREATE_DEVICE_SINGLETHREADED);

	//If you're creating a debug build
	#ifdef _DEBUG
	//Create a debug device
	deviceFlags |= D3D10_CREATE_DEVICE_DEBUG;
	#endif
  
	//Create a list of driver types
	D3D10_DRIVER_TYPE pDriverTypes[] = { D3D10_DRIVER_TYPE_HARDWARE,
										 D3D10_DRIVER_TYPE_REFERENCE,
										 D3D10_DRIVER_TYPE_NULL,
										 D3D10_DRIVER_TYPE_WARP };

	//Count how many driver types were listed
	unsigned int numDriverTypes(sizeof(pDriverTypes) / sizeof(D3D10_DRIVER_TYPE));
  
	//This will store the result of attempts to create devices and swap chains
	HRESULT hr(S_OK);

	for(unsigned int driverIndex(0); driverIndex < numDriverTypes; ++driverIndex)
	{
		//Attempt to create a device and swapchain based on the current configuration
		hr = D3D10CreateDeviceAndSwapChain(NULL,			//
										   pDriverTypes[driverIndex],//Try to use this driver type
										   NULL,			//We are not using our own software rasterizer so this is NULL
										   deviceFlags,		//Use the flags we set before
										   D3D10_SDK_VERSION,//Which version of the SDK to use
										   &swapDesc,		//Description of swap chain you want to create
										   &m_pSwapChain,	//Swapchain to be created
										   &m_pD3DDevice);	//Device to be created

		//If that worked
		if(SUCCEEDED(hr))
		{
			//don't try with any other driver types
			break;
		}
	}

	//If no driver types were successfully created
	if(FAILED(hr))
	{
		//Exit initialisation
		return -1;
	}

	//Resize the 
	if(0 != Resize(width, height))
	{
		return -1;
	}
  
	//Create a rasterizer state description
	D3D10_RASTERIZER_DESC rasterStateDesc;

	rasterStateDesc.FillMode = D3D10_FILL_SOLID;	//Fill in the polygons
	rasterStateDesc.CullMode = D3D10_CULL_NONE;		//Don't cull any faces
	rasterStateDesc.FrontCounterClockwise = false;	//CCW winding for front polygon
	rasterStateDesc.DepthBias = 0;					//Don't add any value to a pixel's depth
	rasterStateDesc.DepthBiasClamp = 0.0f;			//
	rasterStateDesc.SlopeScaledDepthBias = 0.0f;	//
	rasterStateDesc.DepthClipEnable = true;			//
	rasterStateDesc.ScissorEnable = false;			//
	rasterStateDesc.MultisampleEnable = false;		//Don't use multisampling
	rasterStateDesc.AntialiasedLineEnable = false;	//Don't anti-alias lines
	
	ID3D10RasterizerState *pRasterState(NULL);

	//Create and set a rasterizer state based on the rasterizer description
	hr = m_pD3DDevice->CreateRasterizerState(&rasterStateDesc, &pRasterState);

	//If creation of the rasterizer state failed
	if(FAILED(hr))
	{
		//Exit initialisation
		return -1;
	}

	m_pD3DDevice->RSSetState(pRasterState);
  
	//Everything went fine
	return 0;
}

int DXRenderer::Resize(unsigned int width, unsigned int height)
{
	//If there is no created device
	if(!m_pD3DDevice)
	{
		return -1;
	}

	//A window needs some area
	if(width == 0 || height == 0)
	{
		return -1;
	}
  
	//Store the given window dimensions
	m_width = width;
	m_height = height;

	//If there is a valid render target
	if(m_pRenderTargetView)
	{
		m_pRenderTargetView->Release();
	}
    
	//Resize the swap chain's buffer to the given dimensions
	m_pSwapChain->ResizeBuffers(2, 
								m_width, m_height, 
								DXGI_FORMAT_R8G8B8A8_UNORM, 
								DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
        
	ID3D10Texture2D *pBufferTexture(NULL);

	//Get the swap chain's primary backbuffer (index 0)
	HRESULT hr(m_pSwapChain->GetBuffer(0,
									   __uuidof(ID3D10Texture2D),
									   (LPVOID*) &pBufferTexture));

	//If no buffer was retrieved
	if(FAILED(hr))
	{
		return -1;
	}

	//Create a new render target view using this backbuffer
	hr = m_pD3DDevice->CreateRenderTargetView(pBufferTexture, 
											  NULL,
											  &m_pRenderTargetView);

	//Release the reference to the backbuffer
	pBufferTexture->Release();

	//If you failed to create a render target view
	if(FAILED(hr))
	{
		return -1;
	}
  
	//Create a definition of the viewport
	D3D10_VIEWPORT viewPort;

	viewPort.Width = m_width;	//
	viewPort.Height = m_height;	//
	viewPort.MinDepth = 0.0f;	//
	viewPort.MaxDepth = 1.0f;	//
	viewPort.TopLeftX = 0;		//
	viewPort.TopLeftY = 0;		//

	//Set the device's viewport
	m_pD3DDevice->RSSetViewports(1, &viewPort);
  
	//Set the device's render target to the one just created
	m_pD3DDevice->OMSetRenderTargets(1, &m_pRenderTargetView, NULL);

	return 0;
}

void DXRenderer::Render(void)
{
	//The colour that the background will be cleared to
	float pBackgroundColour[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
  
	//Clear the rendering destination to a specified color.
	m_pD3DDevice->ClearRenderTargetView(m_pRenderTargetView, pBackgroundColour);

	//Display the results to the target window (swap chain).
	m_pSwapChain->Present(0, 0);
}