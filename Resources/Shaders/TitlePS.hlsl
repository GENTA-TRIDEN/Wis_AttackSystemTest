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
float random(float2 uv)
{
    return frac(sin(dot(uv, float2(12.9898f, 78.233f))) * 43758.5453f);
}

float2 random2(float2 st)
{
    st = float2(dot(st, float2(127.1f, 311.7f)), dot(st, float2(269.5f, 183.3f)));
    return -1.0f + 2.0f * frac(sin(st) * 43758.5453f);
}

float noise(float2 uv)
{
    float2 p = floor(uv);
    return random(p);
}

float perlinNoise(float2 uv)
{
    float2 p = floor(uv);
    float2 f = frac(uv);
    float2 u = f * f * (3.0 - 2.0 * f);
	
    float2 v00 = random2(p + float2(0, 0));
    float2 v10 = random2(p + float2(1, 0));
    float2 v01 = random2(p + float2(0, 1));
    float2 v11 = random2(p + float2(1, 1));
	
    return lerp(
			lerp(dot(v00, f - float2(0, 0)), dot(v10, f - float2(1, 0)), u.x),
			lerp(dot(v01, f - float2(0, 1)), dot(v11, f - float2(1, 1)), u.x),
			u.y) + 0.5f;
}


float4 motionBlur(float2 uv)
{
    float power = 0.1f;

    float4 output = 0;
    for (int i = 0; i < 9; i++)
    {
        output += tex.Sample(samLinear, uv
			+ float2(sin(time.x - i * 0.1f) * 0.05f, 0.0f)) * power * (10.0f - i) / 5.0f;
    }

    return output;
}

float4 mosaic(float2 baseUV, float mag)
{
    return tex.Sample(samLinear, floor(baseUV * mag) / mag);
}


//float4 oldTV(float2 inUV)
//{
    
//    float2 uv = inUV;
//    //中心をずらす
//    uv = uv - float2(0.5f, 0.5f);
//    float vignette = length(uv);
//    uv /= 1 - vignette * 0.2f;
//    //中心を戻す
//    float2 texUV = uv + 0.5f;
//    bool bol = time.x < 2.0f;
//    float tim = (2.0f - time.x) * bol;
//    //texUV.x += (random(floor(uv.y * 100) + time.x) - 0.5f) * 0.01f;
//    texUV.x += sin(((10.0 + texUV.y) - tim + texUV.y))
//     * tim * 10.0f;
   
//    float4 base = tex.Sample(samLinear, texUV);
    
    
//    base = lerp(base, float4(0, 0, 0, 1), step(0, max(abs(uv.y) - 0.5f, abs(uv.x) - 0.5f)));

//    return base;
//}


float4 oldTV(float2 inUV)
{
    float2 uv = inUV;
//中心をずらす
    uv = uv - 0.5f; // uv - float2(0.5f,0.5f)
    float vignette = length(uv);
    uv /= 1 - vignette * 0.2f;
//中心を戻す
    float2 texUV = uv + 0.5f;
    texUV.x += (random(floor(uv.y * 100) + time.x) - 0.5f) * 0.01f;
    float4 base = tex.Sample(samLinear, texUV);
    return base;

}

float4 main(PS_INPUT input) : SV_TARGET
{
    
    
    //float4 output = mosaic(input.Tex,555.2f);
    float4 output = oldTV(input.Tex);
    
    
    float4 outputBar = tex.Sample(samLinear, input.Tex);
    
    
    return output;
    
}