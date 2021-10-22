#pragma once
#include "framework.h"
#include "Application.h"
template<class T>
class Singleton
{
public:
	static T& Instance()
	{
		static T instance;
		return instance;
	}
};