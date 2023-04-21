
#include "TextureWorker.hpp"

#include <glad/glad.h>

#include <cassert>

#include "Buffer/Buffer.hpp"

ROOT_BGN()

Handle TextureWorker::Compile(std::vector<char> &buffer) {
  // TODO: implement
  assert(false &&
         "TextureWorker::Compile(std::vector<char> &buffer) not implemented");
  return 0;
}

// how to return 2 value from function..... without change the function
// interface
Handle TextureWorker::Compile(Buffer &buffer) {
  if (!buffer.IsModified()) {
    assert(
        false &&
        "TextureWorker::Compile(Buffer &buffer) Load texture must has params");
  }

  const auto madeBindless = buffer.values(
      TextureParamIndex::eMadeBindless);  // get image madeBindless from buffer
  const auto textureType = ConvertTextureType(static_cast<int>(buffer.values(
      TextureParamIndex::eTextureType)));  // get image textureType from buffer
  const auto width =
      buffer.values(TextureParamIndex::eWidth);  // get image width from buffer
  const auto height = buffer.values(
      TextureParamIndex::eHeight);  // get image height from buffer
  const auto nrChannels = buffer.values(
      TextureParamIndex::eNrChannels);  // get image nrChannels from buffer
  /*const auto image = buffer.data(); //get image data from buffer
   */

  // in msvc image data should be copied from the buffer to post to the memory
  // otherwise it will throw an exception which is access violation
  const char *image = (const char *)malloc(buffer.buffer_size());
  memcpy((void *)image, buffer.data(), buffer.buffer_size());

  Handle handle{0};
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glCreateTextures(textureType, 1, &handle);
  glTextureStorage2D(handle, 1, GL_RGB8, width, height);
  glTextureSubImage2D(handle, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE,
                      image);
  glGenerateTextureMipmap(handle);

//  glTextureStorage1D(handle, 1, GL_RGB32F, width);

  // if here is no custom parameters, use default parameters
  if ((TextureParamIndex::eCustomStart) >= buffer.param_count()) {
    UseDefatultParams(handle);
  } else {
    for (int i = (int)(TextureParamIndex::eCustomStart);
         i < buffer.param_count(); ++i) {
      glTextureParameteri(handle, buffer.keys(i), buffer.values(i));
    }
  }

  if (madeBindless == 0) {
    glBindTexture(textureType, handle);
    return handle;
  } else {
    auto handleBindless = static_cast<Handle>(glGetTextureHandleARB(handle));
    glMakeTextureHandleResidentARB(handleBindless);

    return handleBindless;
  }
}

bool TextureWorker::Release(Handle handle) {
  // TODO: TextureWithBindless is not implemented
  /*if (false) {
          glMakeTextureHandleNonResidentARB(handleBindless_);
  }*/
  glDeleteTextures(1, &handle);

  return false;
}

bool TextureWorker::Reload(Handle handle) { return false; }

bool TextureWorker::IsValid(Handle handle) {
  // check texture is valid
  return false;
}

unsigned int TextureWorker::ConvertTextureType(int textureType) {
  if (TextureType::eTexture2D == textureType) {
    return GL_TEXTURE_2D;
  }
  if (TextureType::eTexture3D == textureType) {
    return GL_TEXTURE_3D;
  }
  if (TextureType::eTextureCubeMap == textureType) {
    return GL_TEXTURE_CUBE_MAP;
  }
  return GL_TEXTURE_2D;
}

/// @brief Use default parameters for texture
/// @param handle
void TextureWorker::UseDefatultParams(Handle &handle) {
  glTextureParameteri(handle, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTextureParameteri(handle, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTextureParameteri(handle, GL_TEXTURE_MAX_LEVEL, 0);
  glTextureParameteri(handle, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTextureParameteri(handle, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

/*	Handle TextureWorker::GenerateWithHandle(unsigned char *data, int width,
   int height, int nrChannels) { Handle handle = 0; glGenTextures(1, &handle);
                glBindTexture(GL_TEXTURE_2D, handle);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
   GL_LINEAR); glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                if (nrChannels == 1) {
                        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0,
   GL_RED, GL_UNSIGNED_BYTE, data); } else if (nrChannels == 3) {
                        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
   GL_RGB, GL_UNSIGNED_BYTE, data); } else if (nrChannels == 4) {
                        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height,
   0, GL_RGBA, GL_UNSIGNED_BYTE, data);
                }
                glGenerateMipmap(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, 0);
                return handle;
        }

        Handle TextureWorker::GenerateWithHandleBindless(unsigned char *data,
   int width, int height, int nrChannels) {

                Handle handle{0};
                glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
                glCreateTextures(GL_TEXTURE_2D, 1, &handle);

                glTextureStorage2D(handle, 1, GL_RGBA8, width, height);
                glTextureSubImage2D(handle, 0, 0, 0, width, height, GL_RGBA,
   GL_UNSIGNED_BYTE, data); glGenerateTextureMipmap(handle);
                glTextureParameteri(handle, GL_TEXTURE_MIN_FILTER,
   GL_LINEAR_MIPMAP_LINEAR); glTextureParameteri(handle, GL_TEXTURE_MAG_FILTER,
   GL_LINEAR); glTextureParameteri(handle, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTextureParameteri(handle, GL_TEXTURE_WRAP_T, GL_REPEAT);
                Handle handleBindless = glGetTextureHandleARB(handle);
                glMakeTextureHandleResidentARB(handleBindless);
        }*/

ROOT_END()
