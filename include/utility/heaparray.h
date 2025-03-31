#pragma once
#include<initializer_list>

/*
A data structure that stores an array of a fixed, arbitrary size in the heap. 
Wrapper over a pointer to heap-allocated memory. Size is 16 bytes.
 */
template<class TYPE>
class HeapArray{
  private:
    typedef unsigned long SizeType;
    TYPE* __data__;

  public:
    const SizeType size;
    // No default constructor, needs to have at least a size
    HeapArray() = delete;
    HeapArray(SizeType size, const TYPE& fill = TYPE{});
    // Constructs from an std::initializer_list, automatically deduces size
    HeapArray(const std::initializer_list<TYPE>& initList);
    // Copy/Move constructors, destructor, assignment operators
    HeapArray(const HeapArray<TYPE>& other);
    HeapArray(HeapArray<TYPE>&& other);
    ~HeapArray();
    HeapArray<TYPE>& operator=(const HeapArray<TYPE>& rhs);
    HeapArray& operator=(HeapArray<TYPE>&& rhs);
    // Sets all elements to `TYPE{}`.
    void clear_data();
    // Indexing operators. Operator () admits numbers outside the range [0,size) and reduces modulo size.
    TYPE& operator[](SizeType i);
    const TYPE& operator[](SizeType i) const;
    TYPE& operator()(long i);
    const TYPE& operator()(long i) const;
    bool operator==(const HeapArray<TYPE>& rhs) const;
    // Sets all elements to the one specified by `fill`.
    void fill(const TYPE& fill);
    void swap(HeapArray<TYPE>& other);
    // Iterator classes, works with range-for loops.
    class Iterator;
    class ConstIterator;

    class Iterator{
      private:
        TYPE* ptr;
      public:
        Iterator();
        Iterator(const Iterator& other);
        Iterator& operator=(const Iterator& rhs);
        bool operator==(const Iterator& rhs) const;
        bool operator!=(const Iterator& rhs) const;
        Iterator& operator++();
        inline Iterator& operator++(int){  return this->operator++();  };
        Iterator& operator--();
        inline Iterator& operator--(int){  return this->operator--();  };
        TYPE& operator*();
        TYPE* operator->();

        friend class HeapArray<TYPE>;
    };

    Iterator begin();
    Iterator end();

    class ConstIterator{
      private:
        const TYPE* ptr;
      public:
        ConstIterator();
        ConstIterator(const ConstIterator& other);
        ConstIterator& operator=(const ConstIterator& rhs);
        bool operator==(const ConstIterator& rhs) const;
        bool operator!=(const ConstIterator& rhs) const;
        ConstIterator& operator++();
        inline ConstIterator& operator++(int){  return this->operator++();  };
        ConstIterator& operator--();
        inline ConstIterator& operator--(int){  return this->operator--();  };
        const TYPE& operator*() const;
        const TYPE* operator->() const;

        friend class HeapArray<TYPE>;
    };

    ConstIterator begin() const;
    ConstIterator end() const;
};

#include "heaparray.cpp"
//@c-ignore
//#include "./src/DS/heaparray.cpp"