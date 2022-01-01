#pragma once
//#include <filesystem>
#include <chrono>
#include<vector>
#include<stack>
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
	void* thread;
public:
	std::vector<ResourceListener*> listeners;
	std::stack<int> emptyIndices;
	bool running = false;
	int Watch(std::string path, IResourceUpdateNotify* notify);
	void StopWatch(IResourceUpdateNotify* notify);
	void Start();
	void Stop();
	void NotifyAll();
};