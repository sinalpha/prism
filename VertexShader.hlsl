cbuffer b0 : register(b0)
{
    matrix Mat;
}

float4 main(float3 pos : POSITION) : SV_POSITION
{
    return mul(Mat, float4(pos, 1.f));
}
