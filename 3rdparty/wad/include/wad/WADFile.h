#pragma once

#ifndef _WADFILE_H_
#define _WADFILE_H_

#include <stdio.h>
#include <string>
#include <vector>
typedef struct
{
	unsigned char r;
	unsigned char g;
	unsigned char b;
} stWadRGB;

typedef struct
{
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;
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
	void Init(char* buffer);
	void Release();
};


struct stWadLumpItemInfo
{
	int			filepos;
	int			disksize;
	int			size;					// uncompressed
	char		type;
	char		compression;
	char		pad1, pad2;
	char		name[16];				// must be null terminated
};

class WadLampItem
{
public:
	char* buffer;
	stWadLumpItemInfo meta;
	union
	{
		WadTexture_C textureC;
	};
};


class WadFile
{
	char* buffer;
public:
	stWadHeader header;
	std::vector<WadLampItem> items;

	void Load(std::string filename);
};
#endif

