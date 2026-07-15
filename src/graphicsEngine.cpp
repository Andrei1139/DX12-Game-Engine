#include "graphicsEngine.hpp"
#include <bitset>
#include <vector>

/* */

GraphicsEngine::GraphicsEngine(const EngineWindow& window): window(window) {
    // Viewport and scissor rect setup
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = static_cast<float>(window.getWidth());
    viewport.Height = static_cast<float>(window.getHeight());
    viewport.MinDepth = 0;
    viewport.MaxDepth = 1;

    scissorRect.top = 0;
    scissorRect.left = 0;
    scissorRect.right = static_cast<LONG>(window.getWidth());
    scissorRect.bottom = static_cast<LONG>(window.getHeight());

    backgroundPreparations();
    initCommandSystem();

    createSwapChain();

    createRenderTarget();
    createRootSignature();
    prepareShaders();
    configurePipeline();
    createVertexBuffer();

    prepareFenceSystem();
}

void GraphicsEngine::backgroundPreparations() {
    // Enable debugging
    ID3D12Debug* debugController = NULL;
    D3D12GetDebugInterface(IID_PPV_ARGS(&debugController));
    debugController->EnableDebugLayer();

    // Create device
    res = D3D12CreateDevice(NULL, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(deviceInterface.GetAddressOf()));
    if (FAILED(res)) {
        printHFAILEDoutput();
        throw std::runtime_error("D3D12CreateDevice failed\n");
    }

    // Create factory
    res = CreateDXGIFactory1(IID_PPV_ARGS(factoryInterface.GetAddressOf()));
    if (FAILED(res)) {
        printHFAILEDoutput();
        throw std::runtime_error("CreateDXGIFactory1 failed\n");
    }
}

void GraphicsEngine::initCommandSystem() {
    // Create command queue, list and allocator
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    res = deviceInterface->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(commandQueue.GetAddressOf()));
    if (FAILED(res)) {
        printHFAILEDoutput();
        throw std::runtime_error("CreateCommandQueue failed\n");
    }
    res = deviceInterface->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(commandAllocator.GetAddressOf()));
    if (FAILED(res)) {
        printHFAILEDoutput();
        throw std::runtime_error("CreateCommandAllocator failed\n");
    }
    commandAllocator->Reset();
    res = deviceInterface->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator.Get(), NULL, IID_PPV_ARGS(commandList.GetAddressOf()));
    if (FAILED(res)) {
        printHFAILEDoutput();
        throw std::runtime_error("CreateCommandList failed\n");
    }
    commandList->Close();
}

void GraphicsEngine::createSwapChain() {
    // Create swap chain
    DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
    swapChainDesc.BufferDesc.Width = window.getWidth();
    swapChainDesc.BufferDesc.Height = window.getHeight();
    swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferCount = 2;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.OutputWindow = window.getWindowHandle();
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.Windowed = true;
    res = factoryInterface->CreateSwapChain(commandQueue.Get(), &swapChainDesc, swapChain.GetAddressOf());
    if (FAILED(res)) {
        printHFAILEDoutput();
        throw std::runtime_error("CreateSwapChain failed\n");
    }

    currBuffer = 0;
}

void GraphicsEngine::createRenderTarget() {
    // Get descriptor sizes
    auto RTVDescriptorSize = deviceInterface->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    // Create descriptor heaps
    D3D12_DESCRIPTOR_HEAP_DESC RTVHeapDesc;
    RTVHeapDesc.NumDescriptors = 2;
    RTVHeapDesc.NodeMask = 0;
    RTVHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    RTVHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    res = deviceInterface->CreateDescriptorHeap(&RTVHeapDesc, IID_PPV_ARGS(RTVHeap.GetAddressOf()));
    if (FAILED(res)) {
        printHFAILEDoutput();
        throw std::runtime_error("CreateDescriptorHeap failed for render target\n");
    }

    // Create descriptors/views

    RTVHandleBufferZero = RTVHeap->GetCPUDescriptorHandleForHeapStart();
    RTVHandleBufferOne = RTVHeap->GetCPUDescriptorHandleForHeapStart();
    RTVHandleBufferOne.ptr += RTVDescriptorSize;
    res = swapChain->GetBuffer(0, IID_PPV_ARGS(swapChainBuffers[0].GetAddressOf()));
    if (FAILED(res)) {
        printHFAILEDoutput();
        throw std::runtime_error("GetBuffer failed\n");
    }
    res = swapChain->GetBuffer(1, IID_PPV_ARGS(swapChainBuffers[1].GetAddressOf()));
    if (FAILED(res)) {
        printHFAILEDoutput();
        throw std::runtime_error("GetBuffer failed\n");
    }

    deviceInterface->CreateRenderTargetView(swapChainBuffers[0].Get(), NULL, RTVHandleBufferZero);
    deviceInterface->CreateRenderTargetView(swapChainBuffers[1].Get(), NULL, RTVHandleBufferOne);
}

void GraphicsEngine::createRootSignature() {
    // Create root signatures and pipeline state
    D3D12_ROOT_SIGNATURE_DESC rootSignDesc = {};
    rootSignDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    ComPtr<ID3DBlob> signature, error;
    res = D3D12SerializeRootSignature(&rootSignDesc, D3D_ROOT_SIGNATURE_VERSION_1, signature.GetAddressOf(), error.GetAddressOf());
    if (FAILED(res)) {
        printHFAILEDoutput();
        throw std::runtime_error("D3D12SerializeRootSignature failed\n");
    }
    res = deviceInterface->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(rootSignature.GetAddressOf()));
}

void GraphicsEngine::prepareShaders() {
    res = D3DCompileFromFile(L"shaders//vShader.hlsl", NULL, NULL, "main", "vs_5_0", 0, 0, vertexShader.GetAddressOf(), NULL);
    if (FAILED(res)) {
        printHFAILEDoutput();
        throw std::runtime_error("D3DCompileFromFile failed for vertex shader\n");
    }
    res = D3DCompileFromFile(L"shaders//pShader.hlsl", NULL, NULL, "main", "ps_5_0", 0, 0, pixelShader.GetAddressOf(), NULL);
    if (FAILED(res)) {
        printHFAILEDoutput();
        throw std::runtime_error("D3DCompileFromFile failed for pixel shader\n");
    }
}

void GraphicsEngine::configurePipeline() {
    // Setup Vertex Input Layout and pipeline state
    D3D12_INPUT_ELEMENT_DESC inputElementDescs[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
    };

    D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDesc = {};
    for (int i = 0; i < 1; ++i) {
        pipelineStateDesc.BlendState.RenderTarget[i].SrcBlend = D3D12_BLEND_ONE;
        pipelineStateDesc.BlendState.RenderTarget[i].DestBlend = D3D12_BLEND_ZERO;
        pipelineStateDesc.BlendState.RenderTarget[i].BlendOp = D3D12_BLEND_OP_ADD;
        pipelineStateDesc.BlendState.RenderTarget[i].SrcBlendAlpha = D3D12_BLEND_ONE;
        pipelineStateDesc.BlendState.RenderTarget[i].DestBlendAlpha = D3D12_BLEND_ZERO;
        pipelineStateDesc.BlendState.RenderTarget[i].BlendOpAlpha = D3D12_BLEND_OP_ADD;
        pipelineStateDesc.BlendState.RenderTarget[i].LogicOp = D3D12_LOGIC_OP_NOOP;
        pipelineStateDesc.BlendState.RenderTarget[i].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
    }

    pipelineStateDesc.DepthStencilState.DepthEnable = false;
    pipelineStateDesc.DepthStencilState.StencilEnable = false;
    pipelineStateDesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
    pipelineStateDesc.InputLayout = {inputElementDescs, _countof(inputElementDescs)};
    pipelineStateDesc.NumRenderTargets = 1;
    pipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    pipelineStateDesc.pRootSignature = rootSignature.Get();
    pipelineStateDesc.PS = {pixelShader->GetBufferPointer(), pixelShader->GetBufferSize()};
    pipelineStateDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
    pipelineStateDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
    pipelineStateDesc.RasterizerState.DepthClipEnable = true;
    pipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    pipelineStateDesc.SampleDesc.Count = 1;
    pipelineStateDesc.SampleMask = UINT_MAX;
    pipelineStateDesc.VS = {vertexShader->GetBufferPointer(), vertexShader->GetBufferSize()};

    res = deviceInterface->CreateGraphicsPipelineState(&pipelineStateDesc, IID_PPV_ARGS(pipelineState.GetAddressOf()));
    if (FAILED(res)) {
        printHFAILEDoutput();
        throw std::runtime_error("CreateGraphicsPipelineState failed\n");
    }
}

void GraphicsEngine::createVertexBuffer() {
    // Vertex buffer
    float vertices[3][6] = {{0.0f, 0.25f, 0.0f, 1.0f, 0.0f, 0.0f},
                            {0.25f, -0.25f, 0.0f, 0.0f, 1.0f, 0.0f},
                            {-0.25f, -0.25f, 0.0f, 0.0f, 0.0f, 1.0f}};

    // Preparing heap to upload for vertex buffer
    D3D12_HEAP_PROPERTIES heapProperties = {};
    heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
    heapProperties.CreationNodeMask = 1;
    heapProperties.VisibleNodeMask = 1;

    D3D12_RESOURCE_DESC vertexBufferResResc = {};
    vertexBufferResResc.Width = sizeof(vertices);
    vertexBufferResResc.Height = 1;
    vertexBufferResResc.DepthOrArraySize = 1;
    vertexBufferResResc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    vertexBufferResResc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    vertexBufferResResc.MipLevels = 1;
    vertexBufferResResc.SampleDesc.Count = 1;
    vertexBufferResResc.SampleDesc.Quality = 0;

    res = deviceInterface->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &vertexBufferResResc, D3D12_RESOURCE_STATE_GENERIC_READ,
                                                   NULL, IID_PPV_ARGS(vertexBuffer.GetAddressOf()));
    if (FAILED(res)) {
        printHFAILEDoutput();
        throw std::runtime_error("CreateCommittedResource failed for vertex buffer\n");
    }

    D3D12_RANGE range = {0, 0};
    void *vertexData;
    res = vertexBuffer->Map(0, &range, &vertexData);
    if (FAILED(res)) {
        printHFAILEDoutput();
        throw std::runtime_error("Map failed for vertex buffer\n");
    }
    memcpy(vertexData, vertices, sizeof(vertices));
    vertexBuffer->Unmap(0, NULL);

    vertexBufferView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
    vertexBufferView.SizeInBytes = sizeof(vertices);
    vertexBufferView.StrideInBytes = sizeof(vertices[0]);
}

void GraphicsEngine::prepareFenceSystem() {
    // Fence system initialization for CPU-GPU syncing
    res = deviceInterface->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fenceInterface.GetAddressOf()));
    if (FAILED(res)) {
        printHFAILEDoutput();
        throw std::runtime_error("CreateFence failed\n");
    }
    fenceEventHandle = CreateEvent(NULL, false, false, NULL);
    if (fenceEventHandle == NULL) {
        printHFAILEDoutput();
        throw std::runtime_error("CreateEvent failed\n");
    }
}

void GraphicsEngine::render() {
    // Reset command structs
    res = commandAllocator->Reset();
    if (FAILED(res)) {
        printHFAILEDoutput();
        throw std::runtime_error("Reset failed for command allocator\n");
    }
    res = commandList->Reset(commandAllocator.Get(), pipelineState.Get());
    if (FAILED(res)) {
        printHFAILEDoutput();
        throw std::runtime_error("Reset failed for command list\n");
    }

    commandList->SetGraphicsRootSignature(rootSignature.Get());
    commandList->RSSetViewports(1, &viewport);
    commandList->RSSetScissorRects(1, &scissorRect);

    // Handle backbuffer state
    D3D12_RESOURCE_BARRIER resourceBarrier = {};
    resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    resourceBarrier.Transition.pResource = swapChainBuffers[currBuffer].Get();
    resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
    resourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    commandList->ResourceBarrier(1, &resourceBarrier);

    auto currDescriptorHandle = (currBuffer == 0) ? &RTVHandleBufferZero : &RTVHandleBufferOne;
    commandList->OMSetRenderTargets(1, currDescriptorHandle, false, NULL);

    const float clearColor[] = { 0.5f, 0.5f, 1.0f, 1.0f };
    commandList->ClearRenderTargetView(*currDescriptorHandle, clearColor, 0, NULL);
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
    commandList->DrawInstanced(3, 1, 0, 0);

    resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
    commandList->ResourceBarrier(1, &resourceBarrier);

    commandList->Close();

    ID3D12CommandList *auxCmdListArray[] = {commandList.Get()};
    commandQueue->ExecuteCommandLists(1, auxCmdListArray);

    res = swapChain->Present(0, 0);
    if (FAILED(res)) {
        printHFAILEDoutput();
        throw std::runtime_error("Present failed\n");
    }
}

void GraphicsEngine::finishFrame() {
    auto currFence = fenceValue;
    res = commandQueue->Signal(fenceInterface.Get(), currFence);
    if (FAILED(res)) {
        printHFAILEDoutput();
        throw std::runtime_error("Signal failed\n");
    }
    ++fenceValue;

    if (fenceInterface->GetCompletedValue() < currFence) {
        res = fenceInterface->SetEventOnCompletion(currFence, fenceEventHandle);
        if (FAILED(res)) {
            printHFAILEDoutput();
            syncBeforeClosing = false;
            throw std::runtime_error("SetEventOnCompletion failed\n");
        } 

        WaitForSingleObject(fenceEventHandle, INFINITE);
    }

    currBuffer = 1 - currBuffer;
}

void GraphicsEngine::printHFAILEDoutput() {
    ComPtr<ID3D12InfoQueue> infoQueue;
    if (!FAILED(deviceInterface.As(&infoQueue))) {
        UINT64 count = infoQueue->GetNumStoredMessages();

        for (auto i = 0; i < count; i++)
        {
            SIZE_T size = 0;
            infoQueue->GetMessage(i, NULL, &size);

            std::vector<char> bytes(size);
            auto* msg = reinterpret_cast<D3D12_MESSAGE*>(bytes.data());

            infoQueue->GetMessage(i, msg, &size);

            printf("%s\n", msg->pDescription);
        }

        infoQueue->ClearStoredMessages();
    }
}