#include <algorithm>
#include <iostream>
#include <vector>

void CLook(int requests[], int N, int head) {
  std::cout << head << " ";
  std::vector<int> left;   // requests with smaller indexes than head
  std::vector<int> right;  // requests with bigger indexes than head
  for (int i = 0; i < N; ++i)
    requests[i] > head ? right.push_back(requests[i])
                       : left.push_back(requests[i]);
  std::sort(left.begin(), left.end());
  std::sort(right.begin(), right.end());
  int travel = 0;     // total distance that head travelled
  int prePos = head;  // previous position of the current visiting request
  if (left.size() > 0) {
    for (int i = left.size() - 1; i >= 0; --i) {
      std::cout << left[i] << " ";
      int jump = prePos - left[i];
      travel += jump;
      prePos = left[i];
    }
  }
  if (right.size() > 0) {
    std::cout << right[right.size() - 1] << " ";
    travel += right[right.size() - 1] - prePos;
    prePos = right[right.size() - 1];
    for (int i = right.size() - 2; i >= 0; --i) {
      std::cout << right[i] << " ";
      int jump = prePos - right[i];
      travel += jump;
      prePos = right[i];
    }
  }
  std::cout << std::endl << travel << std::endl;
}

int main() {
  int S;  // initial head position
  int M;  // disk track size
  int N;  // requested track count
  std::cin >> S >> M >> N;
  int requests[N];  // requested track position arranged by arrival order
  for (int i = 0; i < N; ++i) std::cin >> requests[i];
  CLook(requests, N, S);
}
