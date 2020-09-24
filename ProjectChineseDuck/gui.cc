#include "gui.h"
#include <windows.h>
#include <iostream>
#include <tchar.h>

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_impl_dx11.h"

#include <d3d11.h>
//#define DIRECTINPUT_VERSION 0x0800
//#include <dinput.h>

// Data
static ID3D11Device*           d3d_device = NULL;
static ID3D11DeviceContext*    d3d_device_context = NULL;
static IDXGISwapChain*         swap_chain = NULL;
static ID3D11RenderTargetView* main_render_target_view = NULL;

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND window_handle);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WindowProcedure(HWND window, UINT message, WPARAM w_param, LPARAM l_param);

bool gui::InitGui() {
	// Create application window
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WindowProcedure, 0, 0, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("ProjectChineseDuck"), NULL };
	RegisterClassEx(&wc);
	HWND window_handle = CreateWindow(wc.lpszClassName, _T("Project Chinese Duck v1.0"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, 1000, 500, NULL, NULL, wc.hInstance, NULL);

	// Initialize Direct3D
	if (!CreateDeviceD3D(window_handle)) {
		CleanupDeviceD3D();
		UnregisterClass(wc.lpszClassName, wc.hInstance);

		std::cout << "Direct3D device already busy!" << std::endl;

		return 1;
	}

	// Show the window
	ShowWindow(window_handle, SW_SHOWDEFAULT);

	// Setup ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(window_handle);
	ImGui_ImplDX11_Init(d3d_device, d3d_device_context);

	// State
	ImVec4 clear_color = ImVec4(1.00f, 0.498f, 0.314f, 1.00f); // Coral
	//ImVec4 clear_color = ImVec4(0.973f, 0.514f, 0.475f); // Colal pink

	MSG message = {};
	while (GetMessage(&message, NULL, 0, 0)) {
		TranslateMessage(&message);
		DispatchMessage(&message);

		// Start frame
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		// Render
		ImGui::Render();
		d3d_device_context->OMSetRenderTargets(1, &main_render_target_view, NULL);
		d3d_device_context->ClearRenderTargetView(main_render_target_view, (float*) &clear_color);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		swap_chain->Present(1, 0); // Present with vsync
	}

	//ImGuiIO& io = ImGui::GetIO(); (void)io;

	return true;
}

// Helper functions
bool CreateDeviceD3D(HWND window_handle) {
	// Setup swap chain
	DXGI_SWAP_CHAIN_DESC swap_chain_description;
	ZeroMemory(&swap_chain_description, sizeof(swap_chain_description));

	swap_chain_description.BufferCount = 2;

	swap_chain_description.BufferDesc.Width = 0;
	swap_chain_description.BufferDesc.Height = 0;
	swap_chain_description.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swap_chain_description.BufferDesc.RefreshRate.Numerator = 60;
	swap_chain_description.BufferDesc.RefreshRate.Denominator = 1;

	swap_chain_description.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	swap_chain_description.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swap_chain_description.OutputWindow = window_handle;
	swap_chain_description.SampleDesc.Count = 1;
	swap_chain_description.SampleDesc.Quality = 0;
	swap_chain_description.Windowed = TRUE;
	swap_chain_description.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	UINT create_device_flags = 0;

	D3D_FEATURE_LEVEL feature_level;
	const D3D_FEATURE_LEVEL feature_level_array[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };

	if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, create_device_flags, feature_level_array, 2, D3D11_SDK_VERSION, &swap_chain_description, &swap_chain, &d3d_device, &feature_level, &d3d_device_context) != S_OK)
		return false;
	
	CreateRenderTarget();

	return true;
}

void CleanupDeviceD3D() {
	CleanupRenderTarget();

	if (swap_chain) {
		swap_chain->Release();
		swap_chain = NULL;
	}

	if (d3d_device_context) {
		d3d_device_context->Release();
		d3d_device_context = NULL;
	}

	if (d3d_device) {
		d3d_device->Release();
		d3d_device = NULL;
	}
}

void CreateRenderTarget() {
	ID3D11Texture2D* back_buffer;
	swap_chain->GetBuffer(0, IID_PPV_ARGS(&back_buffer));
	d3d_device->CreateRenderTargetView(back_buffer, NULL, &main_render_target_view);
	back_buffer->Release();
}

void CleanupRenderTarget() {
	if (main_render_target_view) {
		main_render_target_view->Release();
		main_render_target_view = NULL;
	}
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND window, UINT message, WPARAM w_param, LPARAM l_param);

// Win 32 message handler
LRESULT WINAPI WindowProcedure(HWND window, UINT message, WPARAM w_param, LPARAM l_param) {
	if (ImGui_ImplWin32_WndProcHandler(window, message, w_param, l_param))
		return true;

	switch (message) {
	case WM_DESTROY:
		PostQuitMessage(0);

		return 0;
	}

	return DefWindowProc(window, message, w_param, l_param);
}