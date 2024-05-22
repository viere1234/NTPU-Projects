#include <Windows.h>

#include "headers.h"
#include "trianglemesh.h"
#include "camera.h"
#include "shaderprog.h"
#include "light.h"
#include "imagetexture.h"
#include "skybox.h"

// GLUI.
int main_window;
int objSpin = 0;
int skyboxSpin = 0;
float skyboxRotationY = 0.0f;
float dirLightRotation[16] = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
float pointLightTranslationXY[2] = { 0, 0 };
float spotLightTranslationXY[2] = { 0, 0 };

// Global variables.
int screenWidth = 600;
int screenHeight = 600;
// Triangle mesh.
TriangleMesh* mesh = nullptr;
// Lights.
DirectionalLight* dirLight = nullptr;
PointLight* pointLight = nullptr;
SpotLight* spotLight = nullptr;
glm::vec3 dirLightDirection = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 dirLightRadiance = glm::vec3(0.6f, 0.6f, 0.6f);
glm::vec3 pointLightPosition = glm::vec3(0.8f, 0.0f, 0.8f);
glm::vec3 pointLightIntensity = glm::vec3(0.5f, 0.1f, 0.1f);
glm::vec3 spotLightPosition = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 spotLightDirection = glm::vec3(0.0f, -1.0f, 0.0f);
glm::vec3 spotLightIntensity = glm::vec3(0.25f, 0.25f, 0.1f);
float spotLightCutoffStartInDegree = 30.0f;
float spotLightTotalWidthInDegree = 45.0f;
glm::vec3 ambientLight = glm::vec3(0.2f, 0.2f, 0.2f);
// Camera.
Camera* camera = nullptr;
glm::vec3 cameraPos = glm::vec3(0.0f, 1.0f, 5.0f);
glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float fovy = 30.0f;
float zNear = 0.1f;
float zFar = 1000.0f;
// Shader.
FillColorShaderProg* fillColorShader = nullptr;
PhongShadingDemoShaderProg* phongShadingShader = nullptr;
SkyboxShaderProg* skyboxShader = nullptr;
// UI.
const float lightMoveSpeed = 0.2f;
// Skybox.
Skybox* skybox = nullptr;


// SceneObject.
struct SceneObject
{
    SceneObject() {
        mesh = nullptr;
        worldMatrix = glm::mat4x4(1.0f);
    }
    TriangleMesh* mesh;
    glm::mat4x4 worldMatrix;
};
SceneObject sceneObj;

// ScenePointLight (for visualization of a point light).
struct ScenePointLight
{
    ScenePointLight() {
        light = nullptr;
        worldMatrix = glm::mat4x4(1.0f);
        visColor = glm::vec3(1.0f, 1.0f, 1.0f);
    }
    PointLight* light;
    glm::mat4x4 worldMatrix;
    glm::vec3 visColor;
};
ScenePointLight pointLightObj;
ScenePointLight spotLightObj;

// Function prototypes.
void ReleaseResources();
// Callback functions.
void RenderSceneCB();
void IdleCB(void);
void ReshapeCB(int, int);
void ProcessSpecialKeysCB(int, int, int);
void ProcessKeysCB(unsigned char, int, int);
void SetupRenderState();
void LoadObjects(const std::string&);
void CreateCamera();
void CreateSkybox(const std::string);
void CreateShaderLib();
void Erase3DModel();
void Load3DModel();
void EraseSkybox();
void LoadSkybox();
void MenuCB(int);
void SetupGLUI();
void GLUICB(int);

enum GLUICBId {
    skyboxRotationCBId,
    dirLightRotationCBId,
    pointLightTranslationXYCBId,
    spotLightTranslationXYCBId
};


void ReleaseResources()
{
    // Delete scene objects and lights.
    if (mesh != nullptr) {
        delete mesh;
        mesh = nullptr;
    }
    if (pointLight != nullptr) {
        delete pointLight;
        pointLight = nullptr;
    }
    if (dirLight != nullptr) {
        delete dirLight;
        dirLight = nullptr;
    }
    if (spotLight != nullptr) {
        delete spotLight;
        spotLight = nullptr;
    }
    // Delete camera.
    if (camera != nullptr) {
        delete camera;
        camera = nullptr;
    }
    // Delete shaders.
    if (fillColorShader != nullptr) {
        delete fillColorShader;
        fillColorShader = nullptr;
    }
    if (phongShadingShader != nullptr) {
        delete phongShadingShader;
        phongShadingShader = nullptr;
    }
    if (skyboxShader != nullptr) {
        delete skyboxShader;
        skyboxShader = nullptr;
    }
}

static float curObjRotationY = 30.0f;
const float rotStep = 0.005f;
void RenderSceneCB()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    if (sceneObj.mesh != nullptr) {
        // Update transform.
        if (objSpin) curObjRotationY += rotStep;
        glm::mat4x4 S = glm::scale(glm::mat4x4(1.0f), glm::vec3(1.5f, 1.5f, 1.5f));
        glm::mat4x4 R = glm::rotate(glm::mat4x4(1.0f), glm::radians(curObjRotationY), glm::vec3(0, 1, 0));
        sceneObj.worldMatrix = S * R;
        glm::mat4x4 normalMatrix = glm::transpose(glm::inverse(sceneObj.worldMatrix));
        glm::mat4x4 MVP = camera->GetProjMatrix() * camera->GetViewMatrix() * sceneObj.worldMatrix;

        // Render
        phongShadingShader->Bind();
        // for vertex shader
        glUniformMatrix4fv(phongShadingShader->GetLocM(), 1, GL_FALSE, glm::value_ptr(sceneObj.worldMatrix));
        glUniformMatrix4fv(phongShadingShader->GetLocNM(), 1, GL_FALSE, glm::value_ptr(normalMatrix));
        glUniformMatrix4fv(phongShadingShader->GetLocMVP(), 1, GL_FALSE, glm::value_ptr(MVP));
        // for fragment shader
        const glm::vec3 cameraPos = camera->GetCameraPos();
        const glm::vec3 dirLightDir = dirLight->GetDirection(), dirLightRadiance = dirLight->GetRadiance();
        const glm::vec3 pointLightPos = pointLightObj.light->GetPosition(), pointLightIntensity = pointLightObj.light->GetIntensity();
        const glm::vec3 spotLightPos = spotLightObj.light->GetPosition(), spotLightIntensity = spotLightObj.light->GetIntensity(), spotLightDir = ((SpotLight*)(spotLightObj.light))->GetDirection();
        glUniform3fv(phongShadingShader->GetLocCameraPos(), 1, glm::value_ptr(camera->GetCameraPos()));
        glUniform3fv(phongShadingShader->GetLocAmbientLight(), 1, glm::value_ptr(ambientLight));
        glUniform3fv(phongShadingShader->GetLocDirLightDir(), 1, glm::value_ptr(dirLightDir));
        glUniform3fv(phongShadingShader->GetLocDirLightRadiance(), 1, glm::value_ptr(dirLightRadiance));
        glUniform3fv(phongShadingShader->GetLocPointLightPos(), 1, glm::value_ptr(pointLightPos));
        glUniform3fv(phongShadingShader->GetLocPointLightIntensity(), 1, glm::value_ptr(pointLightIntensity));
        glUniform3fv(phongShadingShader->GetLocSpotLightPos(), 1, glm::value_ptr(spotLightPos));
        glUniform3fv(phongShadingShader->GetLocSpotLightIntensity(), 1, glm::value_ptr(spotLightIntensity));
        glUniform3fv(phongShadingShader->GetLocSpotLightDir(), 1, glm::value_ptr(spotLightDir));
        glUniform1f(phongShadingShader->GetLocSpotLightTotalWidthCos(), ((SpotLight*)(spotLightObj.light))->GetTotalWidthCos());
        glUniform1f(phongShadingShader->GetLocSpotLightEpsilon(), ((SpotLight*)(spotLightObj.light))->GetEpsilon());
        sceneObj.mesh->Draw(phongShadingShader);
        phongShadingShader->UnBind();
    }
    // -------------------------------------------------------------------------------------------

    // Visualize the light for directional light with fill color.
    if (dirLight != nullptr) {
        const glm::mat4x4 T = glm::translate(glm::mat4(1.0f), glm::vec3(-0.9f, -0.9f, 0.8f)); // position
        const glm::mat4x4 S = glm::scale(glm::mat4x4(1.0f), glm::vec3(0.2f, 0.2f, 0.2f)); // scale down
        const glm::mat4x4 MVP = camera->GetProjMatrix() * camera->GetViewMatrix() * T * S;
        fillColorShader->Bind();
        dirLight->Draw(fillColorShader, MVP);
        fillColorShader->UnBind();
    }
    // Visualize the light with fill color. ------------------------------------------------------
    PointLight* pointLight = pointLightObj.light;
    if (pointLight != nullptr) {
        glm::mat4x4 T = glm::translate(glm::mat4x4(1.0f), pointLight->GetPosition());
        pointLightObj.worldMatrix = T;
        glm::mat4x4 MVP = camera->GetProjMatrix() * camera->GetViewMatrix() * pointLightObj.worldMatrix;
        fillColorShader->Bind();
        glUniformMatrix4fv(fillColorShader->GetLocMVP(), 1, GL_FALSE, glm::value_ptr(MVP));
        glUniform3fv(fillColorShader->GetLocFillColor(), 1, glm::value_ptr(pointLightObj.visColor));
        // Render the point light.
        pointLight->Draw();
        fillColorShader->UnBind();
    }
    SpotLight* spotLight = (SpotLight*)(spotLightObj.light);
    if (spotLight != nullptr) {
        glm::mat4x4 T = glm::translate(glm::mat4x4(1.0f), spotLight->GetPosition());
        spotLightObj.worldMatrix = T;
        glm::mat4x4 MVP = camera->GetProjMatrix() * camera->GetViewMatrix() * spotLightObj.worldMatrix;
        fillColorShader->Bind();
        glUniformMatrix4fv(fillColorShader->GetLocMVP(), 1, GL_FALSE, glm::value_ptr(MVP));
        glUniform3fv(fillColorShader->GetLocFillColor(), 1, glm::value_ptr(spotLightObj.visColor));
        // Render the spot light.
        spotLight->Draw();
        fillColorShader->UnBind();
    }
    // -------------------------------------------------------------------------------------------

    // Render skybox. ----------------------------------------------------------------------------
    if (skybox != nullptr) {
        if (skyboxSpin) skybox->AddRotation(rotStep);
        skybox->Render(camera, skyboxShader);
    }
    // -------------------------------------------------------------------------------------------

    glutSwapBuffers();
}

void IdleCB(void)
{
    if (glutGetWindow() != main_window) glutSetWindow(main_window);
    glutPostRedisplay();
}

void ReshapeCB(int w, int h)
{
    // Update viewport.
    screenWidth = w;
    screenHeight = h;
    glViewport(0, 0, screenWidth, screenHeight);
    // Adjust camera and projection.
    float aspectRatio = (float)screenWidth / (float)screenHeight;
    camera->UpdateProjection(fovy, aspectRatio, zNear, zFar);
}

void ProcessSpecialKeysCB(int key, int x, int y)
{
    // Handle special (functional) keyboard inputs such as F1, spacebar, page up, etc. 
    switch (key) {
    // Rendering mode.
    case GLUT_KEY_F1:
        // Render with point mode.
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
        break;
    case GLUT_KEY_F2:
        // Render with line mode.
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        break;
    case GLUT_KEY_F3:
        // Render with fill mode.
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        break;
    
    // Light control.
    case GLUT_KEY_LEFT:
        if (pointLight != nullptr)
            pointLight->MoveLeft(lightMoveSpeed);
        break;
    case GLUT_KEY_RIGHT:
        if (pointLight != nullptr)
            pointLight->MoveRight(lightMoveSpeed);
        break;
    case GLUT_KEY_UP:
        if (pointLight != nullptr)
            pointLight->MoveUp(lightMoveSpeed);
        break;
    case GLUT_KEY_DOWN:
        if (pointLight != nullptr)
            pointLight->MoveDown(lightMoveSpeed);
        break;
    default:
        break;
    }
}

void ProcessKeysCB(unsigned char key, int x, int y)
{
    // Handle other keyboard inputs those are not defined as special keys.
    if (key == 27) {
        // Release memory allocation if needed.
        ReleaseResources();
        exit(0);
    }
    // Spot light control.
    if (spotLight != nullptr) {
        if (key == 'a')
            spotLight->MoveLeft(lightMoveSpeed);
        if (key == 'd')
            spotLight->MoveRight(lightMoveSpeed);
        if (key == 'w')
            spotLight->MoveUp(lightMoveSpeed);
        if (key == 's')
            spotLight->MoveDown(lightMoveSpeed);
    }
}

void SetupRenderState()
{
    glEnable(GL_DEPTH_TEST);

    glm::vec4 clearColor = glm::vec4(0.44f, 0.57f, 0.75f, 1.00f);
    glClearColor(
        (GLclampf)(clearColor.r), 
        (GLclampf)(clearColor.g), 
        (GLclampf)(clearColor.b), 
        (GLclampf)(clearColor.a)
    );
}

void CreateLights()
{
    // Create a directional light.
    dirLight = new DirectionalLight(dirLightDirection, dirLightRadiance);
    // Create a point light.
    pointLight = new PointLight(pointLightPosition, pointLightIntensity);
    pointLightObj.light = pointLight;
    pointLightObj.visColor = glm::normalize((pointLightObj.light)->GetIntensity());
    // Create a spot light.
    spotLight = new SpotLight(spotLightPosition, spotLightIntensity, spotLightDirection, 
            spotLightCutoffStartInDegree, spotLightTotalWidthInDegree);
    spotLightObj.light = spotLight;
    spotLightObj.visColor = glm::normalize((spotLightObj.light)->GetIntensity());
}

void CreateCamera()
{
    // Create a camera and update view and proj matrices.
    camera = new Camera((float)screenWidth / (float)screenHeight);
    camera->UpdateView(cameraPos, cameraTarget, cameraUp);
    float aspectRatio = (float)screenWidth / (float)screenHeight;
    camera->UpdateProjection(fovy, aspectRatio, zNear, zFar);
}

void CreateShaderLib()
{
    fillColorShader = new FillColorShaderProg();
    if (!fillColorShader->LoadFromFiles("shaders/fixed_color.vs", "shaders/fixed_color.fs"))
        exit(1);

    phongShadingShader = new PhongShadingDemoShaderProg();
    if (!phongShadingShader->LoadFromFiles("shaders/phong_shading_demo.vs", "shaders/phong_shading_demo.fs"))
        exit(1);

    skyboxShader = new SkyboxShaderProg();
    if (!skyboxShader->LoadFromFiles("shaders/skybox.vs", "shaders/skybox.fs"))
        exit(1);
}

void Erase3DModel()
{
    delete sceneObj.mesh;
    mesh = nullptr;
    sceneObj.mesh = nullptr;
}

void Load3DModel()
{
    // Open file
    OPENFILENAME ofn;
    wchar_t szFile[MAX_PATH] = L"";
    ZeroMemory(&ofn, sizeof(OPENFILENAME));

    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = nullptr;
    ofn.lpstrFile = szFile;
    ofn.lpstrFile[0] = L'0';
    ofn.nMaxFile = sizeof(szFile) / sizeof(wchar_t);
    ofn.lpstrFilter = L"OBJ File (*.obj*)\0*.obj*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrInitialDir = nullptr;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileNameW(&ofn)) {
        // Get file path and directory
        const std::wstring wFilePath = ofn.lpstrFile;
        using convert_type = std::codecvt_utf8<wchar_t>;
        std::wstring_convert<convert_type, wchar_t> converter;
        const std::string filePath = converter.to_bytes(wFilePath);
        const std::string fileDirectory = std::experimental::filesystem::path{ filePath }.parent_path().string();
        std::cout << "OBJ file path: " << filePath << std::endl << "File directory: " << fileDirectory << std::endl;
        // Clean previous
        Erase3DModel();
        // Load Objects
        mesh = new TriangleMesh();
        if (!mesh->LoadFromFile(filePath, fileDirectory)) {
            std::cout << "Faild to load model" << std::endl;
            Erase3DModel();
            return;
        }
        mesh->ShowInfo();
        sceneObj.mesh = mesh;
    }
}

void EraseSkybox() {
    delete skybox;
    skybox = nullptr;
}

void LoadSkybox() {
    // Open file
    OPENFILENAME ofn;
    wchar_t szFile[MAX_PATH] = L"";
    ZeroMemory(&ofn, sizeof(OPENFILENAME));

    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = nullptr;
    ofn.lpstrFile = szFile;
    ofn.lpstrFile[0] = L'0';
    ofn.nMaxFile = sizeof(szFile) / sizeof(wchar_t);
    ofn.lpstrFilter = L"PNG File (*.png)\0*.png\0JPEG File (*.jpeg, *jpg)\0*.jpeg;*jpg\0All File (*.*)\0*.*\0\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrInitialDir = nullptr;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileNameW(&ofn)) {
        // Get file path and directory
        const std::wstring wFilePath = ofn.lpstrFile;
        using convert_type = std::codecvt_utf8<wchar_t>;
        std::wstring_convert<convert_type, wchar_t> converter;
        const std::string filePath = converter.to_bytes(wFilePath);
        // Clean previous
        EraseSkybox();
        // Load Skybox
        const int numSlices = 36;
        const int numStacks = 18;
        const float radius = 50.0f;
        skybox = new Skybox(filePath, numSlices, numStacks, radius);
    }
}

void MenuCB(int option)
{
    switch (option) {
    case 0:
        Load3DModel();
        break;
    case 1:
        Erase3DModel();
        break;
    case 2:
        LoadSkybox();
        break;
    case 3:
        EraseSkybox();
        break;
    }
    glutPostRedisplay();
}

void SetupGLUI()
{
    // Register callback functions.
    GLUI_Master.set_glutIdleFunc(IdleCB);
    // Create GLUI standalone window
    GLUI* glui = GLUI_Master.create_glui("Contorl Panel");
    glui->set_main_gfx_window(main_window);
    // Add controls: Model
    GLUI_Panel* gluiModelPanel = glui->add_panel("Model");
    glui->add_checkbox_to_panel(gluiModelPanel, "Spin", &objSpin);
    glui->add_column();
    // Add controls: Skybox
    GLUI_Panel* gluiSkyboxPanel = glui->add_panel("Skybox");
    glui->add_checkbox_to_panel(gluiSkyboxPanel, "Spin", &skyboxSpin);
    glui->add_translation_to_panel(gluiSkyboxPanel, "Rotation Y", 2, &skyboxRotationY, skyboxRotationCBId, GLUICB);
    glui->add_column();
    // Add controls: Lights
    GLUI_Panel* gluiLightsPanel = glui->add_panel("Lights");
    //  Direction Light
    GLUI_Panel* gluiDirLightPanel = glui->add_panel_to_panel(gluiLightsPanel, "Direction Light");
    glui->add_rotation_to_panel(gluiDirLightPanel, "Direction", dirLightRotation, dirLightRotationCBId, GLUICB);
    glui->add_column_to_panel(gluiLightsPanel);
    //  Point Light
    GLUI_Panel* gluiPointLightPanel = glui->add_panel_to_panel(gluiLightsPanel, "Point Light");
    glui->add_translation_to_panel(gluiPointLightPanel, "Translation XY", 0, pointLightTranslationXY, pointLightTranslationXYCBId, GLUICB);
    glui->add_column_to_panel(gluiLightsPanel);
    //  Spot Light
    GLUI_Panel* gluiSpotLightPanel = glui->add_panel_to_panel(gluiLightsPanel, "Spot Light");
    glui->add_translation_to_panel(gluiSpotLightPanel, "Translation XY", 0, spotLightTranslationXY, spotLightTranslationXYCBId, GLUICB);
}

void GLUICB(int id)
{
    switch (id) {
    case skyboxRotationCBId:
        if (skybox != nullptr) skybox->AddRotation(skyboxRotationY * 0.01f);
        break;
    case dirLightRotationCBId:
        if (dirLight != nullptr) dirLight->ChangeDirection(dirLightRotation);
        break;
    case pointLightTranslationXYCBId:
        if (pointLightObj.light != nullptr) pointLightObj.light->TranslationXY(pointLightTranslationXY);
        break;
    case spotLightTranslationXYCBId:
        if (spotLightObj.light != nullptr) spotLightObj.light->TranslationXY(spotLightTranslationXY);
        break;
    }
    glutPostRedisplay();
}

int main(int argc, char** argv)
{
    // Setting window properties.
    glutInit(&argc, argv);
    glutSetOption(GLUT_MULTISAMPLE, 4);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE);
    glutInitWindowSize(screenWidth, screenHeight);
    glutInitWindowPosition(100, 100);
    main_window = glutCreateWindow("Texture Mapping");

    // Initialize GLEW.
    // Must be done after glut is initialized!
    GLenum res = glewInit();
    if (res != GLEW_OK) {
        std::cerr << "GLEW initialization error: " 
                  << glewGetErrorString(res) << std::endl;
        return 1;
    }

    // Initialization.
    SetupRenderState();
    CreateLights();
    CreateCamera();
    //CreateSkybox("textures/photostudio_02_2k.png");
    CreateShaderLib();

    // Register callback functions.
    glutDisplayFunc(RenderSceneCB);
    glutIdleFunc(RenderSceneCB);
    glutReshapeFunc(ReshapeCB);
    glutSpecialFunc(ProcessSpecialKeysCB);
    glutKeyboardFunc(ProcessKeysCB);

    // Setting Menu
    int menu = glutCreateMenu(MenuCB);
    glutAddMenuEntry("Load 3D Model", 0);
    glutAddMenuEntry("Erase 3D Model", 1);
    glutAddMenuEntry("Load Skybox", 2);
    glutAddMenuEntry("Erase Skybox", 3);
    glutAttachMenu(GLUT_RIGHT_BUTTON);

    // Setting GLUI
    SetupGLUI();

    // Start rendering loop.
    glutMainLoop();

    return 0;
}
