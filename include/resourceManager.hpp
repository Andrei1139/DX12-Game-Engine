#pragma once
#include <vector>
#include <cstdio>
#include <unordered_map>
#include <d3d12.h>
#include <WICTextureLoader.h>
#include <ResourceUploadBatch.h>
#include "camera.hpp"
#include "object.hpp"

#define PADDED_SIZE(x) ((x + 255) & ~255)

class ResourceManager {
    public:
        ResourceManager(const Camera &pCamera, Microsoft::WRL::ComPtr<ID3D12Device> &pDeviceInterface,
                        Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> &pCommandList,
                        int pWidth, int pHeight):
                        camera(pCamera), deviceInterface{pDeviceInterface},
                        commandList{pCommandList},
                        width{pWidth},
                        height{pHeight},
                        RUB(pDeviceInterface.Get()) {}
        void addObject(Object object);
        void createResources(ID3D12CommandQueue *queue);
        void updateResources() {updateMatrixCTBuffer();}

        const Model &getModelAt(int index) {return objects.at(index).getModel();}
        size_t getNumModels() const {return objects.size();}

        D3D12_VERTEX_BUFFER_VIEW *getVertexBufferView() {return &vertexBufferView;}
        D3D12_INDEX_BUFFER_VIEW *getIndexBufferView() {return &indexBufferView;}
        ID3D12DescriptorHeap **getCTSRDescriptorHeap() {return CTSRDescriptorHeap.GetAddressOf();}
        ID3D12DescriptorHeap *getDSDescriptorHeap() {return DSVHeap.Get();}

        D3D12_GPU_VIRTUAL_ADDRESS getMatCTBGPUAddress() {
            return constantBuffers.at(0)->GetGPUVirtualAddress();
        }
        D3D12_GPU_VIRTUAL_ADDRESS getLightingCTBGPUAddress() {
            return constantBuffers.at(1)->GetGPUVirtualAddress();
        }
        D3D12_GPU_DESCRIPTOR_HANDLE getInitSRVDescriptorHandle() {
            auto GPUStart = CTSRDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
            GPUStart.ptr += deviceInterface->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * (numModelSections + getNumModels());
            return GPUStart;
        }

    private:
        void initTextureProcessing(ID3D12CommandQueue *queue);
        void initVertexProcessing();
        void initIndexProcessing();
        void initDescriptorHeap();
        void initMatrixCTBufferProcessing();
        void initLightingCTBufferProcessing();
        void initDepthStencilProcessing();

        void updateMatrixCTBuffer();
        void updateLightingCTBuffer();

        void createBuffer(UINT64 width, D3D12_HEAP_TYPE heapType, D3D12_RESOURCE_STATES resourceState, ID3D12Resource **resource);
        void copyDataToBuffer(void *data, UINT64 dataLen, ID3D12Resource **buffer);
        void transition(D3D12_RESOURCE_STATES stateBefore, D3D12_RESOURCE_STATES stateAfter, ID3D12Resource *resource);

        std::vector<Object> objects;
        std::vector<Vertex> aggregateVertexList;
        std::vector<UINT32> aggregateIndexList;

        size_t numModelSections = 0;

        DirectX::ResourceUploadBatch RUB;

        // Resources
        std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> vertexBuffers = std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>>(1);
        std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> indexBuffers = std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>>(1);
        std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> constantBuffers = std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>>(2);
        std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> intermediaryBuffers = std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>>(2);
        Microsoft::WRL::ComPtr<ID3D12Resource> depthStencil;
        std::unordered_map<std::wstring, Microsoft::WRL::ComPtr<ID3D12Resource>> textures;

        // Descriptors/Views
        D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
        D3D12_INDEX_BUFFER_VIEW indexBufferView;
        D3D12_CPU_DESCRIPTOR_HANDLE currDescriptorHandle, initSRVDescriptorHandle;

        // Descriptor heaps
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CTSRDescriptorHeap, DSVHeap;

        const Camera &camera;
        Microsoft::WRL::ComPtr<ID3D12Device> &deviceInterface;
        Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> &commandList;

        HRESULT res;
        int width, height;
};