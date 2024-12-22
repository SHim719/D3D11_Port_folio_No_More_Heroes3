#ifndef SHADER_DEFINE_SSR
#define SHADER_DEFINE_SSR

#include "Shader_Defines.hlsli"

#define SSR_MAX_STEPS 16
#define SSR_BINARY_SEARCH_STEPS 16

float g_fSSRStep = 1.f;
float g_fSSRThreshold = 0.1f;

#endif


