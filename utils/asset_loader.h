#pragma once
#include "../include/Renderer.h"
#include <string>
bool LoadModel(Model* output, std::string file);

//bool LoadBSPMap(Model* output, std::map<std::string, Texture>* internalTextures, const char* file);

bool LoadTexture(Texture* output, std::string file, Sampler* sampler = NULL);