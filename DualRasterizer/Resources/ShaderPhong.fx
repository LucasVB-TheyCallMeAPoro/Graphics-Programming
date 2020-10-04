//Global variables
float4x4 gWorldViewProjectionMatrix : WorldViewProjectionMatrix;
Texture2D gDiffuseMap : DiffuseMap;
Texture2D gNormalMap : NormalMap;
Texture2D gSpecularMap : SpecularMap;
Texture2D gGlossinessMap : GlossinessMap;
static const float3 gDirectionLight = float3(0.577f,-0.577f,0.577f);
float3 gCameraPosition : CameraPosition;
float4x4 gWorldMatrix : WorldMatrix;
static const float gPI = 3.1415;
static const float gLightIntensity = 7.0f/gPI;
static const float gShininess = 25.f;
//---------------------------------------------------------------------------
//	RasterizerState
//---------------------------------------------------------------------------
RasterizerState gRasterizerState
{
	CullMode = back;
	FrontCounterClockwise = true;
};
//---------------------------------------------------------------------------
//	BlendState
//---------------------------------------------------------------------------
BlendState gBlendState
{
	
};
//---------------------------------------------------------------------------
//	DepthStencilState
//---------------------------------------------------------------------------
DepthStencilState gDepthStencilState
{

};
//---------------------------------------------------------------------------
//	SamplePoint
//---------------------------------------------------------------------------
SamplerState samPoint
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = Border; //or Mirror or Clamp
	AddressV = Border; //or Mirror or Clamp
	BorderColor = float4(0.f,0.f,1.f,1.f);	
};
//---------------------------------------------------------------------------
//	SampleLinear
//---------------------------------------------------------------------------
SamplerState samLinear
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Border; //or Mirror or Clamp
	AddressV = Border; //or Mirror or Clamp
	BorderColor = float4(0.f,0.f,1.f,1.f);	
};
//---------------------------------------------------------------------------
//	SampleAnisotropic
//---------------------------------------------------------------------------
SamplerState samAniso
{
	Filter = ANISOTROPIC;
	AddressU = Border; //or Mirror or Clamp
	AddressV = Border; //or Mirror or Clamp
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
	output.WorldPosition = mul(float4(input.Position, 1.f), gWorldMatrix);
	output.Uv = input.Uv;
	output.Normal = mul(input.Normal,(float3x3)gWorldMatrix);
	output.Tangent = mul(input.Tangent,(float3x3)gWorldMatrix);
	return output;
}
//---------------------------------------------------------------------------
//	Blinn-Phong
//---------------------------------------------------------------------------
struct Lighting
{
    float3 Diffuse;
    float3 Specular;
};

struct DirectionalLight
{
	float3 direction;
	float3 diffuseColor;
	float  diffusePower;
	float3 specularColor;
	float  exponent;
};

Lighting GetDirectionLight(DirectionalLight light, float3 viewDir, float3 normal)
{
	Lighting outLight;
	if (light.diffusePower > 0)
	{
		float3 lightDir = light.direction; 

		//Inensity of diffuse light
		//range 0-1
		float intensity = saturate(dot(normal, lightDir));

		// Setting the diffuse color
		outLight.Diffuse = intensity * light.diffuseColor * light.diffusePower;

		//Calculate the half vector between the light direction and the view direction.
		float3 halfVector = normalize(lightDir + viewDir);

		//Intensity of the specular light
		intensity = pow(saturate(dot(normal, halfVector)), light.exponent);
		//setting the specular color
		outLight.Specular = intensity * light.specularColor; 
	}
	return outLight;
}
//---------------------------------------------------------------------------
//	Pixel Shader Point
//---------------------------------------------------------------------------
float4 PSP(VS_OUTPUT input) : SV_TARGET
{
	//Reading normal from map and transform them
	float3 binormal = cross(normalize(input.Normal),normalize(input.Tangent));
	float3 tangentNormal = gNormalMap.Sample(samPoint,input.Uv).xyz;
	tangentNormal = normalize( tangentNormal * 2 - 1);
	float3x3 TBN = float3x3(normalize(input.Tangent), binormal, normalize(input.Normal));
	float3 worldNormal = mul( tangentNormal, TBN);
	float3 normal = normalize(worldNormal);

	//calculating finalcolor with Blinn-phong shading
	float3 viewDirection = normalize(gCameraPosition.xyz - input.WorldPosition.xyz);

	DirectionalLight light;
	light.direction = -gDirectionLight;
	light.diffuseColor = gDiffuseMap.Sample(samPoint, input.Uv);
	light.diffusePower = gLightIntensity;
	light.specularColor = gSpecularMap.Sample(samPoint, input.Uv);
	light.exponent = mul(gGlossinessMap.Sample(samPoint, input.Uv).g , gShininess);
	Lighting calcLight = GetDirectionLight(light, viewDirection, normal);

	return float4(calcLight.Diffuse + calcLight.Specular, 1.f);
}
//--------------------------------------------------------------------------
//	Pixel Shader Linear
//---------------------------------------------------------------------------
float4 PSL(VS_OUTPUT input) : SV_TARGET
{
	//Reading normal from map and transform them
	float3 binormal = cross(normalize(input.Normal),normalize(input.Tangent));
	float3 tangentNormal = gNormalMap.Sample(samLinear,input.Uv).xyz;
	tangentNormal = normalize( tangentNormal * 2 - 1);
	float3x3 TBN = float3x3(normalize(input.Tangent), binormal, normalize(input.Normal));
	float3 worldNormal = mul( tangentNormal, TBN);
	float3 normal = normalize(worldNormal);

	//calculating finalcolor with Blinn-phong shading
	float3 viewDirection = -normalize(gCameraPosition.xyz - input.WorldPosition.xyz);

	DirectionalLight light;
	light.direction = -gDirectionLight;
	light.diffuseColor = gDiffuseMap.Sample(samLinear, input.Uv);
	light.diffusePower = gLightIntensity;
	light.specularColor = gSpecularMap.Sample(samLinear, input.Uv);
	light.exponent = mul(gGlossinessMap.Sample(samLinear, input.Uv).g , gShininess);
	Lighting calcLight = GetDirectionLight(light, viewDirection, normal);

	return float4(calcLight.Diffuse + calcLight.Specular, 1.f);
}
//---------------------------------------------------------------------------
//	Pixel Shader Anisotropic
//---------------------------------------------------------------------------
float4 PSA(VS_OUTPUT input) : SV_TARGET
{
	//Reading normal from map and transform them
	float3 binormal = cross(normalize(input.Normal),normalize(input.Tangent));
	float3 tangentNormal = gNormalMap.Sample(samAniso,input.Uv).xyz;
	tangentNormal = normalize( tangentNormal * 2 - 1);
	float3x3 TBN = float3x3(normalize(input.Tangent), binormal, normalize(input.Normal));
	float3 worldNormal = mul( tangentNormal, TBN);
	float3 normal = normalize(worldNormal);

	//calculating finalcolor with Blinn-phong shading
	float3 viewDirection = -normalize(gCameraPosition.xyz - input.WorldPosition.xyz);

	DirectionalLight light;
	light.direction = -gDirectionLight;
	light.diffuseColor = gDiffuseMap.Sample(samAniso, input.Uv);
	light.diffusePower = gLightIntensity;
	light.specularColor = gSpecularMap.Sample(samAniso, input.Uv);
	light.exponent = mul(gGlossinessMap.Sample(samAniso, input.Uv).g , gShininess);
	Lighting calcLight = GetDirectionLight(light, viewDirection,normal);

	return float4(calcLight.Diffuse + calcLight.Specular, 1.f);
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