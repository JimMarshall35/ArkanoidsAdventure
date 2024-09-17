#include "EngineDLLInterface.h"
#include <windows.h> 
#include "IBackendApp.h"
#include "OBJ/ObjFile.h"
#include <stdio.h>
#include "Pipeline.h"
#include "TestPipeline.h"
#include "Camera.h"
#include  <thread>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Texture.h"
#include "Scene.h"
#include "CameraComponent.h"
#include "DebugLoadBasicScene.h"
#include "XMLArchive.h"
#include "EditorServer.h"
#include "ComponentReg.h"

#define MS_PER_UPDATE 20.0f
#define S_PER_UPDATE (MS_PER_UPDATE / 1000.0f)

namespace Engine
{
    BackendAPI gBackendAPI;
    int gWidth = 800;
    int gHeight = 600;

    
    static void ErrorHandler(const BackendLog& error)
    {
        switch (error.Severity)
        {
        case BackendErrorSeverity::Error:
            printf("\u001b[31m[BACKEND ERROR]\u001b[0m ");
            break;
        case BackendErrorSeverity::Info:
            printf("\u001b[32m[BACKEND INFO]\u001b[0m ");
            break;
        case BackendErrorSeverity::Warning:
            printf("\u001b[33m[BACKEND WARNING]\u001b[0m ");
            break;
        case BackendErrorSeverity::Unknown:
            printf("\u001b[96m[BACKEND UNKNOWN]\u001b[0m ");
            break;
        default:
            assert(false);
        }
        printf("%s\n", error.Msg.c_str());
    }

    static void ResizeHandler(int newW, int newH)
    {
        gWidth = newW;
        gHeight = newH;
        CameraComponent::OnResize(newW, newH);
    }

    const BackendAPI& GetAPI()
    {
        return gBackendAPI;
    }

    void GetWindowWidthAndHeight(int& pxW, int& pxH)
    {
        pxW = gWidth;
        pxH = gHeight;
    }
     
    void Update(float deltaT)
    {
        Scn::GetScene().sysReg.Update(deltaT);
    }

    void Render()
    {
        Scn::GetScene().sysReg.Draw();
    }

    void MainLoop(const EngineInitArgs& args)
    {
        gBackendAPI.InitWindow(gWidth, gHeight);


        gBackendAPI.RegisterErrorHandler(&ErrorHandler);
        gBackendAPI.RegisterResize(&ResizeHandler);

        gBackendAPI.InitRenderer();
        
        if (args.InitialScenePath == "")
        {
            Scn::Scene& s = Scn::GetScene();
            BasicScn::Load(s);
        }
        else
        {
            XMLArchive ar(args.InitialScenePath.c_str(), false);
            Scn::SerializeScene(ar);
        }

        if (args.bUseEditorServer)
        {
            Editor::Init({});
        }
        
        Comp::Init();

        double previous = gBackendAPI.GetTime();
        double lag = 0.0;

        while (gBackendAPI.ShouldGameContinue())
        {
            double current = gBackendAPI.GetTime();
            double elapsed = current - previous;
            previous = current;
            lag += elapsed;

            In::PollInput();
            if (args.bUseEditorServer)
            {
                Editor::PollEditorMessageQueue();
            }

            while (lag >= MS_PER_UPDATE)
            {
                lag -= MS_PER_UPDATE;
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
                MainLoop(args);
            }
            // Free the DLL module.
            
            if (args.bUseEditorServer)
            {
                Editor::DeInit();
            }
            fFreeResult = FreeLibrary(hinstLib);
        }
	}
	
}

