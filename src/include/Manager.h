#pragma once

#include "Prism.h"

class Manager {
public:

	virtual ~Manager() = 0;
	virtual void Init() = 0;

protected:

	Prism* mPrism{ nullptr };

};