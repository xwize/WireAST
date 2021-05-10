#ifndef __DICT_H__
#define __DICT_H__

#include "Common.h"

template<class K, class T> class Dict {
protected:
	typedef int(*hashFunc_t)(const K& v);
	typedef vector<T> valueBin_t;
	typedef vector<K> keyBin_t;
	hashFunc_t	hashFunc;
	valueBin_t* values;
	keyBin_t*	keys;
	int			bins;
protected:
	int _Bin(int hash) const {
		assert(hash >= 0);
		return hash % bins;
	}
	const T* _Search(int bin, const K& key) const {
		size_t binSize = keys[bin].size();
		keyBin_t& keyBin = keys[bin];
		valueBin_t& valueBin = values[bin];
		for (size_t i = 0; i < binSize; ++i) {
			if (keyBin[i] == key)
				return &valueBin[i];
		}
		return nullptr;
	}
protected:
	static int _Hash(const string& str) {
		int mul = 1;
		for (size_t i = 0; i < str.size(); ++i)
			mul *= (1+str[i]);
		return mul & 0xFFFF;
	}
public:
	class ForwardIterator {
	private:
		Dict*	dict;
		int		bin;
		size_t	index;
	public:
		ForwardIterator(Dict* d) : dict(d), index(0), bin(0) {
		}
		const K& Key() const;
		const T& Value() const;
		void Next() {
			index++;
			if(index > dict->values[bin].size())
		}
	};
public:

	Dict(hashFunc_t hash, int binCount) {
		bins = binCount;
		values = new valueBin_t[bins];
		keys = new keyBin_t[bins];
		hashFunc = hash;
	}

	Dict() : Dict(Dict::_Hash, 32) {
	}

	~Dict() {
		delete[] values;
		delete[] keys;
	}

	void Clear() {
		for (int i = 0; i < bins; ++i) {
			values[i].clear();
			keys[i].clear();
		}
	}

	void Put(const K& key, const T& value) {
		int hash = hashFunc(key);
		int bin = _Bin(hash);

		T* v = const_cast<T*>(_Search(bin, key));
		if (v == nullptr) {
			keys[bin].push_back(key);
			values[bin].push_back(value);
		} else {
			(*v) = value;
		}
	}

	T* Get(const K& key) {
		int hash = hashFunc(key);
		int bin = _Bin(hash);
		return const_cast<T*>(_Search(bin, key));
	}

	const T* Get(const K& key) const {
		int hash = hashFunc(key);
		int bin = _Bin(hash);
		return _Search(bin, key);
	}

	size_t Size() const {
		size_t r = 0;
		for (size_t i = 0; i < bins; ++i)
			r += bins[i].size();
		return r;
	}
};

#endif // __DICT_H__
