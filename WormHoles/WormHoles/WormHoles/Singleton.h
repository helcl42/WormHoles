#ifndef __SINGLETON_H__
#define __SINGLETON_H__

namespace WormHoles
{
	template <typename ChildType>
	class Singleton
	{
	private:
		friend ChildType;

	private:
		static ChildType s_instance;

	private:
		Singleton(const Singleton& other) = delete;

		Singleton(Singleton&& other) = delete;

		Singleton& operator=(const Singleton& other) = delete;

		Singleton& operator=(Singleton&& other) = delete;

	private:
		Singleton() = default;

	public:
		virtual ~Singleton() = default;

	public:
		static ChildType& GetInstance()
		{
			return s_instance;
		}
	};

	template <typename ChildType>
	ChildType Singleton<ChildType>::s_instance;
}

#endif
