# Renderer

## Frame flow

So far the render flow for 1 object, still in a very noobie ways, seems just like this

```mermaid
graph LR
FB(FrameBegin)
FE(FrameEnd)
RTS(Render To Screen)
RM1(Render pass 1)
RM2(Render pass 2)
RMx(Render pass ...)
IGD(ImGui Draw)
FB -->RM1 --> RM2--> RMx--> IGD--> FE -->RTS
```

this is the most easy way and the fastest way to code, but waste lots of draw call

so the plan to change this part frame flow, now it should be like

```mermaid
graph TB

RTS(Render to screen)

FRS(Forward render system)
DRS(Deffered render system)

BRI(Base render info) 
PBR(PBR Matrials update)
LPU(light params update)
MC(Mesh collect and submit)
GPP(Gloabal Post Process)
PBR--> LPU
LPU--> MC
MC-->RTS


BRI --> FRS
FRS-->|if need deffered| DRS
FRS-->PBR


DRS--> PBR

MC-->|if has post process| GPP

GPP-->RTS
```

* BaseRenderInfo
  * Camera info update
* Forward render system update
* PBR system update
* Light system update
* Mesh collect and merge
  * collect all meshes and transfer to command list
  * merge all command to 1 upload
* post process
* blit to screen frame 


