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
    
    D3D11_SAMPLER_DESC sampleDesc;
    sampleDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampleDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
    sampleDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
    sampleDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
    sampleDesc.MipLODBias = 0;
    sampleDesc.MaxAnisotropy = 1;
    sampleDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampleDesc.BorderColor[0] = 1.0f;
    sampleDesc.BorderColor[1] = 1.0f;
    sampleDesc.BorderColor[2] = 1.0f;
    sampleDesc.BorderColor[3] = 1.0f;

    DX::ErrorIfFailed(device->CreateSamplerState(&sampleDesc, &pSamplerState), L"Invalid Sampler state");

    /*
    * This is a bog standard depthDesc so we can unbreak the state after using a spritebatch.
    */
    D3D11_DEPTH_STENCIL_DESC depthDesc;
    depthDesc.DepthEnable = true;
    depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthDesc.DepthFunc = D3D11_COMPARISON_LESS;
    depthDesc.StencilEnable = FALSE;
    depthDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
    depthDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

    device->CreateDepthStencilState(&depthDesc, &pDepthStencilState);


    spriteBatch = std::make_unique<SpriteBatch>(context);
    shadowMapper.Init(device);
    LitVoxelShaderPS.LoadShader(device);

    renderer.Initialize(context, device, &LitVoxelShaderPS);
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
    
    // Don't try to render anything before the first Update.
    if (m_timer.GetFrameCount() == 0)
    {
        return;
    }
    Vector3 worldOrigin = cameraPos;
    worldOrigin.y = 128;
    viewMatrix = Matrix::CreateLookAt(Vector3(1000 + cameraPos.x, 1000 , 1000 + cameraPos.z), worldOrigin, Vector3::UnitY);
    
    projectionMatrix = Matrix::CreateOrthographic(512, 512, 1250, 3000);

    Matrix shadowC = (viewMatrix * projectionMatrix);



    m_deviceResources->PIXBeginEvent(L"ShadowMap");
    DXGI_FORMAT fo = m_deviceResources->GetBackBufferFormat();
    shadowMapper.RenderShadowMap(m_deviceResources->GetD3DDevice(), m_deviceResources->GetD3DDeviceContext(), m_deviceResources->GetDepthStencilView());
    renderer.Render(viewMatrix, Matrix::Identity, projectionMatrix, shadowC, false);
    
    m_deviceResources->PIXEndEvent();

    Clear();
    ComputeViewProj();
    
    auto context = m_deviceResources->GetD3DDeviceContext();
    auto device = m_deviceResources->GetD3DDevice();


    

    context->PSSetShaderResources(0, 1, shadowMapper.GetStencilShaderRV());
    context->PSSetSamplers(0, 1, &pSamplerState);
    
    

    m_deviceResources->PIXBeginEvent(L"Render");
    
    renderer.Render(viewMatrix, Matrix::Identity, projectionMatrix, shadowC, true);
    // TODO: Add your rendering code here.
    //context;



    spriteBatch->Begin();
    //(_In_ ID3D11ShaderResourceView* texture, XMFLOAT2 const& position, _In_opt_ RECT const* sourceRectangle, FXMVECTOR color = Colors::White, float rotation = 0, XMFLOAT2 const& origin = Float2Zero, float scale = 1, SpriteEffects effects = SpriteEffects_None, float layerDepth = 0);
    spriteBatch->Draw(shadowMapper.GetShaderResourceView(), XMFLOAT2(0, 0), NULL, Colors::White.v, 0.0f, XMFLOAT2(0, 0), 0.1f);
    spriteBatch->End();

    ID3D11ShaderResourceView* nullsrv[] = { nullptr };
    context->PSSetShaderResources(0, 1, nullsrv);
    
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


    //Revert the depthState
    context->OMSetDepthStencilState(pDepthStencilState, 1);
}

// Helper method to clear the back buffers.
void Game::Clear()
{
    m_deviceResources->PIXBeginEvent(L"Clear");

    // Clear the views.
    auto context = m_deviceResources->GetD3DDeviceContext();
    auto device = m_deviceResources->GetD3DDevice();
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
    
    double y = sin(pitch);
    double r = cos(pitch);
    double z = r * cos(yaw);
    double x = r * sin(yaw);

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

    DX::ThrowIfFailed(
        CreateWICTextureFromFile(device, L"C:\\Users\\Linka\\Documents\\NoTex.png", nullptr,
            m_texture.ReleaseAndGetAddressOf()));
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
        if (ch->GetState() != ChunkMesh::Deallocated)
        {
            ch->chunk->SetBlock(rand() % 32, rand() % 256, rand() % 32, 1);
         
            //delete ch->mesh;
            ch->mesh->Release(); //It's important to release the mesh before re-creating it to avoid a memory leak.
            ch->mesh->CreateChunkMesh(ch->chunk, m_deviceResources->GetD3DDevice(), m_deviceResources->GetD3DDeviceContext(), &LitVoxelShaderPS);
        }
        
        //chunk.Release();
        //World::GetChunks().front().chunk->SetBlock(0, 200, 0, 1);
        //World::GetChunks().front().mesh->GenerateMesh(World::GetChunks().front().chunk);
    }
    Quaternion q = Quaternion::CreateFromYawPitchRoll(yaw, -pitch, 0);
    //move.z -= 1.f;
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
