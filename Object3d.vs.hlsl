struct TransformationMatrix
{
    float32_t4x4 WVP;
};
ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);
struct VertexShaderOutput
{
    float32_t4 position : SV_Position;
};

struct VertxShaderInput
{
    float32_t4 position : POSITION;
};

VertexShaderOutput main(VertxShaderInput input)
{
    VertexShaderOutput output;
    output.position = mul(input.position, gTransformationMatrix.WVP);
    return output;
}