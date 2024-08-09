# DirectX11 hook with ImGui and Kiero

This is a template for building your own overlays with ImGui, Kiero and DirectX.

## Usage
1. Sign in if you are not already
1. Press "Use this template" button on the top right of the page
2. Press "Create a new repository" in the drop-down
3. Enter a repository name and press "Create repository"
4. Download the repository to your computer, using either Git or pressing Code > Download ZIP
5. Run `rename.bat` and change the project name to your desired name
6. Open the project in Visual Studio (2022 recommended)
7. Build the project solution
8. Inject the DLL into the target process using your favourite injector

## Notes:
- `VK_INS`/Insert to toggle the overlay
- `VK_PRIOR`/Page Up to uninstall the hook

## Process flow:
1. DLL is injected into the target process and `DllMain` is called with `DLL_PROCESS_ATTACH`
2. The `Hook` utility is setup so that the render function is assigned to it along with the `HMODULE`
3. `DllMain` creates a new thread with the function `MainThread` as the entry point and detaches from the DLL
4. `MainThread` uses the `Hook` utility to hook DirectX 11 functions and initializes ImGui and Kiero
5. This then sits in a while loop, rendering the overlay until the user presses `VK_PRIOR`/Page Up
6. Once this while loop terminates, the hook is uninstalled and the thread exits
