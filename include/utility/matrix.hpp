#include<vector>
#include<cstring>

namespace util {

/*
    M*N matrix data structure that holds its components contiguously by rows.
    Can be resized, but it's intended to be relatively static size.
*/
template<class Type>
class Matrix {
  private:
    std::vector<Type> m_vec;
    size_t m_rows;
    size_t m_cols;
  public:
    // Default constructor, constructs an empty matrix
    Matrix() : m_vec{}, m_rows(0), m_cols(0) {}

    // Constructs a matrix of size rows * cols with all its elements equal to fill
    Matrix(size_t rows, size_t cols, const Type& fill = Type{}) : m_vec(rows*cols, fill), m_rows(rows), m_cols(cols) {}
    
    // Destructor
    ~Matrix(){}

    ;// Copy and move constructor and assignment work like normal, all handled by std::vector

    Matrix(const Matrix<Type>&) = default;
    Matrix<Type>& operator= (const Matrix<Type>&) = default;
    Matrix(Matrix<Type>&&) = default;
    Matrix<Type>& operator= (Matrix<Type>&&) = default;

    // Allows element modification via the syntax matrix(i,j). Doesn't check whether i and j are in bounds.
    inline Type& operator() (size_t i, size_t j){
        return m_vec[i*m_cols + j];
    }

    // Allows element read-only access via the syntax matrix(i,j). Doesn't check whether i and j are in bounds.
    inline const Type& operator() (size_t i, size_t j) const {
        return m_vec[i*m_cols + j];
    }

    // Allows element modification via the syntax matrix[i][j]. Doesn't check whether i is in bounds
    // (matrix[i] returns a pointer to the i-th row)
    inline Type* operator[] (size_t i){
        return m_vec.data() + i*m_cols;
    }

    // Allows element read-only access via the syntax matrix[i][j]. Doesn't check whether i is in bounds
    // (matrix[i] returns a pointer to the i-th row)
    inline const Type* operator[] (size_t i) const {
        return m_vec.data() + i*m_cols;
    } 

    // Returns the number of valid rows
    inline size_t rows() const {
        return m_rows;
    }

    // Returns the number of valid columns
    inline size_t cols() const {
        return m_cols;
    }

    // Sets all the elements of the matrix to fill
    inline void fill(const Type& fill){
        std::fill(m_vec.begin(), m_vec.end(), fill); // boy i sure hope this is optimized for trivial types
    }

    // Returns the total number of entries in the matrix (the size of the vector)
    inline size_t total_num_elements() const {
        return m_vec.size();
    }

    // Resizes the matrix to be of order newRows * newCols. If this size is bigger than the original
    // size and leaves new rows or columns, all the elements in these spaces are initialized with fill.
    // If the size is smaller than the original size, the elements that are outside the bonds of the
    // new size are deleted.
    inline void resize(size_t newRows, size_t newCols, const Type& fill = Type{}){
        if(m_rows == newRows && m_cols == newCols) return;
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

    // Clears the contents of the matrix and sets its order to 0 * 0
    inline void clear(){
        m_vec.clear();
        m_rows = m_cols = 0;
    }

    // Writable iterator class for easy access to all elements regardless of position
    class Iterator {
      private:
        using InternalItr = typename std::vector<Type>::iterator;
        InternalItr m_itr;
        InternalItr m_end;
        Iterator(const InternalItr& itr, const InternalItr& end) : m_itr(itr), m_end(end) {};
      public:
        // Default constructor
        Iterator() = default;

        ;// Copy constructor and assignment
        Iterator(const Iterator&) = default;
        Iterator& operator=(const Iterator&) = default;

        // Equality comparison
        bool operator== (const Iterator& rhs) const {
            return (this->m_itr == rhs.m_itr);
        }

        // Inequality comparison
        bool operator!= (const Iterator& rhs) const {
            return !(*this == rhs);
        }

        // Dereference operator for element access
        Type& operator* () const {
            return *m_itr;
        }
        
        // Increment operator for iterator advancement. Returns a reference to the 
        // newly advanced iterator
        Iterator& operator++ () {
            if(m_itr != m_end) ++m_itr;
            return *this;
        }
        friend class Matrix;
    };

    // Returns a new iterator pointing to the matrix's first element (0,0)
    inline Iterator begin(){
        return {m_vec.begin(), m_vec.end()};
    }

    // Returns a new iterator indicating the end of the matrix (not dereferenciable)
    inline Iterator end(){
        return {m_vec.end(), m_vec.end()};
    }

    // Read-only iterator class for easy access to all elements regardless of position
    class ConstIterator {
      private:
        using InternalItr = typename std::vector<Type>::const_iterator;
        InternalItr m_itr;
        InternalItr m_end;
        ConstIterator(const InternalItr& itr, const InternalItr& end) : m_itr(itr), m_end(end) {};
      public:
        // Default constructor
        ConstIterator() = default;
        ;// Copy constructor and assignment
        ConstIterator(const ConstIterator&) = default;
        ConstIterator& operator=(const ConstIterator&) = default;

        // Equality comparison
        bool operator== (const ConstIterator& rhs) const {
            return (this->m_itr == rhs.m_itr);
        }

        // Inequality comparison
        bool operator!= (const ConstIterator& rhs) const {
            return !(*this == rhs);
        }

        // Dereference operator for element access
        const Type& operator* () const {
            return *m_itr;
        } 

        // Increment operator for iterator advancement. Returns a reference to the
        // newly advanced iterator
        ConstIterator& operator++ () {
            if(m_itr != m_end) ++m_itr;
            return *this;
        }
        friend class Matrix;
    };

    // Returns a new const iterator pointing to the matrix's first element (0,0)
    inline ConstIterator begin() const {
        return {m_vec.cbegin(), m_vec.cend()};
    }

    // Returns a new const iterator indicating the end of the matrix (not dereferenciable)
    inline ConstIterator end() const {
        return {m_vec.cend(), m_vec.cend()};
    }

    //friend void std::swap<Matrix<Type>>(Matrix<Type>& m1, Matrix<Type>& m2) noexcept;
};


}
/*
namespace std {
    
template<class Type>
void swap<util::Matrix<Type>>(util::Matrix<Type>& m1, util::Matrix<Type>& m2) noexcept {
    std::swap(m1.m_vec, m2.m_vec);
    std::swap(m1.m_rows, m2.m_rows);
    std::swap(m1.m_cols, m2.m_cols);
}

}
*/