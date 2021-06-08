#include <suffix_array.hpp>
#include <string>
#include <vector>
#include <iostream>

void expect_equal_sa(const std::string& t, const std::vector<int>& expected) {
  using std::cout;
  using std::endl;
  cout << "Check " << t << " " << endl << " ... ";
  auto sa = sal::SuffixArray(t.begin(), t.end());
  if (sa == expected) {
    cout << "OK" << endl;
  } else {
    cout << "NG" << endl;
    cout << "expect" << endl;
    for (int c : expected)
      cout << c << ' ';
    cout << endl;
    cout << "get" << endl;
    for (int c : sa)
      cout << c << ' ';
    cout << endl;
  }
}

int main() {
  expect_equal_sa("abracadabra",
                  {11,10,7,0,3,5,8,1,4,6,9,2});
  expect_equal_sa("abababab",
                  {8, 6, 4, 2, 0, 7, 5, 3, 1});
  expect_equal_sa("cacbcacba",
                  {9, 8, 5, 1, 7, 3, 4, 0, 6, 2});
  expect_equal_sa("mmiissiissiippii",
                  {16, 15, 14, 10, 6, 2, 11, 7, 3, 1, 0, 13, 12, 9, 5, 8, 4});
}
