//
// Game.cpp
//

#include "pch.h"
#include "Game.h"
#include "Main.h"
#include "Mesh.h"


extern void ExitGame() noexcept;

using namespace DirectX;

using Microsoft::WRL::ComPtr;

bool Game::gameQuit = false;

Game::Game() noexcept(false)
{
    m_deviceResources = std::make_unique<DX::DeviceResources>();
    // TODO: Provide parameters for swapchain format, depth/stencil format, and backbuffer count.
    //   Add DX::DeviceResources::c_AllowTearing to opt-in to variable rate displays.
    //   Add DX::DeviceResources::c_EnableHDR for HDR10 display.
    m_deviceResources->RegisterDeviceNotify(this);
}

// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND window, int width, int height)
{
    m_deviceResources->SetWindow(window, width, height);
    Main::windowX = width;
    Main::windowY = height;
    m_deviceResources->CreateDeviceResources();
    CreateDeviceDependentResources();

    m_deviceResources->CreateWindowSizeDependentResources();
    CreateWindowSizeDependentResources();

    auto context = m_deviceResources->GetD3DDeviceContext();
    auto device = m_deviceResources->GetD3DDevice();

    renderer.Initialize(context, device);
    //--NoiseData
    world.Initalize();

    for (int i = 0; i < THREAD_COUNT; i++) {
        threadPool[i] = std::jthread(&Game::ThreadedWorldGen, this, i);
    }

    //for (int y = 0; y < 16; y++) {
    //    for (int x = 0; x < 16; x++) {
    //        chunks[x][y] = new Chunk();
    //        chunks[x][y]->FillBlocks(perlin, Vector2(y, x));
    //        mChunks[x][y] = new Mesh();
    //        mChunks[x][y]->CreateChunkMesh(chunks[x][y], device, context);
    //        mChunks[x][y]->hasCompleted = true;
    //    }
    //}

   

    
    // TODO: Change the timer settings if you want something other than the default variable timestep mode.
    // e.g. for 60 FPS fixed timestep update logic, call:
    /*
    m_timer.SetFixedTimeStep(true);
    m_timer.SetTargetElapsedSeconds(1.0 / 60);
    */

    //mouse&K
    m_keyboard = std::make_unique<Keyboard>();
    m_mouse = std::make_unique<Mouse>();
    m_mouse->SetWindow(window);

    
}


#pragma region Frame Update
// Executes the basic game loop.
void Game::Tick()
{
    m_timer.Tick([&]()
    {
        Update(m_timer);
    });

    Render();
}

// Updates the world.
void Game::Update(DX::StepTimer const& timer)
{
    float elapsedTime = float(timer.GetElapsedSeconds());

    // TODO: Add your game logic here.
    elapsedTime;

    Game::HandleKeyAndMouse(elapsedTime);
    world.Update((int)cameraPos.x / 32, (int)cameraPos.z / 32);
}
#pragma endregion

#pragma region Frame Render
// Draws the scene.
void Game::Render()
{
    ComputeViewProj();
    // Don't try to render anything before the first Update.
    if (m_timer.GetFrameCount() == 0)
    {
        return;
    }
   
    
    Clear();

    m_deviceResources->PIXBeginEvent(L"Render");
    auto context = m_deviceResources->GetD3DDeviceContext();
    auto device = m_deviceResources->GetD3DDevice();
    renderer.Render(viewMatrix, Matrix::Identity, projectionMatrix);
    // TODO: Add your rendering code here.
    //context;
    
    
    
    //for (int x = 0; x < 32; x++) {
    //    for (int y = 0; y < 32; y++) {
    //        mat = mat.CreateTranslation(Vector3(y * Chunk::depth, 0, x * Chunk::width));
    //        mChunks[x][y]->Draw(context, viewMatrix, mat, projectionMatrix);
    //    }
    //}
    //
    
    


    m_deviceResources->PIXEndEvent();

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
    auto const viewport = m_deviceResources->GetScreenViewport();
    context->RSSetViewports(1, &viewport);

    m_deviceResources->PIXEndEvent();
}
void Game::ComputeViewProj()
{
    double limit = XM_PIDIV2 - 0.01;
    pitch = std::max(-limit, pitch);
    pitch = std::min(limit, pitch);

    if (yaw > XM_PI) {
        yaw -= XM_2PI;
    }
    else if (yaw < -XM_PI) {
        yaw += XM_2PI;
    }
    
    double y = sinf(pitch);
    double r = cosf(pitch);
    double z = r * cosf(yaw);
    double x = r * sinf(yaw);

    look = cameraPos + Vector3(x, y, z);

    viewMatrix = Matrix::CreateLookAt(cameraPos, look, Vector3::UnitY);
    projectionMatrix = Matrix::CreatePerspectiveFieldOfView(60.0 * XM_PI / 180.0, Main::windowX / Main::windowY, 0.1, 1600);
}
#pragma endregion

#pragma region Message Handlers
// Message handlers
void Game::OnActivated()
{
    // TODO: Game is becoming active window.
}

void Game::OnDeactivated()
{
    // TODO: Game is becoming background window.
}

void Game::OnSuspending()
{
    // TODO: Game is being power-suspended (or minimized).
}

void Game::OnResuming()
{
    m_timer.ResetElapsedTime();

    // TODO: Game is being power-resumed (or returning from minimize).
}

void Game::OnWindowMoved()
{
    auto const r = m_deviceResources->GetOutputSize();
    m_deviceResources->WindowSizeChanged(r.right, r.bottom);
}

void Game::OnDisplayChange()
{
    m_deviceResources->UpdateColorSpace();
}

void Game::OnWindowSizeChanged(int width, int height)
{
    if (!m_deviceResources->WindowSizeChanged(width, height))
        return;
    Main::windowX = width;
    Main::windowY = height;

    CreateWindowSizeDependentResources();

    

    // TODO: Game window is being resized.
}

// Properties
void Game::GetDefaultSize(int& width, int& height) const noexcept
{
    // TODO: Change to desired default window size (note minimum size is 320x200).
    width = 800;
    height = 600;
}
void Game::CleanUp()
{
    for (int i = 0; i < THREAD_COUNT; i++)
    {
        gameQuit = true;
        threadPool[i].request_stop();
        WorldRenderer::End();
        World::ToggleWorldGen();
    }
}
#pragma endregion

#pragma region Direct3D Resources
// These are the resources that depend on the device.
void Game::CreateDeviceDependentResources()
{
    auto device = m_deviceResources->GetD3DDevice();

    // TODO: Initialize device dependent objects here (independent of window size).
    device;
}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateWindowSizeDependentResources()
{
    // TODO: Initialize windows-size dependent objects here.
}

void Game::HandleKeyAndMouse(float deltaTime)
{
    m_mouse->SetMode(Mouse::MODE_RELATIVE);
    kb = m_keyboard->GetState();
    mouse = m_mouse->GetState();


    if (kb.Escape) {
        ExitGame();
    }
   

    //if (mouse.leftButton) {
    //    m_mouse->SetMode(Mouse::MODE_RELATIVE);
    //}
    //else {
    //    m_mouse->SetMode(Mouse::MODE_ABSOLUTE);
    //}
    
    //
    pitch -= mouse.y * 0.001;
    yaw -= mouse.x * 0.001;

    Vector3 move = Vector3::Zero;
    if (kb.W)
    {
        move.z += 1.f;
    }
    if (kb.S)
    {
        move.z -= 1.f;
    }
    if (kb.E)
    {
        cameraPos.x += 500000;
    }
    if (kb.R)
    {
        ChunkMesh* ch = &World::GetChunks()->front();
        if (!ch->GetState() == ChunkMesh::Deallocated)
        {
            ch->mesh->MarkDirty();
        }
        
        //chunk.Release();
        //World::GetChunks().front().chunk->SetBlock(0, 200, 0, 1);
        //World::GetChunks().front().mesh->GenerateMesh(World::GetChunks().front().chunk);
    }
    Quaternion q = Quaternion::CreateFromYawPitchRoll(yaw, -pitch, 0);
    move.z -= 1.f;
    move = Vector3::Transform(move, q);
    move *= (double)deltaTime * 100;

    cameraPos += move;
}

void Game::ThreadedWorldGen(int y)
{
    auto context = m_deviceResources->GetD3DDeviceContext();
    auto device = m_deviceResources->GetD3DDevice();
    while (true) 
    {
        if (gameQuit)
            break;
        
    }
    

    /*for (int y = 0; y < 16; y++) {*/
        //for (int x = 0; x < 32; x++) {
        //    if (gameQuit)
        //        break;
        //    chunks[x][y]->FillBlocks(perlin, Vector2(y, x));
        //    
        //    mChunks[x][y]->CreateChunkMesh(chunks[x][y], device, context);
        //   
        //}
  /*  }*/
}


void Game::OnDeviceLost()
{
    // TODO: Add Direct3D resource cleanup here.
}

void Game::OnDeviceRestored()
{
    CreateDeviceDependentResources();

    CreateWindowSizeDependentResources();
}
#pragma endregion
