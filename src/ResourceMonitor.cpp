#include "../include/ResourceMonitor.h"
#include "../utils/utils.h"
#include <Windows.h>

class ResourceListener
{
public:
	std::string path;
	FILETIME lastWriteTime{ 0 };
	IResourceUpdateNotify* notify = NULL;
};


int ResourceMonitor::Create(std::string path, IResourceUpdateNotify* notify)
{
	ResourceListener* listener = new ResourceListener;

	HANDLE hfile = CreateFileA(path.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	GetFileTime(hfile, NULL, NULL, &listener->lastWriteTime);
	CloseHandle(hfile);
	listener->path = path;
	listener->notify = notify;
	int id = listeners.size();
	listeners.push_back(listener);
	return id;
}

void ResourceMonitor::NotifyAll()
{
	FILETIME lastWriteTime;
	for (auto& l : listeners)
	{
		HANDLE hfile = CreateFileA(l->path.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		GetFileTime(hfile, NULL, NULL, &lastWriteTime);
		CloseHandle(hfile);
		if (l->lastWriteTime.dwHighDateTime != lastWriteTime.dwHighDateTime ||
			l->lastWriteTime.dwLowDateTime != lastWriteTime.dwLowDateTime)
		{
			l->lastWriteTime = lastWriteTime;
			l->notify->OnResourceUpdated();
		}
	}
}