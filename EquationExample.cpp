#include "Equation.h"
#include "Matrix.h"
#include <iostream>

using namespace std;

int main()
{
  double data[2][3] = {
    2, 3, 11,
    3, 1, 6,
  };
  Matrix Ab(2, 3, data);
  cout << "求解方程：" << endl;
  cout << Ab << endl;
  solve_GJ(Ab);
  cout << "解得：" << endl;
  cout << Ab << endl;
}
