#pragma once

#include "Marcos/Marcos.hpp"
#include "WorkerInterface.hpp"

/*
 buffer struct design (with not padding):

 struct {
    bool-> madeBindless : made texture as Bindless or use default texture
 binding way int -> textureType : type of texture int -> width : width of
 texture int -> height : height of texture int -> nrChannels : number of
 channels of texture int* keys : keys of texture parameters int* values : values
 of texture parameters char* data : data of texture
 };
 */
enum TextureParamIndex {
  eMadeBindless = 0,
  eTextureType,
  eWidth,
  eHeight,
  eNrChannels,
  eCustomStart  // here means the start of custom parameters
};

enum TextureType { eTexture2D = 0, eTexture3D, eTextureCubeMap };

class Buffer;
ROOT_BGN()

class TextureWorker : public WorkerInterface {
 public:
  TextureWorker() = default;

  ~TextureWorker() override = default;

  Handle Compile(std::vector<char> &buffer) override;

  Handle Compile(Buffer &buffer) override;

  bool Release(Handle handle) override;

  bool Reload(Handle handle) override;

  bool IsValid(Handle handle) override;

 private:
  /// @brief convert texture type to opengl texture type
  /// @param textureType enum TextureType as int
  /// @return according to textureType return opengl texture type if not exist
  /// return GL_TEXTURE_2D as default
  static unsigned int ConvertTextureType(int textureType);

  __forceinline static void UseDefatultParams(Handle &handle);
};

ROOT_END()