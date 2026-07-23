#pragma once
#include <d3d12.h>
#include <dxgi.h>
#include <dxgi1_4.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <iostream>
#include <cmath>
#include <bitset>
#include <vector>
#include <memory>
#include "camera.hpp"
#include "window.hpp"
#include "resourceManager.hpp"

using Microsoft::WRL::ComPtr;

class GraphicsEngine {
    public:
        GraphicsEngine(const EngineWindow &window, const Camera &camera);
        ~GraphicsEngine() {
            commandList->Close();
            CloseHandle(fenceEventHandle);
        }

        void update();
        void render();
        void finishFrame() {idleUntilCommandQueueFinished(); swapSCBuffers();}

        void printHFAILEDoutput();

    private:
        // float worldMat[4][4] = {{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}};

        const EngineWindow &window;

        ComPtr<ID3D12Device> deviceInterface;
        ComPtr<IDXGIFactory4> factoryInterface;

        ComPtr<ID3D12CommandQueue> commandQueue;
        ComPtr<ID3D12GraphicsCommandList> commandList;
        ComPtr<ID3D12CommandAllocator> commandAllocator;

        ComPtr<IDXGISwapChain> swapChain;

        ComPtr<ID3D12DescriptorHeap> RTVHeap;
        ComPtr<ID3D12DescriptorHeap> DSVHeap;

        ComPtr<ID3D12Resource> depthStencilTexture;
        ComPtr<ID3D12Resource> swapChainBuffers[2];
        UINT currBuffer;
        D3D12_CPU_DESCRIPTOR_HANDLE RTVHandleBufferZero, RTVHandleBufferOne;

        ComPtr<ID3D12RootSignature> rootSignature;

        ComPtr<ID3DBlob> vertexShader, pixelShader;
        ComPtr<ID3D12PipelineState> pipelineState;

        std::unique_ptr<ResourceManager> resourceManager;

        ComPtr<ID3D12Fence> fenceInterface;
        unsigned long long fenceValue = 1;
        HANDLE fenceEventHandle;

        bool syncBeforeClosing = true;

        D3D12_VIEWPORT viewport = {};
        D3D12_RECT scissorRect = {};

        void backgroundPreparations();
        void initCommandSystem();
        void createSwapChain();
        void createRenderTarget();
        void createRootSignature();
        void prepareShaders();
        void configurePipeline();
        void createCTBuffer();
        void prepareFenceSystem();

        void initCTBuffer(UINT64 width, ID3D12Resource **buffer);
        void copyDataToBuffer(void *data, UINT64 dataLen, ID3D12Resource **buffer);

        void updateCTBuffer();

        void resetCommandStructures();
        void executeCommands() {commandList->Close();
                                ID3D12CommandList *auxCmdListArray[] = {commandList.Get()};
                                commandQueue->ExecuteCommandLists(1, auxCmdListArray);}

        void idleUntilCommandQueueFinished();
        inline void swapSCBuffers() {currBuffer = 1 - currBuffer;}

        HRESULT res;

        const Camera &camera;
};