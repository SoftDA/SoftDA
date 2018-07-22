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
#include <regex>
#include <string_view>

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

    bool _next_valid_char(std::string_view, size_t&);

    std::string _match_keyword(std::string_view, size_t = 0) const;

    void _keyword_module(std::string_view);
    void _keyword_wire();
    void _keyword_IO();

};

inline void Des::_keyword_module(std::string_view buf){
  const static std::regex del_head_tail_ws("^ +| +$");
  size_t pos {6};  // Skip keyword "module"

  // Get the name of the module
  if(auto ret=buf.find_first_not_of(" \t\n", pos); 
    ret == std::string::npos or ret == pos){
    return ; // Invalid
  }
  else{
    ret += pos;
  }

  // Get the left and right parenthese
  auto l_par = buf.find_first_of('(', pos);
  auto r_par = buf.find_first_of(')', pos);
  if(l_par == std::string::npos or 
     r_par == std::string::npos or
     l_par > r_par or 
     l_par == pos){
    return ; // Invalid
  }

  std::string module_name {buf.substr(pos, l_par-pos)};
  module_name = std::regex_replace(module_name, del_head_tail_ws, "$1");
  
  // Extract ports

  
  
}


inline std::string Des::_match_keyword(std::string_view buf, size_t pos) const {
  static const std::vector<std::string> keywords 
    {"module", "input", "output", "wire", "endmodule"};
  for(const auto& k: keywords){
    if(k.compare(0, k.size(), buf, pos) == 0){
      return k;
    }
  }
  return {};
}


inline bool Des::_next_valid_char(std::string_view buf, size_t& pos){
  std::string::size_type ret;
  while(pos < buf.size()){
    switch(buf[pos]){
      case '\n':
      case ' ':
      case '\t':
      case '*':
        if(buf[pos] != '*'){
          ret=buf.find_first_not_of(" \t\n", pos+1);  // Skip all whitespace and newline
        }
        else{
          ret=buf.find_first_of("\n", pos+1);         // Skip current line
        }
        if(ret == std::string::npos){
          pos = buf.size();
          return true;
        }
        pos = ret+1;
        break;
      case '/':
        if(pos != buf.size()-1){
          if(buf[pos+1] == '/'){
            if(ret=buf.find_first_of("\n", pos+1); ret == std::string::npos){  // Skip current line
              return true;
            }
            pos = ret+1;
          }
          else if(buf[pos+1] == '*'){
            if(ret=buf.find("*/", pos+1); ret == std::string::npos){
              return false;
            }
            pos = ret+2;
          }
        }
        else{
          return false; // invalid comment
        }
        break;
      default:
        return true;
        break;    
    }
  }
}

inline void Des::parse_module(const std::filesystem::path &p){
  if(not std::filesystem::exists(p)){
    return;
  }

  std::ifstream ifs(p);
  if(not ifs.good()){
    return;
  }

  // Read the file to a local buffer.
  ifs.seekg(0, std::ios::beg);
  std::string buffer(ifs.tellg(), ' ');
  ifs.seekg(0);
  ifs.read(&buffer[0], buffer.size());

  size_t pos {0};
  while(pos < buffer.size()){
    if(_next_valid_char(buffer, pos)){
      if(auto keyword=_match_keyword(buffer, pos); keyword.size() > 0){
        
      }
      else{
      }
    }
    else{
      // Invalid content 
      break;
    }
  }
}

};  // end of namespace sda. ----------------------------------------------------------------------


#endif

