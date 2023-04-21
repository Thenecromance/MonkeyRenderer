#pragma  once


/// @brief Enumrate all the texture types for PBR rendering ,
/// and based on this enum we can get the texture we want but also for an easy way to transfer texture and PBRtexture
enum TextureType : short {
  Diffuse = 0,
  Specular,
  Normal,
  Height,
  Emissive,
  AmbientOcclusion,
  Metalness,
  Roughness,
  Albedo,
  Irradiance,
  Prefiltered,
  BRDF,
  TextureTypeCount
};
