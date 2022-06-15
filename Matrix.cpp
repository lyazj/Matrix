#include "Matrix.h"
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <string>
#include <algorithm>
#include <new>

Matrix::Matrix(size_t nr, size_t nc)
{
  if(nr > nmax)
    throw std::out_of_range("row number exceeded");
  if(nc > nmax)
    throw std::out_of_range("column number exceeded");

  size_t data_size = nr * nc * sizeof(Number);
  size_t malloc_size = data_size + sizeof(size_t);
  refc = (size_t *)malloc(malloc_size);
  if(!refc)
    throw std::bad_alloc();

  data = (Number *)&refc[1];
  *refc = 1;
  nrow = nr;
  ncol = nc;
  srow = nc;
  scol = 1;
}

Matrix::~Matrix()
{
  if(!--*refc)
    free(refc);
}

Matrix::Matrix(const Matrix &matrix)
{
  ++*matrix.refc;
  data = matrix.data;
  refc = matrix.refc;
  nrow = matrix.nrow;
  ncol = matrix.ncol;
  srow = matrix.srow;
  scol = matrix.scol;
}

Matrix &Matrix::operator=(const Matrix &matrix)
{
  return (Matrix &)(*(const Matrix *)this = matrix);
}

const Matrix &Matrix::operator=(const Matrix &matrix) const
{
  if(nr() != matrix.nr() || nc() != matrix.nc())
    throw std::domain_error("inconsistent shapes");
  for(size_t i = 0; i < nr(); ++i)
  {
    StepIterator Ai = (*this)[i];
    StepIterator Bi = matrix[i];
    while(Ai != row_end(i))
      *Ai++ = *Bi++;
  }
  return *this;
}

void Matrix::reset(const Matrix &matrix)
{
  if(refc == matrix.refc)
    memcpy(this, &matrix, sizeof *this);
  else
  {
    this->~Matrix();
    new((void *)this) Matrix(matrix);
  }
}

Matrix Matrix::copy() const
{
  Matrix matrix(nr(), nc());
  return matrix = *this;
}

void Matrix::fill(Number v) const
{
  for(size_t i = 0; i < nr(); ++i)
  {
    StepIterator iter = row_begin(i);
    while(iter != row_end(i))
      *iter++ = v;
  }
}

std::ostream &operator<<(std::ostream &os, const Matrix &matrix)
{
  size_t nr = matrix.nr();
  size_t nc = matrix.nc();
  std::string space_fill(12 * std::max(nc, (size_t)1) - 1, '-');
  os << "+-" << space_fill << "-+\n";
  if(nr) for(size_t i = 0; i < nr; ++i)
  {
    os << "|";
    if(nc) for(size_t j = 0; j < nc; ++j)
      os << ' ' << std::setw(11) << matrix(i, j);
    else
      os << " (empty-row)";
    os << " |\n";
  }
  else
  {
    os << "|";
    if(nc) for(size_t j = 0; j < nc; ++j)
      os << " (empty-clm)";
    else
      os << " (empty-mtx)";
    os << " |\n";
  }
  os << "+-" << space_fill << "-+";
  return os;
}

Matrix operator*(const Matrix &A, const Matrix &B)
{
  if(A.nc() != B.nr())
    throw std::domain_error("inconsistent shapes");
  Matrix C(A.nr(), B.nc());
  // for(size_t i = 0; i < C.nr(); ++i)
  //   for(size_t j = 0; j < C.nc(); ++j)
  //   {
  //     C(i, j) = 0;
  //     for(size_t k = 0; k < A.nc(); ++k)
  //       C(i, j) += A(i, k) * B(k, j);
  //   }
  for(size_t i = 0; i < C.nr(); ++i)
  {
    StepIterator Ci = C.row_begin(i);
    for(size_t j = 0; j < C.nc(); ++j)
    {
      Number c = 0;
      StepIterator Ai = A.row_begin(i);
      StepIterator Bj = B.col_begin(j);
      while(Ai != A.row_end(i))
        c += *Ai++ * *Bj++;
      *Ci++ = c;
    }
  }
  return C;
}

bool operator==(const Matrix &A, const Matrix &B)
{
  if(A.nr() != B.nr() || A.nc() != B.nc())
    throw std::domain_error("inconsistent shapes");
  for(size_t i = 0; i < A.nr(); ++i)
  {
    StepIterator Ai = A[i];
    StepIterator Bi = B[i];
    while(Ai != A.row_end(i))
      if(!(*Ai++ == *Bi++))
        return false;
  }
  return true;
}

Matrix Matrix::row_slice(size_t i1, size_t i2) const
{
  if(i1 > i2)
    throw std::invalid_argument("invalid row interval");
  if(i2 > nr())
    throw std::out_of_range("row index exceeded");
  Matrix matrix(*this);
  matrix.data = &matrix[i1][0];
  matrix.nrow = i2 - i1;
  return matrix;
}

Matrix Matrix::col_slice(size_t j1, size_t j2) const
{
  if(j1 > j2)
    throw std::invalid_argument("invalid column interval");
  if(j2 > nc())
    throw std::out_of_range("column index exceeded");
  Matrix matrix(*this);
  matrix.data = &matrix[0][j1];
  matrix.ncol = j2 - j1;
  return matrix;
}

Matrix Matrix::slice(size_t i1, size_t i2, size_t j1, size_t j2) const
{
  if(i1 > i2 || j1 > j2)
    throw std::invalid_argument("invalid row interval");
  if(i2 > nr() || j2 > nc())
    throw std::out_of_range("row index exceeded");
  Matrix matrix(*this);
  matrix.data = &matrix[i1][j1];
  matrix.nrow = i2 - i1;
  matrix.ncol = j2 - j1;
  return matrix;
}

void Matrix::row_swap(size_t i1, size_t i2) const
{
  StepIterator iter1 = row_begin(i1);
  StepIterator iter2 = row_begin(i2);
  while(iter1 != row_end(i1))
    std::iter_swap(iter1++, iter2++);
}

void Matrix::col_swap(size_t j1, size_t j2) const
{
  StepIterator iter1 = col_begin(j1);
  StepIterator iter2 = col_begin(j2);
  while(iter1 != col_end(j1))
    std::iter_swap(iter1++, iter2++);
}

Matrix Matrix::t() const
{
  Matrix matrix(*this);
  std::swap(matrix.nrow, matrix.ncol);
  std::swap(matrix.srow, matrix.scol);
  return matrix;
}

Matrix Matrix::operator+() const
{
  Matrix matrix(nr(), nc());
  for(size_t i = 0; i < nr(); ++i)
  {
    StepIterator w_it = matrix.row_begin(i);
    StepIterator r_it = row_begin(i);
    while(w_it != matrix.row_end(i))
      *w_it++ = +*r_it++;
  }
  return matrix;
}

Matrix Matrix::operator-() const
{
  Matrix matrix(nr(), nc());
  for(size_t i = 0; i < nr(); ++i)
  {
    StepIterator w_it = matrix.row_begin(i);
    StepIterator r_it = row_begin(i);
    while(w_it != matrix.row_end(i))
      *w_it++ = -*r_it++;
  }
  return matrix;
}

const Matrix &Matrix::operator+=(const Matrix &matrix) const
{
  for(size_t i = 0; i < nr(); ++i)
  {
    StepIterator w_it = row_begin(i);
    StepIterator r_it = matrix.row_begin(i);
    while(w_it != row_end(i))
      *w_it++ += *r_it++;
  }
  return *this;
}

const Matrix &Matrix::operator-=(const Matrix &matrix) const 
{
  for(size_t i = 0; i < nr(); ++i)
  {
    StepIterator w_it = row_begin(i);
    StepIterator r_it = matrix.row_begin(i);
    while(w_it != row_end(i))
      *w_it++ -= *r_it++;
  }
  return *this;
}

const Matrix &Matrix::operator*=(Number k) const 
{
  for(size_t i = 0; i < nr(); ++i)
  {
    StepIterator w_it = row_begin(i);
    while(w_it != row_end(i))
      *w_it++ *= k;
  }
  return *this;
}

const Matrix &Matrix::operator/=(Number k) const 
{
  for(size_t i = 0; i < nr(); ++i)
  {
    StepIterator w_it = row_begin(i);
    while(w_it != row_end(i))
      *w_it++ /= k;
  }
  return *this;
}

Matrix operator+(const Matrix &A, const Matrix &B)
{
  return A.copy() += B;
}

Matrix operator-(const Matrix &A, const Matrix &B)
{
  return A.copy() -= B;
}

Matrix operator*(const Matrix &A, Number k)
{
  return A.copy() *= k;
}

Matrix operator/(const Matrix &A, Number k)
{
  return A.copy() /= k;
}

void Matrix::print() const
{
  std::cout << *this << std::endl;
}
