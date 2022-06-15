#pragma once

#include "Basic.h"
#include "StepIterator.h"
#include "IOStream.h"

class Matrix {
  friend class MatrixTest;
private:
  Number  *data;  // 首元素地址
  size_t  *refc;  // 数据区引用计数
  size_t  nrow;   // 行数
  size_t  ncol;   // 列数
  size_t  srow;   // 行跳步
  size_t  scol;   // 列跳步

public:
  static constexpr size_t nmax = (1 << 14) - 1;

  // 行数或列数大于 nmax 时抛 out_of_range
  // 内存分配失败时抛 bad_alloc
  Matrix(size_t nr, size_t nc);
  template<class M>
    Matrix(size_t nr, size_t nc, const M &);
  template<class M, size_t Nr, size_t Nc>
    Matrix(const M (&)[Nr][Nc]);

  // 非虚函数，递减数据区引用计数，归零时释放内存
  ~Matrix();

  // 拷贝构造具有引用语义
  Matrix(const Matrix &);

  // 引用重置
  void reset(const Matrix &);

  // 拷贝赋值具有值语义
  Matrix &operator=(const Matrix &);
  const Matrix &operator=(const Matrix &) const;
  template<class M>
    Matrix &operator=(const M &);
  template<class M>
    const Matrix &operator=(const M &) const;
  void fill(Number) const;

  // 值拷贝
  Matrix copy() const;

  // 获取引用状态
  size_t ref() const { return *refc; }
  bool ref(const Matrix &m) const { return refc == m.refc; }

  // 获取行数和列数
  size_t nr() const { return nrow; }
  size_t nc() const { return ncol; }
  bool empty() const { return !nrow || !ncol; }
  bool square() const { return nrow == ncol; }

  // 行列访问（无越界检查）
  StepIterator row_begin(size_t i) const;
  StepIterator row_end(size_t i) const;
  StepIterator col_begin(size_t j) const;
  StepIterator col_end(size_t j) const;
  StepIterator operator[](size_t i) const;

  // 矩阵元访问（无越界检查）
  Number &operator()(size_t i, size_t j) const;

  // 行列切片
  Matrix row_slice(size_t i1, size_t i2) const;
  Matrix row_slice(size_t i1) const;  // 一切到尾
  Matrix col_slice(size_t j1, size_t j2) const;
  Matrix col_slice(size_t j1) const;  // 一切到尾
  Matrix row(size_t i1) const;  // 切单行
  Matrix col(size_t j1) const;  // 切单列
  Matrix slice(size_t i1, size_t i2, size_t j1, size_t j2) const;
  Matrix slice(size_t ij1, size_t ij2) const;  // 行列同步
  Matrix slice(size_t ij1) const;  // 行列同步一切到尾

  // 行列交换
  void row_swap(size_t i1, size_t i2) const;
  void col_swap(size_t j1, size_t j2) const;

  // 行列转置
  Matrix t() const;

  // 正负号
  Matrix operator+() const;
  Matrix operator-() const;

  // 复合加法
  Matrix &operator+=(const Matrix &);
  const Matrix &operator+=(const Matrix &) const;
  Matrix &operator-=(const Matrix &);
  const Matrix &operator-=(const Matrix &) const;

  // 复合数量乘法
  Matrix &operator*=(Number);
  const Matrix &operator*=(Number) const;
  Matrix &operator/=(Number);
  const Matrix &operator/=(Number) const;

  // 打印矩阵
  void print() const;
};

// 输出矩阵
std::ostream &operator<<(std::ostream &, const Matrix &);

inline StepIterator Matrix::row_begin(size_t i) const
{
  return StepIterator(&data[i * srow], scol);
}

inline StepIterator Matrix::row_end(size_t i) const
{
  return row_begin(i) + ncol;
}

inline StepIterator Matrix::col_begin(size_t j) const
{
  return StepIterator(&data[j * scol], srow);
}

inline StepIterator Matrix::col_end(size_t j) const
{
  return col_begin(j) + nrow;
}

inline StepIterator Matrix::operator[](size_t i) const
{
  return row_begin(i);
}

inline Number &Matrix::operator()(size_t i, size_t j) const
{
  return (*this)[i][j];
}

template<class M>
Matrix::Matrix(size_t nr, size_t nc, const M &m) : Matrix(nr, nc)
{
  *this = m;
}

template<class M, size_t Nr, size_t Nc>
Matrix::Matrix(const M (&m)[Nr][Nc]) : Matrix(Nr, Nc)
{
  *this = m;
}

template<class M>
Matrix &Matrix::operator=(const M &m)
{
  return (Matrix &)(*(const Matrix *)this = m);
}

template<class M>
const Matrix &Matrix::operator=(const M &m) const
{
  // for(size_t i = 0; i < nr(); ++i)
  //   for(size_t j = 0; j < nc(); ++j)
  //     (*this)(i, j) = m[i][j];
  for(size_t i = 0; i < nr(); ++i)
  {
    StepIterator iter = (*this)[i];
    for(size_t j = 0; j < nc(); ++j)
      *iter++ = m[i][j];
  }
  return *this;
}

// 矩阵加法
Matrix operator+(const Matrix &A, const Matrix &B);
Matrix operator-(const Matrix &A, const Matrix &B);

// 矩阵数量乘法
Matrix operator*(const Matrix &A, Number k);
Matrix operator/(const Matrix &A, Number k);
inline Matrix operator*(Number k, const Matrix &A)
{
  return A * k;
}

// 矩阵乘法
Matrix operator*(const Matrix &A, const Matrix &B);

// 矩阵判等
bool operator==(const Matrix &A, const Matrix &B);
inline bool operator!=(const Matrix &A, const Matrix &B)
{
  return !(A == B);
}

// 行切片到尾
inline Matrix Matrix::row_slice(size_t i1) const
{
  return row_slice(i1, nr());
}

// 列切片到尾
inline Matrix Matrix::col_slice(size_t j1) const
{
  return col_slice(j1, nc());
}

// 切单行
inline Matrix Matrix::row(size_t i1) const
{
  return row_slice(i1, i1 + 1);
}

// 切单列
inline Matrix Matrix::col(size_t j1) const
{
  return col_slice(j1, j1 + 1);
}

// 行列同切
inline Matrix Matrix::slice(size_t ij1, size_t ij2) const
{
  return slice(ij1, ij2, ij1, ij2);
}

// 行列同切到尾
inline Matrix Matrix::slice(size_t ij1) const
{
  return slice(ij1, nr(), ij1, nc());
}

// 固定矩阵复合加法
inline Matrix &Matrix::operator+=(const Matrix &matrix)
{
  return (Matrix &)(*(const Matrix *)this += matrix);
}

// 固定矩阵复合减法
inline Matrix &Matrix::operator-=(const Matrix &matrix)
{
  return (Matrix &)(*(const Matrix *)this -= matrix);
}

// 固定矩阵复合数量乘法
inline Matrix &Matrix::operator*=(Number k)
{
  return (Matrix &)(*(const Matrix *)this *= k);
}

// 固定矩阵复合数量除法
inline Matrix &Matrix::operator/=(Number k)
{
  return (Matrix &)(*(const Matrix *)this /= k);
}
