#pragma once
template<typename T>
class Singleton {
public:
    static T& Instance();

    Singleton(const Singleton&) = delete;
    Singleton& operator= (const Singleton) = delete;

protected:
    struct token {};
    Singleton() {}
};

template<typename T>
T& Singleton<T>::Instance()
{
    static T instance;
    return instance;
}


#define SINGLETON_CTOR(cls) \
friend class Singleton<cls>;\
private:\
    cls(){};


#define SINGLETON_DECL_BEGIN(cls) \
class cls : public Singleton<cls>\
{\
friend class Singleton<cls>; \
private:\
    cls() {};


#define SINGLETON_DECL_END(cls) }