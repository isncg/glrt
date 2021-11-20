#pragma once
// Loading Halflife 1 BPS & WAD to Model & Texture
#include "../../framework.h"
#include <wad/WADFile.h>
#include <bsp30/BSPLoader.h>
#include "../../Renderer.h"

void HLMapLoad(std::string bsp, std::string wad, Model& outputModel, std::map<std::string, Texture>* outputTextures);