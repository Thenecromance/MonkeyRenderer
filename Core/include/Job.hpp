#pragma once

namespace flecs {
struct world;
struct entity;
};  // namespace flecs
using world = flecs::world;
using entity = flecs::entity;


/*
namespace MonkeyPhase{
static const flecs::entity_t PreFrame           = flecs::PreFrame;      // just simply useing alias for more easy ways to use
static const flecs::entity_t PreCameraUpdated   = flecs::OnLoad;      // here will handling the InputHandler and Camera Operations
static const flecs::entity_t CameraUpdate       = flecs ::PostLoad;  // In this phase should only be has CameraUpdate
static const flecs::entity_t LightBinding       = flecs::PreUpdate;  // Binding lights and update lights
static const flecs::entity_t LogicUpdate        = flecs::OnUpdate;   // calculate about mesh moving etc and logic running
static const flecs::entity_t MeshesMerge        = flecs::OnValidate; // as i wish , this phase will merge each meshes in different render pass
static const flecs::entity_t NotInUsed0         = flecs::PostUpdate; // empty for the future use
static const flecs::entity_t NotInUsed1         = flecs::PostUpdate; // empty for the future use
static const flecs::entity_t RenderStage        = flecs::PreStore;   // forward render , deffered render and etc
static const flecs::entity_t PostProcess        = flecs::OnStore;    // render and post process
static const flecs::entity_t BlitRender         = flecs::PostFrame;  // merge each frame buffer and effects to the screen buffers
};*/
