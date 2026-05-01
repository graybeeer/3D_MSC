#include <windows.h>
#include <d3d12.h>
#include "d3dx12.h"
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <string>
#include <vector>
#include <stdexcept>
#include "framework.h"
#include "Labtest.h"


#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

using namespace DirectX;
using Microsoft::WRL::ComPtr;

// 화면 크기
const int width = 800;
const int height = 600;
const int frameCount = 2;

// 정점 구조체
struct Vertex {
    XMFLOAT3 position;
    XMFLOAT4 color;
};

// 상수 버퍼 구조체 (MVP 매트릭스)
struct ConstantBuffer {
    XMMATRIX wvpMat;
};

// 전역 D3D12 객체들
HWND hwnd = NULL;
ComPtr<IDXGIFactory4> factory;
ComPtr<ID3D12Device> device;
ComPtr<ID3D12CommandQueue> commandQueue;
ComPtr<IDXGISwapChain3> swapChain;
ComPtr<ID3D12DescriptorHeap> rtvHeap;
ComPtr<ID3D12Resource> renderTargets[frameCount];
ComPtr<ID3D12CommandAllocator> commandAllocator;
ComPtr<ID3D12GraphicsCommandList> commandList;
ComPtr<ID3D12Fence> fence;
HANDLE fenceEvent;
UINT64 fenceValue = 0;
UINT frameIndex = 0;
UINT rtvDescriptorSize = 0;

ComPtr<ID3D12RootSignature> rootSignature;
ComPtr<ID3D12PipelineState> pipelineState;

ComPtr<ID3D12Resource> vertexBuffer;
D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
ComPtr<ID3D12Resource> indexBuffer;
D3D12_INDEX_BUFFER_VIEW indexBufferView;

ComPtr<ID3D12Resource> constantBuffer;
ConstantBuffer* cbvDataBegin = nullptr;

// HLSL 셰이더 코드 (문자열로 내장)
const char* shaderCode = R"(
cbuffer ConstantBuffer : register(b0) {
    matrix wvpMat;
};
struct VS_INPUT {
    float3 pos : POSITION;
    float4 color : COLOR;
};
struct PS_INPUT {
    float4 pos : SV_POSITION;
    float4 color : COLOR;
};
PS_INPUT VSMain(VS_INPUT input) {
    PS_INPUT output;
    output.pos = mul(float4(input.pos, 1.0f), wvpMat);
    output.color = input.color;
    return output;
}
float4 PSMain(PS_INPUT input) : SV_TARGET {
    return input.color;
}
)";

// 창 프로시저
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_DESTROY) {
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

// GPU 대기 함수
void WaitForPreviousFrame() {
    const UINT64 fenceToWaitFor = fenceValue;
    commandQueue->Signal(fence.Get(), fenceToWaitFor);
    fenceValue++;

    if (fence->GetCompletedValue() < fenceToWaitFor) {
        fence->SetEventOnCompletion(fenceToWaitFor, fenceEvent);
        WaitForSingleObject(fenceEvent, INFINITE);
    }
    frameIndex = swapChain->GetCurrentBackBufferIndex();
}

// 메인 함수
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    // 1. Win32 창 생성
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_HREDRAW | CS_VREDRAW, WindowProc, 0, 0, hInstance, NULL, NULL, NULL, NULL, L"D3D12 Box", NULL };
    RegisterClassEx(&wc);
    hwnd = CreateWindow(L"D3D12 Box", L"D3D12 Box Example", WS_OVERLAPPEDWINDOW, 100, 100, width, height, NULL, NULL, hInstance, NULL);
    ShowWindow(hwnd, nCmdShow);

    // 2. D3D12 초기화 (Device, CommandQueue, SwapChain)
#if defined(_DEBUG)
    ComPtr<ID3D12Debug> debugController;
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
        debugController->EnableDebugLayer();
    }
#endif

    CreateDXGIFactory1(IID_PPV_ARGS(&factory));
    D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device));

    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue));

    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.BufferCount = frameCount;
    swapChainDesc.Width = width;
    swapChainDesc.Height = height;
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.SampleDesc.Count = 1;

    ComPtr<IDXGISwapChain1> tempSwapChain;
    factory->CreateSwapChainForHwnd(commandQueue.Get(), hwnd, &swapChainDesc, nullptr, nullptr, &tempSwapChain);
    tempSwapChain.As(&swapChain);
    frameIndex = swapChain->GetCurrentBackBufferIndex();

    // 3. 렌더 타겟 뷰(RTV) 생성
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
    rtvHeapDesc.NumDescriptors = frameCount;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvHeap));
    rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart());
    for (UINT n = 0; n < frameCount; n++) {
        swapChain->GetBuffer(n, IID_PPV_ARGS(&renderTargets[n]));
        device->CreateRenderTargetView(renderTargets[n].Get(), nullptr, rtvHandle);
        rtvHandle.Offset(1, rtvDescriptorSize);
    }

    device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator));
    device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator.Get(), nullptr, IID_PPV_ARGS(&commandList));
    commandList->Close();

    device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
    fenceValue = 1;
    fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);

    // 4. 루트 시그니처 및 파이프라인 상태(PSO) 생성
    CD3DX12_ROOT_PARAMETER rootParameters[1];
    rootParameters[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_VERTEX);

    CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
    rootSignatureDesc.Init(1, rootParameters, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    ComPtr<ID3DBlob> signature;
    ComPtr<ID3DBlob> error;
    D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);
    device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&rootSignature));

    ComPtr<ID3DBlob> vertexShader;
    ComPtr<ID3DBlob> pixelShader;
    D3DCompile(shaderCode, strlen(shaderCode), nullptr, nullptr, nullptr, "VSMain", "vs_5_0", 0, 0, &vertexShader, nullptr);
    D3DCompile(shaderCode, strlen(shaderCode), nullptr, nullptr, nullptr, "PSMain", "ps_5_0", 0, 0, &pixelShader, nullptr);

    D3D12_INPUT_ELEMENT_DESC inputElementDescs[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
    };

    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
    psoDesc.InputLayout = { inputElementDescs, 2 };
    psoDesc.pRootSignature = rootSignature.Get();
    psoDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
    psoDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShader.Get());
    psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    psoDesc.DepthStencilState.DepthEnable = FALSE;
    psoDesc.DepthStencilState.StencilEnable = FALSE;
    psoDesc.SampleMask = UINT_MAX;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    psoDesc.SampleDesc.Count = 1;
    device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipelineState));

    // 5. 박스(정육면체) 기하학 데이터 생성 (버텍스 & 인덱스 버퍼)
    Vertex vertices[] = {
        // 앞면
        { XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
        { XMFLOAT3(-0.5f,  0.5f, -0.5f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
        { XMFLOAT3(0.5f,  0.5f, -0.5f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
        { XMFLOAT3(0.5f, -0.5f, -0.5f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
        // 뒷면
        { XMFLOAT3(-0.5f, -0.5f,  0.5f), XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f) },
        { XMFLOAT3(-0.5f,  0.5f,  0.5f), XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f) },
        { XMFLOAT3(0.5f,  0.5f,  0.5f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },
        { XMFLOAT3(0.5f, -0.5f,  0.5f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) }
    };

    WORD indices[] = {
        0, 1, 2, 0, 2, 3, // 앞
        4, 6, 5, 4, 7, 6, // 뒤
        4, 5, 1, 4, 1, 0, // 좌
        3, 2, 6, 3, 6, 7, // 우
        1, 5, 6, 1, 6, 2, // 상
        4, 0, 3, 4, 3, 7  // 하
    };

    const UINT vertexBufferSize = sizeof(vertices);
    device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize), D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&vertexBuffer));
    UINT8* pVertexDataBegin;
    vertexBuffer->Map(0, &CD3DX12_RANGE(0, 0), reinterpret_cast<void**>(&pVertexDataBegin));
    memcpy(pVertexDataBegin, vertices, sizeof(vertices));
    vertexBuffer->Unmap(0, nullptr);
    vertexBufferView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
    vertexBufferView.StrideInBytes = sizeof(Vertex);
    vertexBufferView.SizeInBytes = vertexBufferSize;

    const UINT indexBufferSize = sizeof(indices);
    device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(indexBufferSize), D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&indexBuffer));
    UINT8* pIndexDataBegin;
    indexBuffer->Map(0, &CD3DX12_RANGE(0, 0), reinterpret_cast<void**>(&pIndexDataBegin));
    memcpy(pIndexDataBegin, indices, sizeof(indices));
    indexBuffer->Unmap(0, nullptr);
    indexBufferView.BufferLocation = indexBuffer->GetGPUVirtualAddress();
    indexBufferView.Format = DXGI_FORMAT_R16_UINT;
    indexBufferView.SizeInBytes = indexBufferSize;

    // 상수 버퍼 (월드, 뷰, 프로젝션 행렬)
    device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(1024 * 64), D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&constantBuffer));
    constantBuffer->Map(0, &CD3DX12_RANGE(0, 0), reinterpret_cast<void**>(&cbvDataBegin));

    XMMATRIX projMatrix = XMMatrixPerspectiveFovLH(XM_PIDIV4, (float)width / (float)height, 0.1f, 100.0f);
    XMMATRIX viewMatrix = XMMatrixLookAtLH(XMVectorSet(0.0f, 2.0f, -3.0f, 0.0f), XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));

    // 6. 메인 루프 (렌더링)
    MSG msg = {};
    float angle = 0.0f;

    while (msg.message != WM_QUIT) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else {
            // 카메라 매트릭스 업데이트
            angle += 0.01f;
            XMMATRIX worldMatrix = XMMatrixRotationY(angle) * XMMatrixRotationX(angle * 0.5f);
            XMMATRIX wvp = worldMatrix * viewMatrix * projMatrix;
            cbvDataBegin->wvpMat = XMMatrixTranspose(wvp); // HLSL은 열 우선 배열을 쓰므로 Transpose 필요

            // 커맨드 리스트 기록
            commandAllocator->Reset();
            commandList->Reset(commandAllocator.Get(), pipelineState.Get());

            commandList->SetGraphicsRootSignature(rootSignature.Get());
            commandList->SetGraphicsRootConstantBufferView(0, constantBuffer->GetGPUVirtualAddress());

            D3D12_VIEWPORT viewport = { 0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, 1.0f };
            D3D12_RECT scissorRect = { 0, 0, width, height };
            commandList->RSSetViewports(1, &viewport);
            commandList->RSSetScissorRects(1, &scissorRect);

            commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(renderTargets[frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

            CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart(), frameIndex, rtvDescriptorSize);
            commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

            const float clearColor[] = { 0.1f, 0.2f, 0.4f, 1.0f };
            commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

            commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
            commandList->IASetIndexBuffer(&indexBufferView);
            commandList->DrawIndexedInstanced(36, 1, 0, 0, 0);

            commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(renderTargets[frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

            commandList->Close();

            // 커맨드 실행 및 화면 표시
            ID3D12CommandList* ppCommandLists[] = { commandList.Get() };
            commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
            swapChain->Present(1, 0);

            WaitForPreviousFrame();
        }
    }

    WaitForPreviousFrame();
    CloseHandle(fenceEvent);
    return 0;
}