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
    
    		//	if•¶‚Å’†‰›‚©‚çŠÛ‚­Ø‚è”²‚­
    float aspect = 720.0f / 720.0f;
    float2 uv = input.Tex;
    float scales = 1.0f;

    float4 output2 = input.Color;

    float Distance = distance(
		uv * scales,
		float2(0.5f * scales * aspect,
			0.5f * scales));

    
    //ŠO‚És‚­‚É‚Â‚ê‚Ä‚¾‚ñ‚¾‚ñ“§–¾‚É‚·‚é
    output2 = float4(output2.xyz, saturate(1.0f - Distance*2.0f ));
    
    
  //  //0.01fˆÈ‰º‚ðŠ®‘S‚É“§–¾‚É‚·‚é
  //  output2 = float4(output2.xyz, distance(
		//uv * scales,
		//float2(0.5f * scales * aspect,
		//	0.5f * scales)) < 0.01f);
    
    
    
    
    

    return output2;

    
    
    return input.Color;
    return float4(0.1f, 0.1f, 0.1f, 0.8f);

}