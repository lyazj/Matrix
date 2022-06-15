#pragma once

#include "Basic.h"

class Matrix;

// 选择矩阵首列绝对值最大的元素作为首行主元
// 退回被选中主元原来所在行
// 对空矩阵，抛 domain_error
size_t select_pivot(const Matrix &A);

// 将矩阵化为单位上三角矩阵
// 退回成功转化的行列数目
size_t transform_UUT(const Matrix &A);

// 求解单位上三角方程组
// A 非方阵时抛 domain_error
// A 和 b 行数不等时抛 invalid_argument
void solve_UUT(const Matrix &A, const Matrix &b);

// 高斯约当消元法解线性方程组
// Ab 的行多于列时抛 domain_error
// Ab 中最左最大方阵被视为 A ，其余为 b
// 主元选取失败时抛 size_t 失败行号
void solve_GJ(const Matrix &Ab);
