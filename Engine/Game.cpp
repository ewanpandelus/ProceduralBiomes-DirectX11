//
// Game.cpp
//

#include "pch.h"
#include "Game.h"


//toreorganise
#include <fstream>

extern void ExitGame();

using namespace DirectX;
using namespace DirectX::SimpleMath;
using namespace ImGui;

using Microsoft::WRL::ComPtr;

Game::Game() noexcept(false)
{
    m_deviceResources = std::make_unique<DX::DeviceResources>();
    m_deviceResources->RegisterDeviceNotify(this);
}

Game::~Game()
{
#ifdef DXTK_AUDIO
    if (m_audEngine)
    {
        m_audEngine->Suspend();
    }
#endif
}

// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND window, int width, int height)
{

    m_regionSize = 0;
    m_input.Initialise(window);

    m_poissonDiscSampling = PoissonDiscSampling(m_biomeObjects);
    m_biomeObjects.SetEntityData(&m_entityData);
    m_biomeObjects.SetBarycentricCoordinates(&m_barycentricCoordinates);
    m_barycentricCoordinates.SetTerrainScale(m_terrainScale);

    m_deviceResources->SetWindow(window, width, height);
    m_deviceResources->CreateDeviceResources();
    CreateDeviceDependentResources();

    m_deviceResources->CreateWindowSizeDependentResources();
    CreateWindowSizeDependentResources();

    //setup imgui.  its up here cos we need the window handle too
    //pulled from imgui directx11 example
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::StyleColorsDark();
    ImGui_ImplWin32_Init(window);		//tie to our window
    ImGui_ImplDX11_Init(m_deviceResources->GetD3DDevice(), m_deviceResources->GetD3DDeviceContext());	//tie to directx

    m_fullscreenRect.left = 0;
    m_fullscreenRect.top = 0;
    m_fullscreenRect.right = 800;
    m_fullscreenRect.bottom = 600;

    m_CameraViewRect.left = 500;
    m_CameraViewRect.top = 0;
    m_CameraViewRect.right = 800;
    m_CameraViewRect.bottom = 240;

    //setup light
    m_diffuseLight[0] = 0.8f;
    m_diffuseLight[1] = 0.8f;
    m_diffuseLight[2] = 0.65f;

    m_ambientLight[0] = 0.4f;
    m_ambientLight[1] = 0.4f;
    m_ambientLight[2] = 0.4f;

    //setup light
    m_Light.setAmbientColour(m_ambientLight[0], m_ambientLight[1], m_ambientLight[2], 1.0f);
    m_Light.setDiffuseColour(m_diffuseLight[0], m_diffuseLight[1], m_diffuseLight[2], 1.0f);
    m_Light.setPosition(-100.0f, 3.0f, -100.0f);
    m_Light.setDirection(-0.8f, -1.0f, 0.0f);

    //setup camera
    m_Camera01.setPosition(Vector3(0, 100.0f, 0));
    m_Camera01.setRotation(Vector3(180.0f, 0.0f, 0.0f));
    m_Camera01.setPosition(Vector3(0.0f, 1.0f, 0.0f));
    m_Camera01.setRotation(Vector3(-90.0f, -180.0f, 0.0f));	//orientation is -90 becuase zero will be looking up at the sky straight up. 
    m_regionSize = *m_poissonDiscSampling.GetSampleRegionSize();

#ifdef DXTK_AUDIO
    // Create DirectXTK for Audio objects
    AUDIO_ENGINE_FLAGS eflags = AudioEngine_Default;
#ifdef _DEBUG
    eflags = eflags | AudioEngine_Debug;
#endif

    m_audEngine = std::make_unique<AudioEngine>(eflags);

    m_audioEvent = 0;
    m_audioTimerAcc = 10.f;
    m_retryDefault = false;

    m_waveBank = std::make_unique<WaveBank>(m_audEngine.get(), L"adpcmdroid.xwb");

    m_soundEffect = std::make_unique<SoundEffect>(m_audEngine.get(), L"MusicMono_adpcm.wav");
    m_effect1 = m_soundEffect->CreateInstance();
    m_effect2 = m_waveBank->CreateInstance(10);

    m_effect1->Play(true);
    m_effect2->Play();
#endif
}

#pragma region Frame Update
// Executes the basic game loop.
void Game::Tick()
{
    //take in input
    m_input.Update();								//update the hardware
    m_gameInputCommands = m_input.getGameInput();	//retrieve the input for our game

    //Update all game objects
    m_timer.Tick([&]()
        {
            Update(m_timer);
        });

    //Render all game content. 
    Render();

#ifdef DXTK_AUDIO
    // Only update audio engine once per frame
    if (!m_audEngine->IsCriticalError() && m_audEngine->Update())
    {
        // Setup a retry in 1 second
        m_audioTimerAcc = 1.f;
        m_retryDefault = true;
    }
#endif


}

// Updates the world.
void Game::Update(DX::StepTimer const& timer)
{

    m_Light.setAmbientColour(m_ambientLight[0], m_ambientLight[1], m_ambientLight[2], 1.0f);
    m_Light.setDiffuseColour(m_diffuseLight[0], m_diffuseLight[1], m_diffuseLight[2], 1.0f);
    auto device = m_deviceResources->GetD3DDevice();
    float deltaTime = m_timer.GetElapsedSeconds();
    m_elapsedTime += m_timer.GetElapsedSeconds();

    Vector3 rotation = m_Camera01.getRotation();
    if (m_gameInputCommands.left && m_elapsedTime > 0.1)
    {
        m_elapsedTime = 0;
        m_hideUI = !m_hideUI;
    }
    if (m_gameInputCommands.P && m_elapsedTime > 0.1) {
        m_elapsedTime = 0;
        m_playMode = !m_playMode;
        m_input.SwapPlayMode(m_playMode);
    }
    if (m_gameInputCommands.rotX && m_playMode)
    {
        float deltaY = m_input.GetDeltaY();
        rotation.x = rotation.x -= m_Camera01.getRotationSpeed() * deltaY * deltaTime;
        if (rotation.x < -170)
        {
            rotation.x = -169;
        }
        if (rotation.x > 80)
        {
            rotation.x = 70;
        }
        m_Camera01.setRotation(rotation);
    }
    if (m_gameInputCommands.rotY && m_playMode)
    {
        float deltaX = m_input.GetDeltaX();
        rotation.y = rotation.y -= m_Camera01.getRotationSpeed() * deltaX * deltaTime;
        m_Camera01.setRotation(rotation);
    }
    if (m_gameInputCommands.forward)
    {
        Vector3 position = m_Camera01.getPosition(); //get the position
        position += (m_Camera01.getForward() * m_Camera01.getMoveSpeed() * deltaTime);
        m_Camera01.setPosition(position);
    }
    if (m_gameInputCommands.back)
    {
        Vector3 position = m_Camera01.getPosition(); //get the position
        position -= (m_Camera01.getForward() * m_Camera01.getMoveSpeed() * deltaTime);
        m_Camera01.setPosition(position);
    }

    if (m_gameInputCommands.generate)
    {
        GenerateBiomes(device);
    }

    m_Camera01.Update();	//camera update.
    m_terrain.Update();		//terrain update.  doesnt do anything at the moment. 

    m_view = m_Camera01.getCameraMatrix();
    m_world = Matrix::Identity;

    /*create our UI*/


#ifdef DXTK_AUDIO
    m_audioTimerAcc -= (float)timer.GetElapsedSeconds();
    if (m_audioTimerAcc < 0)
    {
        if (m_retryDefault)
        {
            m_retryDefault = false;
            if (m_audEngine->Reset())
            {
                // Restart looping audio
                m_effect1->Play(true);
            }
        }
        else
        {
            m_audioTimerAcc = 4.f;

            m_waveBank->Play(m_audioEvent++);

            if (m_audioEvent >= 11)
                m_audioEvent = 0;
        }
    }
#endif


    if (m_input.Quit())
    {
        ExitGame();
    }
}
#pragma endregion

#pragma region Frame Render
// Draws the scene.
void Game::Render()
{
    if (m_timer.GetFrameCount() == 0)
    {
        return;
    }

    Clear();
    RenderTexturePass();
    m_deviceResources->PIXBeginEvent(L"Render");
    auto context = m_deviceResources->GetD3DDeviceContext();
    auto renderTargetView = m_deviceResources->GetRenderTargetView();
    auto depthTargetView = m_deviceResources->GetDepthStencilView();

    // Draw Text to the screen

    char string[20];
    itoa(m_timer.GetFramesPerSecond(), string, 10);


    //Set Rendering states. 
    context->OMSetBlendState(m_states->Opaque(), nullptr, 0xFFFFFFFF);
    context->OMSetDepthStencilState(m_states->DepthDefault(), 0);
    context->RSSetState(m_states->CullClockwise());
    //  context->RSSetState(m_raster.Get());
      //context->RSSetState(m_states->Wireframe());

      //prepare transform for floor object. 
    m_world = SimpleMath::Matrix::Identity; //set world back to identity

    SimpleMath::Matrix objectPosition = SimpleMath::Matrix::CreateTranslation(0, 0, 0);


    m_geometryShader.EnableShader(context, false);
    float index = 0;
    SimpleMath::Matrix objectScale = SimpleMath::Matrix::CreateScale(1);
    float time = m_timer.GetTotalSeconds();

    std::vector<ModelClass> models = m_entityData.GetModels();

    for each (auto model in models)
    {

        m_world = SimpleMath::Matrix::Identity; //set world back to identity
//     SimpleMath::Vector3 objPos = m_objectMap[index].position;
        objectPosition = SimpleMath::Matrix::CreateTranslation(-m_terrainWidth + xOffset, 0, -m_terrainWidth + zOffset);


        m_world = m_world * objectPosition;
        m_geometryShader.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light,
            model.GetTexture().Get(), m_timer.GetTotalSeconds());
        model.Render(context);

    }


    m_world = SimpleMath::Matrix::Identity; //set world back to identity
    SimpleMath::Matrix positionAccountedFor = SimpleMath::Matrix::CreateTranslation(-m_terrainWidth * m_terrainScale / 2 +64, 0.f, -m_terrainWidth * m_terrainScale / 2+64);
    m_world = m_world * positionAccountedFor;
    m_terrainShader.EnableShader(context);
    m_terrainMap = m_terrainLoader.GetTerrainMap();
    for each (auto terrain in *m_terrainMap) {
        m_world = SimpleMath::Matrix::Identity; //set world back to identity
        positionAccountedFor = SimpleMath::Matrix::CreateTranslation(terrain.position.x, 0.f, terrain.position.y);
        m_world = m_world * positionAccountedFor;
        m_terrainShader.SetBiomeShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light,
           terrain.climateMapTex.Get(), m_noiseTexture.Get());
        terrain.terrain.Render(context);
    }
 /*   m_terrainShader.EnableShader(context);
    m_terrainShader.SetBiomeShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light,
        m_generatedClimateMapTexture.Get(), m_noiseTexture.Get());
    m_terrain.Render(context);*/

    context->OMSetBlendState(m_states->NonPremultiplied(), nullptr, 0xFFFFFFFF);
    m_waterShader.EnableShader(context, false);
    m_waterShader.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light,
        m_FirstRenderPass->getShaderResourceView(), m_timer.GetTotalSeconds());
 //   m_water.Render(context);




    //render our GUI


    if (!m_hideUI) {
        SetupGUI();
        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    }
    m_sprites->Begin();
    m_font->DrawString(m_sprites.get(), string, XMFLOAT2(10, 10), Colors::Yellow);
    m_sprites->End();
    // Show the new frame.
    m_deviceResources->Present();
}
void Game::RenderTexturePass()
{
    auto context = m_deviceResources->GetD3DDeviceContext();
    auto renderTargetView = m_deviceResources->GetRenderTargetView();
    auto depthTargetView = m_deviceResources->GetDepthStencilView();
    m_FirstRenderPass->setRenderTarget(context);
    m_FirstRenderPass->clearRenderTarget(context, 0.0f, 0.0f, 1.0f, 1.0f);


    context->OMSetBlendState(m_states->Opaque(), nullptr, 0xFFFFFFFF);
    context->OMSetDepthStencilState(m_states->DepthDefault(), 0);
    context->RSSetState(m_states->CullClockwise());

    m_terrainShader.EnableShader(context);
    m_terrainShader.SetBiomeShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light,
        m_generatedClimateMapTexture.Get(), m_noiseTexture.Get());
    m_terrain.Render(context);

    // Reset the render target back to the original back buffer and not the render to texture anymore.	
    context->OMSetRenderTargets(1, &renderTargetView, depthTargetView);
}

void Game::GenerateBiomes(ID3D11Device* device)
{
    GenerateClimate(device);

    GenerateTerrain(device);
    //SetupModelPositions(device);
}

// Helper method to clear the back buffers.
void Game::Clear()
{
    m_deviceResources->PIXBeginEvent(L"Clear");

    // Clear the views.
    auto context = m_deviceResources->GetD3DDeviceContext();
    auto renderTarget = m_deviceResources->GetRenderTargetView();
    auto depthStencil = m_deviceResources->GetDepthStencilView();

    context->ClearRenderTargetView(renderTarget, Colors::CornflowerBlue);
    context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    context->OMSetRenderTargets(1, &renderTarget, depthStencil);

    // Set the viewport.
    auto viewport = m_deviceResources->GetScreenViewport();
    context->RSSetViewports(1, &viewport);

    m_deviceResources->PIXEndEvent();
}

#pragma endregion

#pragma region Message Handlers
// Message handlers
void Game::OnActivated()
{
}

void Game::OnDeactivated()
{
}

void Game::OnSuspending()
{
#ifdef DXTK_AUDIO
    m_audEngine->Suspend();
#endif
}

void Game::OnResuming()
{
    m_timer.ResetElapsedTime();

#ifdef DXTK_AUDIO
    m_audEngine->Resume();
#endif
}

void Game::OnWindowMoved()
{
    auto r = m_deviceResources->GetOutputSize();
    m_deviceResources->WindowSizeChanged(r.right, r.bottom);
}

void Game::OnWindowSizeChanged(int width, int height)
{
    if (!m_deviceResources->WindowSizeChanged(width, height))
        return;

    CreateWindowSizeDependentResources();
}

#ifdef DXTK_AUDIO
void Game::NewAudioDevice()
{
    if (m_audEngine && !m_audEngine->IsAudioDevicePresent())
    {
        // Setup a retry in 1 second
        m_audioTimerAcc = 1.f;
        m_retryDefault = true;
    }
}
#endif

// Properties
void Game::GetDefaultSize(int& width, int& height) const
{
    width = 800;
    height = 600;
}
#pragma endregion

#pragma region Direct3D Resources
// These are the resources that depend on the device.
void Game::CreateDeviceDependentResources()
{
    auto context = m_deviceResources->GetD3DDeviceContext();
    auto device = m_deviceResources->GetD3DDevice();

    m_states = std::make_unique<CommonStates>(device);
    m_fxFactory = std::make_unique<EffectFactory>(device);
    m_sprites = std::make_unique<SpriteBatch>(context);
    m_font = std::make_unique<SpriteFont>(device, L"SegoeUI_18.spritefont");
    m_batch = std::make_unique<PrimitiveBatch<VertexPositionColor>>(context);
    m_FirstRenderPass = new RenderTexture(device, 960, 600, 1, 2);

    //setup our terrain
    m_climateMap.Initialize(m_terrainWidth, m_terrainWidth);
   // m_generatedClimateMapTexture = m_climateMap.GenerateClimateMapTexture(device);
    m_terrain.SetClimateMap(m_climateMap.GetClimateMap());
    m_terrain.Initialize(device, m_terrainWidth, m_terrainWidth, m_terrainScale);

   
    *m_poissonDiscSampling.GetSampleRegionSize() = m_terrainWidth - 1;
    m_water.Initialize(device, m_terrainWidth, m_terrainWidth, m_terrainScale);
    //setup map of climate over the terrain


    //load and set up our Vertex and Pixel Shaders
    m_terrainShader.InitStandard(device, L"terrain_vs.cso", L"terrain_ps.cso");
    m_waterShader.InitStandard(device, L"water_vs.cso", L"water_gs.cso", L"water_ps.cso");
    m_geometryShader.InitStandard(device, L"instancedObj_vs.cso", L"instancedObj_gs.cso", L"instancedObj_ps.cso");
    m_standardShader.InitStandard(device, L"object_vs.cso", L"object_ps.cso");

    //load Textures
    
    m_terrainLoader.Initialise(device, m_terrainWidth-1);
    m_terrainMap = m_terrainLoader.GetTerrainMap();
    m_noiseTexture = m_climateMap.GenerateNoiseTexture(device);
    //DesertBiome    
    SetupDesertBiome(device);

    //Forest Biome 
    SetupForestBiome(device);

    //Snow Biome 
    SetupSnowBiome(device);

   // m_depthTexture = m_depthTextureClass.GenerateDepthTexture(device);
}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateWindowSizeDependentResources()
{
    auto size = m_deviceResources->GetOutputSize();
    float aspectRatio = float(size.right) / float(size.bottom);
    float fovAngleY = 70.0f * XM_PI / 180.0f;

    // This is a simple example of change that can be made when the app is in
    // portrait or snapped view.
    if (aspectRatio < 1.0f)
    {
        fovAngleY *= 2.0f;
    }

    // This sample makes use of a right-handed coordinate system using row-major matrices.
    m_projection = Matrix::CreatePerspectiveFieldOfView(
        fovAngleY,
        aspectRatio,
        0.01f,
        500.0f
    );
}



void Game::SetupGUI()
{

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Noise Texture Param");
    ImGui::SliderFloat("Temp Amplitude", m_climateMap.GetTemperatureAmplitude(), 0.0f, 1.f);
    ImGui::SliderFloat("Temp Frequency", m_climateMap.GetTemperatureFrequency(), 0.0f, 0.05f);
    ImGui::SliderFloat("Temp Offset", m_climateMap.GetTemperatureOffset(), 0.0f, 100.f);

    ImGui::SliderFloat("Rainfall Amplitude", m_climateMap.GetRainfallAmplitude(), 0.0f, 1.f);
    ImGui::SliderFloat("Rainfall Frequency", m_climateMap.GetRainfallFrequency(), 0.0f, 0.05f);
    ImGui::SliderFloat("Rainfall Offset", m_climateMap.GetRainfallOffset(), 0.0f, 100.f);
    ImGui::SliderFloat("Positional Offset X", m_climateMap.GetPositionalOffsetX(), -100.f, 100.);
    ImGui::SliderFloat("Positional Offset Z", m_climateMap.GetPositionalOffsetZ(), -100.f, 100.f);

    ImGui::ColorEdit3("Diffuse Light Colour", m_diffuseLight);
    ImGui::ColorEdit3("Ambient Light Colour", m_ambientLight);
    ImGui::ColorEdit3("Desert Colour", m_terrainShader.SetDesertColour());
    ImGui::ColorEdit3("Forest Colour", m_terrainShader.SetForestColour());
    ImGui::ColorEdit3("Snow Colour", m_terrainShader.SetSnowColour());

    ImGui::End();

    ImGui::Begin("Poisson Disk Sample Params");

    ImGui::SliderInt("Number of samples before rejection", m_poissonDiscSampling.GetNumSamplesBeforeRejection(), 1, 60);
    ImGui::SliderFloat("Radius", m_poissonDiscSampling.GetRadius(), 0.0f, 30.0f);
    ImGui::End();

    ImGui::SetNextWindowSize(ImVec2(450, 320));
    ImGui::Begin("Procedural Terrain Parameters");
    ImGui::SliderFloat("Amplitude", m_terrain.GetAmplitude(), 0.0f, 10.0f);
    ImGui::SliderFloat("Frequency", m_terrain.GetFrequency(), 0.0f, 2.0f);
    ImGui::SliderFloat("Lacunarity", m_terrain.GetLacunarity(), 0.0f, 1.0f);
    ImGui::SliderFloat("Persistance", m_terrain.GetPersistance(), 0.0f, 2.5f);
    ImGui::SliderInt("Octaves", m_terrain.GetOctaves(), 1.0f, 10.0f);
    ImGui::End();

}


void Game::SetupDesertBiome(ID3D11Device* device)
{
    CreateDDSTextureFromFile(device, L"desert_colours.dds", nullptr, m_desertTexture.ReleaseAndGetAddressOf());
    CreateDDSTextureFromFile(device, L"desert2.dds", nullptr, m_desertTexture2.ReleaseAndGetAddressOf());

    m_desertCactus.InitializeModel(device, "desert_cactus.obj", m_desertTexture);
    m_desertCactus2.InitializeModel(device, "desert_cactus2.obj", m_desertTexture);
    m_desertCactus3.InitializeModel(device, "desert_cactus3.obj", m_desertTexture2);
    m_desertCactus4.InitializeModel(device, "desert_cactus4.obj", m_desertTexture2);

    m_desertCactus.SetPlacementPercentage(15);
    m_desertCactus2.SetPlacementPercentage(15);
    m_desertAloe.SetPlacementPercentage(15);
    m_desertRock.SetPlacementPercentage(15);

    m_desertAloe.InitializeModel(device, "desert_aloe.obj", m_desertTexture2);
    m_desertRock.InitializeModel(device, "desert_rock.obj", m_desertTexture);



    m_biomeObjects.SetIsSmall(false);
    m_biomeObjects.AddToObjects(m_entityData.AddToMap(m_desertCactus), 0);
    m_biomeObjects.AddToObjects(m_entityData.AddToMap(m_desertCactus2), 0);
    m_biomeObjects.AddToObjects(m_entityData.AddToMap(m_desertAloe), 0);
    m_biomeObjects.AddToObjects(m_entityData.AddToMap(m_desertRock), 0);
    //m_biomeObjects.AddToObjects(m_entityData.AddToMap(m_desertCactus3), 0);
  //  m_biomeObjects.AddToObjects(m_entityData.AddToMap(m_desertCactus4), 0);
    m_biomeObjects.SetIsSmall(true);



}
void Game::SetupForestBiome(ID3D11Device* device)
{
    CreateDDSTextureFromFile(device, L"forest_tree_cold.dds", nullptr, m_forestTreeColdTexture.ReleaseAndGetAddressOf());
    CreateDDSTextureFromFile(device, L"forest_grass.dds", nullptr, m_forestGrassTexture.ReleaseAndGetAddressOf());
    CreateDDSTextureFromFile(device, L"forest_flowers.dds", nullptr, m_forestColourPalletTexture.ReleaseAndGetAddressOf());
    CreateDDSTextureFromFile(device, L"forest_details.dds", nullptr, m_forestDetailsTexture.ReleaseAndGetAddressOf());

    m_forestTreeModel.InitializeModel(device, "forest_tree.obj", m_forestTreeColdTexture);
    m_forestGrassModel.InitializeModel(device, "forest_grass.obj", m_forestGrassTexture);
    m_forestGrassModel2.InitializeModel(device, "forest_grass2.obj", m_forestGrassTexture);
    m_forestGrassModel3.InitializeModel(device, "forest_grass3.obj", m_forestGrassTexture);
    m_forestFlowerModel.InitializeModel(device, "forest_flower.obj", m_forestColourPalletTexture);
    m_forestMushroomModel.InitializeModel(device, "forest_mushroom.obj", m_forestColourPalletTexture);
    m_forestMushroomModel2.InitializeModel(device, "forest_mushroom2.obj", m_forestColourPalletTexture);

    m_forestGrassRockModel.InitializeModel(device, "forest_grass_rock.obj", m_forestDetailsTexture);

    m_biomeObjects.SetIsSmall(false);
    m_biomeObjects.AddToObjects(m_entityData.AddToMap(m_forestTreeModel), 1);
    m_biomeObjects.SetIsSmall(true);
    m_biomeObjects.AddToObjects(m_entityData.AddToMap(m_forestGrassModel), 1);
    m_biomeObjects.AddToObjects(m_entityData.AddToMap(m_forestGrassModel2), 1);
    m_biomeObjects.AddToObjects(m_entityData.AddToMap(m_forestGrassModel3), 1);
    m_biomeObjects.AddToObjects(m_entityData.AddToMap(m_forestMushroomModel), 1);
    m_biomeObjects.AddToObjects(m_entityData.AddToMap(m_forestMushroomModel2), 1);



    // m_biomeObjects.AddToObjects(m_entityData.AddToMap(m_forestFlowerModel), 1);
    // m_biomeObjects.AddToObjects(m_entityData.AddToMap(m_forestGrassRockModel), 1);
}



void Game::SetupSnowBiome(ID3D11Device* device)
{
    m_biomeObjects.SetIsSmall(false);
    CreateDDSTextureFromFile(device, L"snowTreeTex.dds", nullptr, m_snowTreeTextures.ReleaseAndGetAddressOf());
    m_snowTreeModel.InitializeModel(device, "snowTree.obj", m_snowTreeTextures);
    m_snowTreeModel.SetPlacementPercentage(20);
    m_biomeObjects.AddToObjects(m_entityData.AddToMap(m_snowTreeModel), 2);
    m_biomeObjects.SetIsSmall(true);
    m_biomeObjects.AddToObjects(m_entityData.AddToMap(m_forestGrassModel2), 2);
}

void Game::GenerateClimate(ID3D11Device* device)
{
    m_climateMap.GenerateClimateMap(0, 0);
    m_biomeObjects.SetClimateMap(m_climateMap.GetClimateMap());
    m_terrain.SetClimateMap(m_climateMap.GetClimateMap());
   // m_generatedClimateMapTexture = m_climateMap.GenerateClimateMapTexture(device);

}

void Game::GenerateTerrain(ID3D11Device* device)
{
    m_terrain.GenerateHeightMap(device, m_terrainScale, 0, 0);
    m_barycentricCoordinates.SetHeightMap(m_terrain.GetHeightMap());
}

void Game::SetupModelPositions(ID3D11Device* device)
{
    m_entityData.ClearModelBuffers();
    m_poissonPositionsBigObjects.clear();
    m_poissonPositionsSmallObjects.clear();
    m_poissonDiscSampling.GenerateAllPoints(3, 1.5f, 2, 0.3f);
    m_poissonPositionsBigObjects = m_poissonDiscSampling.GetBigObjPoints();
    m_poissonPositionsSmallObjects = m_poissonDiscSampling.GetSmallObjPoints();
    m_biomeObjects.SetIsSmall(false);
    m_biomeObjects.SetupObjectsAccordingToBiomes(m_poissonPositionsBigObjects, m_terrainWidth, m_terrainScale);
    m_biomeObjects.SetIsSmall(true);
    m_biomeObjects.SetupObjectsAccordingToBiomes(m_poissonPositionsSmallObjects, m_terrainWidth, m_terrainScale);
    m_entityData.SetupModelBuffers(device);
}

void Game::OnDeviceLost()
{
    m_states.reset();
    m_fxFactory.reset();
    m_sprites.reset();
    m_font.reset();
    m_batch.reset();
    m_testmodel.reset();
    m_batchInputLayout.Reset();
    m_raster.Reset();
}

void Game::OnDeviceRestored()
{
    CreateDeviceDependentResources();
    CreateWindowSizeDependentResources();
}
#pragma endregion
