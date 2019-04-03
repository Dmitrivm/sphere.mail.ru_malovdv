#include <iostream>
#include <cstring>

// D.V. Malov
// Matrix class

struct Matrix {

  // proxy-class for [][] implemetation
  struct Row {
    private:  
      int* memstart;
      int colsnum;
    
    public:

      Row() { 
        std::cout << "Default constructor call." << std::endl;
      }
      
      Row(int* m, int cn) : memstart(m), colsnum(cn) {} 

      int& operator[](int colnum) {
        if (colnum >= this->colsnum)
          throw std::out_of_range("Column number is out of range");
        return *(memstart + colnum);
      }

      const int& operator[](int colnum) const {
        if (colnum >= this->colsnum)
          throw std::out_of_range("Column number is out of range");
        return *(memstart + colnum);
      }
  };

  private:
    int* matmem;
    Matrix::Row* membuf;

    int rowsnum;
    int colsnum;

  public:
    Matrix(int x, int y) {
      this->rowsnum = x;
      this->colsnum = y;
      this->matmem = new int[x * y];

      for (int i = 0; i < (x * y); i++) {
        this->matmem[i] = 0;
      }

      this->membuf = (Matrix::Row*) new char(x * sizeof(Matrix::Row));
      for (int i = 0; i < x; i++) {
        new (this->membuf + i * sizeof(Matrix::Row)) Matrix::Row(&matmem[i * y], y);
      }
    }

    ~Matrix() {
      delete this->membuf;
      delete[] this->matmem;
    }

    Matrix::Row& operator[](int rownum) {
      if (rownum >= this->getRows())
        throw std::out_of_range("Row number is out of range");
      return *(this->membuf + rownum * sizeof(Matrix::Row));
    }

    // можно использовать const_cast
    const Matrix::Row& operator[](int rownum) const {
      if (rownum >= this->getRows())
        throw std::out_of_range("Row number is out of range");
      const Matrix::Row& mr = *(this->membuf + rownum * sizeof(Matrix::Row));
      return mr;
    }

    Matrix& operator*=(const int mult)
    {
      for (int rc = 0; rc < this->GetFullLen(); rc++) {
        this->matmem[rc] *= mult;
      }    
      return *this;
    }

    bool operator==(const Matrix& other) const
    {
      bool res = true;

      if ((other.getRows() != this->getRows()) | (other.getColumns() != this->getColumns()))
      {
        res = false;
      }
      else
      {
        res = (memcmp(this->matmem, other.matmem, this->GetFullLen() * sizeof(int)) == 0);
      }
      return res;
    }

    bool operator!=(const Matrix& other) const
    {
      return !(*this == other);
    }

    int getRows() const {
      return this->rowsnum;
    }

    int getColumns() const {
      return this->colsnum;
    }

    int GetFullLen() const {
      return this->rowsnum * this->colsnum;
    }

    void DbgPrint() {
      for (int rc = 0; rc < (this->rowsnum * this->colsnum); rc++) {
          std::cout << matmem[rc] << " ";
      }
      std::cout << std::endl;
    }

    void Print() {
      for (int r = 0; r < this->rowsnum; r++) {
        for (int c = 0; c < this->colsnum; c++) {
          std::cout << matmem[(r * this->colsnum) + c] << " ";
        }
        std::cout << std::endl;
      }
    }
};
