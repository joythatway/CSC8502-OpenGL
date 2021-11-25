#include "../NCLGL/window.h"
#include "Renderer.h"

int main()	{
	Window w("Make your own project!", 1280, 720, false);

	if(!w.HasInitialised()) {
		return -1;
	}
	//
	Renderer renderer(w);
	if(!renderer.HasInitialised()) {
		return -1;
	}

	w.LockMouseToWindow(true);
	w.ShowOSPointer(false);

	while(w.UpdateWindow()  && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)){
		renderer.UpdateScene(w.GetTimer()->GetTimeDeltaSeconds());
		renderer.RenderScene();
		renderer.SwapBuffers();
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_F5)) {
			Shader::ReloadAllShaders();
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_2)) {
			//
		}if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_3)) {
			//mutip light
			renderer.deferredrendering = !renderer.deferredrendering;
		}if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_4)) {
			//
			renderer.changecamera = !renderer.changecamera;
		}if (Window::GetKeyboard()->KeyDown(KEYBOARD_5)) {
			//
		}
	}
	return 0;
}