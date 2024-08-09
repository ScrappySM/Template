#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <d3d11.h>
#include <dxgi.h>
#include <string>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"
#include "kiero/kiero.h"

typedef HRESULT(__stdcall *Present)(IDXGISwapChain *pSwapChain, UINT SyncInterval, UINT Flags);
typedef LRESULT(CALLBACK *WNDPROC)(HWND, UINT, WPARAM, LPARAM);
typedef uintptr_t PTR;

// Hook in WndProc
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace Hook {
Present oPresent;
HWND window = NULL;
WNDPROC oWndProc;
ID3D11Device *pDevice = NULL;
ID3D11DeviceContext *pContext = NULL;
ID3D11RenderTargetView *mainRenderTargetView;
bool hasInitiated = false;
bool showMenu = true;
HMODULE hMod = NULL;

int consoleResult = 0;
FILE *consoleFile = nullptr;

void (*menuRenderFunction)() = nullptr;

void allocate_console() {
  consoleResult = AllocConsole();
  freopen_s(&consoleFile, "CONOUT$", "w", stdout);
  if (consoleResult == 0)
    printf("[TEMPLATE] Not starting new console, code %d\n", consoleResult);
}

void setup(void (*menuRenderFunction)(), HMODULE hMod) {
  Hook::hMod = hMod;
  Hook::menuRenderFunction = menuRenderFunction;
}

static LRESULT WINAPI WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam,
                              LPARAM lParam) {
  if (!ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam) &&
      Hook::showMenu) {
    SetCursor(LoadCursor(NULL, IDC_ARROW));
    return true;
  }

  return CallWindowProc(Hook::oWndProc, hWnd, uMsg, wParam, lParam);
}

static HRESULT __stdcall hkPresent(IDXGISwapChain *pSwapChain,
                                   UINT SyncInterval, UINT Flags) {
  if (!hasInitiated) {
    if (SUCCEEDED(
            pSwapChain->GetDevice(__uuidof(ID3D11Device), (void **)&pDevice))) {
      pDevice->GetImmediateContext(&pContext);
      DXGI_SWAP_CHAIN_DESC sd;
      pSwapChain->GetDesc(&sd);
      window = sd.OutputWindow;
      ID3D11Texture2D *pBackBuffer = nullptr;
      pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
                            (LPVOID *)&pBackBuffer);
      if (pBackBuffer != 0)
        pDevice->CreateRenderTargetView(pBackBuffer, NULL,
                                        &mainRenderTargetView);
      pBackBuffer->Release();
      oWndProc =
          (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)WndProc);

      ImGui::CreateContext();
      ImGuiIO &io = ImGui::GetIO();
      io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
      ImFont *font =
          io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Arial.ttf", 16);
      ImGui_ImplWin32_Init(window);
      ImGui_ImplDX11_Init(pDevice, pContext);

      hasInitiated = true;
    } else {
      return oPresent(pSwapChain, SyncInterval, Flags);
    }
  }

  ImGui_ImplDX11_NewFrame();
  ImGui_ImplWin32_NewFrame();
  ImGui::NewFrame();

  if (showMenu)
    if (menuRenderFunction != nullptr)
      menuRenderFunction();

  ImGui::Render();

  pContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
  ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
  return oPresent(pSwapChain, SyncInterval, Flags);
}

void unhook() {
  SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)oWndProc);
  kiero::shutdown();

  printf("[TEMPLATE] Releasing console\n");

  if (consoleFile)
    fclose(consoleFile);
  if (consoleResult)
    FreeConsole();

  FreeLibraryAndExitThread(hMod, 0);
}

void hook() {
  if (kiero::init(kiero::RenderType::D3D11) == kiero::Status::Success) {
    kiero::bind(8, (void **)&oPresent, hkPresent);
    printf("[TEMPLATE] Hooked D3D11 Present\n");
  } else {
    printf("[TEMPLATE] (ERROR) Failed to hook D3D11 Present\n");
  }
}
} // namespace Hook
