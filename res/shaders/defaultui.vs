/*************************************************************************************/
/** defaultui.vs by Alex Koukoulas (C) 2017 All Rights Reserved                     **/
/** File Description:                                                               **/
/*************************************************************************************/

cbuffer cbPerObject
{
	float4x4 gWorld;
    float4   gColor;	
	int      gColorEnabled;
	int      gScrollTexCoordsEnabled;
	float2   gTexCoordOffsets;	
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
	
	vout.PosW    = mul(gWorld, float4(vin.PosL, 1.0f)).xyz;
	vout.NormalW = vin.NormalL;	
	vout.PosH     = float4(vout.PosW, 1.0f);
	vout.texcoord = vin.TexcoordL;
	
	if (gScrollTexCoordsEnabled)
	{
	    vout.texcoord.x += gTexCoordOffsets.x;
		vout.texcoord.y += gTexCoordOffsets.y;
	}

	return vout;
}
 
