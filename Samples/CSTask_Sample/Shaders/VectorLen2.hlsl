
Buffer<float3> gInput  : register(t0);
RWBuffer<float>  gOutput : register(u0);

[numthreads(64, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    gOutput[DTid.x] = length(gInput[DTid.x]);
}