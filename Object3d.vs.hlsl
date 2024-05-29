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
    output.position = input.position;
    return output;
}