#include "dot3_include.hlsl"

//--------------------------------------------------------------------------------------
// Stream Input
// ------
// Stream Input is input that is walked by the vertex shader.  
// If you say "Draw(3,0)", you are telling to the GPU to expect '3' sets, or 
// elements, of input data.  IE, 3 vertices.  Each call of the VertxShader
// we be processing a different element. 
//--------------------------------------------------------------------------------------

// inputs are made up of internal names (ie: uv) and semantic names
// (ie: TEXCOORD).  "uv" would be used in the shader file, where
// "TEXCOORD" is used from the client-side (cpp code) to attach ot. 
// The semantic and internal names can be whatever you want, 
// but know that semantics starting with SV_* usually denote special 
// inputs/outputs, so probably best to avoid that naming.
struct vs_input_t 
{
   float3 position      : POSITION;
   float3 normal        : NORMAL;
   float3 tangent       : TANGENT;
   float3 biTangent     : BITANGENT;

   float4 color         : COLOR; 
   float2 uv            : TEXCOORD; 
}; 



//--------------------------------------------------------------------------------------
// Uniform Input
// ------
// Uniform Data is also externally provided data, but instead of changing
// per vertex call, it is constant for all vertices, hence the name "Constant Buffer"
// or "Uniform Buffer".  This is read-only memory; 
//
// I tend to use all cap naming here, as it is effectively a 
// constant from the shader's perspective. 
//
// register(b2) determines the buffer unit to use.  In this case
// we'll say this data is coming from buffer slot 2. 
//--------------------------------------------------------------------------------------
cbuffer camera_constants : register(b2)
{
   float4x4 VIEW; 
   float4x4 PROJECTION; 
   
   float3 CAMERA_POSITION;    
   float cam_unused0;   
};

//--------------------------------------------------------------------------------------
cbuffer model_constants : register(b3)
{
   float4x4 MODEL;  // LOCAL_TO_WORLD
}

//--------------------------------------------------------------------------------------
// Texures & Samplers
// ------
// Another option for external data is a Texture.  This is usually a large
// set of data (like an image) that we want to "sample" from.  
//
// A sampler are the rules for how to collect texel data for a given UV. 
//
// Like constant buffers, these hav ea slot they're expecting to be bound
// t0 means use texture unit 0,
// s0 means use sampler unit 0,
//
// In D3D11, constant buffers, textures, and samplers all have their own set 
// of slots.  Some data types may share a slot space (for example, unordered access 
// views (uav) use the texture space). 
//--------------------------------------------------------------------------------------
Texture2D<float4> tAlbedo : register(t0); // texutre I'm using for albedo (color) information
SamplerState sAlbedo : register(s0);      // sampler I'm using for the Albedo texture

Texture2D<float4> tNormalMap : register(t1);   // default "flat" (.5, .5, 1.0)
SamplerState sNormalMap : register(s1);

Texture2D<float4> tEmissiveMap : register(t2); // defualt "black"
SamplerState sEmissiveMap : register(s2);

//--------------------------------------------------------------------------------------
// Programmable Shader Stages
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// for passing data from vertex to fragment (v-2-f)
struct v2f_t 
{
   float4 position : SV_POSITION; 
   float3 normal : NORMAL;
   float3 tangent : TANGENT;
   float3 biTangent : BITANGENT;

   float3 worldPos : WORLDPOS;
   float4 color : COLOR; 
   float2 uv : UV; 
}; 

//--------------------------------------------------------------------------------------
float RangeMap( float v, float inMin, float inMax, float outMin, float outMax ) 
{ 
   return ( ( (v - inMin) * (outMax - outMin) / (inMax - inMin) ) + outMin); 
}

//--------------------------------------------------------------------------------------
// Vertex Shader
v2f_t VertexFunction(vs_input_t input)
{
   v2f_t v2f = (v2f_t)0;

   float4 local_pos = float4( input.position, 1.0f ); 
   float4 world_pos = mul( MODEL, local_pos );
   float4 view_pos = mul( VIEW, world_pos ); 
   float4 clip_pos = mul( PROJECTION, view_pos ); 

   v2f.position = clip_pos;

   //v2f.color = input.color; 

   float4 normals = mul( MODEL, float4(input.normal, 0.f));
   float4 tangents = mul( MODEL, float4(input.tangent, 0.f));

   v2f.color.x = RangeMap(tangents.x, -1.0f, 1.0f, 0.0f, 1.0f);
   v2f.color.y = RangeMap(tangents.y, -1.0f, 1.0f, 0.0f, 1.0f);
   v2f.color.z = RangeMap(tangents.z, -1.0f, 1.0f, 0.0f, 1.0f);
   v2f.color.w = 1.0f;

   v2f.uv = input.uv; 
   
    
   return v2f;
}

//--------------------------------------------------------------------------------------
// Fragment Shader
// 
// SV_Target0 at the end means the float4 being returned
// is being drawn to the first bound color target.
float4 FragmentFunction( v2f_t input ) : SV_Target0
{
   // First, we sample from our texture
   //float4 texColor = tAlbedo.Sample( sAlbedo, input.uv ); 

   // component wise multiply to "tint" the output
   float4 finalColor = input.color; 

   // output it; 
   return finalColor; 
}
