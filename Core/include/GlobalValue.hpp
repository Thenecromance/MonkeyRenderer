#pragma once

namespace Uniform {
enum BindingLocation {
  ePerFrameData,  // perframe data for view ,projection,CameraPosition
  eVertices,      // vertices data
  eMatrices,      // Transformed Matrices

  ePointLight,
  eDirectionLight,
  eSpotLight,
};
}