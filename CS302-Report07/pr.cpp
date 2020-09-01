#include <iostream>
#include <list>
#include <unordered_map>
#include <set>
#include <utility>
#include <vector>

int cacheSize, algorithm, pageNum, hitCount, missCount;
std::vector<std::pair<int, std::string>> algorithms;
std::vector<int> pageQueries;

void FIFO();
void Min();
void LRU();
void Clock();
void SecondChance();

int main() {
  cacheSize = 0;
  algorithm = 0;
  pageNum = 0;
  pageQueries.clear();
  algorithms.clear();
  algorithms.push_back(std::make_pair(0, "FIFO"));
  algorithms.push_back(std::make_pair(1, "LRU"));
  algorithms.push_back(std::make_pair(2, "Min"));
  algorithms.push_back(std::make_pair(3, "Clock"));
  algorithms.push_back(std::make_pair(4, "Second-chance"));
#ifdef _DEBUG
  std::clog << "Enter cache size" << std::endl;
#endif
  std::cin >> cacheSize;
  if (cacheSize <= 0) {
    std::cerr << "Invalid cache size" << std::endl;
    exit(EXIT_FAILURE);
  }
#ifdef _DEBUG
  std::clog << "Successfully set cache size to " << cacheSize << std::endl;
  std::clog << "Enter algorithm of choice:" << std::endl;
  for (int i = 0; i < algorithms.size(); ++i) {
    std::clog << "\t" << algorithms[i].first << ": " << algorithms[i].second
              << std::endl;
  }
#endif
  std::cin >> algorithm;
  if (algorithm < 0 || algorithm >= algorithms.size()) {
    std::cerr << "Invalid algorithm of choice" << std::endl;
    exit(EXIT_FAILURE);
  }
#ifdef _DEBUG
  std::clog << "Successfully select algorithm " << algorithms[algorithm].second
            << std::endl;
  std::clog << "Enter number of page queries" << std::endl;
#endif
  std::cin >> pageNum;
  if (pageNum <= 0) {
    std::cerr << "Invalid page number" << std::endl;
    exit(EXIT_FAILURE);
  }

  for (int i = 0; i < pageNum; ++i) {
    int page;
    std::cin >> page;
    pageQueries.push_back(page);
  }
#ifdef _DEBUG
  std::clog << "Successfully emulate " << pageNum << " page queries"
            << std::endl;
#endif

  switch (algorithm) {
    case 0:
      FIFO();
      break;
    case 1:
      LRU();
      break;
    case 2:
      Min();
      break;
    case 3:
      Clock();
      break;
    case 4:
      SecondChance();
      break;
    default:
      break;
  }
#ifdef _DEBUG
  std::clog << "Hit: " << hitCount << std::endl;
  std::clog << "Miss: " << missCount << std::endl;
#endif

  printf("Hit ratio = %.2f%%\n", (double)hitCount * 100 / pageNum);
}

void FIFO() {
#ifdef _DEBUG
  std::clog << "FIFO" << std::endl;
#endif
  hitCount = 0;
  std::list<int> cache;
  for (int page : pageQueries) {
    bool hit = false;
    for (int cachedPage : cache) {
      if (page == cachedPage) {
        hit = true;
        break;
      }
    }
    if (hit) {
      ++hitCount;
    } else {
      cache.push_back(page);
      if (cache.size() > cacheSize) cache.pop_front();
    }
  }
}

void LRU() {
#ifdef _DEBUG
  std::clog << "LRU" << std::endl;
#endif
  hitCount = 0;
  std::vector<std::pair<int, int>> cache;  // first: index, second: idle time
  for (int page : pageQueries) {
    bool hit = false;
    int maxIdleOffset = 0;
    int maxIdleTime = 0;
    for (int i = 0; i < cache.size(); ++i) {
      ++cache[i].second;
      if (page == cache[i].first) {
        hit = true;
        cache[i].second = 0;
      }
      if (cache[i].second > maxIdleTime) {
        maxIdleOffset = i;
        maxIdleTime = cache[i].second;
      }
    }
    if (hit) {
      ++hitCount;
    } else {
      cache.push_back(std::make_pair(page, 0));
      if (cache.size() > cacheSize) cache.erase(cache.begin() + maxIdleOffset);
    }
  }
}

void Min() {
  hitCount = 0;
  std::list<int> ls;
  std::unordered_map<int, std::list<int>> timeSequence;
  std::unordered_map<int, std::list<int>::iterator> cache;
  for (int i = 0; i < pageNum; ++i) timeSequence[pageQueries[i]].push_back(i);
  for (int page : pageQueries) timeSequence[page].push_back(pageNum + 1);
  for (int i = 0; i < pageNum; ++i) {
    if (cache.find(pageQueries[i]) == cache.end()) {
      if (cache.size() == cacheSize) {
        int maxOffset = -1;
        int maxOffsetCache = -1;
        for (auto &x : cache) {
          auto time = timeSequence[x.first].begin();
          while (time != timeSequence[x.first].end()) {
            if (*time < i)
              timeSequence[x.first].pop_front();
            else {
              if (*time > maxOffset) {
                maxOffsetCache = x.first;
                maxOffset = *time;
              }
              break;
            }
            time = timeSequence[x.first].begin();
          }
        }
        ls.erase(cache[maxOffsetCache]);
        cache.erase(cache.find(maxOffsetCache));
      }
      ls.push_back(pageQueries[i]);
      cache[pageQueries[i]] = --ls.end();
    } else
      ++hitCount;
  }
}

void Clock() {
#ifdef _DEBUG
  std::clog << "Clock" << std::endl;
#endif
  hitCount = 0;

  // emulate circular linked list with two arrays
  int cachePages[cacheSize];
  bool cacheValidBits[cacheSize];
  for (int i = 0; i < cacheSize; ++i) {
    cachePages[i] = -1;
    cacheValidBits[i] = false;
  }

  // clock hand for page replacement
  int hand = 0;
  for (int page : pageQueries) {
#ifdef _DEBUG
    for (int i = 0; i < cacheSize; ++i) {
      std::clog << "[" << cachePages[i] << "," << cacheValidBits[i] << "] ";
    }
#endif
    bool hit = false;
    for (int i = 0; i < cacheSize; ++i) {
      if (cachePages[i] == page) {
        hit = true;
        break;
      }
    }
    if (hit) {
#ifdef _DEBUG
      std::clog << "cache hit" << std::endl;
#endif
      ++hitCount;
    } else {
#ifdef _DEBUG
      std::clog << "cache miss" << std::endl;
      std::clog << "hand: " << cachePages[hand]
                << ", valid: " << cacheValidBits[hand] << std::endl;
#endif
      for (int i = 0; i <= cacheSize; ++i) {
        if (!cacheValidBits[hand]) {
#ifdef _DEBUG
          std::clog << "hand: " << cachePages[hand] << " invalid" << std::endl;
#endif
          cachePages[hand] = page;
          cacheValidBits[hand] = true;
          ++hand;
          if (hand == cacheSize) hand = 0;
          break;
        } else {
#ifdef _DEBUG
          std::clog << "hand: " << cachePages[hand] << " valid" << std::endl;
#endif
          cacheValidBits[hand] = false;
          ++hand;
          if (hand == cacheSize) hand = 0;
        }
      }
    }
  }
}

void SecondChance() {
#ifdef _DEBUG
  std::clog << "Second chance" << std::endl;
#endif
  hitCount = 0;
  std::list<int> FIFOcache;
  std::vector<std::pair<int, int>> LRUcache;  // first: index, second: idle time
  for (int page : pageQueries) {
    bool hit = false;
    int maxIdleOffset = 0;
    int maxIdleTime = 0;
    for (int cachedPage : FIFOcache) {
      if (page == cachedPage) {
        hit = true;
        break;
      }
      if (hit) {
        for (auto itr = LRUcache.begin(); itr != LRUcache.end();) {
          ++itr->second;
        }
      }
    }
    if (!hit) {
      for (int i = 0; i < LRUcache.size(); ++i) {
        ++LRUcache[i].second;
        if (page == LRUcache[i].first) {
          hit = true;
        }
        if (LRUcache[i].second > maxIdleTime) {
          maxIdleOffset = i;
          maxIdleTime = LRUcache[i].second;
        }
      }
      if (hit) {
        int tmp = FIFOcache.back();
        // remove last page in FIFO cache
        FIFOcache.pop_back();
        // push last page in FIFO cache into LRU cache
        LRUcache.push_back(std::make_pair(tmp, 0));
        // remove page found in LRU cache
        LRUcache.erase(LRUcache.begin() + maxIdleOffset);
        // push page found in LRU cache into FIFO cache
        FIFOcache.push_back(page);
      }
    }
    if (hit) {
      ++hitCount;
    } else {
      if (FIFOcache.size() < cacheSize / 2) {
#ifdef _DEBUG
        std::clog << "FIFO cache not full" << std::endl;
#endif
        FIFOcache.push_back(page);
      } else {
#ifdef _DEBUG
        std::clog << "FIFO cache full" << std::endl;
#endif
        int tmp = FIFOcache.back();
        FIFOcache.pop_back();
        FIFOcache.push_back(page);
        LRUcache.push_back(std::make_pair(tmp, 0));
        if (LRUcache.size() == cacheSize - cacheSize / 2)
          LRUcache.erase(LRUcache.begin() + maxIdleOffset);
      }
    }
  }
}
