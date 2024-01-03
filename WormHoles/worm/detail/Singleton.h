#ifndef __WH_SINGLETON_H__
#define __WH_SINGLETON_H__

namespace worm::detail {
template <typename ChildType>
class Singleton {
public:
    virtual ~Singleton() = default;

public:
    static ChildType& Instance()
    {
        static ChildType instance{};
        return instance;
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
};
} // namespace worm::detail

#endif
