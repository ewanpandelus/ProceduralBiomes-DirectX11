//
// Game.h
//
#pragma once

#include "DeviceResources.h"
#include "StepTimer.h"
#include "modelclass.h"
#include "Light.h"
#include "Input.h"
#include "Camera.h"
#include "RenderTexture.h"
#include "Terrain.h"
#include "Water.h"
#include "PoissonDiscSampling.h"
#include "DepthTexture.h"
#include "TerrainShader.h"
#include "Shader.h"
#include "ClimateMap.h"
#include "BiomeObjects.h"
#include "WaterShader.h"



// A basic game implementation that creates a D3D11 device and
// provides a game loop.
class Game final : public DX::IDeviceNotify
{
public:

    Game() noexcept(false);
    ~Game();

    // Initialization and management
    void Initialize(HWND window, int width, int height);

    // Basic game loop
    void Tick();

    // IDeviceNotify
    virtual void OnDeviceLost() override;
    virtual void OnDeviceRestored() override;

    // Messages
    void OnActivated();
    void OnDeactivated();
    void OnSuspending();
    void OnResuming();
    void OnWindowMoved();
    void OnWindowSizeChanged(int width, int height);
#ifdef DXTK_AUDIO
    void NewAudioDevice();
#endif

    // Properties
    void GetDefaultSize( int& width, int& height ) const;
	
private:

	struct MatrixBufferType
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX projection;
	}; 

    void Update(DX::StepTimer const& timer);
    void Render();
    void Clear();
    void CreateDeviceDependentResources();
    void CreateWindowSizeDependentResources();
    void GenerateBiomes(ID3D11Device* device);
	void SetupGUI();
    void SetupDesertBiome(ID3D11Device* device);
    void SetupForestBiome(ID3D11Device* device);
    void SetupSnowBiome(ID3D11Device* device);


    // Device resources.
    std::unique_ptr<DX::DeviceResources>    m_deviceResources;

    // Rendering loop timer.
    DX::StepTimer                           m_timer;

	//input manager. 
	Input									m_input;
	InputCommands							m_gameInputCommands;

    // DirectXTK objects.
    std::unique_ptr<DirectX::CommonStates>                                  m_states;
    std::unique_ptr<DirectX::BasicEffect>                                   m_batchEffect;	
    std::unique_ptr<DirectX::EffectFactory>                                 m_fxFactory;
    std::unique_ptr<DirectX::SpriteBatch>                                   m_sprites;
    std::unique_ptr<DirectX::SpriteFont>                                    m_font;

	// Scene Objects
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>>  m_batch;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>                               m_batchInputLayout;
	std::unique_ptr<DirectX::GeometricPrimitive>                            m_testmodel;

	//lights
	Light																	m_Light;

	//Cameras
	Camera																	m_Camera01;

	//textures 
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_generatedClimateMapTexture;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_generatedRainfallMapTexture;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_noiseTexture;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_depthTexture;








    //Snow
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_snowTreeTextures;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_snowTexture;





    //Lighting 
    float                                                                  m_diffuseLight[3];
    float                                                                  m_ambientLight[3];

	//Shaders
	TerrainShader														    m_terrainShader;
    Shader														            m_standardShader;
    WaterShader														        m_waterShader;
    Shader                                                                  m_geometryShader;
  


	//Scene. 
    Terrain																	m_terrain;
    Water																	m_water;;
 




    //Desert Models
	ModelClass																m_desertCactus;
    ModelClass																m_desesrtCactus2;
    ModelClass																m_desertCactus3;
    ModelClass																m_desertCactus4;
    ModelClass																m_desertAloe;



    ModelClass																m_desertRock;

    //Desert Textures
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_desertTexture;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_desertTexture2;




    //Forest Models
    ModelClass																m_forestTreeModel;
    ModelClass																m_forestGrassModel;
    ModelClass																m_forestGrassModel2;
    ModelClass																m_forestGrassModel3;
    ModelClass																m_forestGrassRockModel;




    ModelClass																m_forestFlowerModel;

    //Forest Textures
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_forestTreeColdTexture;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_forestTreeColdTexture2;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_forestGrassTexture;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_forestDetailsTexture;




    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_forestColourPalletTexture;


    //Snow Biome 
    ModelClass																m_snowTreeModel;

	//RenderTextures
	RenderTexture*															m_FirstRenderPass;
	RECT																	m_fullscreenRect;
	RECT																	m_CameraViewRect;
  


#ifdef DXTK_AUDIO
    std::unique_ptr<DirectX::AudioEngine>                                   m_audEngine;
    std::unique_ptr<DirectX::WaveBank>                                      m_waveBank;
    std::unique_ptr<DirectX::SoundEffect>                                   m_soundEffect;
    std::unique_ptr<DirectX::SoundEffectInstance>                           m_effect1;
    std::unique_ptr<DirectX::SoundEffectInstance>                           m_effect2;
#endif
    

#ifdef DXTK_AUDIO
    uint32_t                                                                m_audioEvent;
    float                                                                   m_audioTimerAcc;

    bool                                                                    m_retryDefault;
#endif

    DirectX::SimpleMath::Matrix                                             m_world;
    DirectX::SimpleMath::Matrix                                             m_view;
    DirectX::SimpleMath::Matrix                                             m_projection;

    BiomeObjects                                                            m_biomeObjects;
    std::vector<BiomeObjects::BiomeObjectType>                              m_objectMap;
    DepthTexture                                                            m_depthTextureClass;
    ClimateMap                                                              m_climateMap;
    PoissonDiscSampling                                                     m_poissonDiscSampling;



    float                                                                   m_elapsedTime = 0;

    std::vector<SimpleMath::Vector2>                                        m_poissonPositions;
    int                                                                     m_regionSize;
    int                                                                     m_terrainWidth = 64;

    float xOffset = -63.4f;
    float zOffset = -63.0f;
 
};