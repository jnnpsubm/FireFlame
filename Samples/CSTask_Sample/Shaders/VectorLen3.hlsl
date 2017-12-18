struct Vector
{
    float x;
    float y;
    float z;
};

ConsumeStructuredBuffer<Vector> gInput : register(u0);
AppendStructuredBuffer<float> gOutput : register(u1);

[numthreads(64, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    Vector v = gInput.Consume();
    //float len = length(float3(v));
    float len = length(float3(1.f,2.f,3.f));
    gOutput.Append(len);
}