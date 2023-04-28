#pragma  once


#include <flecs.h>


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
static const flecs::entity_t NotInUsed                = flecs::OnStore;    // render and post process
static const flecs::entity_t ImGuiRender              = flecs::PostFrame;  // merge each frame buffer and effects to the screen buffers
}