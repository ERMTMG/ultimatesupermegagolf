#include<vector>
#include<cstring>

namespace util {

static void memcpy_chunks(const void* src, void* dest, size_t chunkSize, size_t totalBytes){
    for(size_t i = 0; i < totalBytes; i += chunkSize){
        memcpy(dest, src, chunkSize);
    }
}

template<class Type>
class Matrix {
  private:
    std::vector<Type> m_vec;
    size_t m_rows;
    size_t m_cols;
  public:
    Matrix() : m_vec{}, m_rows(0), m_cols(0) {}
    Matrix(size_t rows, size_t cols, const Type& fill = Type{}) : m_vec(rows*cols, fill), m_rows(rows), m_cols(cols) {}
    ~Matrix(){}
    Matrix(const Matrix<Type>&) = default;
    Matrix<Type>& operator= (const Matrix<Type>&) = default;
    Matrix(Matrix<Type>&&) = default;
    Matrix<Type>& operator= (Matrix<Type>&&) = default;

    inline Type& operator() (size_t i, size_t j){
        return m_vec[i*m_rows + j];
    }

    inline const Type& operator() (size_t i, size_t j) const {
        return m_vec[i*m_rows + j];
    }

    inline Type* operator[] (size_t i){
        return m_vec.data() + i*m_rows;
    }

    inline const Type* operator[] (size_t i) const {
        return m_vec.data() + i*m_rows;
    } 

    inline size_t rows() const {
        return m_rows;
    }

    inline size_t cols() const {
        return m_cols;
    }

    inline void fill(const Type& fill){
        std::fill(m_vec.begin(), m_vec.end(), fill); // boy i sure hope this is optimized for trivial types
    }

    inline size_t total_num_elements() const {
        return m_vec.size();
    }

    inline void resize(size_t newRows, size_t newCols, const Type& fill = Type{}){
        std::vector<Type> newBuffer(newRows * newCols, fill);
        size_t minRows = std::min(newRows, m_rows);
        size_t minCols = std::min(newCols, m_cols);
        for(size_t i = 0; i < minRows; i++){
            for(size_t j = 0; j < minCols; i++){
                newBuffer[i*newRows + j] = m_vec[i*m_rows + j];
            }
        }
        m_vec = std::move(newBuffer);
        m_rows = newRows;
        m_cols = newCols;
    }

    inline void clear(){
        m_vec.clear();
        m_rows = m_cols = 0;
    }

    class Iterator {
      private:
        using InternalItr = typename std::vector<Type>::iterator;
        InternalItr m_itr;
        InternalItr m_end;
        Iterator(const InternalItr& itr, const InternalItr& end) : m_itr(itr), m_end(end) {};
      public:
        Iterator() = default;
        Iterator(const Iterator&) = default;
        Iterator& operator=(const Iterator&) = default;

        bool operator== (const Iterator& rhs) const {
            return (this->m_itr == rhs.m_itr);
        }

        bool operator!= (const Iterator& rhs) const {
            return !(*this == rhs);
        }

        Type& operator* () const {
            return *m_itr;
        }
        
        Iterator& operator++ () {
            if(m_itr != m_end) ++m_itr;
            return *this;
        }
        friend class Matrix;
    };

    inline Iterator begin(){
        return {m_vec.begin(), m_vec.end()};
    }
    inline Iterator end(){
        return {m_vec.end(), m_vec.end()};
    }

    class ConstIterator {
      private:
        using InternalItr = typename std::vector<Type>::const_iterator;
        InternalItr m_itr;
        InternalItr m_end;
        ConstIterator(const InternalItr& itr, const InternalItr& end) : m_itr(itr), m_end(end) {};
      public:
        ConstIterator() = default;
        ConstIterator(const ConstIterator&) = default;
        ConstIterator& operator=(const ConstIterator&) = default;

        bool operator== (const ConstIterator& rhs) const {
            return (this->m_itr == rhs.m_itr);
        }

        bool operator!= (const ConstIterator& rhs) const {
            return !(*this == rhs);
        }

        const Type& operator* () const {
            return *m_itr;
        } 
        ConstIterator& operator++ () {
            if(m_itr != m_end) ++m_itr;
            return *this;
        }
        friend class Matrix;
    };

    inline ConstIterator begin() const {
        return {m_vec.cbegin(), m_vec.cend()};
    }
    inline ConstIterator end() const {
        return {m_vec.cend(), m_vec.cend()};
    }
};


}