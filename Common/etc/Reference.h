//------------------------------------------------------------------------
// Name:    Reference.h
// Author:  jjuiddong
// Date:    12/22/2012
// 
// 인자로 넘어온 타입의 포인터를 멤버변수로 가진다.
// 
// 스마트 포인터가 아니다.
//
// 다른 곳에서 할당된 메모리 포인터를 참조할 때 사용하는 클래스다.
// 실수로 메모리를 제거하는 것을 막기위해 만들어졌다.
//
// delete 연산자로 ReferencePtr을 제거할 수 있는 문제가 있다. 
// T* operator를 수정해야함
//------------------------------------------------------------------------
#pragma once

namespace common
{
	template <class T>
	class ReferencePtr
	{
	public:
		ReferencePtr():m_p(NULL) {}
		ReferencePtr(T *p) : m_p(p) {}
		ReferencePtr( const ReferencePtr<T> &rhs );
		template<class T1> ReferencePtr( const ReferencePtr<T1> &rhs );

		T* operator->() const { return m_p; }
		bool operator!() const { return (m_p)? false : true; }
		T* operator=(T *p) { m_p = p; return m_p; }
		bool operator==(T *p) const { return m_p == p; }
		bool operator!=(T *p) const { return m_p != p; }
		bool operator==(const ReferencePtr<T> &rhs) const { return m_p == rhs.m_p; }
		bool operator!=(const ReferencePtr<T> &rhs) const { return m_p != rhs.m_p; }
		operator T*() { return m_p; } // casting operator
		T* Get() const { return m_p; }

		//typedef T* this_type::*unspecified_bool_type;
		// 		operator unspecified_bool_type() const 
		// 		{ 
		// 			return (m_p == NULL)?  0 : &this_type::m_p;
		// 		} // casting operator

	private:
		//typedef ReferencePtr<T> this_type;
		T *m_p;
	};

	template<class T> 
	inline ReferencePtr<T>::ReferencePtr( const ReferencePtr<T> &rhs ) {
		m_p = rhs.m_p;
	}
	
	template<class T> template<class T1> 
	inline ReferencePtr<T>::ReferencePtr( const ReferencePtr<T1> &rhs ) {
		m_p = dynamic_cast<T*>(rhs.Get());
	}

}
