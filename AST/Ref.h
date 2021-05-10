#ifndef __REF_H__
#define __REF_H__

class RefObject {
private:
	friend void RefIncrement(RefObject*);
	friend bool RefDecrement(RefObject*);
	int __count = 0;
};

void RefIncrement(RefObject* n);
bool RefDecrement(RefObject* n);

template <typename T> class Ref {
private:
	T* ptr;
	void _release() {
		if (ptr != nullptr) {
			if (RefDecrement(ptr)) {
				delete ptr;
				ptr = nullptr;
			}
		}
	}
	void _addRef() {
		if (ptr != nullptr)
			RefIncrement(ptr);
	}
	void _assign(T* other) {
		_release();
		ptr = other;
		_addRef();
	}
public:
	Ref() : ptr(nullptr) {
	}

	Ref(T* ptrIn) : ptr(nullptr) {
		_assign(ptrIn);
	}

	Ref(const Ref<T>& other) : ptr(nullptr) {
		_assign(other.ptr);
	}

	Ref(Ref<T>&& other) : ptr(nullptr) {
		ptr = other.ptr;
		other.ptr = nullptr;
	}

	template <typename U> Ref(const Ref<U>& other) : ptr(nullptr) {
		_assign(other.get());
	}

	~Ref() {
		_release();
	}

	const Ref<T>& operator = (const Ref<T>& other) {
		_assign(other.ptr);
		return (*this);
	}

	bool operator ! () const {
		return ptr == nullptr;
	}

	bool operator && (bool other) const {
		return ((ptr != nullptr) && other);
	}

	bool operator == (T* other) const {
		return ptr == other;
	}

	bool operator != (T* other) const {
		return ptr != other;
	}

	T* operator->() {
		return ptr;
	}

	const T* operator->() const {
		return ptr;
	}

	T* get() const {
		return ptr;
	}
};

#endif // __REF_H__
