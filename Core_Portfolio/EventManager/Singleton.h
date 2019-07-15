//Matthew Rosen
#pragma once

template<typename T>
class Singleton
{
public:
    static T & Instance()
    {
        return s_instance;
    }

private:

    static T s_instance;
};

template <typename T>
T Singleton<T>::s_instance;