#pragma  once


#include <flecs.h>

//if using Experimental's modules just need to becareful to use PreFrame and ImGuiRender just because the GLADModule and GLFWModule will be loaded in PreFrame and running in the ImGuiRender
namespace Monkey::Phase{
static const flecs::entity_t PreFrame                 = flecs::PreFrame;    // this is a very heavy phase, it will handle the input.
static const flecs::entity_t PreCameraUpdated         = flecs::OnLoad;      // here will handling the InputHandler and Camera Operations
static const flecs::entity_t CameraUpdate             = flecs ::PostLoad;  // In this phase should only be has CameraUpdate
static const flecs::entity_t LightBinding             = flecs::PreUpdate;  // Binding lights and update lights
static const flecs::entity_t LogicUpdate              = flecs::OnUpdate;   // calculate about mesh moving etc and logic running
static const flecs::entity_t MeshesMerge              = flecs::OnValidate; // as i wish , this phase will merge each meshes in different render pass
static const flecs::entity_t RenderStage              = flecs::PostUpdate; // empty for the future use
static const flecs::entity_t PostProcess              = flecs::PostUpdate; // empty for the future use
static const flecs::entity_t BlitRender               = flecs::PreStore;   // forward render , deffered render and etc
static const flecs::entity_t ImGuiRender              = flecs::OnStore;    // render and post process
static const flecs::entity_t PostFrame                = flecs::PostFrame;  // merge each frame buffer and effects to the screen buffers
}