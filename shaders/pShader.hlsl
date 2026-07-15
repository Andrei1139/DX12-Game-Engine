struct PS_IN {
    float4 pos: SV_Position;
    float3 col: COLOR;
};

struct PS_OUT {
    float4 col: SV_Target;
};

PS_OUT main(PS_IN input) {
    PS_OUT output;
    output.col = float4(input.col.xyz, 1.0);

    return output;
}