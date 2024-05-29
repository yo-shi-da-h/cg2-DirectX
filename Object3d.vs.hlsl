float4 main( float4 pos : POSITION ) : SV_POSITION
{


	return pos;
}
struct VertexShaderOutput
{
    float32_t4 position : SV_Position;
};
struct VertexShaderInput
{
    float32_t4 position : POSITION0;
};
VectexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;
    output.position = input.position;
    return output;
}