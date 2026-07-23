#pragma once

#define FPS 200
#define NORM(DAMPER) (DAMPER * FPS)

#include <iostream>
#include <cmath>
#include <vector>
#include <d3d12.h>
#include <DirectXMath.h>
#include <wrl.h>

typedef struct {
    float x, y, z;
    float r, g, b;
} Vertex;

static void printHFAILEDoutputGlobal(Microsoft::WRL::ComPtr<ID3D12Device> &deviceInterface) {
    Microsoft::WRL::ComPtr<ID3D12InfoQueue> infoQueue;
    if (!FAILED(deviceInterface.As(&infoQueue))) {
        UINT64 count = infoQueue->GetNumStoredMessages();
        for (auto i = 0; i < count; i++) {
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