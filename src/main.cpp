#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <stdio.h>

#include "hook.hpp"
#include "imgui/imgui.h"

static void menu_render() {
  ImGui::Begin("Menu", &Hook::showMenu);
  ImGui::Text("Hello, world!");
  ImGui::End();
}

static DWORD WINAPI MainThread(LPVOID lpReserved) {
  Hook::allocate_console();
  Hook::hook();

  while (!GetAsyncKeyState(VK_PRIOR))
    if (GetAsyncKeyState(VK_INSERT) & 1) Hook::showMenu = !Hook::showMenu;

  Hook::unhook();

  return TRUE;
}

static BOOLEAN WINAPI DllMain(IN HMODULE hMod, IN DWORD dwReason,
                              IN LPVOID lpReserved) {
  Hook::setup(menu_render, hMod);

  switch (dwReason) {
    case DLL_PROCESS_ATTACH:
      DisableThreadLibraryCalls(hMod);
      CreateThread(nullptr, 0, MainThread, hMod, 0, nullptr);
      break;

    case DLL_PROCESS_DETACH:
      SetWindowLongPtr(Hook::window, GWLP_WNDPROC, (LONG_PTR)Hook::oWndProc);
      kiero::shutdown();
      break;
  }

  return TRUE;
}
