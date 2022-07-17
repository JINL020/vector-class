#ifndef VECTOR_H
#define VECTOR_H

#include<iostream>
#include<stdexcept>
#include<initializer_list>

using std::size_t;
using std::runtime_error;
using std::ostream;

template<typename T>
class Vector{
public:
	class ConstIterator;
	class Iterator;
	//using alias
	using value_type = T;
	using size_type = std::size_t;
	using difference_type = std::ptrdiff_t;
	using reference = value_type&;
	using const_reference = const value_type&;
	using pointer = value_type*;
	using const_pointer = const value_type*;
	using iterator = Vector::Iterator;
	using const_iterator = Vector::ConstIterator;

private:
	size_type size;
	size_type max_size;
	pointer values;
	static constexpr size_t min_size{5};

public:
	// default and size_type n constructor 
	Vector(size_type n = min_size) : size{0}, max_size{(n < min_size) ? min_size : n}, values{new value_type[max_size]}{}


	//constructor with std::initializer_list
	Vector(std::initializer_list<value_type> list) : Vector(list.size()){
		for(const auto& value : list) {
			values[size++] = value;
        }
	}


	// copy constructor
	Vector(const Vector& other) : Vector(other.max_size){
		for(const auto& value : other){
			values[size++] = value;
        }
	}


	// copy assignment operator
	Vector operator=(Vector cpy){ //kein const Vector&!!!!
		std::swap(size,cpy.size);
		std::swap(max_size,cpy.max_size);
		std::swap(values,cpy.values);
		return *this;
	}


	// destructor
	~Vector(){
		delete[] values;
	}


	size_type size()const{
		return size;
	}
	

	size_type capacity()const{
		return max_size;
	}


	bool empty()const{
		return size == 0;
	}


	void clear(){
		size = 0;
	}


	void shrink_to_fit(){
		if(size == 0)
			throw runtime_error("trying to shrink to 0 but a array can not be size 0");
		if(size == max_size)
			return;
		if(size < max_size){
			pointer temp = new value_type[size];//soll es immer zu size shrinken oder zu min_size, wenn n < min_sz?
			for(size_type i = 0; i < size; i++){
        		temp[i] = values[i];
            }
			delete[] values;
			values = temp;
			delete[] temp;
			max_size = size;	
		}
	}


	void reserve(size_type n){
		if(n <= max_size)
			return;
        value_type *values2 = new value_type[n];
        for(size_type i{0}; i < size; i++){
            values2[i] = values[i];
        }
        delete[] values;
        values = values2;
        max_size = n;
	}


	void push_back(value_type x){
		if(size >= max_size) // Defensive Programmierung: >= statt == 
			this->reserve(size*2);
		values[size++] = x;
	}

	void pop_back(){
		if(size == 0)
			throw runtime_error("Can not pop_back: Vector is empty!");
		--size;
	}


	reference operator[](size_type index){
        if(size == 0)
			throw runtime_error("operator[]: vector is empty!"); 
		if(index >= size)
			throw runtime_error("operator[]: index out of range!"); 
		return values[index];
	}
	
	
	const_reference operator[](size_type index)const{
		if(size == 0)
			throw runtime_error("const operator[]: vector is empty!"); 
        if(index >= size)
			throw runtime_error("const operator[]: index out of range!"); 
		return values[index];
	}


	iterator insert(const_iterator pos, const_reference val) {
		auto diff = pos-begin(); //value count
		if (diff < 0 || static_cast<size_type>(diff) > size)
			throw std::runtime_error("Iterator out of bounds");
		size_type current{static_cast<size_type>(diff)};
		if (size >= max_size)
			reserve(max_size*2);
		for (size_type i{size}; i-- > current;)
			values[i+1]=values[i];
		values[current]=val;
		++size;
		return iterator{values+current};
	}

	//erase
	iterator erase(const_iterator pos) {
		auto diff = pos-begin();
		if(diff < 0 || static_cast<size_type>(diff) >= size)
			throw std::runtime_error("Iterator out of bounds");
		size_type current{static_cast<size_type>(diff)};
		for (size_type i{current}; i < size-1; i++)
			values[i]=values[i+1];
		--size;
		return iterator{values+current};
	}


	ostream& print(ostream& o)const{
		o << '['; 
		bool first{true};
		for(size_type i{0}; i < size; i++){
			if(!first){
				o << ", ";
			}
			o << values[i];
			first = false;
		}
		o << ']';
		return o; 
	}


	//begin() und end()
	iterator begin(){
		return iterator(values);
	}	


	iterator end(){
		return iterator(values + size);
	}


	class Iterator{
	public:
		using value_type = Vector::value_type;
		using reference = Vector::reference;
		using pointer = Vector::pointer;
		using difference_type = Vector::difference_type;
		using iterator_category = std::forward_iterator_tag;


	private:
		pointer ptr;


	public:
		Iterator(pointer ptr = nullptr): ptr{ptr}{}


		reference operator*(){
			return *ptr; //dereference
		}


		pointer operator->()const{ //wieso const
			return ptr; //Was muss man machen? (*this).operation == this->operation
		}

		
		bool operator==(const const_iterator& rop)const{
			return ptr == rop.operator->(); //ob der pointer der selbe ist oder obs auf das gleiche Element zeigt?
		}

		
		bool operator!=(const const_iterator& rop)const{
			return ptr != rop.operator->();
		}


		//präfix
		iterator& operator++(){
			++ptr;
			return *this;
		}


		//postfix
		iterator operator++(int){//int ist ein dummy
			Iterator temp(*this);
			++ptr;
			return temp;
		}


		// type conversion
		operator const_iterator()const{
			return const_iterator{ptr};
		}

	};// end of Iterator class


	//const_begin() und const_end()
	const_iterator begin()const{
		return const_iterator(values);
	}


	const_iterator end()const{
		return const_iterator(values + size);
	}

	//ConstIterator
	class ConstIterator{
	public:
		using value_type = Vector::value_type;
		using reference = Vector::const_reference;
		using pointer = Vector::const_pointer;
		using difference_type = Vector::difference_type;
		using iterator_category = std::forward_iterator_tag;


	private:
		pointer ptr;


	public:
		ConstIterator(const_pointer ptr = nullptr): ptr{ptr}{}
		
		reference operator*(){
			return *ptr;
		}


		pointer operator->()const{
			return ptr; //Was muss man machen? (*this).operation == this->operation
		}
		

		//wurde verändert!!!
		bool operator==(const const_iterator& rop)const{
			return ptr == rop.operator->();
		}
	

		//wurde verändert!!!
		bool operator!=(const const_iterator& rop)const{
			return ptr != rop.operator->();
		}


		//präfix
		const_iterator& operator++(){
			++ptr;
			return *this;
		}


		//postfix
		const_iterator operator++(int){
			const_iterator temp{this->ptr};
			++ptr;
			return temp;
		}


		friend Vector::difference_type operator-(const Vector::ConstIterator& lop, const Vector::ConstIterator& rop){
			return lop.ptr-rop.ptr;
		}

	};// end of ConstIterator class

};// end of Vector class

template<typename T>
ostream& operator<<(ostream& o, const Vector<T>& v){
	return v.print(o);
}

#endif
