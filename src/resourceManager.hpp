#pragma once
#include <vector>
#include <d3d12.h>
#include "camera.hpp"
#include "object.hpp"

#define PADDED_SIZE(x) ((x + 255) & ~255)

class ResourceManager {
    public:
        ResourceManager(const Camera &pCamera, Microsoft::WRL::ComPtr<ID3D12Device> &pDeviceInterface,
                        Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> &pCommandList,
                        int pWidth, int pHeight):
                        camera(pCamera), deviceInterface{pDeviceInterface}, commandList{pCommandList}, width{pWidth}, height{pHeight} {}
        void addObject(Object object);
        void createResources() {initVertexProcessing(); initIndexProcessing(); initCTBufferProcessing(); initDepthStencilProcessing();}
        void updateResources() {updateCTBuffer();}

        const Model &getModelAt(int index) {return objects.at(index).getModel();}
        size_t getNumModels() const {return objects.size();}

        D3D12_VERTEX_BUFFER_VIEW *getVertexBufferView() {return &vertexBufferView;}
        D3D12_INDEX_BUFFER_VIEW *getIndexBufferView() {return &indexBufferView;}
        ID3D12DescriptorHeap **getCTDescriptorHeap() {return CTDescriptorHeap.GetAddressOf();}
        ID3D12DescriptorHeap *getDSDescriptorHeap() {return DSVHeap.Get();}

    private:
        void initVertexProcessing();
        void initIndexProcessing();
        void initCTBufferProcessing();
        void initDepthStencilProcessing();

        void updateCTBuffer();

        void createBuffer(UINT64 width, D3D12_HEAP_TYPE heapType, D3D12_RESOURCE_STATES resourceState, ID3D12Resource **resource);
        void copyDataToBuffer(void *data, UINT64 dataLen, ID3D12Resource **buffer);
        void transition(D3D12_RESOURCE_STATES stateBefore, D3D12_RESOURCE_STATES stateAfter, ID3D12Resource *resource);

        std::vector<Object> objects;
        std::vector<Vertex> aggregateVertexList;
        std::vector<UINT32> aggregateIndexList;

        UINT64 paddedCTDataSize = 0;
        UINT paddedCTElementSize = 0;

        std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> vertexBuffers = std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>>(1);
        std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> indexBuffers = std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>>(1);
        std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> constantBuffers = std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>>(1);
        std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> intermediaryBuffers = std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>>(2);
        Microsoft::WRL::ComPtr<ID3D12Resource> depthStencil;

        D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
        D3D12_INDEX_BUFFER_VIEW indexBufferView;
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CTDescriptorHeap, DSVHeap;


        const Camera &camera;
        Microsoft::WRL::ComPtr<ID3D12Device> &deviceInterface;
        Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> &commandList;

        HRESULT res;
        int width, height;
};