#include "../include/ResourceMonitor.h"
#include "../utils/utils.h"
#include <Windows.h>
struct ResourceListenerThreadParam
{
	int id;
};

DWORD WINAPI ResourceListenerThread(_In_ LPVOID lpParameter)
{
	ResourceListenerThreadParam* ptr = (ResourceListenerThreadParam*)lpParameter;
	while (true)
	{
		WaitForSingleObject(ResourceMonitor::Instance().Get(ptr->id)->handle, 1000);
		if (FindNextChangeNotification(ResourceMonitor::Instance().Get(ptr->id)->handle))
		{
			log(string_format("File change detected: %s", ResourceMonitor::Instance().Get(ptr->id)->fullpath.c_str()).c_str());
			ResourceMonitor::Instance().Get(ptr->id)->flag |= 0x01;
		}
	}
	delete ptr;
}


int ResourceMonitor::Create(std::string path)
{
	char buffer[1024];
	GetFullPathNameA(path.c_str(), path.size(), buffer, NULL);
	ResourceListener listener;
	listener.fullpath = buffer;
	listener.flag = 0;
	listener.id = listeners.size();
	listener.handle = FindFirstChangeNotificationA(listener.fullpath.c_str(), FALSE, FILE_NOTIFY_CHANGE_LAST_WRITE);
	listeners.push_back(listener);
	listeners.back().thread = CreateThread(NULL, 0, ResourceListenerThread, new ResourceListenerThreadParam{ listener.id }, 0, 0);
	log(string_format("Listen file: %s", buffer).c_str());
	return listener.id;
}

ResourceListener* ResourceMonitor::Get(int id)
{
	return &listeners[id];
}
