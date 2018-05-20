/*
    Utils header
*/

#pragma once

#include <type_traits>

/*
	Function reference
*/
template<typename Signature>
class FunctionRef;

template<typename Return_t, typename ... Args_t>
class FunctionRef<Return_t(Args_t ...)>
{
private:
    
    void* m_ptr;
	Return_t(*m_func)(void*,Args_t...);

public:
    
	template<typename Function_t>
	FunctionRef(Function_t&& f)
	{
		using FPtr = typename std::add_pointer<Function_t>::type;

		m_ptr = &f;
		m_func = [](void* ptr, Args_t...args)->Return_t{
			return (*reinterpret_cast<FPtr>(ptr))(std::forward<Args_t>(args)...);
		};
	}

	Return_t operator()(Args_t ... args) const
	{
		return m_func(m_ptr, std::forward<Args_t>(args)...);
	}
};
