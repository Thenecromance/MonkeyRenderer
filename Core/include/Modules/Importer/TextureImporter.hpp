#pragma once 

#include "Job.hpp"

class TextureImporter{
public:
TextureImporter(world & ecs );

void LoadObserver(world & ecs );
void RegisterComponent(world & ecs );
};