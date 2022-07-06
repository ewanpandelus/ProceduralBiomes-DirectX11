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

    m_regionSize = 80;
    m_input.Initialise(window);
    m_climateMap.Initialize(128, 128);
    m_poissonDiscSampling  =  PoissonDiscSampling(m_biomeObjects);

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
    m_elapsedTime += m_timer.GetElapsedSeconds();
    Vector3 rotation = m_Camera01.getRotation();


    if (m_gameInputCommands.rotX)
    {
        float deltaY = m_input.GetDeltaY();
        rotation.x = rotation.x -= m_Camera01.getRotationSpeed() * deltaY;
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
    if (m_gameInputCommands.rotY)
    {
        float deltaX = m_input.GetDeltaX();
        rotation.y = rotation.y -= m_Camera01.getRotationSpeed() * deltaX;
        m_Camera01.setRotation(rotation);
    }


    //note that currently.  Delta-time is not considered in the game object movement. 
    /*if (m_gameInputCommands.left)
    {

        rotation.y = rotation.y += m_Camera01.getRotationSpeed();
        m_Camera01.setRotation(rotation);
    }
    if (m_gameInputCommands.right)
    {
        Vector3 rotation = m_Camera01.getRotation();
        rotation.x = rotation.x -= m_Camera01.getRotationSpeed();
        m_Camera01.setRotation(rotation);
    }*/
    if (m_gameInputCommands.forward)
    {
        Vector3 position = m_Camera01.getPosition(); //get the position
        position += (m_Camera01.getForward() * m_Camera01.getMoveSpeed()); //add the forward vector
        m_Camera01.setPosition(position);
    }
    if (m_gameInputCommands.back)
    {
        Vector3 position = m_Camera01.getPosition(); //get the position
        position -= (m_Camera01.getForward() * m_Camera01.getMoveSpeed()); //add the forward vector
        m_Camera01.setPosition(position);
    }

    if (m_gameInputCommands.generate && m_elapsedTime > 0.1f)
    {
        m_elapsedTime = 0;
        m_flickBetweenMaps = !m_flickBetweenMaps;
        m_poissonPositions.clear();
        m_poissonPositions = m_poissonDiscSampling.GeneratePoints();
        m_regionSize = *m_poissonDiscSampling.GetSampleRegionSize();
       // m_Terrain.GenerateHeightMap(device);

        m_objectMap.clear();
        m_biomeObjects.SetClimateMap(m_climateMap.GenerateClimateMap());
        m_biomeObjects.SetCellWidth(m_poissonDiscSampling.GetCellWidth());
        m_objectMap =  m_biomeObjects.SetupObjectsAccordingToBiomes(m_poissonPositions);
        
        
        m_generatedClimateMapTexture = m_climateMap.GenerateClimateMapTexture(device);


    }

    m_Camera01.Update();	//camera update.
    m_Terrain.Update();		//terrain update.  doesnt do anything at the moment. 

    m_view = m_Camera01.getCameraMatrix();
    m_world = Matrix::Identity;

    /*create our UI*/
    SetupGUI();

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
    // Don't try to render anything before the first Update.
    if (m_timer.GetFrameCount() == 0)
    {
        return;
    }

    Clear();

    m_deviceResources->PIXBeginEvent(L"Render");
    auto context = m_deviceResources->GetD3DDeviceContext();
    auto renderTargetView = m_deviceResources->GetRenderTargetView();
    auto depthTargetView = m_deviceResources->GetDepthStencilView();

    // Draw Text to the screen
    m_sprites->Begin();
    m_font->DrawString(m_sprites.get(), L"Procedural Biomes", XMFLOAT2(10, 10), Colors::Yellow);
    m_sprites->End();

    //Set Rendering states. 
    context->OMSetBlendState(m_states->Opaque(), nullptr, 0xFFFFFFFF);
    context->OMSetDepthStencilState(m_states->DepthDefault(), 0);
    context->RSSetState(m_states->CullClockwise());
    //	context->RSSetState(m_states->Wireframe());

        //prepare transform for floor object. 
    m_world = SimpleMath::Matrix::Identity; //set world back to identity

    SimpleMath::Matrix objectPosition = SimpleMath::Matrix::CreateTranslation(0, 0, 0);
   // m_standardShader.EnableShader(context);
    geometryShader.EnableShader(context);
    float x = 0;
    float y = 0;
    float index = 0;
    SimpleMath::Matrix objectScale = SimpleMath::Matrix::CreateScale(1);
    float time = m_timer.GetTotalSeconds();
    for each (auto position in m_objectMap)
    {
        m_world = SimpleMath::Matrix::Identity; //set world back to identity
        x = m_objectMap[index].x;
        y = m_objectMap[index].z;
        objectPosition = SimpleMath::Matrix::CreateTranslation(x - m_regionSize / 2, 0.0f, y - m_regionSize / 2);
        m_world = m_world * objectPosition * objectScale;
        geometryShader.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light,
            m_objectMap[index].texture.Get(), m_timer.GetTotalSeconds());

        m_objectMap[index].model.Render(context);
        index++;

    }
    m_terrainShader.EnableShader(context);
    m_world = SimpleMath::Matrix::Identity; //set world back to identity
    SimpleMath::Matrix positionAccountedFor = SimpleMath::Matrix::CreateTranslation(-64, 0.f, -64);
    SimpleMath::Matrix newScale = SimpleMath::Matrix::CreateScale(1);		//scale the terrain down a little. 
    m_world = m_world * positionAccountedFor;
    m_terrainShader.SetBiomeShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light,
        m_generatedClimateMapTexture.Get(), m_desertTexture.Get(),m_desert2Texture.Get(),  m_grassTexture.Get(),m_snowTexture.Get(), m_noiseTexture.Get());
    m_Terrain.Render(context);


    //mesh->sendData(context);
   
    //geometryShader->render(context, mesh->getIndexCount());


    //render our GUI
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());


    // Show the new frame.
    m_deviceResources->Present();
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

    //setup our terrain
    m_Terrain.Initialize(device, 128, 128);

    //setup our test model
    m_BasicModel.InitializeSphere(device);
    m_desertModel.InitializeModel(device,"drone.obj");

    

    m_BasicModel3.InitializeBox(device, 10.0f, 0.1f, 10.0f);	//box includes dimensions
    
    //load and set up our Vertex and Pixel Shaders
    m_terrainShader.InitStandard(device, L"terrain_vs.cso", L"terrain_ps.cso");
    geometryShader.InitStandard(device, L"triangle_vs.cso", L"triangle_gs.cso", L"triangle_ps.cso");
    m_standardShader.InitStandard(device, L"object_vs.cso", L"object_ps.cso");
    //load Textures
    m_generatedClimateMapTexture = m_climateMap.GenerateClimateMapTexture(device);
    m_noiseTexture = m_climateMap.GenerateNoiseTexture(device);
    //DesertBiome    

    CreateDDSTextureFromFile(device, L"desert.dds", nullptr, m_desertTexture.ReleaseAndGetAddressOf());
    CreateDDSTextureFromFile(device, L"desert2.dds", nullptr, m_desert2Texture.ReleaseAndGetAddressOf());

    //Forest Biome 
    CreateDDSTextureFromFile(device, L"grass.dds", nullptr, m_grassTexture.ReleaseAndGetAddressOf());
    CreateDDSTextureFromFile(device, L"tree.dds", nullptr, m_forestTreeTexture.ReleaseAndGetAddressOf());
    m_forestTreeModel.InitializeModel(device, "tree.obj");

    //Snow Biome 
    CreateDDSTextureFromFile(device, L"snowTreeTex.dds", nullptr, m_snowTreeTextures.ReleaseAndGetAddressOf());
    CreateDDSTextureFromFile(device, L"snow.dds", nullptr, m_snowTexture.ReleaseAndGetAddressOf());
    m_snowTreeModel.InitializeModel(device, "snowTree.obj");

    m_biomeObjects.SetLargeModels(m_desertModel, m_forestTreeModel, m_snowTreeModel);
    m_biomeObjects.SetLargeModelsTextures(m_desertTexture, m_forestTreeTexture,m_snowTexture);


    //Initialise Render to texture
    m_FirstRenderPass = new RenderTexture(device, 800, 600, 1, 2);	//for our rendering, We dont use the last two properties. but.  they cant be zero and they cant be the same. 

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
    ImGui::SliderFloat("Temp Frequency", m_climateMap.GetTemperatureFrequency(), 0.0f, 0.25f);
    ImGui::SliderFloat("Temp Offset", m_climateMap.GetTemperatureOffset(), 0.0f, 100.f);

    ImGui::SliderFloat("Rainfall Amplitude", m_climateMap.GetRainfallAmplitude(), 0.0f, 1.f);
    ImGui::SliderFloat("Rainfall Frequency", m_climateMap.GetRainfallFrequency(), 0.0f, 0.25f);
    ImGui::SliderFloat("Rainfall Offset", m_climateMap.GetRainfallOffset(), 0.0f, 100.f);


    ImGui::ColorEdit3("Diffuse Light Colour", m_diffuseLight);
    ImGui::ColorEdit3("Ambient Light Colour", m_ambientLight);
    ImGui::End();

    ImGui::Begin("Poisson Disk Sample Params");

    ImGui::SliderInt("Number of samples before rejection", m_poissonDiscSampling.GetNumSamplesBeforeRejection(), 1, 60);
    ImGui::SliderFloat("Radius", m_poissonDiscSampling.GetRadius(), 0.0f, 30.0f);
    ImGui::End();

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
}

void Game::OnDeviceRestored()
{
    CreateDeviceDependentResources();
    CreateWindowSizeDependentResources();
}
#pragma endregion
