#include "resourceManager.hpp"

void ResourceManager::addModel(Model model) {
    models.emplace_back(model);

    const auto *vertices = model.getVertices();
    for (int i = 0; i < model.getNumVertices(); ++i) {
        aggregateVertexList.push_back(vertices[i]);
    }

    const auto *indices = model.getIndices();
    for (int i = 0; i < model.getNumIndices(); ++i) {
        aggregateIndexList.push_back(indices[i]);
    }
}

// D3D12_VERTEX_BUFFER_VIEW *ResourceManager::getVertexBufferView(int index) {
//     vertexBufferView.BufferLocation = vertexBuffers.at(0)->GetGPUVirtualAddress();
//     for (int i = 0; i < index; ++i) {
//         vertexBufferView.BufferLocation += models.at(i).getNumVertices() * sizeof(Vertex);
//     }
//     vertexBufferView.SizeInBytes = static_cast<UINT>(aggregateVertexList.size() * sizeof(Vertex));

//     return &vertexBufferView;
// }

// D3D12_INDEX_BUFFER_VIEW *ResourceManager::getIndexBufferView(int index) {
//     indexBufferView.BufferLocation = indexBuffers.at(0)->GetGPUVirtualAddress();
//     for (int i = 0; i < index; ++i) {
//         indexBufferView.BufferLocation += models.at(i).getNumIndices() * sizeof(UINT32);
//     }
//     indexBufferView.SizeInBytes = static_cast<UINT>(aggregateIndexList.size() * sizeof(UINT32));

//     return &indexBufferView;
// }

void ResourceManager::initVertexProcessing() {
    auto &intermediaryBuffer = intermediaryBuffers.at(0);
    auto &vertexBuffer = vertexBuffers.at(0);

    createBuffer(aggregateVertexList.size() * sizeof(Vertex), D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ, intermediaryBuffer.GetAddressOf());
    createBuffer(aggregateVertexList.size() * sizeof(Vertex), D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COPY_DEST, vertexBuffer.GetAddressOf());

    copyDataToBuffer(aggregateVertexList.data(), aggregateVertexList.size() * sizeof(Vertex), intermediaryBuffer.GetAddressOf());

    commandList->CopyResource(vertexBuffer.Get(), intermediaryBuffer.Get());

    transition(D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, vertexBuffer.Get());

    vertexBufferView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
    vertexBufferView.SizeInBytes = static_cast<UINT>(aggregateVertexList.size() * sizeof(Vertex));
    vertexBufferView.StrideInBytes = sizeof(Vertex);
}

void ResourceManager::initIndexProcessing() {
    auto &intermediaryBuffer = intermediaryBuffers.at(1);
    auto &indexBuffer = indexBuffers.at(0);

    createBuffer(aggregateIndexList.size() * sizeof(UINT32), D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ, intermediaryBuffer.GetAddressOf());
    createBuffer(aggregateIndexList.size() * sizeof(UINT32), D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COPY_DEST, indexBuffer.GetAddressOf());

    copyDataToBuffer(aggregateIndexList.data(), aggregateIndexList.size() * sizeof(Vertex), intermediaryBuffer.GetAddressOf());

    commandList->CopyResource(indexBuffer.Get(), intermediaryBuffer.Get());

    transition(D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_INDEX_BUFFER, indexBuffer.Get());

    indexBufferView.BufferLocation = indexBuffer->GetGPUVirtualAddress();
    indexBufferView.SizeInBytes = static_cast<UINT>(aggregateIndexList.size() * sizeof(UINT32));
    indexBufferView.Format = DXGI_FORMAT_R32_UINT;
}

void ResourceManager::initCTBufferProcessing() {
    auto &CTBuffer = constantBuffers.at(0);

    // UINT64 modelsCTData = (models.size() * sizeof(DirectX::XMMATRIX) + 255) & ~255;
    createBuffer(512, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ, CTBuffer.GetAddressOf());

    D3D12_DESCRIPTOR_HEAP_DESC CTBHeapDesc = {};
    CTBHeapDesc.NumDescriptors = models.size();
    CTBHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    CTBHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    res = deviceInterface->CreateDescriptorHeap(&CTBHeapDesc, IID_PPV_ARGS(CTDescriptorHeap.GetAddressOf()));
    if (FAILED(res)) {
        printHFAILEDoutputGlobal(deviceInterface);
        throw std::runtime_error("CreateDescriptorHeap failed for constant buffer\n");
    }

    D3D12_CONSTANT_BUFFER_VIEW_DESC CTBDesc = {};
    CTBDesc.SizeInBytes = 256;
    D3D12_CPU_DESCRIPTOR_HANDLE currDescriptorHandle = CTDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
    
    for (int i = 0; i < models.size(); ++i) {
        CTBDesc.BufferLocation = CTBuffer->GetGPUVirtualAddress() + i * 256;
        deviceInterface->CreateConstantBufferView(&CTBDesc, currDescriptorHandle);
        currDescriptorHandle.ptr += deviceInterface->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    }
}

void ResourceManager::updateCTBuffer() {
    byte *data = new byte[512];
    for (int i = 0; i < models.size(); ++i) {
        DirectX::XMMATRIX worldMat = DirectX::XMMatrixTransformation(DirectX::XMVectorZero(),
                                                                     DirectX::XMVectorZero(),
                                                                     DirectX::XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f),
                                                                     DirectX::XMVectorZero(),
                                                                     DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
                                                                     DirectX::XMVectorSet(0.0f, 0.2f, 1.0f, 0.0f));
            
        float sinRotX = std::sin(camera.getRotX());
        float sinRotY = std::sin(camera.getRotY());
        float cosRotX = std::cos(camera.getRotX());
        float cosRotY = std::cos(camera.getRotY());
        DirectX::XMMATRIX viewMat = DirectX::XMMatrixLookToLH(DirectX::XMVectorSet(camera.getX(), camera.getY(), camera.getZ(), 1.0f),
                                                            DirectX::XMVectorSet(sinRotY * cosRotX, -sinRotX, cosRotY * cosRotX, 1.0f),
                                                            DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
                                                            
        DirectX::XMMATRIX projMat = DirectX::XMMatrixPerspectiveFovLH(camera.getFOV(),
                                                                camera.getAspectRatio(),
                                                                0.1f, 1000.0f);

        DirectX::XMMATRIX product = DirectX::XMMatrixMultiply(DirectX::XMMatrixMultiply(worldMat, viewMat), projMat);

        memcpy(data + i * 256, &product, sizeof(product));
    }

    copyDataToBuffer(data, 512, constantBuffers.at(0).GetAddressOf());
}

void ResourceManager::createBuffer(UINT64 width, D3D12_HEAP_TYPE heapType, D3D12_RESOURCE_STATES resourceState, ID3D12Resource **resource) {
    D3D12_RESOURCE_DESC desc = {};
    desc.Width = width;
    desc.Height = 1;
    desc.DepthOrArraySize = 1;
    desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    desc.MipLevels = 1;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;

    D3D12_HEAP_PROPERTIES heapProperties = {};
    heapProperties.Type = heapType;
    heapProperties.CreationNodeMask = 1;
    heapProperties.VisibleNodeMask = 1;

    res = deviceInterface->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &desc, resourceState,
                                                   NULL, IID_PPV_ARGS(resource));
    if (FAILED(res)) {
        printHFAILEDoutputGlobal(deviceInterface);
        throw std::runtime_error("CreateCommittedResource failed for intermediary buffer\n");
    }
}

void ResourceManager::copyDataToBuffer(void *data, UINT64 dataLen, ID3D12Resource **buffer) {
    void *tempData;
    res = (*buffer)->Map(0, NULL, &tempData);
    if (FAILED(res)) {
        printHFAILEDoutputGlobal(deviceInterface);
        throw std::runtime_error("Map failed\n");
    }
    memcpy(tempData, data, dataLen);
    (*buffer)->Unmap(0, NULL);
}

void ResourceManager::transition(D3D12_RESOURCE_STATES stateBefore, D3D12_RESOURCE_STATES stateAfter, ID3D12Resource *resource) {
    D3D12_RESOURCE_BARRIER resourceBarrier = {};
    resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    resourceBarrier.Transition.pResource = resource;
    resourceBarrier.Transition.StateBefore = stateBefore;
    resourceBarrier.Transition.StateAfter = stateAfter;
    resourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    commandList->ResourceBarrier(1, &resourceBarrier);
}