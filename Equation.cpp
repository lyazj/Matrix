#include "Equation.h"
#include "Matrix.h"
#include <stdexcept>
#include <cmath>

using std::abs;

size_t select_pivot(const Matrix &A)
{
  if(A.empty())
    throw std::domain_error("selecting pivot for empty matrix");
  size_t pivot_r = 0;
  StepIterator iter = A.col_begin(0);
  Number pivot = abs(*iter++);
  for(size_t i = 1; i < A.nr(); ++i)
  {
    Number value = abs(*iter++);
    if(value > pivot)
    {
      pivot = value;
      pivot_r = i;
    }
  }
  if(pivot_r)
    A.row_swap(0, pivot_r);
  return pivot_r;
}

size_t transform_UUT(const Matrix &_A)
{
  Matrix A(_A);
  size_t cnt = 0;
  while(!A.empty())
  {
    select_pivot(A);
    if(A[0][0] == 0)
      break;
    A.row(0) /= A[0][0];
    for(size_t i = 1; i < A.nr(); ++i)
      A.row(i) -= A[i][0] * A.row(0);
    ++cnt;
    A.reset(A.slice(1));
  }
  return cnt;
}

void solve_UUT(const Matrix &A, const Matrix &b)
{
  if(!A.square())
    throw std::domain_error("solving non-square equation");
  if(A.nr() != b.nr())
    throw std::invalid_argument("inconsistent A and b");
  for(size_t i = 1; i < A.nr(); ++i)
  {
    Matrix As = A.slice(A.nr() - i - 1, A.nr() - i, A.nc() - i, A.nc());
    Matrix bs = b.row_slice(b.nr() - i, b.nr());
    b.row(b.nr() - i - 1) -= As * bs;
    As.fill(0);
  }
}

void solve_GJ(const Matrix &Ab)
{
  if(Ab.nr() > Ab.nc())
    throw std::domain_error("invalid argumented matrix");
  size_t r = transform_UUT(Ab);
  if(r != Ab.nr())
    throw r;
  Matrix A = Ab.col_slice(0, Ab.nr());
  Matrix b = Ab.col_slice(Ab.nr());
  solve_UUT(A, b);
}
