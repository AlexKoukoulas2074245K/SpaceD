/*************************************************************************************/
/** defaultui.vs by Alex Koukoulas (C) 2017 All Rights Reserved                     **/
/** File Description:                                                               **/
/*************************************************************************************/

cbuffer cbPerObject
{
	float4x4 gWorld;
}; 

struct VertexIn
{
	float3 PosL    : POSITION;	
	float2 TexcoordL: TEXCOORD;	
};

struct VertexOut
{
	float4 PosW    : SV_POSITION;    
	float2 texcoord : TEXCOORD0;    
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	
	// Transform to world space space.
	vout.PosW    = mul(gWorld, float4(vin.PosL, 1.0f));
	vout.texcoord = vin.TexcoordL;

	return vout;
}
 
