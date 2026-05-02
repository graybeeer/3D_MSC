#pragma once
#include "d3d12.h"
#include "dxgi1_4.h"
#include "D3Dcompiler.h"
#include "DXGIDebug.h"
#include <wrl.h>
using namespace Microsoft::WRL;
#define D3D12_FRAME_BUFFER_COUNT 2


class CD3D12RenderingEngine
{
public:
    CD3D12RenderingEngine();
    virtual ~CD3D12RenderingEngine();

    // 초기화 및 정리
    bool Initialize(HWND hWnd, UINT nWidth, UINT nHeight);
    void Shutdown();

    // 렌더링 명령
    void BeginRender();
    void EndRender();
    void ClearScreen(const FLOAT* pClearColor);
    void Present();

    // 디바이스 접근
    ID3D12Device* GetDevice() const { return m_pDevice.Get(); }
    ID3D12CommandQueue* GetCommandQueue() const { return m_pCommandQueue.Get(); }
    ID3D12GraphicsCommandList* GetCommandList() const { return m_pCommandList.Get(); }
    IDXGISwapChain3* GetSwapChain() const { return m_pSwapChain.Get(); }

private:
    // 디바이스 및 스왑체인
    ComPtr<ID3D12Device> m_pDevice;
    ComPtr<IDXGIFactory4> m_pFactory;
    ComPtr<IDXGISwapChain3> m_pSwapChain;
    ComPtr<ID3D12CommandQueue> m_pCommandQueue;

    // 명령 할당자 및 리스트
    ComPtr<ID3D12CommandAllocator> m_pCommandAllocator[D3D12_FRAME_BUFFER_COUNT];
    ComPtr<ID3D12GraphicsCommandList> m_pCommandList;

    // 렌더 타겟
    ComPtr<ID3D12DescriptorHeap> m_pRtvHeap;
    ComPtr<ID3D12Resource> m_pRenderTargets[D3D12_FRAME_BUFFER_COUNT];

    // 펜스 및 동기화
    ComPtr<ID3D12Fence> m_pFence;
    HANDLE m_hFenceEvent;
    UINT64 m_nFenceValue;
    UINT64 m_nFenceValues[D3D12_FRAME_BUFFER_COUNT];

    // 상태
    UINT m_nCurrentFrameIndex;
    UINT m_nRtvDescriptorSize;

    // 해상도
    UINT m_nScreenWidth;
    UINT m_nScreenHeight;
};