/*
	Copyright (c) by Valery Goryachev (Wal)
*/


/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	19.08.2012 ����� ������������ ccollect::del
	��� �������� �������� ��� ���������� ����� �� ��������� �� �� ���� (stop)
	��� ������� (����� ������� �� ������ � Layout, ���� ��� ���� � Layout �������� ��������������� ���������� ����� ���������)
*/



#ifndef WAL_TMPLS_H
#define WAL_TMPLS_H

//Wal templates 2.1
//Created by Wal (Krasnoyarsk)

#include <stdlib.h>
#include <stdio.h>

#ifdef _MFC_VER
#include <afx.h>
#else
	#ifndef ASSERT
		#ifdef _DEBUG
			#include <assert.h>
			#define ASSERT(a) assert(a) 
		#else 
			#define ASSERT(a)
		#endif
	#endif
#endif

namespace wal {

	//��� cptr � carray 
	template <class T> void set_const_data( const T *ptr, T data)
	{
		*((T*)ptr) = data;
	}

	/*
	smart pointer ������
	*/

	template <class T> class carray {
	public:
		carray();
		carray(int n);
		carray(T *p);
		carray(const carray &a);
		void clear(); //���������� ������
		void drop(); //��������� ������
		carray& operator = (T *p);
		carray& operator = (const carray &a);
		void alloc(int n);
		T* ptr();
		const T* const_ptr() const;
		T& operator [] (int n);
		const T& const_item(int n) const;
		~carray();
	private:
		T *data;
	};

	/*
	smart pointer ������
	*/
	template <class T> class cptr {
	public:
		cptr();
		cptr(T *p);
		cptr(const cptr &a);
		void clear();
		void drop();
		cptr& operator = (T *p);
		cptr& operator = (const cptr &a);
		T* operator->();
		T* ptr();
		const T* const_ptr() const;
		~cptr();
	private:
		T *data;
	};

	template <class T, int step=16> class ccollect {
	public:
		ccollect();
		ccollect(int n);
		ccollect(const ccollect &a);
		void del(int n);
		void del(int n, int count);
		void set(int n, const T &a);
		T& get(int n);
		const T& const_item(int n) const;
		const T* const_ptr() const;
		T* ptr();
		carray<T> grab();
		T& operator [] (int n);
		void insert(int n);
		void insert(int n, const T&a);
		void insert(int n, int count);
		void append();
		void append (const T& a);
		void append_list (const T *list, int number);
		void append_n (const T &p, int number);
		void append (const ccollect &a);
		void clear();
		ccollect& operator = (const ccollect &a);
		int count() const;
		~ccollect();
	private:
		carray<T> data; 
		int cnt;
		void realloc(int n); 
		void copy_from(const ccollect &a);
	};

	

/// carray /////////////////////////////////////////////////////////////////////////////
	template <class T> inline carray<T>::carray()
	{
		data = NULL;
	}

	template <class T> inline carray<T>::carray(int n)
	{
		ASSERT(n>=0);
		data = n>0 ? new T[n] : (T*)NULL; 
	}

	template <class T> inline carray<T>::carray(T *p)
	{
		data = p;
	}

	template <class T> inline carray<T>::carray(const carray<T> &a)
	{
			data = a.data;
			set_const_data<T*>(&a.data,NULL);
	}

	template <class T> inline void carray<T>::clear()
	{
		if (data) {
			delete [] data;
			data = NULL;
		}
	}

	template <class T> inline void carray<T>::drop()
	{
		data = NULL;
	}

	template <class T> inline carray<T>& carray<T>::operator = (T *p)
	{
		if (data)
			delete [] data;
		data = p;
		return *this;
	}

	template <class T> inline carray<T>& carray<T>::operator = (const carray &a)
	{
		ASSERT(this != &a);
		if (data)
			delete [] data;

		data = a.data;
		set_const_data<T*>(&a.data,NULL);

		return *this;
	}

	template <class T> inline void carray<T>::alloc(int n)
	{
		ASSERT( n>=0 );
		T *p = n>0 ? new T[n] : (T*)NULL;
		if (data)
			delete [] data;
		data = p;
	}

	template <class T> inline const T& carray<T>::const_item(int n) const
	{
		ASSERT(data);
		return data[n];
	}


	template <class T> inline T& carray<T>::operator [] (int n)
	{
		ASSERT(data);
		return data[n];
	}


	template <class T> inline T* carray<T>::ptr()
	{
		return data;
	}

	template <class T> inline const T* carray<T>::const_ptr() const
	{
		return data;
	}



	template <class T> inline carray<T>::~carray()
	{
		if (data)
			delete [] data;
	}

/// cptr /////////////////////////////////////////////////////////////////////
	template <class T> inline cptr<T>::cptr()
	{
		data = NULL;
	}

	template <class T> inline cptr<T>::cptr(T *p)
	{
		data = p;
	}

	template <class T> inline cptr<T>::cptr(const cptr &a)
	{
		data = a.data;
		set_const_data<T*>(&a.data,NULL);
	}

	template <class T> inline void cptr<T>::clear()
	{
		if (data) {
			delete data;
			data = NULL;
		}
	}

	template <class T> inline void cptr<T>::drop()
	{
		data = NULL;
	}

	template <class T> inline cptr<T>& cptr<T>::operator = (T *p)
	{
		if (data)
			delete data;
		data = p;
		return *this;
	}

	template <class T> inline cptr<T>& cptr<T>::operator = (const cptr &a)
	{
		ASSERT(this != &a);
		if (data)
			delete data;

		data = a.data;
		set_const_data<T*>(&a.data,NULL);

		return *this;

	}
	template <class T> inline T* cptr<T>::operator->()
	{
		//ASSERT(data);
		return data;
	}

	template <class T> inline T* cptr<T>::ptr()
	{
		return data;
	}
	
	template <class T> inline const T* cptr<T>::const_ptr() const
	{
		return data;
	}


	template <class T> inline cptr<T>::~cptr()
	{
		if (data)
			delete data;
	}

/// ccollect //////////////////////////////////////////////////////////

	template <class T, int step> ccollect<T, step>::ccollect()
	{
		cnt = 0;
	}

	template <class T, int step> ccollect<T, step>::ccollect(int n)
	{
		ASSERT( n>=0 );
		int x = (n+step-1)/step;
		data.alloc(x*step);
		cnt = n;
	}

	template <class T, int step> inline ccollect<T, step>::ccollect(const ccollect &a)
	{
		copy_from(a);
	}

	template <class T, int step> void ccollect<T, step>::realloc(int n) 
	{
		ASSERT(n>=0);
		int a = (cnt+step-1)/step, b = (n+step-1)/step;
		if (a!=b) {
			carray<T> p(b*step);
			for (int i = cnt>n ? n-1 : cnt-1; i>=0; i--)
				p[i] = data[i]; 
			data = p;
		}
		cnt = n;
	}

//!!!�������� ��� �������� (���� ������� ��������� �������, �� ���������� �� �� ������ ���������, � ������ ��� �������� ���� (� ��������� ������ ����������))
	template <class T, int step> void ccollect<T, step>::del(int n)
	{
		ASSERT(n>=0 && n<cnt);
		int i;
		for ( i = n; i<cnt-1; i++) 
			data[i] = data[i+1];
		realloc(cnt-1);
	}

	template <class T, int step> void ccollect<T, step>::del(int n, int count)
	{
		ASSERT(n>=0);
		ASSERT(count>=0);
		ASSERT(n+count<=cnt);
		if (count <=0) 
                	return;
		int i, x = cnt-count;
		for ( i = n; i<x; i++) 
			data[i] = data[i + count];
		realloc(cnt - count);
	}

	template <class T, int step> inline void ccollect<T, step>::set(int n, const T &a)
	{
		ASSERT(n>=0 && n<cnt);
		data[n]=a;
	}

	template <class T, int step> inline T& ccollect<T, step>::get(int n)
	{
		ASSERT(n>=0 && n<cnt);
		return data[n];
	}

	template <class T, int step> inline T* ccollect<T, step>::ptr()
	{
		return data.ptr();
	}

	template <class T, int step> carray<T> ccollect<T, step>::grab()
	{
		cnt = 0;
		return data;
	}

	template <class T, int step> inline const T& ccollect<T, step>::const_item(int n) const
	{
		ASSERT(n>=0 && n<cnt);
		return data.const_item(n);
	}

	template <class T, int step> inline const T* ccollect<T, step>::const_ptr() const
	{
		return data.const_ptr();
	}

	template <class T, int step> T& ccollect<T, step>::operator [] (int n)
	{
		ASSERT(n>=0 && n<cnt);
		return get(n);
	}


	template <class T, int step> inline void ccollect<T, step>::append()
	{
		realloc(cnt+1);
	}

	template <class T, int step> inline void ccollect<T, step>::append (const T& a)
	{
		realloc(cnt+1);
		set(cnt-1, a);
	}


	template <class T, int step> void ccollect<T, step>::append_list (const T *p, int number)
	{
		int oldcount = cnt;
		realloc(cnt + number);
		for (int i = 0; i<number; i++, p++)
			set(oldcount+i, *p);
	}
	
	template <class T, int step> void ccollect<T, step>::append_n(const T &p, int number)
	{
		int oldcount = cnt;
		realloc(cnt + number);
		for (int i = 0; i<number; i++)
			set(oldcount+i, p);
	}


	template <class T, int step> void ccollect<T, step>::append (const ccollect &a)
	{
		for (int i = 0; i<a.count(); i++)
			append(a.const_item(i));
	}

	template <class T, int step> inline void ccollect<T, step>::insert(int n)
	{
		ASSERT(n>=0 && n<=cnt);
		realloc(cnt+1);
		for (int i = cnt-1; i>=n; i--) 
			data[i+1] = data[i];
	}

	template <class T, int step> inline void ccollect<T, step>::insert(int n, const T&a)
	{
		ASSERT(n>=0 && n<=cnt);
		realloc(cnt+1);
		for (int i = cnt-2; i>=n; i--) //cnt-1 заменено на cnt-2 25.08.2011 был косяк
			data[i+1] = data[i];
		data[n]=a;
	}

	template <class T, int step> inline void ccollect<T, step>::clear()
	{
		data.clear();
		cnt = 0;
	}

	template <class T, int step> inline ccollect<T, step>& ccollect<T, step>::operator = (const ccollect &a)
	{
		copy_from(a);
		return *this;
	}



	template <class T, int step> ccollect<T, step>::~ccollect()
	{
	}

	template <class T, int step> inline int ccollect<T, step>::count() const
	{
		return cnt;
	}

	template <class T, int step> void ccollect<T, step>::copy_from(const ccollect &a)
	{
		if (this != &a) {
			carray<T> x(a.cnt);
			for (int i = 0; i<a.cnt; i++)
				x[i] = a.data.const_item(i);
			data = x;
			cnt = a.cnt;
		}
	}

/// hash tables ////////////////////////////////////////////////////////////////////////////
	typedef int HashIndex;
	HashIndex hash_lover_table_size(HashIndex size);
	HashIndex hash_upper_table_size(HashIndex size);


#if defined(_MSC_VER) && defined(_DEBUG)
#define FloatTableParam F_TP
#endif

	template <HashIndex minsize=7, HashIndex minlevel=4, HashIndex maxlevel=4> class FloatTableParam {
	public:
		static bool need_expand(HashIndex count, HashIndex tableSize)
		{
			return (count > tableSize*maxlevel);
		}

		static bool need_shrink(HashIndex count, HashIndex tableSize)
		{
			return (tableSize > count*minlevel) && (tableSize > minsize);
		}

		static HashIndex lover(HashIndex tableSize) 
		{
			HashIndex n = hash_lover_table_size(tableSize);
			return n >= minsize ? n : minsize;
		}

		static HashIndex upper(HashIndex tableSize)
		{
			HashIndex n = hash_upper_table_size(tableSize);
			return n >= minsize ? n : minsize;
		}
	};


	template <HashIndex size> class StaticTableParam {
	public:
		static bool need_expand(HashIndex count, HashIndex tableSize){ return false;	}
		static bool need_shrink(HashIndex count, HashIndex tableSize) {	return false;   }
		static HashIndex lover(HashIndex tableSize) {	return size; }
		static HashIndex upper(HashIndex tableSize)	{	return size; }
	};


	template <class LT> class hash_iterator {
		const HashIndex tableSize;
		HashIndex i;
		carray<LT * > &table;
		LT *ptr;
		hash_iterator() {}
	public:
		void next()
		{
			if (ptr) 
				ptr = ptr->next;

			while  (!ptr && i<tableSize) 
				ptr = table[i++];
		}

		hash_iterator(carray<LT* > &tab, HashIndex size) 
			: tableSize(size), table(tab) 
		{
			if (size <= 0) {
				ptr = NULL;
				i = 0;
			} else {
				i = 1;
				ptr = table[0];
				if (!ptr)
					next();
			}
		}

		LT* get()
		{
			ASSERT(valid());
			return ptr;
		}

		bool valid(){ return ptr != NULL; }

	};



	template <class IT, class T, class P = FloatTableParam<> > class cinthash;

	/* 
		LT 
			const KT& key() 
			unsigned intKey()
			���� LT *next
			����������� �����������

		KT - ��� �����
			������ ����� operator unsigned()
			������ ������������ �� ==

		IC - true 
			���� ������� ���������� intKey(), � ����� key()
			����� ������ key()

			(���� KT ������������� � ����� intKey() �� ���������� 2 ���� �������
			 (������� ������))
	*/


	template <class LT, class KT, bool IC = true, class P = FloatTableParam<> > class internal_hash {
	public:
		internal_hash();
		internal_hash(const internal_hash &a);

		internal_hash& operator = (const internal_hash &a); 

		LT* find(unsigned intKey, const KT& key);			

		LT* append(unsigned intKey, LT * listNode);

		bool del(unsigned intKey, const KT& key, bool shrink = true);
		
		HashIndex count() const;
		void foreach(void f(LT*, void *), void *data); //nt
		void clear(); 
		hash_iterator<LT> first();

		void statistic(cinthash<int,int> *result);

		~internal_hash();
	private:
		void realloc(int newSize); //nt
		void copy(const internal_hash &a); 
		HashIndex tableSize;
		HashIndex itemCount;
		carray<LT*> table;
	};


	template <class LT, class KT, bool IC, class P>
	internal_hash<LT,KT,IC,P>::internal_hash()
	{
		tableSize = 0;
		itemCount = 0;
	}

	template <class LT, class KT, bool IC, class P>
	internal_hash<LT,KT,IC,P>::internal_hash(const internal_hash& a)
	{
		copy(a);
	}

	template <class LT, class KT, bool IC, class P>
	inline LT* internal_hash<LT,KT,IC,P>::find(unsigned intKey, const KT& key)
	{
		if (tableSize > 0) {
			for (LT *p = table[intKey % tableSize]; p ; p=p->next)
			{
				if (IC) {
					if (intKey == p->intKey() && 
						p->key() == key) //������� ��������� � == �����
						return p;
				} else {
					if (p->key() == key) //������� ��������� � == �����
						return p;
				}
			}
		}
		return NULL;
	}

	template <class LT, class KT, bool IC, class P>
	inline LT* internal_hash<LT,KT,IC,P>::append(unsigned intKey, LT *listNode)
	{
		try {
			if (tableSize<=0)
				realloc(P::upper(1));
		} catch (...) {
			delete listNode;
			throw;
		}

		unsigned n = intKey % tableSize;
		listNode->next = table[n];
		table[n] = listNode;

		itemCount++;

		if (P::need_expand(itemCount, tableSize))
			realloc(P::upper(tableSize));

		return listNode;
	}


	template <class LT, class KT, bool IC, class P>
	bool internal_hash<LT,KT,IC,P>::del(unsigned intKey, const KT& key, bool shrink)
	{
		if (tableSize<=0)
			return false;

		HashIndex n = intKey % tableSize;

		LT **p;
		for ( p = table.ptr() + n; *p; p = &(p[0]->next))

			//������� ��������� ��������� �����
			//����� ���������� ������ ���������� �������� == � KT 
			if ( IC ) {
				if (p[0]->intKey() == intKey &&  p[0]->key() == key)
					break;
			} else {
				if (p[0]->key() == key)
					break;
			}

		if (*p) {
			LT *t = *p;
			*p = t->next;
			delete t;
			itemCount--;
			if (shrink && P::need_shrink(itemCount, tableSize))
				realloc(P::lover(tableSize));
			return true;
		}

		return false;
	}

	template <class LT, class KT, bool IC, class P>
	inline void internal_hash<LT,KT,IC,P>::foreach(void f(LT*, void *), void *data)
	{
		for (HashIndex i = 0; i<tableSize; i++) {
			for( LT *p = table[i]; p; p = p->next)
				f(p, data);
		}
	}

	template <class LT, class KT, bool IC, class P>
	inline HashIndex internal_hash<LT,KT,IC,P>::count() const
	{
		return itemCount;
	}


	template <class LT> void destroy_hash_table(carray<LT*> table, HashIndex size)
	{
		for (HashIndex i = 0; i<size; i++) {
			for( LT *p = table[i]; p; ) {
				LT *t = p;
				p = p->next;
				delete t;
			}
		}
	}

	template <class LT, class KT, bool IC, class P>
	void internal_hash<LT,KT,IC,P>::clear()
	{
		destroy_hash_table<LT>(table, tableSize);
		tableSize = 0;
		itemCount = 0;
	}

	template <class LT, class KT, bool IC, class P>
	inline hash_iterator<LT> internal_hash<LT,KT,IC,P>::first() 
	{
		return hash_iterator<LT>(table, tableSize);
	}

	template <class LT, class KT, bool IC, class P>
	internal_hash<LT,KT,IC,P>::~internal_hash()
	{
		clear();
	}


	template <class LT, class KT, bool IC, class P>
	void internal_hash<LT,KT,IC,P>::realloc(int newSize)
	{
		ASSERT(newSize >= 0);

		if (tableSize == newSize) 
			return;

		if (newSize == 0) {
			ASSERT(itemCount == 0);
			table.clear();
			tableSize = 0;
		} else {
			carray<LT*> newTable(newSize);
			HashIndex i;
			for (i = 0; i<newSize; i++)
				newTable[i] = NULL;
			for (i = 0; i<tableSize; i++) {
				for (LT *p = table[i]; p; ) {
					LT *t = p;
					p = p->next;
					HashIndex n = t->intKey() % newSize;
					t->next = newTable[n];
					newTable[n] = t;
				}
			}
			table = newTable;
			tableSize = newSize;
		}
	}

	template <class LT, class KT, bool IC, class P>
	void internal_hash<LT,KT,IC,P>::copy(const internal_hash &a)
	{
		carray< LT *> tmpTable(a.tableSize);
				
		try {
			HashIndex i;
			for (i = 0; i<a.tableSize; i++)
				tmpTable[i] = NULL;

			for (i = 0; i<a.tableSize; i++) {
				for( const LT *p = a.table.const_ptr()[i];
				p; 
				p=p->next ) {
					LT *tmp = new LT(*p);
					tmp->next = tmpTable[i];
					tmpTable[i] = tmp;
				}
			}
		} catch (...) {
			destroy_hash_table<LT>(tmpTable, a.tableSize);
			throw;
		}

		if (table.ptr())
			destroy_hash_table<LT>(table, tableSize);

		table = tmpTable;
		tableSize = a.tableSize;
		itemCount = a.itemCount;
	}

	template <class LT, class KT, bool IC, class P>
	inline internal_hash<LT,KT,IC,P>& internal_hash<LT,KT,IC,P>::operator = (const internal_hash &a)
	{
		copy(a);
		return *this;
	}

	/*
		T - must have const KT& key() const
		KT must have	
			operator unsigned() const ;
			bool operator == (const KT &a) const; ///const function!!!!!!!
	*/

	template <class T, class KT, class P = FloatTableParam<> > class chash {

		struct  Node {
			unsigned hashKey;
			T data;

			Node(const T &d, unsigned k) : hashKey(k), data(d) {}
			Node(const Node &a):data(a.data),hashKey(a.hashKey){}
			const KT& key(){ return data.key(); }
			unsigned intKey(){ return hashKey; }
			Node *next;
		private:
			Node(){};
		};

		internal_hash<Node, KT, true, P> hash;
	public:
		chash(){}
		chash(const chash &a):hash(a.hash){}
		HashIndex count() const { return hash.count(); };

		T* get(const KT &k)
		{
			Node *p = hash.find(unsigned(k), k);
			return p ? &(p->data) : NULL;
		}

		chash& operator = (const chash &a)
		{
			hash = a.hash;
			return *this;
		}

		T* put(const T &data)
		{
			unsigned intk = unsigned(data.key());
			Node *p = hash.find(intk, data.key());

			if (!p) {
				p = new Node(data, intk);
				hash.append(intk, p);
			}

			return &p->data;
		}

		bool del(const KT &k, bool shrink = true )
		{
			return hash.del(unsigned(k), k, shrink);
		}

		void foreach(void (*f)(T*, void*), void*parm)
		{
			for (hash_iterator<Node> i = hash.first(); i.valid();i.next())
				f( &(i.get()->data), parm);
		}

		carray<T*> get_all()
		{
			int n = hash.count();
			carray<T*> ret(n);
			int j = 0;
			for (hash_iterator<Node> i = hash.first(); i.valid();i.next(), j++)
			{
				ASSERT(j<n);
				ret[j] =  &(i.get()->data);
			}
			return ret;
		}

		void statistic(cinthash<int,int> *result)
		{
			hash.statistic(result);
		}

		void clear(){ hash.clear();}
		~chash(){}
	};


	/*
		�������, ��� �������� �������� ����� ����� (IT)
		IT - ����� �����, ��� ���, ���������� � unsigned (), ���������� � ������������ �� ==
	*/

	template <class IT, class T, class P/* = FloatTableParam<>*/ > class cinthash {
		struct Node {
			IT k;
			T data;
			Node(const T& d, const IT &n):k(n), data(d){}
			Node(const Node& a):data(a.data),k(a.k){}
			const IT& key() const { return k; }
			unsigned intKey() const { return (unsigned)k; } 

			Node *next;
		private:
			Node(){}
		};

		internal_hash<Node, IT, false, P> hash;
		T defaultValue;
	public:
		cinthash(){}
		cinthash(const T& def):defaultValue(def){ }

		cinthash(const cinthash &a)
		:hash(a.hash),
		defaultValue(a.defaultValue)
		{
		}

		void setdefault(const T& dv){ defaultValue = dv; }
		HashIndex count() const { return hash.count(); };

		T* exist(const IT &k)
		{
			Node *p = hash.find(unsigned(k), k);
			return p ? &(p->data) : NULL;
		}

		T& get(const IT &k)
		{
			Node *p = hash.find(unsigned(k), k);
			if (!p) {
				p = new Node(defaultValue, k);
				hash.append(unsigned(k), p);
			}
			return p->data;
		}

		T& operator [](const IT& k) { return get(k); }
		cinthash& operator = (const cinthash &a)
		{
			if (this != &a)
			{
				hash = a.hash;
				defaultValue = a.defaultValue;
			}
			return *this;
		}

		T* put(IT k, const T &data)
		{
			Node *p = hash.find(unsigned(k), data.key());
			if (!p) 
				p = hash.append(unsigned(k), Node(data,k));
			else 
				p->data = data;

			return &(p->data);
		}

		bool del(const IT &k, bool shrink = true )
		{
			return hash.del(unsigned(k), k, shrink);
		}

		void foreach(void (*f)(const IT& key, T*, void*), void*parm)
		{
			for (hash_iterator<Node> i = hash.first(); i.valid();i.next())
				f( i.get()->k, &(i.get()->data), parm);
		}

		carray<IT> keys()
		{
			int n = hash.count();
			carray<IT> ret(n);
			int j = 0;
			for (hash_iterator<Node> i = hash.first(); i.valid();i.next(), j++)
			{
				ASSERT(j<n);
				ret[j] = i.get()->intKey();
			}
			ASSERT(j == hash.count());
			return ret;
		}

		void statistic(cinthash<int,int> *result)
		{
			hash.statistic(result);
		}

		void clear(){ hash.clear();}
		~cinthash(){}
	};

	template <class LT, class KT, bool IC, class P> 
		void internal_hash<LT,KT,IC,P>::statistic(cinthash<int,int> *result)
	{
		result->clear();
		result->setdefault(0);

		for (HashIndex i = 0; i<tableSize; i++) {
			LT *p = table[i];
			int j;
			for (j = 0; p; p=p->next, j++)
				NULL;
			result->get(j)++;
		}
	}

	template <class T> class chstring {

		static int size(const T*s)
		{
			int n = 0;
			for (;*s;s++)
				n++;

			return n+1;
		}

		static void cp(T *d, const T *s)
		{
			while(*(d++) = *(s++));
		}

		static T* newstr(const T*s, int size)
		{
			T *p = new T[size];
			cp(p,s);
			return p;
		}
		
		int strSize;
		T *ptr;

		void operator = (const chstring &){}

		static bool eq(const T*a, const T*b)
		{
			for (; *a /* && *b */ && *a==*b; a++, b++)
				NULL;
			return *a == *b;
		}

	public:
		chstring(){ ptr = NULL; }
		chstring(const T *s)
		{
			strSize = size(s);
			ptr = newstr(s, strSize);
		}

		chstring(const chstring &a)
		{
			ASSERT(a.ptr);
			ptr = newstr(a.ptr, a.strSize);
			strSize = a.strSize;
		}

		void operator = (const T *s)
		{
			int n = size(s);
			T *p = newstr(s, n);
			if (ptr)
				delete [] ptr;
			ptr = p;
			strSize = n;
		}

		const T* str() { ASSERT(ptr); return ptr; }

		static unsigned key(const T *s)
		{
			unsigned n = 0;
			for (; *s; s++)
				n = (n<<1) ^ unsigned(*s);
			return n;
		}

		bool operator == (const T *s) const
		{
			return eq(ptr, s);
		}

		unsigned key()
		{
			ASSERT(ptr);
			return key(ptr);
		}

		~chstring()
		{
			if (ptr) 
				delete [] ptr;
		}
	};


	template <class T, class CT = char, class P = FloatTableParam<> > class cstrhash {
		struct Node {
			T data;
			unsigned hashKey;
			chstring<CT> string;

			Node(const T& d, const CT *s, unsigned h)
			:data(d), string(s), hashKey(h)
			{
			}

			const chstring<CT>& key() const { return string; }
			unsigned intKey() const { return hashKey; }
			Node *next;
		private:
			Node(){}
		};

		internal_hash<Node, const CT*, true, P> hash;
		T defaultValue;
	public:
		cstrhash(){};

		cstrhash(const T& def)
		:defaultValue(def)
		{
		}

		cstrhash(const cstrhash &a)
		:hash(a.hash),
		defaultValue(a.defaultValue)
		{
		}

		void setdefault(const T &def) { defaultValue = def; }
		HashIndex count() const { return hash.count(); };

		T* exist(const CT *s)
		{
			Node *p = hash.find(chstring<CT>::key(s), s);
			return p ? &(p->data) : NULL;
		}

		T& get(const CT *s)
		{
			unsigned hashKey = chstring<CT>::key(s);
			Node *p = hash.find(hashKey, s);
			if (!p) {
				p = new Node(defaultValue, s, hashKey);
				hash.append(hashKey, p);
			}
			return p->data;
		}

		T& operator [](const CT *s) { return get(s); }

		cstrhash& operator = (const cstrhash &a) 
		{
			if (this != &a) {
				hash = a.hash;	
				defaultValue = a.defaultValue;
			}
			return *this;
		}

		T& put(CT *s, const T &data)
		{
			unsigned hashKey = chstring<CT>::key(s);
			Node *p = hash.find(hashKey, s);

			if (!p) {
				p = new Node(data, s, hashKey);
				hash.append(hashKey, p);
			} else
				p->data = data;

			return p->data;
		}

		bool del(const CT *s, bool shrink = true )
		{
			return hash.del( chstring<CT>::key(s), s, shrink);
		}

		void foreach(void (*f)(const CT *s, T*, void*), void*parm)
		{
			for (hash_iterator<Node> i = hash.first(); i.valid(); i.next())
				f( i.get()->string.str(), &(i.get()->data), parm);
		}

		carray<const CT*> keys()
		{
			int n = hash.count();
			carray<const CT*> ret(n);
			int j = 0;
			for (hash_iterator<Node> i = hash.first(); i.valid(); i.next(), j++)
			{
				ASSERT(j<n);
				ret[j] = i.get()->string.str();
			}
			ASSERT(j == hash.count());
			return ret;
		}

		void statistic(cinthash<int,int> *result)
		{
			hash.statistic(result);
		}

		void clear(){ hash.clear();}
 
		~cstrhash(){};
	};


// merge sort /////////////////////////////////////////////////////////////////

	template <class T> void sort2m(T*buffer, int count, bool lessf(T *a,T *b))
	{
		T *p0 = buffer, *nb= NULL, *p1=NULL, *tmp;
		nb = new T[count];
		p1 = nb;
		try {
			int sl = 1;
		
			do {
				//int sl2 = sl*2;
				T *pp0 = p0, *pp1 = p1;
				for (int n = count;n>0;) {
					int lc = n>sl ? sl : n;
					n-=lc;
					int rc = n>sl ? sl : n;
					n-=rc;
					T *lp, *rp;
					for (lp = pp0, rp = pp0+lc; lc>0 && rc>0; pp1++)
						if (lessf(lp, rp)) { 
							*pp1 = *(lp++); 
							lc--; 
						} else  {
							*pp1 = *(rp++);
							rc--;
						}
					for (;lc>0; lc--, lp++, pp1++) *pp1 = *lp;
					for (;rc>0; rc--, rp++, pp1++) *pp1 = *rp;
					pp0 = rp;
				}
				{
					tmp = p1;
					p1 = p0; 
					p0 = tmp;
				}
				sl *= 2;
			} while (sl < count);

			if (p0!=buffer) 
				for (int i = 0; i<count; i++) *(p1++) = *(p0++);
		} catch (...) {
			if (nb) delete [] nb;
			throw;
		}
		delete [] nb;
	}

	template <class T, class DataT> void sort2m_data(T*buffer, int count, bool lessf(T *a,T *b, DataT *data), DataT *data)
	{
		T *p0 = buffer, *nb= NULL, *p1=NULL, *tmp;
		nb = new T[count];
		p1 = nb;
		try {
			int sl = 1;
		
			do {
				int sl2 = sl*2;
				T *pp0 = p0, *pp1 = p1;
				for (int n = count;n>0;) {
					int lc = n>sl ? sl : n;
					n-=lc;
					int rc = n>sl ? sl : n;
					n-=rc;
					T *lp, *rp;
					for (lp = pp0, rp = pp0+lc; lc>0 && rc>0; pp1++)
						if (lessf(lp, rp, data)) { 
							*pp1 = *(lp++); 
							lc--; 
						} else  {
							*pp1 = *(rp++);
							rc--;
						}
					for (;lc>0; lc--, lp++, pp1++) *pp1 = *lp;
					for (;rc>0; rc--, rp++, pp1++) *pp1 = *rp;
					pp0 = rp;
				}
				{
					tmp = p1;
					p1 = p0; 
					p0 = tmp;
				}
				sl *= 2;
			} while (sl < count);

			if (p0!=buffer) 
				for (int i = 0; i<count; i++) *(p1++) = *(p0++);
		} catch (...) {
			if (nb) delete [] nb;
			throw;
		}
		delete [] nb;
	}


	// ���� T - ������������
	template <class T> void sort2m(T*buffer, int count, bool ascend = true)
	{
		T *p0 = buffer, *nb= NULL, *p1=NULL, *tmp;
		nb = new T[count];
		p1 = nb;
		try {
			int sl = 1;
		
			do {
				int sl2 = sl*2;
				T *pp0 = p0, *pp1 = p1;
				for (int n = count;n>0;) {
					int lc = n>sl ? sl : n;
					n-=lc;
					int rc = n>sl ? sl : n;
					n-=rc;
					T *lp, *rp;
					for (lp = pp0, rp = pp0+lc; lc>0 && rc>0; pp1++)

  					if ( (ascend) == ( *lp <= *rp) ) { 
							*pp1 = *(lp++); 
							lc--; 
						} else  {
							*pp1 = *(rp++);
							rc--;
						}
					for (;lc>0; lc--, lp++, pp1++) *pp1 = *lp;
					for (;rc>0; rc--, rp++, pp1++) *pp1 = *rp;
					pp0 = rp;
				}
				{
					tmp = p1;
					p1 = p0; 
					p0 = tmp;
				}
				sl *= 2;
			} while (sl < count);

			if (p0!=buffer) 
				for (int i = 0; i<count; i++) *(p1++) = *(p0++);
		} catch (...) {
			if (nb) delete [] nb;
			throw;
		}
		delete [] nb;
	}

}; //namespace wal

//for use sort2m<char *>(strArray, size, strless< char* >);
//or sort2m<const char *>(strArray, size, strless< const char* >);

template <class T> inline bool strless(T *a,T *b)
{
	T s1 = *a, s2 = *b;
	for (; *s1 && *s1 == *s2; s1++, s2++)
		NULL;
	return *s1 <= *s2;
}


template <class T> inline char *positive_to_char_decimal(T n, char *s)
{
	char c[32];
	int i=0;
	for (; n>0; i++, n/=10)
		c[i]=char(n%10)+'0';
	for (i--; i>=0; i--)
		*(s++) = c[i];
	return s;	
}

template <class T>  char *unsigned_to_char(T n, char *s, char terminator = 0)
{
	if (n==0)
		*(s++)='0';
	else
		s = positive_to_char_decimal<T>(n,s);
	*(s++) = terminator;
	return s;
}

template <class T> char *int_to_char(T n, char *s)
{
	if (n<0)
	{
		*(s++) = '-';
		n = -n;
	}
	return unsigned_to_char<T>(n, s);
}

#endif
