#include <Windows.h>

#include "Prism.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
    PSTR cmdLine, int showCmd)
{

	Prism prism{ hInstance };
	if (!prism.initialize())
		return 0;


	

}