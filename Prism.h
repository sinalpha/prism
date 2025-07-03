#pragma once

#include <Windows.h>

class Prism {
public:
	
	Prism(HINSTANCE pHInstance) : hInstance{ pHInstance } {};
	Prism(const Prism&) = delete;
	Prism& operator=(const Prism&) = delete;
	~Prism();

	bool initialize();

private:
	


	HINSTANCE hInstance;


};