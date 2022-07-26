#ifndef __SINGLETON_H__
#define __SINGLETON_H__

namespace worm::detail {
template <typename ChildType>
class Singleton {
public:
    virtual ~Singleton() = default;

public:
    static ChildType& Instance()
    {
        return s_instance;
    }

private:
    Singleton(const Singleton& other) = delete;

    Singleton(Singleton&& other) = delete;

    Singleton& operator=(const Singleton& other) = delete;

    Singleton& operator=(Singleton&& other) = delete;

private:
    Singleton() = default;

private:
    friend ChildType;

private:
    static ChildType s_instance;
};

template <typename ChildType>
ChildType Singleton<ChildType>::s_instance;
} // namespace worm::detail

#endif
