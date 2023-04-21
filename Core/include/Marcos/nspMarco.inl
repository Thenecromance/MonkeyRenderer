#pragma once

#define NSP_BGN(name) namespace name {
#define NSP_END(name) }

#define ROOT_BGN() NSP_BGN(Monkey)
#define ROOT_END() NSP_END(Monkey)

// Monkey::Module::customModule
#define MOD_BGN(name) ROOT_BGN() NSP_BGN(Module)
#define MOD_END(name) ROOT_END() NSP_END(Module)

#define COMP_BGN(v) ROOT_BGN() NSP_BGN(Component)
#define COMP_END(v) ROOT_END() NSP_END(Component)

#define USING_MOD using namespace Monkey::Module