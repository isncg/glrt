#pragma once
#include<vector>
#include<string>
#include"../utils/singleton.h"
class ResourceListener
{
public:
	int id;
	unsigned int flag = 0;
	void* handle;
	void* thread;
	std::string fullpath;
};

class ResourceMonitor: public Singleton<ResourceMonitor>
{
	SINGLETON_CTOR(ResourceMonitor)
	std::vector<ResourceListener> listeners;
public:
	int Create(std::string fullpath);
	ResourceListener* Get(int id);
};