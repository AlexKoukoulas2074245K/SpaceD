/*************************************************************************************/
/** default3d.vs by Alex Koukoulas (C) 2017 All Rights Reserved                     **/
/** File Description:                                                               **/
/*************************************************************************************/

cbuffer cbPerObject
{
	float4x4 gWorld;
	float4x4 gWorldInvTranspose;
	float4x4 gWorldViewProj;	
}; 

struct VertexIn
{
	float3 PosL    : POSITION;	
	float2 TexcoordL: TEXCOORD;
	float3 NormalL : NORMAL;
};

struct VertexOut
{
	float4 PosH    : SV_POSITION;
    float3 PosW    : POSITION;
	float2 texcoord : TEXCOORD0;
    float3 NormalW : NORMAL;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	
	// Transform to world space space.
	vout.PosW    = mul(gWorld, float4(vin.PosL, 1.0f)).xyz;
	vout.NormalW = mul((float3x3)gWorldInvTranspose, vin.NormalL);
		
	// Transform to homogeneous clip space.
	vout.PosH = mul(gWorldViewProj, float4(vin.PosL, 1.0f));
	vout.texcoord = vin.TexcoordL;

	return vout;
}
 
