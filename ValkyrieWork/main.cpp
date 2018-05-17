#include "res.h"
#include "valkAPI.h"
#include "memory.h"
#include "globals.h"
#include <dwmapi.h>
#include <d3d9.h>

//signal close between threads
static std::atomic_bool memThreadState;
static std::atomic_bool mainThreadState;

//TEMPORARY!!
using namespace valkyrie;

typedef std::tuple<HWND, LPDIRECT3D9, LPDIRECT3DDEVICE9> WindowTuple_t;

template<typename T>
using DXObject = std::unique_ptr<std::remove_pointer_t<T>, void(*)(T*)>;

template<typename T>
static auto ReleaseDXObject(T* dxObject) -> void
{
	dxObject->Release();
}

static auto createD3DDevice(HWND window, LPDIRECT3D9 directX) -> LPDIRECT3DDEVICE9
{
	if (directX == nullptr)
	{
		PRINTLN_LOG("Failed to initialize DirectX context");
		return nullptr;
	}

	LPDIRECT3DDEVICE9 dxDevice = nullptr;
	D3DPRESENT_PARAMETERS d3dPresent = { 0 };

	d3dPresent.Windowed = TRUE;
	d3dPresent.hDeviceWindow = window;
	d3dPresent.MultiSampleQuality = D3DMULTISAMPLE_NONE;
	d3dPresent.BackBufferFormat = D3DFMT_A8R8G8B8;
	d3dPresent.BackBufferWidth = globals.screenWidth;
	d3dPresent.BackBufferHeight = globals.screenHeight;
	d3dPresent.SwapEffect = D3DSWAPEFFECT_DISCARD;

	d3dPresent.AutoDepthStencilFormat = D3DFMT_D16;
	d3dPresent.EnableAutoDepthStencil = TRUE;

	directX->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, window, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dPresent, &dxDevice);

	if (dxDevice == nullptr)
	{
		PRINTLN_LOG("Failed to initialize d3d device");
		directX->Release();
		return nullptr;
	}

	return dxDevice;
}

static auto createD3DWindow(string const& title) -> WindowTuple_t
{
	constexpr auto windowFlags = WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_COMPOSITED | WS_EX_TRANSPARENT;
	
	const MARGINS margin = { 0, 0, globals.screenWidth, globals.screenHeight };
	const auto WndProc = [](auto hwnd, auto message, auto wParam, auto lParam) -> LRESULT
	{
		switch (message)
		{
		case WM_CLOSE:
			DestroyWindow(hwnd);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hwnd, message, wParam, lParam);
		}

		return 0;
	};

	WNDCLASSEX wcex = { 0 };
	HWND hwnd = nullptr;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpszClassName = TEXT("D3DClass");
	wcex.lpfnWndProc = WndProc;
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);

	if (!RegisterClassEx(&wcex))
	{
		return WindowTuple_t(nullptr, nullptr, nullptr);
	}

	//TODO: randomstring
	hwnd = CreateWindowEx(windowFlags, wcex.lpszClassName, title.c_str(), WS_POPUP, CW_USEDEFAULT, CW_USEDEFAULT,
		globals.screenWidth, globals.screenHeight, nullptr, nullptr, nullptr, nullptr);

	SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 255, LWA_ALPHA);
	ShowWindow(hwnd, SW_SHOW);
	DwmExtendFrameIntoClientArea(hwnd, &margin);

	//TODO: CALL inputthread and setwindowshookexa
	//TODO: production

	LPDIRECT3D9 directX = Direct3DCreate9(D3D_SDK_VERSION);
	LPDIRECT3DDEVICE9 dxDevice = createD3DDevice(hwnd, directX);
	if (dxDevice == nullptr)
	{
		CloseWindow(hwnd);
		CloseHandle(hwnd);
		return WindowTuple_t(nullptr, nullptr, nullptr);
	}

	return WindowTuple_t(hwnd, directX, dxDevice);
}

//returns success status of PPL
//TODO: me
static auto pplifyMe() -> bool 
{
	return true;
}

static auto renderFrame(DXObject<LPDIRECT3DDEVICE9> const& device) -> void
{
	device->Clear(0, nullptr, D3DCLEAR_TARGET, 0, 1.0f, 0);

	if (SUCCEEDED(device->BeginScene()))
	{

		device->EndScene();
	}

	device->Present(nullptr, nullptr, nullptr, nullptr);
}

static auto manageCSGOMemory() -> void
{
	auto const waitForGame = []() -> const DWORD
	{
		while (!ProcessMgr32::checkProcessExists("csgo.exe", true))
		{
			std::this_thread::sleep_for(std::chrono::seconds(5));
		}
		
		return ProcessMgr32::getProcessIDList(false)["csgo.exe"];
	};
	
	const DWORD csgoPID = waitForGame();
	if (!csgoProc.openProcessById(csgoPID))
	{
		mainThreadState = false;
		return;
	}

	while (memThreadState.load() && csgoProc.isProcessRunning())
	{
		
	}
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	//don't do if csgo exists
	if (ProcessMgr32::checkProcessExists("csgo.exe", true))
	{
		PRINT_LOG("CSGO is running");
		return -1;
	}

	globals.screenWidth = GetSystemMetrics(SM_CXSCREEN);
	globals.screenHeight = GetSystemMetrics(SM_CYSCREEN);

	//TODO: PASS ME RANDOMIZED STRING
	auto const& windowData = createD3DWindow("TEMP");

	if (std::get<0>(windowData) == nullptr)
	{
		PRINT_LOG("Failed to create window");
		return -2;
	}

	//init window
	WindowsHandle hwnd = WindowsHandle(std::get<0>(windowData), CloseHandle);
	auto directX = DXObject<LPDIRECT3D9>(std::get<1>(windowData), ReleaseDXObject<LPDIRECT3D9>);
	auto dxDevice = DXObject<LPDIRECT3DDEVICE9>(std::get<2>(windowData), ReleaseDXObject<LPDIRECT3DDEVICE9>);

	if (!pplifyMe())
	{
		//if NTSTATUS is STATUS_INVALID_IMAGE_HASH, direct them to disable secure boot on msdn
		//otherwise tell them to contact devs, something is wrong with capcom
		PRINT_LOG("Failed to PPL process");
		return -3;
	}

	memThreadState = true;
	mainThreadState = true;
	thread memThread = thread(manageCSGOMemory);

	const auto messageLoop = [&dxDevice]() -> void
	{
		MSG msg = { 0 };
		while (mainThreadState.load() && msg.message != WM_QUIT)
		{
			if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				renderFrame(dxDevice);
			}
		}
	};

	messageLoop();

	mainThreadState = false;
	memThreadState = false;

	//ensure our memory thread exited cleanly
	memThread.join();

	//OK exit
	return 0;
}