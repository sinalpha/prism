cbuffer b0 : register(b0)
{
    float4x4 Mat;
}

void main(
	float3 pos : POSITION,
	out float4 outPos : SV_POSITION,
	out float4 outCOLOR : COLOR
)
{
    outPos = mul(float4(pos, 1.0f), Mat);
    outCOLOR = float4(1.0f, 1.0f, 1.0f, 1.0f);

}