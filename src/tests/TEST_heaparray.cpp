#include <iostream>
#include"DS/heaparray.h"

void print(const HeapArray<int>& arr, std::ostream& out){
    out << '[';
    for(const int& x : arr){
        out << x << ',';
    }
    out << "\b]";
}

int main() {
    HeapArray<int> array1(5);
    HeapArray<int> array2(5);
    HeapArray<int> array3(6);
    for(int i = 0; i < array1.size; i++){
        array1[i] = i;
    }
    for(int i = -1; i >= -((long)array2.size); i--){
        array2(i) = -i - 1;
    }
    array1.swap(array2);
    array3.fill(-1);
    for(int& x : array1){
        x++;
    }
    std::cout << "array1: "; print(array1, std::cout); std::cout << '\n';
    std::cout << "array2: "; print(array2, std::cout); std::cout << '\n';
    std::cout << "array3: "; print(array3, std::cout); std::cout << '\n';
    return 0;
}