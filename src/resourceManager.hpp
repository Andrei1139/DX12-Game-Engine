#pragma once
#include <vector>
#include <d3d12.h>
#include "camera.hpp"
#include "model.hpp"

class ResourceManager {
    public:
        ResourceManager(const Camera &pCamera, Microsoft::WRL::ComPtr<ID3D12Device> &pDeviceInterface,
                        Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> &pCommandList):
                        camera(pCamera), deviceInterface{pDeviceInterface}, commandList{pCommandList} {}
        void addModel(Model model);
        void createResources() {initVertexProcessing(); initIndexProcessing(); initCTBufferProcessing();}
        void updateResources() {updateCTBuffer();}

        const Model &getModelAt(int index) {return models.at(index);}
        size_t getNumModels() const {return models.size();}

        D3D12_VERTEX_BUFFER_VIEW *getVertexBufferView() {return &vertexBufferView;}
        D3D12_INDEX_BUFFER_VIEW *getIndexBufferView() {return &indexBufferView;}
        ID3D12DescriptorHeap **getDescriptorHeap() {return CTDescriptorHeap.GetAddressOf();}

    private:
        void initVertexProcessing();
        void initIndexProcessing();
        void initCTBufferProcessing();
        void updateCTBuffer();
        void createBuffer(UINT64 width, D3D12_HEAP_TYPE heapType, D3D12_RESOURCE_STATES resourceState, ID3D12Resource **resource);
        void copyDataToBuffer(void *data, UINT64 dataLen, ID3D12Resource **buffer);
        void transition(D3D12_RESOURCE_STATES stateBefore, D3D12_RESOURCE_STATES stateAfter, ID3D12Resource *resource);

        std::vector<Model> models;
        std::vector<Vertex> aggregateVertexList;
        std::vector<UINT32> aggregateIndexList;
        std::vector<DirectX::XMFLOAT3> aggregateCoordsList;

        std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> vertexBuffers = std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>>(1);
        std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> indexBuffers = std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>>(1);
        std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> constantBuffers = std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>>(1);
        std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> intermediaryBuffers = std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>>(2);

        D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
        D3D12_INDEX_BUFFER_VIEW indexBufferView;
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CTDescriptorHeap;

        const Camera &camera;
        Microsoft::WRL::ComPtr<ID3D12Device> &deviceInterface;
        Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> &commandList;

        HRESULT res;
};