#include <stdexcept>
#include"heaparray.h"

#ifndef USMG_HEAPARRAY_CPP
#define USMG_HEAPARRAY_CPP
// Constructor with size and optional fill value
template<class TYPE>
HeapArray<TYPE>::HeapArray(SizeType size, const TYPE& fill) : size(size) {
    if (size == 0) throw std::invalid_argument("Size cannot be zero");
    __data__ = new TYPE[size];
    for (SizeType i = 0; i < size; ++i) {
        __data__[i] = fill;
    }
}

// Constructor from initializer list
template<class TYPE>
HeapArray<TYPE>::HeapArray(const std::initializer_list<TYPE>& initList) : size(initList.size()) {
    __data__ = new TYPE[size];
    SizeType i = 0;
    for(auto itr = initList.begin(); itr != initList.end() && i < size; ++itr){
        __data__[i] = *itr; i++;
    }
}

// Copy constructor
template<class TYPE>
HeapArray<TYPE>::HeapArray(const HeapArray<TYPE>& other) : size(other.size) {
    __data__ = new TYPE[size];
    for (SizeType i = 0; i < size; ++i) {
        __data__[i] = other.__data__[i];
    }
}

// Move constructor
template<class TYPE>
HeapArray<TYPE>::HeapArray(HeapArray<TYPE>&& other) : size(other.size), __data__(other.__data__) {
    other.__data__ = nullptr;
}

// Destructor
template<class TYPE>
HeapArray<TYPE>::~HeapArray() {
    delete[] __data__;
}

// Copy assignment
template<class TYPE>
HeapArray<TYPE>& HeapArray<TYPE>::operator=(const HeapArray<TYPE>& rhs){
    if (this != &rhs) {
        if (size != rhs.size) throw std::runtime_error("Cannot assign arrays of different sizes");
        for (SizeType i = 0; i < size; ++i) {
            __data__[i] = rhs.__data__[i];
        }
    }
    return *this;
}

//Move assignment
template<class TYPE>
HeapArray<TYPE>& HeapArray<TYPE>::operator=(HeapArray<TYPE>&& rhs) {
    if (this != &rhs) {
        if (size != rhs.size) throw std::runtime_error("Cannot assign arrays of different sizes");
        delete[] __data__;
        __data__ = rhs.__data__;
        rhs.__data__ = nullptr;
    }
    return *this;
}

// Clear data
template<class TYPE>
void HeapArray<TYPE>::clear_data() {
    for (SizeType i = 0; i < size; ++i) {
        __data__[i] = TYPE{};
    }
}

// Bracket operator with bounds checking
template<class TYPE>
TYPE& HeapArray<TYPE>::operator[](SizeType i) {
    if (i >= size) throw std::out_of_range("Index out of bounds");
    return __data__[i];
}

template<class TYPE>
const TYPE& HeapArray<TYPE>::operator[](SizeType i) const {
    if (i >= size) throw std::out_of_range("Index out of bounds");
    return __data__[i];
}

// Parenthesis operator with modulo indexing
template<class TYPE>
TYPE& HeapArray<TYPE>::operator()(long i) {
    return __data__[(i % (long)size + size) % size];
}

template<class TYPE>
const TYPE& HeapArray<TYPE>::operator()(long i) const {
    return __data__[(i % (long)size + size) % size];
}

// Equality operator
template<class TYPE>
bool HeapArray<TYPE>::operator==(const HeapArray<TYPE>& rhs) const {
    if (size != rhs.size) return false;
    for (SizeType i = 0; i < size; ++i) {
        if (__data__[i] != rhs.__data__[i]) return false;
    }
    return true;
}

// Fill method
template<class TYPE>
void HeapArray<TYPE>::fill(const TYPE& fill) {
    for (SizeType i = 0; i < size; ++i) {
        __data__[i] = fill;
    }
}

// Swap function
template<class TYPE>
void HeapArray<TYPE>::swap(HeapArray<TYPE>& other) {
    if (size != other.size) throw std::runtime_error("Cannot swap arrays of different sizes");
    TYPE* temp = __data__;
    __data__ = other.__data__;
    other.__data__ = temp;
}

template<class TYPE> HeapArray<TYPE>::Iterator::Iterator(){}

template<class TYPE> HeapArray<TYPE>::Iterator::Iterator(const Iterator& other): ptr(other.ptr) {}

template<class TYPE>
typename HeapArray<TYPE>::Iterator& HeapArray<TYPE>::Iterator::operator=(const Iterator& rhs){
    this->ptr = rhs.ptr; return *this;
}

template<class TYPE>
bool HeapArray<TYPE>::Iterator::operator==(const Iterator& rhs) const{
    return (this->ptr == rhs.ptr);
}

template<class TYPE>
bool HeapArray<TYPE>::Iterator::operator!=(const Iterator& rhs) const{
    return !(*this == rhs);
}

template<class TYPE>
typename HeapArray<TYPE>::Iterator& HeapArray<TYPE>::Iterator::operator++(){
    ptr++; return *this;
}

template<class TYPE>
typename HeapArray<TYPE>::Iterator& HeapArray<TYPE>::Iterator::operator--(){
    ptr--; return *this;
}

template<class TYPE>
TYPE& HeapArray<TYPE>::Iterator::operator*(){
    return *ptr;
}

template<class TYPE>
TYPE* HeapArray<TYPE>::Iterator::operator->(){
    return ptr;
}

template<class TYPE>
typename HeapArray<TYPE>::Iterator HeapArray<TYPE>::begin(){
    Iterator output; 
    output.ptr = this->__data__; 
    return output;
}

template<class TYPE>
typename HeapArray<TYPE>::Iterator HeapArray<TYPE>::end(){
    Iterator output; 
    output.ptr = this->__data__ + size; 
    return output;
}

template<class TYPE> HeapArray<TYPE>::ConstIterator::ConstIterator(){}

template<class TYPE> HeapArray<TYPE>::ConstIterator::ConstIterator(const ConstIterator& other): ptr(other.ptr) {}

template<class TYPE>
typename HeapArray<TYPE>::ConstIterator& HeapArray<TYPE>::ConstIterator::operator=(const ConstIterator& rhs){
    this->ptr = rhs.ptr; return *this;
}

template<class TYPE>
bool HeapArray<TYPE>::ConstIterator::operator==(const ConstIterator& rhs) const{
    return (this->ptr == rhs.ptr);
}

template<class TYPE>
bool HeapArray<TYPE>::ConstIterator::operator!=(const ConstIterator& rhs) const{
    return !(*this == rhs);
}

template<class TYPE>
typename HeapArray<TYPE>::ConstIterator& HeapArray<TYPE>::ConstIterator::operator++(){
    ptr++; return *this;
}

template<class TYPE>
typename HeapArray<TYPE>::ConstIterator& HeapArray<TYPE>::ConstIterator::operator--(){
    ptr--; return *this;
}

template<class TYPE>
const TYPE& HeapArray<TYPE>::ConstIterator::operator*() const{
    return *ptr;
}

template<class TYPE>
const TYPE* HeapArray<TYPE>::ConstIterator::operator->() const{
    return ptr;
}

template<class TYPE>
typename HeapArray<TYPE>::ConstIterator HeapArray<TYPE>::begin() const{
    ConstIterator output; 
    output.ptr = this->__data__; 
    return output;
}

template<class TYPE>
typename HeapArray<TYPE>::ConstIterator HeapArray<TYPE>::end() const{
    ConstIterator output; 
    output.ptr = this->__data__ + size; 
    return output;
}
#endif