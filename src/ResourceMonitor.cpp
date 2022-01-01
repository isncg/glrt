#include "../include/ResourceMonitor.h"
#include "../utils/utils.h"
#include <Windows.h>

class ResourceListener
{
public:

	bool needUpdate = false;
	std::string path;
	FILETIME lastWriteTime{ 0 };
	IResourceUpdateNotify* notify = NULL;
};


int ResourceMonitor::Watch(std::string path, IResourceUpdateNotify* notify)
{
	for (int i = 0; i < listeners.size(); i++)
	{
		if (listeners[i] != nullptr && listeners[i]->notify == notify && listeners[i]->path == path)
		{
			return i;
		}
	}
	ResourceListener* listener = new ResourceListener;

	HANDLE hfile = CreateFileA(path.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	GetFileTime(hfile, NULL, NULL, &listener->lastWriteTime);
	CloseHandle(hfile);
	listener->path = path;
	listener->notify = notify;
	if (emptyIndices.size() <= 0)
	{
		int id = (int)listeners.size();
		listeners.push_back(listener);
		return id;
	}
	int id = emptyIndices.top();
	emptyIndices.pop();
	listeners[id] = listener;
	return id;
}

void ResourceMonitor::StopWatch(IResourceUpdateNotify* notify)
{
	for (int i = 0; i < listeners.size(); i++)
	{
		if (listeners[i] != nullptr && listeners[i]->notify == notify)
		{
			delete listeners[i];
			listeners[i] = nullptr;
			emptyIndices.push(i);
		}
	}
}

void ResourceMonitor::NotifyAll()
{
	if (!running)
		return;
	for (auto& l : listeners)
	{
		if (nullptr != l && l->needUpdate)
		{
			l->needUpdate = false;
			l->notify->OnResourceUpdated();
		}
	}
}

void CheckResources()
{
}

DWORD WINAPI MonitorThread(LPVOID lpParameter)
{
	ResourceMonitor* pMonitor = (ResourceMonitor*)lpParameter;
	FILETIME lastWriteTime;
	while (nullptr != pMonitor && pMonitor->running)
	{
		for (int i = 0; i < pMonitor->listeners.size(); i++)
		{
			if (nullptr == pMonitor || nullptr == pMonitor->listeners[i])
				continue;
			HANDLE hfile = CreateFileA(pMonitor->listeners[i]->path.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			GetFileTime(hfile, NULL, NULL, &lastWriteTime);
			CloseHandle(hfile);
			if (nullptr == pMonitor || nullptr == pMonitor->listeners[i])
				continue;
			if (pMonitor->listeners[i]->lastWriteTime.dwHighDateTime != lastWriteTime.dwHighDateTime ||
				pMonitor->listeners[i]->lastWriteTime.dwLowDateTime != lastWriteTime.dwLowDateTime)
			{
				pMonitor->listeners[i]->lastWriteTime = lastWriteTime;
				pMonitor->listeners[i]->needUpdate = true;
			}
		}
	}
	return 0;
}

void ResourceMonitor::Start()
{
	running = true;
	thread = CreateThread(NULL, 0, MonitorThread, this, 0, NULL);
}


void ResourceMonitor::Stop()
{
	running = false;
	WaitForSingleObject(thread, 1000);
}