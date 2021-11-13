#include <wad/WADFile.h>
#include <stdio.h>
#include <string.h>
#include <corecrt_malloc.h>

void WadFile::Load(std::string filename)
{
	FILE* f = 0;

	errno_t err = fopen_s(&f, filename.c_str(), "rb");
	if (err)
		return;

	size_t sz = fread(&header, sizeof(stWadHeader), 1, f);
	if (sz != 1)
		return;

	if (strncmp(header.identification, "WAD2", 4) && strncmp(header.identification, "WAD3", 4))
		throw("Invalid WAD header");
	fseek(f, 0, SEEK_END);
	auto size = ftell(f);
	fseek(f, 0, SEEK_SET);
	buffer = new uint8_t[size];
	fread(buffer, 1, size, f);
	fclose(f);
	stWadLumpItemInfo* wadlumps = (stWadLumpItemInfo*)(buffer + header.infotableofs);
	for (int i = 0; i < header.numlumps; i++)
	{
		WadLampItem item{ buffer, wadlumps[i] };
		auto ptr = buffer + item.meta.filepos;
		if(item.meta.type == 'C')
			item.textureC.Init(ptr);
		items.push_back(item);
	}	
}

void WadTexture_C::Init(uint8_t* buffer)
{
	meta = (stWadTexture_C*)buffer;
	unsigned int mipsize0 = meta->width * meta->height;
	unsigned int mipsize1 = mipsize0 >> 2;
	unsigned int mipsize2 = mipsize1 >> 2;
	unsigned int mipsize3 = mipsize2 >> 2;
	mipmaps[0] = new stWadBGRA[mipsize0];
	mipmaps[1] = new stWadBGRA[mipsize1];
	mipmaps[2] = new stWadBGRA[mipsize2];
	mipmaps[3] = new stWadBGRA[mipsize3];
	int mipsize[4]{ mipsize0 ,mipsize1 ,mipsize2 ,mipsize3 };

	uint8_t* unknown = buffer + meta->mipOffsets[3] + mipsize3;
	if (unknown[0] != 0x00 && unknown[1] != 0x01)
		return;
	palette = (stWadRGB*)(unknown + 2);

	int miplevel = 0;
	while (miplevel<4)
	{
		uint8_t* origin = buffer + meta->mipOffsets[miplevel];
		for (int i = 0; i < mipsize[miplevel]; i++)
		{
			uint8_t index = origin[i];
			stWadRGB color = palette[index];
			if (color.b == 0xff)
				mipmaps[miplevel][i] = stWadBGRA{ 0,0,0,0 };
			else
				mipmaps[miplevel][i] = stWadBGRA{ color.r,color.g,color.b, 0xff };
		}
		miplevel++;
	}
}