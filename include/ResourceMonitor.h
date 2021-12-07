#pragma once
#include <filesystem>
#include <chrono>
#include<vector>
#include<string>
#include"../utils/singleton.h"

class IResourceUpdateNotify
{
public:
	virtual void OnResourceUpdated() = 0;
};


class ResourceListener;
class ResourceMonitor: public Singleton<ResourceMonitor>
{
	SINGLETON_CTOR(ResourceMonitor)
	std::vector<ResourceListener*> listeners;
public:
	int Create(std::string path, IResourceUpdateNotify* notify);
	void NotifyAll();
};