﻿#include "systemclass.h"

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow )
{
	SystemClass* system { new SystemClass };
	bool result;

	if ( !system )
	{
		return 0;
	}

	result = system->Initialize();
	if ( result )
	{
		system->Run();
	}

	system->Shutdown();
	delete( system );
	system = 0;

	return 0;
}