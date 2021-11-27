#pragma once
#include "../include/Renderer.h"

bool LoadModel(Model* output, const char* file);

//bool LoadBSPMap(Model* output, std::map<std::string, Texture>* internalTextures, const char* file);

bool LoadTexture(Texture* output, const char* file, Sampler* sampler = NULL);