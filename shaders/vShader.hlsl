cbuffer matBuffer: register(b0) {
    float4x4 worldViewProjMatrix;
}

struct VS_IN {
    float3 pos: POSITION;
    float2 tex: TEXTURE;
};

struct VS_OUT {
    float4 pos: SV_Position;
    float2 tex: TEXTURE;
};

VS_OUT main(VS_IN input) {
    VS_OUT output;
    output.pos = mul(worldViewProjMatrix, float4(input.pos.x, input.pos.y, input.pos.z, 1.0));
    output.tex = input.tex;

    return output;
}