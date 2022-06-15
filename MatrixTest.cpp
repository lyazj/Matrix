#include "TestBasic.h"
#include "Matrix.h"
#include <cstdint>
#include <type_traits>
#include <vector>
#include <deque>
#include <ctime>
#include <cstdlib>

using namespace std;

// 为部分内联函数生成代码，检查汇编
Number &at_sub(const Matrix &matrix, size_t i, size_t j)
{
  return matrix[i][j];
}

// 为部分内联函数生成代码，检查汇编
Number &at_par(const Matrix &matrix, size_t i, size_t j)
{
  return matrix(i, j);
}

// 为部分模板函数生成代码，检查汇编
template
const Matrix &Matrix::operator=(const int (&)[3][3]) const;

class MatrixTest {
private:
  Matrix get_Matrix_3_3() const;
  void test_Number() const;
  void test_Matrix_nmax() const;
  void test_Matrix_nr_nc() const;
  void test_Matrix_init(const Matrix &, size_t nr, size_t nc) const;
  void test_Matrix_Matrix() const;
  void test_Matrix_Matrix_repeat() const;
  void test_Matrix_reset() const;
  void test_Matrix_assign() const;
  void test_Matrix_assign_repeat() const;
  void test_Matrix_copy() const;
  void test_Matrix_elem() const;
  void test_Matrix_3_3_repeat() const;
  void test_Matrix_ostream() const;
  void test_Matrix_slice() const;
  void test_Matrix_swap() const;
  void test_Matrix_dot() const;
  void test_Matrix_dot_repeat() const;
  void test_Matrix_pn() const;
  void test_Matrix_cplus_cminus() const;
  void test_Matrix_cmultiplies() const;
public:
  void test() const;
};

int main()
{
  MatrixTest().test();
}

void MatrixTest::test() const
{
  test_Number();
  test_Matrix_nmax();
  test_Matrix_nr_nc();
  test_Matrix_Matrix();
  test_Matrix_Matrix_repeat();
  test_Matrix_reset();
  test_Matrix_assign();
  test_Matrix_assign_repeat();
  test_Matrix_copy();
  test_Matrix_elem();
  test_Matrix_3_3_repeat();
  test_Matrix_ostream();
  test_Matrix_slice();
  test_Matrix_swap();
  test_Matrix_dot();
  test_Matrix_dot_repeat();
  test_Matrix_pn();
  test_Matrix_cplus_cminus();
  test_Matrix_cmultiplies();
}

Matrix MatrixTest::get_Matrix_3_3() const
{
  double data[3][3] = {
    1, 2, 3, 4, 5, 6, 7, 8, 9
  };
  return Matrix (3, 3, data);
}

void MatrixTest::test_Number() const
{
  static_assert(is_trivial_v<Number>);
  TEST_PASSED;
}

void MatrixTest::test_Matrix_nmax() const
{
  // 需要保证
  //   nmax * nmax * sizeof(Number) + sizeof(size_t)
  // 不溢出
  uint64_t n = Matrix::nmax, size = ~sizeof(size_t);
  assert(n < ((uint64_t)1 << 32));
  assert(n * n <= size / sizeof(Number));
  TEST_PASSED;
}

void MatrixTest::test_Matrix_nr_nc() const
{
  size_t nmax = Matrix::nmax;

  // 测试行数和列数极限值
  test_Matrix_init(Matrix(0, 0), 0, 0);
  test_Matrix_init(Matrix(0, nmax), 0, nmax);
  test_Matrix_init(Matrix(nmax, 0), nmax, 0);
  test_Matrix_init(Matrix(nmax, nmax), nmax, nmax);

  // 测试大于 nmax 的行数或列数
  ASSERT_EXCEPTION(out_of_range, Matrix(nmax + 1, 0);)
  ASSERT_EXCEPTION(out_of_range, Matrix(0, nmax + 1);)
  ASSERT_EXCEPTION(out_of_range, Matrix(nmax + 1, nmax + 1);)

  // 测试内存分配失败时的行为
  ASSERT_EXCEPTION(bad_alloc,
    vector<Matrix> matrices;
    for(;;)
    {
      matrices.emplace_back(nmax, nmax);
      test_Matrix_init(matrices.back(), nmax, nmax);
    }
  )

  TEST_PASSED;
}

void MatrixTest::test_Matrix_init(
    const Matrix &matrix, size_t nr, size_t nc) const
{
  assert(matrix.refc);
  assert((void *)matrix.data == (void *)&matrix.refc[1]);
  assert(*matrix.refc == 1);
  assert(matrix.ncol == matrix.srow);
  assert(matrix.scol == 1);
  assert(matrix.nrow == nr);
  assert(matrix.ncol == nc);
}

void MatrixTest::test_Matrix_Matrix() const
{
  size_t refc_max = 32;
  Matrix matrix(0, 0);
  deque<Matrix> matrices;
  for(size_t i = 1; i < refc_max; ++i)
  {
    matrices.push_back(matrix);
    assert(*matrix.refc == i + 1);
  }
  for(size_t i = refc_max - 1; i; --i)
  {
    matrices.pop_front();
    assert(*matrix.refc == i);
  }
  TEST_PASSED;
}

void MatrixTest::test_Matrix_Matrix_repeat() const
{
  Matrix matrix = get_Matrix_3_3();
  vector<Matrix> matrices;
  matrices.reserve(1000000);
  clock_t start = clock();
  for(int i = 0; i < 1000000; ++i)
    matrices.emplace_back(matrix);
  clock_t diff = clock() - start;
  cout << "It took " << (double)diff / CLOCKS_PER_SEC
       << " s to duplicate 1000000 3x3 matrix." << endl;
  start = clock();
  matrices.clear();
  diff = clock() - start;
  cout << "It took " << (double)diff / CLOCKS_PER_SEC
       << " s to destroy 1000000 3x3 matrix." << endl;
  TEST_PASSED;
}

void MatrixTest::test_Matrix_reset() const
{
  Matrix A = get_Matrix_3_3();
  Matrix B = A;
  assert(A.refc == B.refc);
  assert(*A.refc == 2);
  A.reset(A);
  assert(A.refc == B.refc);
  assert(*A.refc == 2);
  A.reset(get_Matrix_3_3());
  assert(A.refc != B.refc);
  assert(*A.refc == 1);
  A.reset(B);
  assert(A.refc == B.refc);
  assert(*A.refc == 2);
  TEST_PASSED;
}

void MatrixTest::test_Matrix_assign() const
{
  Matrix(0, 0) = Matrix(0, 0);
  ASSERT_EXCEPTION(domain_error, Matrix(1, 2) = Matrix(3, 2);)
  ASSERT_EXCEPTION(domain_error, Matrix(1, 2) = Matrix(1, 3);)
  ASSERT_EXCEPTION(domain_error, Matrix(1, 2) = Matrix(3, 4);)
  Matrix A(3, 3);
  srand(time(NULL));
  for(int t = 0; t < 10000; ++t)
  {
    for(size_t i = 0; i < A.nr(); ++i)
    {
      StepIterator iter = A.row_begin(i);
      while(iter != A.row_end(i))
        *iter++ = rand();
    }
    Matrix B(3, 3);
    B = A;
    assert(A == B);
  }
  TEST_PASSED;
}

void MatrixTest::test_Matrix_assign_repeat() const
{
  Matrix A = get_Matrix_3_3(), B = A.copy();
  clock_t start = clock();
  for(int i = 0; i < 1000000; ++i)
    B = A;
  clock_t diff = clock() - start;
  cout << "It took " << (double)diff / CLOCKS_PER_SEC
       << " s to assign to 1000000 3x3 matrices." << endl;
  TEST_PASSED;
}

void MatrixTest::test_Matrix_copy() const
{
  Matrix A1 = get_Matrix_3_3();
  Matrix A2 = A1.copy();
  assert(A1.refc != A2.refc);
  assert(*A1.refc == 1);
  assert(*A2.refc == 1);
  assert(A1 == A2);
  TEST_PASSED;
}

void MatrixTest::test_Matrix_elem() const
{
  size_t nrow = 16;
  size_t ncol = 64;
  Matrix matrix(nrow, ncol);
  for(size_t i = 0; i < nrow; ++i)
  {
    StepIterator iter = matrix.row_begin(i);
    for(size_t j = 0; j < ncol; ++j)
    {
      assert(&matrix[i][j] == &matrix(0, 0) + i * ncol + j);
      assert(&matrix(i, j) == &matrix(0, 0) + i * ncol + j);
      assert(iter < matrix.row_end(i));
      assert(&*iter == &matrix(0, 0) + i * ncol + j);
      ++iter;
    }
    assert(iter == matrix.row_end(i));
  }
  for(size_t j = 0; j < ncol; ++j)
  {
    StepIterator iter = matrix.col_begin(j);
    for(size_t i = 0; i < nrow; ++i)
    {
      assert(iter < matrix.col_end(j));
      assert(&*iter == &matrix(0, 0) + i * ncol + j);
      ++iter;
    }
    assert(iter == matrix.col_end(j));
  }
  TEST_PASSED;
}

void MatrixTest::test_Matrix_3_3_repeat() const
{
  clock_t start = clock();
  for(int i = 0; i < 100000; ++i)
    get_Matrix_3_3();
  clock_t diff = clock() - start;
  cout << "It took " << (double)diff / CLOCKS_PER_SEC
       << " s to create 100000 3x3 matrix." << endl;
  TEST_PASSED;
}

void MatrixTest::test_Matrix_ostream() const
{
  cout << Matrix(0, 0) << endl;
  cout << Matrix(0, 2) << endl;
  cout << Matrix(2, 0) << endl;
  cout << get_Matrix_3_3() << endl;
  TEST_PASSED;
}

void MatrixTest::test_Matrix_slice() const
{
  Matrix A = get_Matrix_3_3();
  for(size_t i1 = 0; i1 <= A.nr() + 1; ++i1)
  for(size_t i2 = 0; i2 <= A.nr() + 1; ++i2)
  for(size_t j1 = 0; j1 <= A.nc() + 1; ++j1)
  for(size_t j2 = 0; j2 <= A.nc() + 1; ++j2)
  {
    if(i1 > i2 || j1 > j2)
    {
      ASSERT_EXCEPTION(invalid_argument, A.slice(i1, i2, j1, j2);)
        continue;
    }
    if(i2 > A.nr() || j2 > A.nc())
    {
      ASSERT_EXCEPTION(out_of_range, A.slice(i1, i2, j1, j2);)
        continue;
    }
    Matrix As = A.slice(i1, i2, j1, j2);
    assert(&As[0][0] == &A[i1][j1]);
    assert(As.nr() == i2 - i1);
    assert(As.nc() == j2 - j1);
    assert(As == A.row_slice(i1, i2).col_slice(j1, j2));
    assert(As == A.col_slice(j1, j2).row_slice(i1, i2));
  }
  TEST_PASSED;
}

void MatrixTest::test_Matrix_swap() const
{
  Matrix A = get_Matrix_3_3(), B = A.copy();
  for(size_t i1 = 0; i1 < A.nr(); ++i1)
  for(size_t i2 = 0; i2 < A.nr(); ++i2)
  {
    A.row_swap(i1, i2);
    assert(A.row(i1) == B.row(i2));
    assert(A.row(i2) == B.row(i1));
    A.row_swap(i1, i2);
    assert(A.row(i1) == B.row(i1));
    assert(A.row(i2) == B.row(i2));
  }
  for(size_t j1 = 0; j1 < A.nc(); ++j1)
  for(size_t j2 = 0; j2 < A.nc(); ++j2)
  {
    A.col_swap(j1, j2);
    assert(A.col(j1) == B.col(j2));
    assert(A.col(j2) == B.col(j1));
    A.col_swap(j1, j2);
    assert(A.col(j1) == B.col(j1));
    assert(A.col(j2) == B.col(j2));
  }
  TEST_PASSED;
}

void MatrixTest::test_Matrix_dot() const
{
  Matrix A = get_Matrix_3_3();
  Matrix A2(A.nr(), A.nc());
  for(size_t i = 0; i < A.nr(); ++i)
    for(size_t j = 0; j < A.nc(); ++j)
    {
      A2[i][j] = 0;
      for(size_t k = 0; k < A.nc(); ++k)
        A2[i][j] += A[i][k] * A[k][j];
    }
  Matrix AdotA = A * A;
  assert(AdotA == A2);
  cout << AdotA << endl;
  TEST_PASSED;
}

void MatrixTest::test_Matrix_dot_repeat() const
{
  Matrix A = get_Matrix_3_3();
  clock_t start = clock();
  for(int i = 0; i < 10000; ++i)
    A * A;
  clock_t diff = clock() - start;
  cout << "It took " << (double)diff / CLOCKS_PER_SEC
       << " s to dot two 3x3 matrices 10000 times." << endl;
  TEST_PASSED;
}

void MatrixTest::test_Matrix_pn() const
{
  Matrix A = get_Matrix_3_3();

  Matrix pA = +A;
  assert(A.refc != pA.refc);
  assert(*A.refc == 1);
  assert(*pA.refc == 1);
  assert(pA == A);
  cout << pA << endl;

  Matrix nA = -A;
  assert(A.refc != nA.refc);
  assert(*A.refc == 1);
  assert(*nA.refc == 1);
  assert(-nA == A);
  cout << nA << endl;

  TEST_PASSED;
}

void MatrixTest::test_Matrix_cplus_cminus() const
{
  Matrix A = get_Matrix_3_3(), Ac = A.copy();
  size_t *refc = A.refc;

  A += A;
  assert(A.refc == refc);
  assert(*A.refc == 1);
  for(size_t i = 0; i < A.nr(); ++i)
    for(size_t j = 0; j < A.nc(); ++j)
      assert(A[i][j] == Ac[i][j] + Ac[i][j]);

  A -= Ac;
  assert(A.refc == refc);
  assert(*A.refc == 1);
  assert(A == Ac);

  A += Ac;
  assert(A.refc == refc);
  assert(*A.refc == 1);
  for(size_t i = 0; i < A.nr(); ++i)
    for(size_t j = 0; j < A.nc(); ++j)
      assert(A[i][j] == Ac[i][j] + Ac[i][j]);

  A -= A;
  assert(A.refc == refc);
  assert(*A.refc == 1);
  for(size_t i = 0; i < A.nr(); ++i)
    for(size_t j = 0; j < A.nc(); ++j)
      assert(A[i][j] == 0);

  TEST_PASSED;
}

void MatrixTest::test_Matrix_cmultiplies() const
{
  Matrix A = get_Matrix_3_3(), Ac = A.copy();
  size_t *refc = A.refc;

  A *= 3;
  assert(A.refc == refc);
  assert(*A.refc == 1);
  for(size_t i = 0; i < A.nr(); ++i)
    for(size_t j = 0; j < A.nc(); ++j)
      assert(A[i][j] == Ac[i][j] * 3);

  A /= 6;
  assert(A.refc == refc);
  assert(*A.refc == 1);
  for(size_t i = 0; i < A.nr(); ++i)
    for(size_t j = 0; j < A.nc(); ++j)
      assert(A[i][j] == Ac[i][j] / 2);

  TEST_PASSED;
}
