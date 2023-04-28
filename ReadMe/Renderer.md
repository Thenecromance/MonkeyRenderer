# Frame Flow

## Default Flecs Pipeline

build with flecs default flecs's pipeline , also , the pipeline running sequence is based on it's owned toplogical sort

```cpp
/* Builtin pipeline tags */
static const flecs::entity_t OnStart = EcsOnStart;
static const flecs::entity_t PreFrame = EcsPreFrame;
static const flecs::entity_t OnLoad = EcsOnLoad;
static const flecs::entity_t PostLoad = EcsPostLoad;
static const flecs::entity_t PreUpdate = EcsPreUpdate;
static const flecs::entity_t OnUpdate = EcsOnUpdate;
static const flecs::entity_t OnValidate = EcsOnValidate;
static const flecs::entity_t PostUpdate = EcsPostUpdate;
static const flecs::entity_t PreStore = EcsPreStore;
static const flecs::entity_t OnStore = EcsOnStore;
static const flecs::entity_t PostFrame = EcsPostFrame;
```

in monkey renderer, it use a very simple way to transformed like this

```cpp
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
```

in current system design, the real PreFrame operation and PostFrame already implenment in Core::OnUpdate() so in flecs default pipeline, I got another free PreFrame and PostFrame.
