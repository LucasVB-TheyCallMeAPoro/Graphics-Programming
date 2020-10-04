//Global variables
float4x4 gWorldViewProjectionMatrix : WorldViewProjectionMatrix;
Texture2D gDiffuseMap : DiffuseMap;
float3 gCameraPosition : CameraPosition;
float4x4 gWorldMatrix : WorldMatrix;
//---------------------------------------------------------------------------
//	RasterizerState
//---------------------------------------------------------------------------
RasterizerState gRasterizerState
{
	CullMode = NONE;
	FrontCounterClockwise = true;
};
//---------------------------------------------------------------------------
//	BlendState
//---------------------------------------------------------------------------
BlendState gBlendState
{
	BlendEnable[0] = true;
	SrcBlend = src_alpha;
	DestBlend = inv_src_alpha;
	BlendOp = add;
	SrcBlendAlpha = zero;
	DestBlendAlpha = zero;
	BlendOpAlpha = add;
	RenderTargetWriteMask[0] = 0x0F;
};
//---------------------------------------------------------------------------
//	DepthStencilState
//---------------------------------------------------------------------------
DepthStencilState gDepthStencilState
{
	DepthEnable = true;
	DepthWriteMask = zero;
	DepthFunc = less;
	StencilEnable = false;

	StencilReadMask = 0x0F;
	StencilWriteMask = 0x0F;

	FrontFaceStencilFunc = always;
	BackFaceStencilFunc = always;

	FrontFaceStencilDepthFail = keep;
	BackFaceStencilDepthFail = keep;

	FrontFaceStencilPass = keep;
	BackFaceStencilPass = keep;

	FrontFaceStencilFail = keep;
	BackFaceStencilFail = keep;

};
//---------------------------------------------------------------------------
//	SamplePoint
//---------------------------------------------------------------------------
SamplerState samPoint
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = Clamp; //or Mirror or Border
	AddressV = Clamp; //or Mirror or Border
	BorderColor = float4(0.f,0.f,1.f,1.f);	
};
//---------------------------------------------------------------------------
//	SampleLinear
//---------------------------------------------------------------------------
SamplerState samLinear
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Clamp; //or Mirror or Border
	AddressV = Clamp; //or Mirror or Border
	BorderColor = float4(0.f,0.f,1.f,1.f);	
};
//---------------------------------------------------------------------------
//	SampleAnisotropic
//---------------------------------------------------------------------------
SamplerState samAniso
{
	Filter = ANISOTROPIC;
	AddressU = Clamp; //or Mirror or Border
	AddressV = Clamp; //or Mirror or Border
	MaxAnisotropy = 8; //Amount of samples
};
//---------------------------------------------------------------------------
// 	Input/Output Structs
//---------------------------------------------------------------------------
struct VS_INPUT
{
	float3 Position : POSITION;
	float2 Uv : TEXCOORD;
	float3 Normal : NORMAL;
	float3 Tangent : TANGENT;
};

struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
	float3 WorldPosition : COLOR;
	float2 Uv : TEXCOORD;
	float3 Normal : NORMAL;
	float3 Tangent : TANGENT;
};

//---------------------------------------------------------------------------
//	Vertex Shader
//---------------------------------------------------------------------------
VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	output.Position = mul(float4(input.Position, 1.f),gWorldViewProjectionMatrix);
	output.WorldPosition = mul(float4(input.Position, 1.f),gWorldMatrix);
	output.Uv = input.Uv;
	output.Normal = mul(input.Normal,(float3x3)gWorldMatrix);
	output.Tangent = mul(input.Tangent,(float3x3)gWorldMatrix);
	return output;
}
//---------------------------------------------------------------------------
//	Pixel Shader Point
//---------------------------------------------------------------------------
float4 PSP(VS_OUTPUT input) : SV_TARGET
{
	return gDiffuseMap.Sample(samPoint, input.Uv);
}
//---------------------------------------------------------------------------
//	Pixel Shader Linear
//---------------------------------------------------------------------------
float4 PSL(VS_OUTPUT input) : SV_TARGET
{
	return gDiffuseMap.Sample(samLinear, input.Uv);
}
//---------------------------------------------------------------------------
//	Pixel Shader Anisotropic
//---------------------------------------------------------------------------
float4 PSA(VS_OUTPUT input) : SV_TARGET
{
	return gDiffuseMap.Sample(samAniso, input.Uv);
}
//---------------------------------------------------------------------------
//	Technique Point
//---------------------------------------------------------------------------
technique11 DefaultTechnique
{
	pass P0
	{
		SetRasterizerState(gRasterizerState);
		SetDepthStencilState(gDepthStencilState, 0);
		SetBlendState(gBlendState, float4(0.0f,0.0f,0.0f,0.0f), 0xFFFFFFFF);
		SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader( ps_5_0, PSP() ) );
	}
}
//---------------------------------------------------------------------------
//	Technique Linear
//---------------------------------------------------------------------------
technique11 TechniqueLinear
{
	pass P0
	{
		SetRasterizerState(gRasterizerState);
		SetDepthStencilState(gDepthStencilState, 0);
		SetBlendState(gBlendState, float4(0.0f,0.0f,0.0f,0.0f), 0xFFFFFFFF);
		SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader( ps_5_0, PSL() ) );
	}
}
//---------------------------------------------------------------------------
//	Technique Anisotropic
//---------------------------------------------------------------------------
technique11 TechniqueAnisotropic
{
	pass P0
	{
		SetRasterizerState(gRasterizerState);
		SetDepthStencilState(gDepthStencilState, 0);
		SetBlendState(gBlendState, float4(0.0f,0.0f,0.0f,0.0f), 0xFFFFFFFF);
		SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader( ps_5_0, PSA() ) );
	}
}
