#include "DebugLoadBasicScene.h"
#include "Scene.h"
#include "stb_image.h"
#include "OBJ/ObjFile.h"
#include "MeshComponent.h"
#include "TransformComponent.h"
#include "TestPipeline.h"
#include "CameraComponent.h"
#include "PointLightComponent.h"
#include "Pipeline.h"
#include "EditorSystems.h"
#include "EditorCamControlsComponent.h"
#include "DrawMeshComponentsFunction.h"
#include "EngineDLLInterface.h"
#include "IBackendApp.h"

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



Entity BasicScn::Load(Scn::Scene& scn)
{
    const BackendAPI& api = Engine::GetAPI();

    // Textures
    scn.textureReg.Clear();
    scn.textureReg.UploadTextureFile("Shuriken.tga", "Shuriken");

    // Meshes
    scn.meshReg.UploadMeshData("Shuriken.obj");
    HMesh shurikenMesh = scn.meshReg.TryGetMesh("Shuriken.obj");

    // Entities
    EntityReg& reg = scn.entities.GetReg();
    scn.rootTransformEntity = reg.create();
    Transform t = reg.emplace<Transform>(scn.rootTransformEntity);
    t.setLocalPosition({ 0,0,0 });
    t.setLocalScale({ 1,1,1 });
    t.setLocalRotation({ 0,0,0 });


    Entity meshEntity = reg.create();

    MeshComponent& mc = reg.emplace<MeshComponent>(meshEntity);
    PipeLine* pPipeline = PipeLine::TryGetPipeline("Test");
   // pPipeline->Create();
    mc.Create(shurikenMesh, *pPipeline, meshEntity);

    Transform& t2 = reg.emplace<Transform>(meshEntity);
    Transform::AddChild(scn.rootTransformEntity, meshEntity);
    t2.setLocalPosition({ 0,0,0 });
    t2.setLocalScale({ 1,1,1 });
    t2.setLocalRotation({ 0,0,0 });

    TestPipelineMaterial& mat = reg.emplace<TestPipelineMaterial>(meshEntity);
    mat.hTexture = scn.textureReg.GetTexture("Shuriken");
    mat.ambientStrength = 0.4f;
    mat.diffuseStrength = 0.5f;
    mat.specularStrength = 0.2f;
    mat.shininess = 8.0f;

    Entity meshEntity2 = reg.create();

    MeshComponent& mc2 = reg.emplace<MeshComponent>(meshEntity2);
    mc2.Create(shurikenMesh, *pPipeline, meshEntity2);

    Transform& t9 = reg.emplace<Transform>(meshEntity2);
    Transform::AddChild(meshEntity, meshEntity2);
    t9.setLocalPosition({ 0,10,0 });
    t9.setLocalScale({ 1,1,1 });
    t9.setLocalRotation({ 0,0,0 });

    TestPipelineMaterial& mat2 = reg.emplace<TestPipelineMaterial>(meshEntity2);
    mat2.hTexture = scn.textureReg.GetTexture("Shuriken");
    mat2.ambientStrength = 0.4f;
    mat2.diffuseStrength = 0.5f;
    mat2.specularStrength = 0.2f;
    mat2.shininess = 8.0f;


    Entity lightEnt = reg.create();
    PointLightComponent& plc = reg.emplace<PointLightComponent>(lightEnt);
    plc.colour = { 1, 1, 1 };
    Transform& t1 = reg.emplace<Transform>(lightEnt);
    t1.setLocalPosition({ 0, 100, 0 });
    Transform::AddChild(scn.rootTransformEntity, lightEnt);

    Entity camEnt = reg.create();

    Transform& camTrans = reg.emplace<Transform>(camEnt);
    Transform::AddChild(scn.rootTransformEntity, camEnt);
    camTrans.setLocalPosition({ 0,0,5 });
    camTrans.setLocalScale({ 1,1,1 });
    camTrans.setLocalRotation({ 0,0,0 });

    scn.activeCameraAntity = camEnt;

    CameraComponent & cam = reg.emplace<CameraComponent>(camEnt);
    int w; int h;
    Engine::GetWindowWidthAndHeight(w, h);
    float aspect = (float)w / (float)h;
    cam.aspect = aspect;
    cam.fovY = 1.0;//glm::pi<float>() * 0.5f;

    cam.zNear = 0.1f;
    cam.zFar = 5000.0f;
    cam.bIsMain = true;
    cam.Name = "EditorCam";

    // camera controls
    EditorCamControlsComponent& controls = reg.emplace<EditorCamControlsComponent>(camEnt);

#define BTN(bname, handleStoreLocation)\
In::LogicalButton bname; bname.name = #bname;\
handleStoreLocation = In::RegisterLogicalButton(bname);\
In::EnableDisableBtn(handleStoreLocation, true);

    memset((void*)&controls.MovementButtons[0], 0, sizeof(controls.MovementButtons));
    BTN(editorcamActivateBtn, controls.ActivateButton);
    BTN(editorCamForwardBtn, controls.MovementButtons[(u32)Directions::FORWARD]);
    BTN(editorCamBackwardBtn, controls.MovementButtons[(u32)Directions::BACKWARD]);
    BTN(editorCamLeftBtn, controls.MovementButtons[(u32)Directions::LEFT]);
    BTN(editorCamRightBtn, controls.MovementButtons[(u32)Directions::RIGHT]);
    BTN(editorCamUpBtn, controls.MovementButtons[(u32)Directions::UP]);
    BTN(editorCamDownBtn, controls.MovementButtons[(u32)Directions::DOWN]);
    BTN(editorCamMoveSpeedUpBtn, controls.MoveSpeedUpBtn);
    BTN(editorCamMoveSpeedDownBtn, controls.MoveSpeedDownBtn);

    BTN(Ent1, controls.Ent1);
    BTN(Ent2, controls.Ent2);
    BTN(Ent3, controls.Ent3);
    BTN(Ent4, controls.Ent4);


#undef BTN

    In::LogicalAxis editorCamX;
    editorCamX.type = In::LogicalAxisType::Delta;
    editorCamX.name = "EditorCamX";

    In::LogicalAxis editorCamY;
    editorCamY.type = In::LogicalAxisType::Delta;
    editorCamY.name = "EditorCamY";

    

    controls.YawAxis = In::RegisterLogicalAxis(editorCamX);
    controls.PitchAxis = In::RegisterLogicalAxis(editorCamY);

    In::EnableDisableAxis(controls.PitchAxis, true);
    In::EnableDisableAxis(controls.YawAxis, true);

    In::MapLogicalAxisToMouseX(controls.YawAxis, In::LogicalAxisType::Delta);
    In::MapLogicalAxisToMouseY(controls.PitchAxis, In::LogicalAxisType::Delta);
    In::MapLogicalButtonToMouseKey(controls.ActivateButton, api.GetInputCodeForMouseBtn(MouseButtons::LEFT));
    In::MapLogicalButtonToMouseKey(controls.MoveSpeedUpBtn, api.GetInputCodeForMouseBtn(MouseButtons::MB_4));
    In::MapLogicalButtonToMouseKey(controls.MoveSpeedDownBtn, api.GetInputCodeForMouseBtn(MouseButtons::MB_5));

    In::MapLogicalButtonToKeyboardKey(controls.MovementButtons[(u32)Directions::FORWARD], api.GetInputCodeForAscii('w'));
    In::MapLogicalButtonToKeyboardKey(controls.MovementButtons[(u32)Directions::BACKWARD], api.GetInputCodeForAscii('s'));
    In::MapLogicalButtonToKeyboardKey(controls.MovementButtons[(u32)Directions::LEFT], api.GetInputCodeForAscii('a'));
    In::MapLogicalButtonToKeyboardKey(controls.MovementButtons[(u32)Directions::RIGHT], api.GetInputCodeForAscii('d'));
    In::MapLogicalButtonToKeyboardKey(controls.MovementButtons[(u32)Directions::UP], api.GetInputCodeForAscii('e'));
    In::MapLogicalButtonToKeyboardKey(controls.MovementButtons[(u32)Directions::DOWN], api.GetInputCodeForAscii('q'));

    In::MapLogicalButtonToKeyboardKey(controls.Ent1, api.GetInputCodeForAscii('1'));
    In::MapLogicalButtonToKeyboardKey(controls.Ent2, api.GetInputCodeForAscii('2'));
    In::MapLogicalButtonToKeyboardKey(controls.Ent3, api.GetInputCodeForAscii('3'));
    In::MapLogicalButtonToKeyboardKey(controls.Ent4, api.GetInputCodeForAscii('4'));

    In::InputSet editorInputSet = In::GetCurrentInputSet();

    //DrawSystem* ds = DrawSystem::GetByName(DRAW_MESH_COMPONENT_SYSTEM_NAME);
    //HDrawSystem hDS = scn.sysReg.RegisterDrawSystem(ds);
    //
    //UpdateSystem* up = UpdateSystem::GetByName(EDITOR_CONTROLS_SYSTEM_NAME);
    //HUpdateSystem hUS = scn.sysReg.RegisterUpdateSystem(up);

    return meshEntity;
}
