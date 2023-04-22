#include "PostProcessModule.hpp"

#include <flecs.h>
#include <glad/glad.h>
#include <imgui.h>

#include "Logger.hpp"
#include "renderComp.hpp"
MOD_BGN(PostProcessModule)
void PostProcessInit(flecs::iter& it, size_t i, PostProcess& process) {
  // full screen quad
  float quadVertices[] = {
      // vertex attributes for a quad that fills the entire screen in Normalized
      // Device Coordinates. positions   // texCoords
      -1.0f, 1.0f, 0.0f, 1.0f,  -1.0f, -1.0f,
      0.0f,  0.0f, 1.0f, -1.0f, 1.0f,  0.0f,

      -1.0f, 1.0f, 0.0f, 1.0f,  1.0f,  -1.0f,
      1.0f,  0.0f, 1.0f, 1.0f,  1.0f,  1.0f};

  unsigned int vbo = 0;
  glCreateVertexArrays(1, &process.vao);
  glCreateBuffers(1, &vbo);
  glBindVertexArray(process.vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices,
               GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                        (void*)(2 * sizeof(float)));

  // create an frame buffer for post process
  {
    auto e = it.entity(i);
    if (e.has<FrameBuffer>()) {
      Logger::get<PostProcessModule>()->warn("entity already has framebuffer");
    } else {
      e.add<FrameBuffer>();
    }
  }
}

void PostProcessRelease(flecs::iter& it, size_t i, PostProcess& process) {
  glDeleteVertexArrays(1, &process.vao);
  for (auto handle : process.backupHandles) {
    glDeleteProgram(handle);
  }
  it.entity(i).remove<FrameBuffer>();
}

void PostProcessOnRender(PostProcess& process, FrameBuffer& buffer) {
  glBindFramebuffer(GL_FRAMEBUFFER,
                    0);  // let 's the post process render to screen
  glDisable(GL_DEPTH_TEST);
  glUseProgram(process.handle);  // use screen shader
  glBindVertexArray(process.vao);
  glBindTexture(GL_TEXTURE_2D, buffer.colorHandle);
  glDrawArrays(4, 0, 6);

  glEnable(GL_DEPTH_TEST);
}

void DebugUI(PostProcess& process, FrameBuffer& buffer) {
  ImGui::Begin("PostProcess");
  const int size = process.backupHandles.size();
  static int selected = -1;
  for (int i = 0; i < size; ++i) {
    char buf[32];
    sprintf(buf, "Shader %d", i);
    if (ImGui::Selectable(buf, selected == i)) {
      selected = i;
      process.handle = process.backupHandles[i];
    }
  }
  ImGui::End();

  ImGui::Begin("FrameBuffer");
  ImGui::Text("Color buffer \nColor Handle: %d", buffer.colorHandle);
  ImGui::Image((void*)(intptr_t)buffer.colorHandle, ImVec2(500, 500),
               ImVec2(1, 1), ImVec2(0, 0));
  ImGui::End();

  ImGui::ShowDemoWindow();
}
// Post process should be after each renderer module running
PostProcessModule::PostProcessModule(world& ecs) {
  ecs.module<PostProcessModule>();

  {
    ecs.observer<PostProcess>()
        //        .event(flecs::OnAdd)
        .event(flecs::OnSet)
        .each(PostProcessInit);
    ecs.observer<PostProcess>()
        .event(flecs::OnRemove)
        .event(flecs::UnSet)
        .each(PostProcessRelease);
  }

  ecs.system<PostProcess, FrameBuffer>("PostProcessOnRender")
      .kind(flecs::PreStore)
      .each(PostProcessOnRender);

  //  ecs.system<PostProcess, FrameBuffer>("BaseRenderSystem")
  //      .kind(flecs::PreStore)
  //      .each([](PostProcess& post, FrameBuffer& data) {
  //        glBindFramebuffer(GL_FRAMEBUFFER, data.handle);
  //        // make sure we clear the framebuffer's content
  //        //        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  //        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  //      });

  ecs.system<PostProcess, FrameBuffer>("PostProcessUI")
      .kind(flecs::PreUpdate)
      .each(DebugUI);
}

MOD_END(PostProcessModule)