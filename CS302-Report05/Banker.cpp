#include <iostream>
#include <map>
#include <string>
#include <vector>

std::vector<int> args;                       // input resource values
std::vector<int> totalAva;                   // total resource remaining
std::vector<int> totalCap;                   // total resource values
std::map<int, std::vector<int>> needed;      // each process request remaining
std::map<int, std::vector<int>> procCapMap;  // each process max request
std::map<int, std::vector<int>> procOccMap;  // each process occupation

bool HandleNew(int r, int pid);
bool HandleRequest(int r, int pid);
bool SafeCheck(int r, std::map<int, std::vector<int>> &tmpOccMap,
               std::map<int, std::vector<int>> &tmpNeeded);

int main() {
  int r;
  std::cin >> r;
  for (int i = 0; i < r; ++i) {
    int tmp;
    std::cin >> tmp;
    totalAva.push_back(tmp);
    totalCap.push_back(tmp);
    args.push_back(0);
  }
  int pid;
  std::string cmd;
  while (std::cin >> pid >> cmd) {
    bool okay = false;
    if (!cmd.compare("terminate") && procCapMap.find(pid) != procCapMap.end()) {
      for (int i = 0; i < r; ++i) totalAva[i] += procOccMap[pid][i];
      needed.erase(pid);
      procCapMap.erase(pid);
      procOccMap.erase(pid);
      okay = true;
    } else if (!cmd.compare("new")) {
      for (int i = 0; i < r; ++i) std::cin >> args[i];
      okay = HandleNew(r, pid);
    } else if (!cmd.compare("request")) {
      for (int i = 0; i < r; ++i) std::cin >> args[i];
      okay = HandleRequest(r, pid);
    }
    std::string msg = okay ? "OK" : "NOT OK";
    std::cout << msg << std::endl;
  }
}

bool HandleNew(int r, int pid) {
  if (procCapMap.find(pid) == procCapMap.end()) {
    for (int i = 0; i < r; ++i)
      if (args[i] > totalCap[i]) return false;
    if (needed.find(pid) != needed.end()) needed.erase(pid);
    if (procCapMap.find(pid) != needed.end()) procCapMap.erase(pid);
    if (procOccMap.find(pid) != needed.end()) procOccMap.erase(pid);
    for (int i = 0; i < r; ++i) {
      needed[pid].push_back(args[i]);
      procCapMap[pid].push_back(args[i]);
      procOccMap[pid].push_back(0);
    }
  }
  return true;
}

bool HandleRequest(int r, int pid) {
  std::map<int, std::vector<int>> tmpNeeded;
  std::map<int, std::vector<int>> tmpOccMap;
  for (int i = 0; i < r; ++i)
    if (args[i] > totalAva[i] ||
        args[i] + procOccMap[pid][i] > procCapMap[pid][i])
      return false;
  for (int i = 0; i < r; ++i) {
    tmpNeeded[pid].push_back(needed[pid][i] - args[i]);
    tmpOccMap[pid].push_back(procOccMap[pid][i] + args[i]);
  }
  for (auto &itr : needed) {
    int id = itr.first;
    if (id == pid) continue;
    for (int i = 0; i < r; ++i) {
      tmpNeeded[id].push_back(needed[id][i]);
      tmpOccMap[id].push_back(procOccMap[id][i]);
    }
  }
  return SafeCheck(r, tmpOccMap, tmpNeeded);
}

bool SafeCheck(int r, std::map<int, std::vector<int>> &tmpOccMap,
               std::map<int, std::vector<int>> &tmpNeeded) {
  std::map<int, bool> finish;
  for (auto &itr : tmpNeeded) finish[itr.first] = false;
  std::vector<int> totalLeft;
  for (int i = 0; i < r; ++i) totalLeft.push_back(totalAva[i] - args[i]);
  for (size_t i = 0; i < tmpNeeded.size(); ++i) {
    for (auto &itr : tmpNeeded) {
      int id = itr.first;
      if (!finish[id]) {
        bool flag = true;
        for (int j = 0; j < r; ++j) {
          if (itr.second[j] > totalLeft[j]) {
            flag = false;
            break;
          }
        }
        if (flag) {
          finish[id] = true;
          for (int j = 0; j < r; ++j) totalLeft[j] += tmpOccMap[itr.first][j];
        }
      }
    }
  }
  for (auto &itr : finish)
    if (!itr.second) return false;
  for (int i = 0; i < r; ++i) totalAva[i] -= args[i];
  for (auto itr : tmpNeeded) {
    int id = itr.first;
    needed[id] = tmpNeeded[id];
    procOccMap[id] = tmpOccMap[id];
  }
  return true;
}
