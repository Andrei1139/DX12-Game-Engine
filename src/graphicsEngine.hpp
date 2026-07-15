#pragma once
#include <d3d12.h>
#include <dxgi.h>
#include <dxgi1_4.h>
#include <d3dcompiler.h>
#include <wrl.h>
#include <iostream>
#include "window.hpp"

using Microsoft::WRL::ComPtr;

class GraphicsEngine {
    public:
        GraphicsEngine(const EngineWindow& window);
        ~GraphicsEngine() {
            if (syncBeforeClosing) {
                finishFrame();
            }

            CloseHandle(fenceEventHandle);
        }

        void finishFrame();
        void render();

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
        D3D12_VERTEX_BUFFER_VIEW vertexBufferView;

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
        void prepareFenceSystem();
        void printHFAILEDoutput();

        HRESULT res;

};