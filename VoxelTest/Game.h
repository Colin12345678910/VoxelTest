//
// Game.h
//

#pragma once

#include "DeviceResources.h"
#include "StepTimer.h"
#include "Mesh.h"
#include <d3d11.h>
#include <SimpleMath.h>
#include <Keyboard.h>
#include <Mouse.h>
#include "Noise.h"
#include "World.h"
#include "WorldRenderer.h"
#include <thread>

using namespace DirectX::SimpleMath;

// A basic game implementation that creates a D3D11 device and
// provides a game loop.
class Game final : public DX::IDeviceNotify
{
public:
        
    static bool gameQuit;
    //DirectX::SimpleMath::
    
    Matrix viewMatrix;
    Matrix projectionMatrix;

    Game() noexcept(false);
    ~Game() = default;

    Game(Game&&) = default;
    Game& operator= (Game&&) = default;

    Game(Game const&) = delete;
    Game& operator= (Game const&) = delete;

    // Initialization and management
    void Initialize(HWND window, int width, int height);

    // Basic game loop
    void Tick();

    // IDeviceNotify
    void OnDeviceLost() override;
    void OnDeviceRestored() override;

    // Messages
    void OnActivated();
    void OnDeactivated();
    void OnSuspending();
    void OnResuming();
    void OnWindowMoved();
    void OnDisplayChange();
    void OnWindowSizeChanged(int width, int height);
    Vector3 cameraPos = Vector3(0, 150, 0);
    // Properties
    void GetDefaultSize( int& width, int& height ) const noexcept;

    void CleanUp();

private:

    void Update(DX::StepTimer const& timer);
    void Render();
    Matrix mat;
    void Clear();
    void ComputeViewProj();
    void CreateDeviceDependentResources();
    void CreateWindowSizeDependentResources();
    void HandleKeyAndMouse(float deltaTime);
    
    void ThreadedWorldGen(int y);

    std::unique_ptr<DirectX::Keyboard> m_keyboard;
    std::unique_ptr<DirectX::Mouse> m_mouse;

    double pitch = 0;
    double yaw = 0;

    Vector3 look;
    //Noise perlin;
    Keyboard::State kb;
    Mouse::State mouse;
    World world;
    WorldRenderer renderer;
    
    //Mesh mesh;
    //Mesh* mChunks[32][32];
    //Chunk* chunks[32][32];
    

    // Device resources.
    std::unique_ptr<DX::DeviceResources>    m_deviceResources;

    //Threads (For Generation
    static const int THREAD_COUNT = 1;
    std::jthread threadPool[THREAD_COUNT];
    
    
    // Rendering loop timer.
    DX::StepTimer                           m_timer;
};
