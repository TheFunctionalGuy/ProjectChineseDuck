#include "gui.h"
#include "vector.h"

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
static WNDCLASSEX              window_class;

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND window_handle);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WindowProcedure(HWND window, UINT message, WPARAM w_param, LPARAM l_param);

// Colors
static ImVec4 clear_color = ImVec4(1.00f, 0.498f, 0.314f, 1.00f); // Coral
// static ImVec4 clear_color = ImVec4(0.973f, 0.514f, 0.475f); // Colal pink

// Constants
const float kAliveColumnWidth = 50.0f;
const float kNameColumnWidth = 125.0f;
const float kHealthColumnWidth = 55.0f;
const float kArmorColumnWidth = 55.0f;
const float kPositionColumnWidth = 220.0f;
const float kDistanceColumnWidth = 75.0f;
const float kAngleColumnWidth = 200.0f;

const float kTotalWidth = kAliveColumnWidth + kNameColumnWidth + kHealthColumnWidth + kArmorColumnWidth + kPositionColumnWidth + kDistanceColumnWidth + 2.0f * kAngleColumnWidth;

HWND gui::InitGui() {
	// Create application window
	window_class = { sizeof(WNDCLASSEX), CS_CLASSDC, WindowProcedure, 0, 0, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("ProjectChineseDuck"), NULL };
	RegisterClassEx(&window_class);
	HWND window_handle = CreateWindow(window_class.lpszClassName, _T("Project Chinese Duck v1.0"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 1200, 500, NULL, NULL, window_class.hInstance, NULL);

	// Initialize Direct3D
	if (!CreateDeviceD3D(window_handle)) {
		CleanupDeviceD3D();
		UnregisterClass(window_class.lpszClassName, window_class.hInstance);

		std::cout << "Direct3D device already busy!" << std::endl;

		return NULL;
	}

	// Show the window
	ShowWindow(window_handle, SW_SHOWDEFAULT);
	UpdateWindow(window_handle);

	// Setup ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	//ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(window_handle);
	ImGui_ImplDX11_Init(d3d_device, d3d_device_context);

	return window_handle;
}

void gui::Render() {
	ImGui::Render();
	d3d_device_context->OMSetRenderTargets(1, &main_render_target_view, NULL);
	d3d_device_context->ClearRenderTargetView(main_render_target_view, (float*)&clear_color);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	swap_chain->Present(1, 0); // Present with vsync
}

void gui::StartFrame() {
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void gui::Cleanup(HWND window_handle) {
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	CleanupDeviceD3D();
	DestroyWindow(window_handle);
	UnregisterClass(window_class.lpszClassName, window_class.hInstance);
}

// TODO: Print local player position and angles
//std::cout << "Player position:" << std::endl;
//std::cout << vec::ToString(local_player.position) << std::endl;
void gui::ShowLocalPlayerInformation(const PlayerEntity player) {
	ImGui::Begin("Local player information");

	ImGui::Columns(2, "local_player_columns");
	ImGui::Separator();
	ImGui::Text("Name"); ImGui::NextColumn();
	ImGui::Text(player.name); ImGui::NextColumn();
	ImGui::Separator();
	ImGui::Text("Health"); ImGui::NextColumn();
	ImGui::Text("%d" ,player.health); ImGui::NextColumn();
	ImGui::Separator();
	ImGui::Text("Armor"); ImGui::NextColumn();
	ImGui::Text("%d", player.armor); ImGui::NextColumn();
	ImGui::Separator();
	ImGui::Text("Yaw"); ImGui::NextColumn();
	ImGui::Text("%f", player.angles[0]); ImGui::NextColumn();
	ImGui::Separator();
	ImGui::Text("Pitch"); ImGui::NextColumn();
	ImGui::Text("%f", player.angles[1]); ImGui::NextColumn();
	ImGui::Columns(1);
	ImGui::Separator();
	
	ImGui::End();
}

void gui::ShowPlayerInformation(const std::vector<ExtendedPlayerEntity> players, const float fov) {
	ImGui::SetNextWindowSizeConstraints({ kTotalWidth, -1.0f }, { kTotalWidth, -1.0f });
	ImGui::SetNextWindowPos({ 2.0f, 2.0f });
	ImGui::Begin("Player information", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);

	if (players.size() > 0) {
		ImGui::Columns(8, "player_columns");
		ImGui::SetColumnWidth(0, kAliveColumnWidth);
		ImGui::SetColumnWidth(1, kNameColumnWidth);
		ImGui::SetColumnWidth(2, kHealthColumnWidth);
		ImGui::SetColumnWidth(3, kArmorColumnWidth);
		ImGui::SetColumnWidth(4, kPositionColumnWidth);
		ImGui::SetColumnWidth(5, kDistanceColumnWidth);
		ImGui::SetColumnWidth(6, kAngleColumnWidth);
		ImGui::SetColumnWidth(7, kAngleColumnWidth);
		ImGui::Separator();
		ImGui::Text("Alive"); ImGui::NextColumn();
		ImGui::Text("Name"); ImGui::NextColumn();
		ImGui::Text("Health"); ImGui::NextColumn();
		ImGui::Text("Armor"); ImGui::NextColumn();
		ImGui::Text("Position"); ImGui::NextColumn();
		ImGui::Text("Distance"); ImGui::NextColumn();
		ImGui::Text("Angles from local player"); ImGui::NextColumn();
		ImGui::Text("Angle differences"); ImGui::NextColumn();
		ImGui::Separator();

		for (const auto& ent : players) {
			ImGui::Text("%d", !ent.player.is_dead); ImGui::NextColumn();
			ImGui::Text(ent.player.name); ImGui::NextColumn();
			ImGui::Text("%d", ent.player.health); ImGui::NextColumn();
			ImGui::Text("%d", ent.player.armor); ImGui::NextColumn();
			ImGui::Text("X: %6.2f Y: %6.2f Z: %6.2f", ent.player.position[0], ent.player.position[1], ent.player.position[2]); ImGui::NextColumn();
			ImGui::Text("%6.2f m", ent.distance_to_local_player); ImGui::NextColumn();
			ImGui::Text("Yaw: %6.2f Pitch: %6.2f", ent.angles_from_local_player[0], ent.angles_from_local_player[1]); ImGui::NextColumn();

			if (ent.fov_diff[0] < fov / 2.0f && ent.fov_diff[1] < fov / 2.0f) {
				ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "Yaw: %6.2f Pitch: %6.2f", ent.fov_diff[0], ent.fov_diff[1]); ImGui::NextColumn();
			} else {
				ImGui::Text("Yaw: %6.2f Pitch: %6.2f", ent.fov_diff[0], ent.fov_diff[1]); ImGui::NextColumn();
			}
		}
		ImGui::Columns(1);
		ImGui::Separator();
	} else {
		ImGui::Text("No other players on server.");
	}

	ImGui::End();
}

void gui::ShowExtraPlayerInformation() {
	ImGui::Columns();
}

void gui::ShowOptions(Settings* settings) {
	ImGui::SetNextWindowPos({ kTotalWidth + 4.0f, 2.0f });
	ImGui::SetNextWindowSizeConstraints({ 198.0f, -1.0f }, { 198.0f, -1.0f });
	ImGui::Begin("Options", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);

	ImGui::SliderFloat("FOV", &settings->fov, 0.0f, 360.0f, "%0.f deg");
	ImGui::Checkbox("Aim for head", &settings->aim_for_head);
	ImGui::Checkbox("Ignore team", &settings->ignore_teams);
	ImGui::Checkbox("No recoil", &settings->no_recoil);
	ImGui::Checkbox("Unlimited ammo", &settings->unlimited_ammo);
	ImGui::Checkbox("Unlimited health", &settings->unlimited_healh);
	ImGui::Checkbox("Unlimited armor", &settings->unlimited_armor);

	ImGui::End();
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
		case WM_SIZE:
			if (d3d_device != NULL && w_param != SIZE_MINIMIZED) {
				CleanupRenderTarget();
				swap_chain->ResizeBuffers(0, (UINT)LOWORD(l_param), (UINT)HIWORD(l_param), DXGI_FORMAT_UNKNOWN, 0);
				CreateRenderTarget();
			}

			return 0;
		case WM_DESTROY:
			PostQuitMessage(0);

			return 0;
	}

	return DefWindowProc(window, message, w_param, l_param);
}