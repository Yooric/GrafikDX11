
//Tydligen Globals, Man kommer �t dem fr�n c++koden OFC�rse
cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

////Typedefs
struct VertexInputType
{
	float4 position : POSITION;
	float4 color : COLOR;
};
struct PixelInputType
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};


//VertexShader
PixelInputType ColorVertexShader(VertexInputType input)
{
	PixelInputType output;
	input.position.w = 1.0f;
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);
	output.color = input.color;
	return output;
}