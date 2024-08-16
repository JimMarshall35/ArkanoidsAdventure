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

#define MS_PER_UPDATE 20.0f
#define S_PER_UPDATE (MS_PER_UPDATE / 1000.0f)

namespace Engine
{
    BackendAPI gBackendAPI;
    BackendInputState gBackendInputState;
    glm::mat4 gProj;
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
        gProj = glm::perspectiveFov(90.0f, (float)newW, (float)newH, 0.1f, 1000.0f);
    }

    const BackendAPI& GetAPI()
    {
        return gBackendAPI;
    }

#pragma region Testing
    const char* LoadFileIntoBuffer(const char* path, long& outBufferSize)
    {
        char* source = NULL;

        FILE* fp = nullptr;
        fopen_s(&fp, path, "r");
        if (fp != NULL) {
            /* Go to the end of the file. */
            if (fseek(fp, 0L, SEEK_END) == 0) {
                /* Get the size of the file. */
                outBufferSize = ftell(fp);
                if (outBufferSize == -1) { /* Error */ }

                /* Allocate our buffer to that size. */
                source = (char*)malloc(sizeof(char) * (outBufferSize + 1));

                /* Go back to the start of the file. */
                if (fseek(fp, 0L, SEEK_SET) != 0) { /* Error */ }

                /* Read the entire file into memory. */
                size_t newLen = fread(source, sizeof(char), outBufferSize, fp);
                if (ferror(fp) != 0) {
                    fputs("Error reading file", stderr);
                }
                else {
                    source[newLen++] = '\0'; /* Just to be safe. */
                }
            }
            fclose(fp);
        }
        return source;
    }

    HDrawable hDrawable = ENGINE_NULL_HANDLE;
#pragma endregion
     
    void Update(float deltaT)
    {

    }

    void Render()
    {
#pragma region Testing
        gBackendAPI.DrawDrawables(&hDrawable, 1);
#pragma endregion
    }

    void MainLoop()
    {
        /*
            Camera(glm::vec3 eye, glm::vec3 lookat, glm::vec3 upVector)
        : m_eye(std::move(eye))
        , m_lookAt(std::move(lookat))
        , m_upVector(std::move(upVector))
    {
        */
        Camera cam = Camera(glm::vec3(5, 0, 0), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3{ 0.0f,0.0f,1.0f });
        cam.SetLookAt({ 0.0f,0.0f,0.0f });

        gBackendAPI.InitWindow(gWidth, gHeight);
        float FOV = 90.0f;//glm::pi<float>() * 0.25f;
        float Aspect = (float)gWidth / (float)gHeight;
        float Near = 0.1f;
        float Far = 5000.0f;
        // create a perspective projection matrix with a 90 degree field-of-view and widescreen aspect ratio
        gProj = glm::perspective(FOV, Aspect, Near, Far);

        gBackendAPI.RegisterErrorHandler(&ErrorHandler);
        gBackendAPI.RegisterResize(&ResizeHandler);

        gBackendAPI.InitRenderer();
        
        Scn::Scene;
#pragma region Testing

        EVec<EString> errors;
        long bufSize = 0;
        const char* fileData = LoadFileIntoBuffer("Shuriken.obj", bufSize);
        PipelineMeshData md;
        if (OBJLoader::LoadFromFile(fileData, bufSize, errors, md))
        {
            for (const EString& error : errors)
            {
                printf("%s", error.c_str());
            }
        }
        free((void*)fileData);

        PipeLine p;
        HPipeline hTest = BuildTestPipeline(p, gBackendAPI);
        HMesh hMesh = gBackendAPI.UploadMesh(hTest, md);
        PerInstanceUniforms perInstanceUni;
        hDrawable = GetTestPipelineDrawable(hMesh, &perInstanceUni);
        perInstanceUni.ambientStrength = 0.4f;
        perInstanceUni.diffuseStrength = 0.5f;
        perInstanceUni.speculatStrength = 0.2f;
        perInstanceUni.shininess = 8.0f;
        perInstanceUni.m = glm::mat4(1.0f);

        PerDrawUniforms& pipeUniforms = GetPipelineUniforms();
        pipeUniforms.v = cam.GetViewMatrix();
        pipeUniforms.p = gProj;
        pipeUniforms.lightPos = glm::vec3{ 100.0f, 100.0f, 100 };
        pipeUniforms.lightColour = glm::vec3{ 1.0, 1.0, 1.0 };

        pipeUniforms.lightPos = cam.GetEye();

        int width, height, nrChannels;
        unsigned char* data = stbi_load("Shuriken.tga", &width, &height, &nrChannels, 0);

        TextureData td;
        td.HeightPx = height;
        td.WidthPx = width;
        td.pData = data;
        td.DataSize = height * width * nrChannels;

        HTexture t = gBackendAPI.UploadTexture(td);
        perInstanceUni.hTexture = t;
        
        stbi_image_free(data);
#pragma endregion

        double previous = gBackendAPI.GetTime();
        double lag = 0.0;

        while (gBackendAPI.ShouldGameContinue())
        {
            double current = gBackendAPI.GetTime();
            double elapsed = current - previous;
            previous = current;
            lag += elapsed;

            gBackendAPI.PollInput(gBackendInputState);

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
                MainLoop();
            }
            // Free the DLL module.
            

            fFreeResult = FreeLibrary(hinstLib);
        }
	}
	
}

