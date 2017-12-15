struct Vector
{
    float x;
    float y;
    float z;
};

StructuredBuffer<Vector> gInput : register(t0);
RWStructuredBuffer<float> gOutput : register(u0);

[numthreads(64, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    float3 v = float3(gInput[DTid.x].x, gInput[DTid.x].y, gInput[DTid.x].z);
    gOutput[DTid.x] = length(v);

    // testing
    /*unsigned int i = 0;
    while (++i < 10000*10)
    {
        gOutput[DTid.x] = length(float3(length(v)*length(v), length(v)*length(v), length(v)*length(v)));
        gOutput[DTid.x] = cos(pow(abs(gOutput[DTid.x]), 128));
    }*/
}