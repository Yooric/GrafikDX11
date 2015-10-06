///Globaler
cbuffer MatrixBuffer
{
	matrix worldMatrix
	matrix viewMatrix;
	matrix projectionMatrix;
}

//typdefs
struct VertexInputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
}

struct PixelInput
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
}

//VertexCheddarn
PixelInputType TextureVertexShader(VertexInputType input)
{
	PixelInputType output;
	//behöver ett fjärde värde på position för matrix calculations
	input.position.w = 1.0f;

	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	output.tex = input.tex;
}