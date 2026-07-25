Texture2D tex: register(t0);
SamplerState samplerState: register(s0);

struct PS_IN {
    float4 pos: SV_Position;
    float2 tex: TEXTURE;
};

struct PS_OUT {
    float4 col: SV_Target;
};

PS_OUT main(PS_IN input) {
    PS_OUT output;
    output.col = tex.Sample(samplerState, input.tex);

    return output;
}