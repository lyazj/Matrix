#include "TestBasic.h"
#include "Equation.h"
#include "Matrix.h"
#include <random>
#include <ctime>
#include <iomanip>

using namespace std;

void test_select_pivot();
void test_transform_UUT();
void test_solve_UUT();
void test_solve_GJ();
void test_solve_GJ_repeat();

int main()
{
  test_select_pivot();
  test_transform_UUT();
  test_solve_UUT();
  test_solve_GJ();
  test_solve_GJ_repeat();
}

void test_select_pivot()
{
  ASSERT_EXCEPTION(domain_error, select_pivot(Matrix(0, 0));)
  ASSERT_EXCEPTION(domain_error, select_pivot(Matrix(1, 0));)
  ASSERT_EXCEPTION(domain_error, select_pivot(Matrix(0, 1));)

  {
    double in[3][3] = {
      1, 1, 1,
      0, 0, 0,
      -2, -2, -2,
    };
    double out[3][3] = {
      -2, -2, -2,
      0, 0, 0,
      1, 1, 1,
    };
    Matrix A(3, 3, in);
    assert(select_pivot(A) == 2);
    assert(A == Matrix(3, 3, out));
  }

  {
    double in[3][3] = {
      1, 2, 3,
      0, 0, 0,
      1, 4, 5,
    };
    double out[3][3] = {
      1, 2, 3,
      0, 0, 0,
      1, 4, 5,
    };
    Matrix A(3, 3, in);
    assert(select_pivot(A) == 0);
    assert(A == Matrix(3, 3, out));
  }

  TEST_PASSED;
}

void test_transform_UUT()
{
  transform_UUT(Matrix(0, 0));

  {
    double in[1][1] = {8};
    double out[1][1] = {1};
    Matrix A(1, 1, in);
    assert(transform_UUT(A) == 1);
    assert(A == Matrix(1, 1, out));
  }

  {
    double in[2][3] = {
      1, 2, 16,
      4, 8, 16,
    };
    double out[2][3] = {
      1, 2,  4,
      0, 0, 12,
    };
    Matrix A(2, 3, in);
    assert(transform_UUT(A) == 1);
    cout << A << endl;
    assert(A == Matrix(2, 3, out));
  }

  {
    double in[2][3] = {
      1, 2, 24,
      2, 6, 12,
    };
    double out[2][3] = {
      1, 3,   6,
      0, 1, -18,
    };
    Matrix A(2, 3, in);
    assert(transform_UUT(A) == 2);
    cout << A << endl;
    assert(A == Matrix(2, 3, out));
  }

  {
    double in[2][3] = {
      1, 2, 3,
      2, 3, 5,
    };
    double out[2][3] = {
      1, 1.5, 2.5,
      0,   1,   1,
    };
    Matrix A(2, 3, in);
    assert(transform_UUT(A) == 2);
    cout << A << endl;
    assert(A == Matrix(2, 3, out));
  }

  TEST_PASSED;
}

void test_solve_UUT()
{
  ASSERT_EXCEPTION(domain_error, solve_UUT(Matrix(0, 1), Matrix(2, 3));)
  ASSERT_EXCEPTION(invalid_argument, solve_UUT(Matrix(1, 1), Matrix(2, 3));)
  solve_UUT(Matrix(0, 0), Matrix(0, 3));

  {
    double in[2][3] = {
      1, 2, 3,
      0, 1, 4,
    };
    double out[2][3] = {
      1, 0, -5,
      0, 1,  4,
    };
    Matrix A(2, 3, in);
    solve_UUT(A.col_slice(0, 2), A.col_slice(2));
    cout << A << endl;
    assert(A == Matrix(2, 3, out));
  }

  TEST_PASSED;
}

void test_solve_GJ()
{
  solve_GJ(Matrix(0, 0));
  ASSERT_EXCEPTION(domain_error, solve_GJ(Matrix(1, 0));)

  {
    double in[2][3] = {
      1, 2, 3,
      2, 3, 5,
    };
    double out[2][3] = {
      1, 0, 1,
      0, 1, 1,
    };
    Matrix A(2, 3, in);
    solve_GJ(A);
    cout << A << endl;
    assert(A == Matrix(2, 3, out));
  }

  TEST_PASSED;
}

void test_solve_GJ_repeat()
{
  time_t seed = time(NULL);
  cout << "Use seed: " << seed << endl;
  default_random_engine e;
  e.seed(seed);
  uniform_real_distribution urd;

  for(int i = 0; i < 10000; ++i)
  {
    Matrix At(3, 4);
    for(size_t j = 0; j < 12; ++j)
      (&At[0][0])[j] = urd(e);
    Matrix A = At.col_slice(0, 3).copy();
    Matrix b = At.col_slice(3).copy();
    solve_GJ(At);
    Matrix x = At.col_slice(3);
    Matrix err = b - A * x;
    assert((err.t() * err)[0][0] < 1e-20);
  }
  cout << "Solved 10000 3-variable linear equations." << endl;

  TEST_PASSED;
}
