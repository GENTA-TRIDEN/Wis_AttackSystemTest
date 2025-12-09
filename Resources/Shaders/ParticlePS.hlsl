#include "Particle.hlsli"

Texture2D tex : register(t0);
Texture2D tex2 : register(t1);
SamplerState samLinear : register(s0);

float4 blur(float2 uv)
{
    float power = 0.1f;
    float center = 1.0f - power * 9.0f;
    float4 output = 0;
    for (int i = 0; i < 9; i++)
    {
        output += tex.Sample(samLinear, uv +
        float2(((i % 3) - 1) * 0.01f, ((i / 3) - 1) * 0.01f)) * power;
    }
    output += tex.Sample(samLinear, uv) * center;
    return output;
}

float gray(float4 col)
{
    return col.r * 0.3f + col.g * 0.6f + col.b * 0.1f;
}

float4 sepia(float4 col)
{
    float g = gray(col);
    return float4(g * 1.44f, g * 0.99f, g * 0.57f, col.a);
}


float4 main(PS_INPUT input) : SV_TARGET
{
    
    return float4(0.1f, 0.1f, 0.1f, 0.8f);

}