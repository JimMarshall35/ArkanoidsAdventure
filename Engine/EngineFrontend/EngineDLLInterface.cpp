#include "EngineDLLInterface.h"
#include <windows.h> 
#include "IBackendApp.h"

#define MS_PER_UPDATE 20

namespace Engine
{
    BackendAPI gBackendAPI;
    BackendInputState gBackendInputState;

    void Update(float deltaT)
    {

    }

    void Render()
    {

    }

    void MainLoop()
    {
        gBackendAPI.InitWindow();
        gBackendAPI.InitRenderer();
        double previous = gBackendAPI.GetTime();
        double lag = 0.0;
        while (gBackendAPI.ShouldGameContinue())
        {
            double current = gBackendAPI.GetTime();
            double elapsed = current - previous;
            previous = current;
            lag += elapsed;

            while (lag >= MS_PER_UPDATE)
            {
                lag -= MS_PER_UPDATE;
                gBackendAPI.PollInput(gBackendInputState);
                Update(MS_PER_UPDATE);
            }
            gBackendAPI.PreRender();
            Render();
            gBackendAPI.PostRender();
        }
        gBackendAPI.Cleanup();
    }
    
	void Run(const EngineInitArgs& args)
	{
        HINSTANCE hinstLib;
        BackEndAppFactory ProcAdd;
        BOOL fFreeResult, fRunTimeLinkSuccess = FALSE;

        // Get a handle to the DLL module.
        LPCWSTR name = args.BackEndRendererDLLPath.c_str();
        hinstLib = LoadLibrary(name);

        // If the handle is valid, try to get the function address.

        if (hinstLib != NULL)
        {
            ProcAdd = (BackEndAppFactory)GetProcAddress(hinstLib, "GetApp");

            // If the function address is valid, call the function.

            if (NULL != ProcAdd)
            {
                fRunTimeLinkSuccess = TRUE;
                gBackendAPI = (ProcAdd)();
                MainLoop();
            }
            // Free the DLL module.
            

            fFreeResult = FreeLibrary(hinstLib);
        }
	}
	
}

