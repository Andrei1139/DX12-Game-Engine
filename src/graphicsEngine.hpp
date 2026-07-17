#pragma once
#include <d3d12.h>
#include <dxgi.h>
#include <dxgi1_4.h>
#include <d3dcompiler.h>
#include <wrl.h>
#include <iostream>
#include <stacktrace>
#include "window.hpp"

using Microsoft::WRL::ComPtr;

class GraphicsEngine {
    public:
        GraphicsEngine(const EngineWindow& window);
        ~GraphicsEngine() {
            commandList->Close();
            CloseHandle(fenceEventHandle);
        }

        void render();
        void finishFrame() {idleUntilCommandQueueFinished(); swapSCBuffers();}

        void printHFAILEDoutput();

    private:
        const EngineWindow &window;

        ComPtr<ID3D12Device> deviceInterface;
        ComPtr<IDXGIFactory4> factoryInterface;

        ComPtr<ID3D12CommandQueue> commandQueue;
        ComPtr<ID3D12GraphicsCommandList> commandList;
        ComPtr<ID3D12CommandAllocator> commandAllocator;

        ComPtr<IDXGISwapChain> swapChain;

        ComPtr<ID3D12DescriptorHeap> RTVHeap;
        ComPtr<ID3D12DescriptorHeap> DSVHeap;

        ComPtr<ID3D12Resource> swapChainBuffers[2];
        UINT currBuffer;
        D3D12_CPU_DESCRIPTOR_HANDLE RTVHandleBufferZero, RTVHandleBufferOne, DSVHandle;

        ComPtr<ID3D12RootSignature> rootSignature;

        ComPtr<ID3DBlob> vertexShader, pixelShader;
        ComPtr<ID3D12PipelineState> pipelineState;

        ComPtr<ID3D12Resource> vertexBuffer;
        ComPtr<ID3D12Resource> intermediaryVertexResource;
        D3D12_VERTEX_BUFFER_VIEW vertexBufferView;

        ComPtr<ID3D12Resource> indexBuffer;
        ComPtr<ID3D12Resource> intermediaryIndexResource;
        D3D12_INDEX_BUFFER_VIEW indexBufferView;

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
        void createVertexBuffer();
        void createIndexBuffer();
        void prepareFenceSystem();

        void initGPUOnlyBuffer(UINT64 width, void *data, ID3D12Resource **finalResource ,ID3D12Resource **intermResource);

        void resetCommandStructures();
        void executeCommands() {commandList->Close();
                                ID3D12CommandList *auxCmdListArray[] = {commandList.Get()};
                                commandQueue->ExecuteCommandLists(1, auxCmdListArray);}

        void idleUntilCommandQueueFinished();
        inline void swapSCBuffers() {currBuffer = 1 - currBuffer;}

        HRESULT res;
};