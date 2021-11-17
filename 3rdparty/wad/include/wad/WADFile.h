#pragma once

#ifndef _WADFILE_H_
#define _WADFILE_H_

#include <stdio.h>
#include <string>
#include <vector>
typedef struct
{
	uint8_t r;
	uint8_t g;
	uint8_t b;
} stWadRGB;

typedef struct
{
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;
} stWadBGRA;


typedef struct
{
	char		identification[4];		// should be WAD2/WAD3
	int			numlumps;
	int			infotableofs;
} stWadHeader;


typedef struct
{
	char			name[16];
	unsigned int	width, height;
	unsigned int	mipOffsets[4];			// variably sized
}stWadTexture_C; //0x67;

class WadTexture_C
{
public:
	stWadTexture_C* meta;
	stWadBGRA* mipmaps[4];
	stWadRGB* palette;
	void Init(uint8_t* buffer);
	void Release();
};


struct stWadLumpItemInfo
{
	int			filepos;
	int			disksize;
	int			size;					// uncompressed
	uint8_t		type;
	uint8_t		compression;
	uint8_t		pad1, pad2;
	char		name[16];				// must be null terminated
};

class WadLampItem
{
public:
	uint8_t* buffer;
	stWadLumpItemInfo meta;
	union
	{
		WadTexture_C textureC;
	};
};


class WadFile
{
	uint8_t* buffer;
public:
	stWadHeader header;
	std::vector<WadLampItem> items;

	void Load(std::string filename);
};
#endif

