cbuffer matBuffer: register(b0) {
    float4x4 worldViewProjMatrix;
}

struct VS_IN {
    float3 pos: POSITION;
    float3 col: COLOR;
};

struct VS_OUT {
    float4 pos: SV_Position;
    float3 col: COLOR;
};

VS_OUT main(VS_IN input) {
    VS_OUT output;
    output.pos = mul(worldViewProjMatrix, float4(input.pos.x, input.pos.y, input.pos.z, 1.0));
    output.col = input.col;

    return output;
}