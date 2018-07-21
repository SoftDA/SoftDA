#ifndef DES_HPP_
#define DES_HPP_

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <mutex>
#include <deque>
#include <vector>
#include <algorithm>
#include <thread>
#include <future>
#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <sstream>
#include <list>
#include <forward_list>
#include <numeric>
#include <iomanip>
#include <cassert>
#include <experimental/filesystem>

namespace std {

namespace filesystem = experimental::filesystem;

};

namespace sda{

struct Cell {
  std::unordered_set<std::string> inputs;
  std::unordered_set<std::string> outputs;

  std::string args;
};


class Des{
  public:

    void parse_file(const std::filesystem::path&);

};


inline void Des::parse_file(const std::filesystem::path &p){
  if(not std::filesystem::exists(p)){

    return;
  }
}

};  // end of namespace sda. ----------------------------------------------------------------------


#endif

