#define MAX_LIGHTS (8)
#define GAMMA (2.2)

struct light_t 
{
   float3 color; 
   float intensity; 

   float3 position; 
   float pad10; 

   float3 direction; 
   float is_directional;

   float3 diffuse_attenuation; 
   float pad20; 

   float3 specular_attenuation;
   float pad30; 
};

//--------------------------------------------------------------------------------------
cbuffer light_buffer : register(b4) 
{
   float4 AMBIENT; 

   float SPEC_FACTOR;   // (0, 1), limits specular amount
   float SPEC_POWER; 
   float EMISSIVE_FACTOR;

   float pad00; 

   light_t LIGHTS[MAX_LIGHTS]; 
};

//--------------------------------------------------------------------------------------
struct lighting_t
{
   float3 diffuse; 
   float3 specular; 
};


float GetEmissiveFactor()
{
	return EMISSIVE_FACTOR;
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
lighting_t GetLighting( float3 eye_pos, float3 surface_position, float3 surface_normal )
{

   lighting_t lighting; 
   lighting.diffuse = (AMBIENT.rgb * AMBIENT.a); 
   //lighting.diffuse = (0.f); 

   float3 dir_to_eye = normalize(eye_pos - surface_position); 


   // directional light; 
   for (int i = 0; i < MAX_LIGHTS; i++) 
   {
      light_t light = LIGHTS[i]; 

      // directional 
      float3 dir_dir = normalize(light.direction); 
      float3 point_dir = normalize(surface_position - light.position); 
      float3 light_dir = lerp( point_dir, dir_dir, light.is_directional ); 

      // common things
      // directional light
      float dir_dist = abs( dot( (surface_position - light.position), light.direction ) );   // for directional
      float point_dist = length( surface_position - light.position );                          // for point
      float distance = lerp( point_dist, dir_dist, light.is_directional ); 

      // Diffuse Part
      float3 la = light.diffuse_attenuation; 
      float attenuation = 1.0f / (la.x + la.y * distance + la.z * distance * distance); 

      float dot3 = max( dot( -light_dir, surface_normal ), 0.0f ); 

      float3 diffuse_color = light.color * light.intensity * attenuation * dot3; 
      //float3 diffuse_color = light.color * light.intensity * 1.0f * dot3; 
      lighting.diffuse += diffuse_color; 

      
      // Specular 
      // blinn-phong 
      // dot( H, N );  -> H == half_vector, N == normal
		//Edit: Make sure the direction between light and normal is less than 90 degrees in difference
      float dotCheck = dot (surface_normal, -light_dir);
      float dotResult = (dotCheck >= 0.0f) ? 1.0f : 0.0f; 


      float3 dir_to_light = -light_dir; 
      float3 half_vector = normalize( dir_to_eye + dir_to_light ); 
      float spec_coefficient = dotResult * max( dot( half_vector, surface_normal ), 0.0f ); // DO not saturate - spec can go higher;  

      float3 sa = light.specular_attenuation; 
      float spec_attenuation = 1.0f / (sa.x + sa.y * distance + sa.z * distance * distance); 

      // finalize coefficient
      spec_coefficient = SPEC_FACTOR * pow( spec_coefficient, SPEC_POWER ); 
      float3 specular_color = light.color * light.intensity * spec_attenuation * spec_coefficient; 
      //float3 specular_color = light.color * light.intensity * spec_coefficient; 
      lighting.specular += specular_color; 
      
   }

   lighting.diffuse = saturate(lighting.diffuse); // clamp this to (0, 1)
   // light.specular is untouched - greater than one can tell us information on how bright it is - used for bloom;  

   return lighting; 
}
