struct TransformationMatrix
{
    float32_t4x4 WVP;
};
ConstantBuffer<TransformationMatrix> gTranfsformationMatrix : register(b0);

struct VertexShaderOutput
{
    float32_t4 posision : SV_POSITION;
};

struct VertexShaderInput
{
    float32_t4 posision : POSITION0;
};

VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;
    output.posision = mul(input.posision, gTranfsformationMatrix.WVP);
    return output;
}