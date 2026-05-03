#include "stdafx.h"
#include "msc_D3D12RenderingEngine.h"

msc_D3D12RenderingEngine::msc_D3D12RenderingEngine()
    : m_nCurrentFrameIndex(0)
    , m_nRtvDescriptorSize(0)
    , m_nFenceValue(1)
    , m_hFenceEvent(NULL)
    , m_nScreenWidth(FRAMEBUFFER_WIDTH)
    , m_nScreenHeight(FRAMEBUFFER_HEIGHT)
{
    ZeroMemory(m_nFenceValues, sizeof(m_nFenceValues));
}

msc_D3D12RenderingEngine::~msc_D3D12RenderingEngine()
{
    Shutdown();
}

bool msc_D3D12RenderingEngine::Initialize(HWND hWnd, UINT nWidth, UINT nHeight)
{
    m_nScreenWidth = nWidth;
    m_nScreenHeight = nHeight;

    // 1. DXGI 팩토리 생성
    if (FAILED(CreateDXGIFactory1(IID_PPV_ARGS(&m_pFactory))))
    {
        std::cout << " CreateDXGIFactory1 실패" << std::endl;
        return false;
    }

    // 2. D3D12 디바이스 생성
    if (FAILED(D3D12CreateDevice(NULL, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_pDevice))))
    {
        std::cout << " D3D12CreateDevice 실패" << std::endl;
        return false;
    }

    // 3. 명령 큐 생성
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

    if (FAILED(m_pDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_pCommandQueue))))
    {
        std::cout << " CreateCommandQueue 실패" << std::endl;
        return false;
    }

    // 4. 스왑체인 생성
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.BufferCount = D3D12_FRAME_BUFFER_COUNT;
    swapChainDesc.Width = nWidth;
    swapChainDesc.Height = nHeight;
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.SampleDesc.Count = 1;

    ComPtr<IDXGISwapChain1> pSwapChain1;
    if (FAILED(m_pFactory->CreateSwapChainForHwnd(m_pCommandQueue.Get(), hWnd, &swapChainDesc, NULL, NULL, &pSwapChain1)))
    {
        std::cout << " CreateSwapChainForHwnd 실패" << std::endl;
        return false;
    }

    if (FAILED(pSwapChain1.As(&m_pSwapChain)))
    {
        std::cout << " SwapChain1 to SwapChain3 변환 실패" << std::endl;
        return false;
    }

    m_nCurrentFrameIndex = m_pSwapChain->GetCurrentBackBufferIndex();

    // 5. 렌더 타겟 뷰 힙 생성
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
    rtvHeapDesc.NumDescriptors = D3D12_FRAME_BUFFER_COUNT;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

    if (FAILED(m_pDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_pRtvHeap))))
    {
        std::cout << " CreateDescriptorHeap (RTV) 실패" << std::endl;
        return false;
    }

    m_nRtvDescriptorSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    // 6. 렌더 타겟 생성
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_pRtvHeap->GetCPUDescriptorHandleForHeapStart());

    for (int i = 0; i < D3D12_FRAME_BUFFER_COUNT; i++)
    {
        if (FAILED(m_pSwapChain->GetBuffer(i, IID_PPV_ARGS(&m_pRenderTargets[i]))))
        {
            std::cout << " SwapChain GetBuffer 실패" << std::endl;
            return false;
        }

        m_pDevice->CreateRenderTargetView(m_pRenderTargets[i].Get(), NULL, rtvHandle);
        rtvHandle.ptr += m_nRtvDescriptorSize;
    }

    // 7. 명령 할당자 생성
    for (int i = 0; i < D3D12_FRAME_BUFFER_COUNT; i++)
    {
        if (FAILED(m_pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_pCommandAllocator[i]))))
        {
            std::cout << " CreateCommandAllocator 실패" << std::endl;
            return false;
        }
    }

    // 8. 명령 리스트 생성
    if (FAILED(m_pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_pCommandAllocator[0].Get(), NULL, IID_PPV_ARGS(&m_pCommandList))))
    {
        std::cout << " CreateCommandList 실패" << std::endl;
        return false;
    }

    // 명령 리스트를 닫은 상태로 시작
    m_pCommandList->Close();

    // 9. 펜스 생성
    if (FAILED(m_pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_pFence))))
    {
        std::cout << " CreateFence 실패" << std::endl;
        return false;
    }

    m_nFenceValue = 1;

    // 펜스 이벤트 생성
    m_hFenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (m_hFenceEvent == NULL)
    {
        std::cout << " CreateEvent (Fence) 실패" << std::endl;
        return false;
    }

    std::cout << " D3D12 초기화 성공" << std::endl;
    return true;
}

void msc_D3D12RenderingEngine::Shutdown()
{
    // GPU 완료 대기
    if (m_pCommandQueue)
    {
        if (m_pFence)
        {
            m_pCommandQueue->Signal(m_pFence.Get(), m_nFenceValue);

            if (m_hFenceEvent)
            {
                if (SUCCEEDED(m_pFence->SetEventOnCompletion(m_nFenceValue, m_hFenceEvent)))
                {
                    WaitForSingleObject(m_hFenceEvent, INFINITE);
                }
            }
        }
    }

    if (m_hFenceEvent)
    {
        CloseHandle(m_hFenceEvent);
        m_hFenceEvent = NULL;
    }

    m_pRtvHeap.Reset();
    for (int i = 0; i < D3D12_FRAME_BUFFER_COUNT; i++)
    {
        m_pRenderTargets[i].Reset();
        m_pCommandAllocator[i].Reset();
    }
    m_pCommandList.Reset();
    m_pSwapChain.Reset();
    m_pCommandQueue.Reset();
    m_pDevice.Reset();
    m_pFactory.Reset();
    m_pFence.Reset();
}

void msc_D3D12RenderingEngine::BeginRender()
{
    auto pCommandAllocator = m_pCommandAllocator[m_nCurrentFrameIndex].Get();
    pCommandAllocator->Reset();
    m_pCommandList->Reset(pCommandAllocator, NULL);

    // 렌더 타겟으로 전환
    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = m_pRenderTargets[m_nCurrentFrameIndex].Get();
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

    m_pCommandList->ResourceBarrier(1, &barrier);

    // 렌더 타겟 설정
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_pRtvHeap->GetCPUDescriptorHandleForHeapStart());
    rtvHandle.ptr += m_nCurrentFrameIndex * m_nRtvDescriptorSize;

    m_pCommandList->OMSetRenderTargets(1, &rtvHandle, FALSE, NULL);

    // 뷰포트 및 시저 설정
    D3D12_VIEWPORT viewport = { 0.0f, 0.0f, static_cast<float>(m_nScreenWidth), static_cast<float>(m_nScreenHeight), 0.0f, 1.0f };
    D3D12_RECT scissorRect = { 0, 0, static_cast<LONG>(m_nScreenWidth), static_cast<LONG>(m_nScreenHeight) };

    m_pCommandList->RSSetViewports(1, &viewport);
    m_pCommandList->RSSetScissorRects(1, &scissorRect);
}

void msc_D3D12RenderingEngine::ClearScreen(const FLOAT* pClearColor)
{
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_pRtvHeap->GetCPUDescriptorHandleForHeapStart());
    rtvHandle.ptr += m_nCurrentFrameIndex * m_nRtvDescriptorSize;

    m_pCommandList->ClearRenderTargetView(rtvHandle, pClearColor, 0, NULL);
}

void msc_D3D12RenderingEngine::EndRender()
{
    // 렌더 타겟에서 프리젠트 상태로 전환
    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = m_pRenderTargets[m_nCurrentFrameIndex].Get();
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

    m_pCommandList->ResourceBarrier(1, &barrier);
    m_pCommandList->Close();

    // 명령 리스트 실행
    ID3D12CommandList* ppCommandLists[] = { m_pCommandList.Get() };
    m_pCommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

    // 신호 발생
    if (FAILED(m_pCommandQueue->Signal(m_pFence.Get(), m_nFenceValue)))
    {
        std::cout << " CommandQueue Signal 실패" << std::endl;
        return;
    }

    m_nFenceValues[m_nCurrentFrameIndex] = m_nFenceValue;
    m_nFenceValue++;
}

void msc_D3D12RenderingEngine::Present()
{
    if (FAILED(m_pSwapChain->Present(1, 0)))
    {
        std::cout << " SwapChain Present 실패" << std::endl;
        return;
    }

    // 다음 프레임으로 진행
    const UINT64 fence = m_nFenceValues[m_nCurrentFrameIndex];
    if (FAILED(m_pCommandQueue->Signal(m_pFence.Get(), fence)))
    {
        std::cout << " Signal 실패" << std::endl;
        return;
    }

    m_nCurrentFrameIndex = m_pSwapChain->GetCurrentBackBufferIndex();

    // GPU가 완료될 때까지 대기
    if (m_pFence->GetCompletedValue() < m_nFenceValues[m_nCurrentFrameIndex])
    {
        if (FAILED(m_pFence->SetEventOnCompletion(m_nFenceValues[m_nCurrentFrameIndex], m_hFenceEvent)))
        {
            std::cout << " SetEventOnCompletion 실패" << std::endl;
            return;
        }
        WaitForSingleObject(m_hFenceEvent, INFINITE);
    }
}
