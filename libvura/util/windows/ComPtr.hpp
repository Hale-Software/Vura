/*******************************************************************************
     Copyright (c) 2026.  by Andrew Hale <halea2196@gmail.com>

     This program is free software: you can redistribute it and/or modify
     it under the terms of the GNU General Public License as published by
     the Free Software Foundation, either version 3 of the License, or
     (at your option) any later version.

     This program is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     GNU General Public License for more details.

     You should have received a copy of the GNU General Public License
     along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/

#pragma once

#ifdef _WIN32
#include <Unknwn.h>
#endif

/* Oh no I have my own com pointer class, the world is ending, how dare you
 * write your own! */

template<class T> class ComPtr {

protected:
	T *ptr;

	inline void Kill()
	{
		if (ptr)
			ptr->Release();
	}

	inline void Replace(T *p)
	{
		if (ptr != p) {
			if (p)
				p->AddRef();
			if (ptr)
				ptr->Release();
			ptr = p;
		}
	}

public:
	inline ComPtr() : ptr(nullptr) {}
	inline ComPtr(T *p) : ptr(p)
	{
		if (ptr)
			ptr->AddRef();
	}
	inline ComPtr(const ComPtr<T> &c) : ptr(c.ptr)
	{
		if (ptr)
			ptr->AddRef();
	}
	inline ComPtr(ComPtr<T> &&c) noexcept : ptr(c.ptr) { c.ptr = nullptr; }
	template<class U> inline ComPtr(ComPtr<U> &&c) noexcept : ptr(c.Detach()) {}
	inline ~ComPtr() { Kill(); }

	inline void Clear()
	{
		if (ptr) {
			ptr->Release();
			ptr = nullptr;
		}
	}

	inline ComPtr<T> &operator=(T *p)
	{
		Replace(p);
		return *this;
	}

	inline ComPtr<T> &operator=(const ComPtr<T> &c)
	{
		Replace(c.ptr);
		return *this;
	}

	inline ComPtr<T> &operator=(ComPtr<T> &&c) noexcept
	{
		if (&ptr != &c.ptr) {
			Kill();
			ptr = c.ptr;
			c.ptr = nullptr;
		}

		return *this;
	}

	template<class U> inline ComPtr<T> &operator=(ComPtr<U> &&c) noexcept
	{
		Kill();
		ptr = c.Detach();

		return *this;
	}

	inline T *Detach()
	{
		T *out = ptr;
		ptr = nullptr;
		return out;
	}

	inline void CopyTo(T **out)
	{
		if (out) {
			if (ptr)
				ptr->AddRef();
			*out = ptr;
		}
	}

	inline ULONG Release()
	{
		ULONG ref;

		if (!ptr)
			return 0;
		ref = ptr->Release();
		ptr = nullptr;
		return ref;
	}

	inline T **Assign()
	{
		Clear();
		return &ptr;
	}
	inline void Set(T *p)
	{
		Kill();
		ptr = p;
	}

	inline T *Get() const { return ptr; }

	inline T **operator&() { return Assign(); }

	inline operator T *() const { return ptr; }
	inline T *operator->() const { return ptr; }

	inline bool operator==(T *p) const { return ptr == p; }
	inline bool operator!=(T *p) const { return ptr != p; }

	inline bool operator!() const { return !ptr; }
};

#ifdef _WIN32

template<class T> class ComQIPtr : public ComPtr<T> {

public:
	inline ComQIPtr(IUnknown *unk)
	{
		this->ptr = nullptr;
		unk->QueryInterface(__uuidof(T), (void **)&this->ptr);
	}

	template<class U> inline ComQIPtr(const ComPtr<U> &c)
	{
		this->ptr = nullptr;
		c->QueryInterface(__uuidof(T), (void **)&this->ptr);
	}

	inline ComPtr<T> &operator=(IUnknown *unk)
	{
		ComPtr<T>::Clear();
		unk->QueryInterface(__uuidof(T), (void **)&this->ptr);
		return *this;
	}
};

#endif
