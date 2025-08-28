#include <Windows.h>

#include "Prism.h"

int WINAPI WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE prevInstance,
	_In_ PSTR cmdLine,
	_In_ int showCmd)
{

	Prism prism;

	if (!prism.Initialize(hInstance))
		return 0;

	return prism.Run();

}