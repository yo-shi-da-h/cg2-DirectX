#include "object3d.hlsli"
struct TransformationMatrix
{
    float32_t4x4 WVP;
};
ConstantBuffer<TransformationMatrix> gTranfsformationMatrix : register(b0);



struct VertexShaderInput
{
    float32_t4 posision : POSITION;
    float32_t2 texcoord : TEXCOORD0;
};

VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;
    output.posision = mul(input.posision, gTranfsformationMatrix.WVP);
    output.texcoord = input.texcoord;
    return output;
}