#pragma once

#define FPS 200
#define NORM(DAMPER) (DAMPER * FPS)

#include <iostream>
#include <cmath>
#include <vector>
#include <d3d12.h>
#include <DirectXMath.h>
#include <wrl.h>

typedef struct {float x, y, z;} Position;
typedef struct {float u, v;} Tex;
typedef struct {float x, y, z;} Normal;

typedef struct {
    Position pos;
    Tex tex;
} Vertex;

static bool operator==(const Vertex &v1, const Vertex &v2) {
    return v1.pos.x == v2.pos.x && v1.pos.y == v2.pos.y && v1.pos.z == v2.pos.z && v1.tex.u == v2.tex.u && v1.tex.v == v2.tex.v; 
}

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