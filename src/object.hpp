#pragma once
#include "model.hpp"
#include "utils.hpp"

class Object {
    public:
        Object(Model pModel): model{pModel} {}

        Object &setPosX(float pX) {x = pX; return *this;}
        Object &setPosY(float pY) {y = pY; return *this;}
        Object &setPosZ(float pZ) {z = pZ; return *this;}
        Object &setScaleX(float pX) {scaleX = pX; return *this;}
        Object &setScaleY(float pY) {scaleY = pY; return *this;}
        Object &setScaleZ(float pZ) {scaleZ = pZ; return *this;}
        Object &setRotX(float pX) {rotX = pX; return *this;}
        Object &setRotY(float pY) {rotY = pY; return *this;}
        Object &setRotZ(float pZ) {rotZ = pZ; return *this;}

        DirectX::XMVECTOR getPosition() {return DirectX::XMVectorSet(x, y, z, 0.0f);}
        DirectX::XMVECTOR getScaling() {return DirectX::XMVectorSet(scaleX, scaleY, scaleZ, 0.0f);}
        DirectX::XMVECTOR getRotation() {return DirectX::XMVectorSet(rotX, rotY, rotZ, 0.0f);}
        const Model &getModel() {return model;}
    private:
        Model model;
        float x = 0.0f, y = 0.2f, z = 1.0f;
        float scaleX = 1.0f, scaleY = 1.0f, scaleZ = 1.0f;
        float rotX = 0.0f, rotY = 0.0f, rotZ = 0.0f;
};