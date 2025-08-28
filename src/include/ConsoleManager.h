#pragma once

#include <stdio.h>

#include <Windows.h>

#include "Manager.h"

class ConsoleManager : public Manager {
public:

	ConsoleManager(Prism*);

	void Init();

private:

};