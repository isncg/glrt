#pragma once

class IScriptable;
class IScriptContext
{
public:
	std::string instance_name;
	std::string class_name;
	IScriptable* pData;
	virtual void BindScript(const char* class_name) = 0;
	virtual void Invoke(const char* method) = 0;

	static IScriptContext* Alloc(IScriptable* pData);
	template<typename T>
	T* data();
};

class IScriptable
{
public:
	IScriptContext* pScriptContext = NULL;
	void SetScript(const char* className);
	void InvokeScript(const char* method);
	template<typename T>
	T* as();
};

template<typename T>
inline T* IScriptable::as()
{
	return (T*)this;
}

template<typename T>
inline T* IScriptContext::data()
{
	return pData->as<T>();
}
