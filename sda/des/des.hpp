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

class Des{

  struct Instance{
    std::string name;
    std::string module_name;
    std::unordered_map<std::string, std::string> pin2wire;
    std::unordered_map<std::string, std::string> wire2pin;
  };

  struct Module{
    std::string name;
    std::unordered_set<std::string> inputs;
    std::unordered_set<std::string> outputs;

    std::unordered_map<std::string, std::pair<std::string, std::string>> wires;
    std::unordered_map<std::string, Instance> instances;
  };

  public:
    void parse_module(const std::filesystem::path&);


  private:
    std::unordered_set<std::string> _modules;

};


inline void Des::parse_module(const std::filesystem::path &p){
  if(not std::filesystem::exists(p)){
    return;
  }

  std::ifstream ifs(p);
  if(not ifs.good()){
    return;
  }

  
  

   
}

};  // end of namespace sda. ----------------------------------------------------------------------


#endif

