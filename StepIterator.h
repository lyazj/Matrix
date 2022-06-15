#pragma once

#include "Basic.h"

class StepIterator {
  friend class StepIteratorTest;
private:
  Number  *data;  // 当前位置
  size_t  step;   // 跳步大小

public:
  // 从数据区位置和跳步大小创建跳步迭代器
  StepIterator(Number *, size_t);

  // 默认构造函数和三类拷贝控制函数
  StepIterator() = default;  // 未初始化
  ~StepIterator() = default;
  StepIterator(const StepIterator &) = default;
  StepIterator &operator=(const StepIterator &) = default;

  // 前置增减
  StepIterator &operator++();
  StepIterator &operator--();

  // 后置增减
  StepIterator operator++(int);
  StepIterator operator--(int);

  // 解引用
  Number &operator*() const;

  // 全序比较
  bool operator==(const StepIterator &) const;
  bool operator!=(const StepIterator &) const;
  bool operator<(const StepIterator &) const;
  bool operator>(const StepIterator &) const;
  bool operator<=(const StepIterator &) const;
  bool operator>=(const StepIterator &) const;

  // 随机跳步
  StepIterator &operator+=(ptrdiff_t);
  StepIterator &operator-=(ptrdiff_t);
  StepIterator operator+(ptrdiff_t) const;
  StepIterator operator-(ptrdiff_t) const;
  Number &operator[](size_t) const;

  // 步差计算
  ptrdiff_t operator-(const StepIterator &) const;
};

inline StepIterator::StepIterator(Number *d, size_t s) : data(d), step(s)
{
  // empty function body
}

inline StepIterator &StepIterator::operator++()
{
  data += step;
  return *this;
}

inline StepIterator StepIterator::operator++(int)
{
  StepIterator iter(*this);
  ++*this;
  return iter;
}

inline Number &StepIterator::operator*() const
{
  return *data;
}

inline bool StepIterator::operator==(const StepIterator &iter) const
{
  return data == iter.data;
}

inline bool StepIterator::operator!=(const StepIterator &iter) const
{
  return !(*this == iter);
}

inline StepIterator &StepIterator::operator--()
{
  data -= step;
  return *this;
}

inline StepIterator StepIterator::operator--(int)
{
  StepIterator iter(*this);
  --*this;
  return iter;
}

inline StepIterator &StepIterator::operator+=(ptrdiff_t diff)
{
  data += step * diff;
  return *this;
}

inline StepIterator &StepIterator::operator-=(ptrdiff_t diff)
{
  data -= step * diff;
  return *this;
}

inline StepIterator StepIterator::operator+(ptrdiff_t diff) const
{
  return StepIterator(*this) += diff;
}

inline StepIterator StepIterator::operator-(ptrdiff_t diff) const
{
  return StepIterator(*this) -= diff;
}

inline bool StepIterator::operator<(const StepIterator &iter) const
{
  return data < iter.data;
}

inline bool StepIterator::operator<=(const StepIterator &iter) const
{
  return *this < iter || *this == iter;
}

inline bool StepIterator::operator>(const StepIterator &iter) const
{
  return !(*this <= iter);
}

inline bool StepIterator::operator>=(const StepIterator &iter) const
{
  return !(*this < iter);
}

inline Number &StepIterator::operator[](size_t i) const
{
  return data[i * step];
}

inline ptrdiff_t StepIterator::operator-(const StepIterator &iter) const
{
  return (data - iter.data) / step;
}
