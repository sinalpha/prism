#include "Prism.h"

bool Prism::Initialize(HINSTANCE pHInstance) {

    mWindowController.Init(pHInstance);
    mConsoleController.Init();
	mD3DController.Init(this);

	mCurrentScene = new Scene();
	mCurrentScene->Init(this);

	return true;
    
}

int Prism::Run() {


    MSG msg = { 0 };
    
    while (msg.message != WM_QUIT)
    {

        if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
        {

            TranslateMessage(&msg);
            DispatchMessage(&msg);

        }
        else {

            mCurrentScene->Update();
            mD3DController.Render();

        }
    }

    return (int)msg.wParam;

}

