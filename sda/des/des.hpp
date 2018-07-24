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
#include <cctype>

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

    std::unordered_set<std::string> ports;
    std::unordered_set<std::string_view> inputs;
    std::unordered_set<std::string_view> outputs;

    std::unordered_set<std::string> dependency_wire;   
    std::unordered_set<std::string> stream_wire;   

    std::unordered_map<std::string, std::pair<std::string, std::string>> wires;
    std::unordered_map<std::string, Instance> instances;
  };

  public:
    void parse_module(const std::filesystem::path&);


  //private:
    std::unordered_set<std::string> _modules;

    bool _next_valid_char(std::string_view, size_t&);

    std::string _match_keyword(std::string_view, size_t = 0) const;

    bool _keyword_module(std::string_view, Module&);
    bool _keyword_wire(std::string_view, Module&);
    bool _keyword_io(std::string_view, Module&);

    void _parse_cell(std::string_view);

    std::vector<std::string> _split_on_space(std::string&);

    bool _is_word_valid(std::string_view);
};


inline bool Des::_is_word_valid(std::string_view sv){
  static constexpr auto is_char_valid = [](char c){ return std::isalnum(c) or c == '_'; };
  return std::all_of(sv.begin(), sv.end(), is_char_valid);
}



inline std::vector<std::string> Des::_split_on_space(std::string& s){
  std::regex ws_re ("\\s+");
  return std::vector<std::string>(std::sregex_token_iterator(s.begin(), s.end(), ws_re, -1), {});
}

inline bool Des::_keyword_module(std::string_view buf, Module& mod){
  // regex for deleting whitespace and tab
  static const std::regex del_head_tailws("^[ \t\n]+|[ \t\n]+$");
  size_t pos {6};  // Skip keyword "module"

  // Get the name of the module
  if(auto ret=buf.find_first_not_of(" \t\n", pos); 
    ret == std::string::npos or ret == pos){
    return false; // Invalid
  }
  else{
    ret += pos;
  }

  // Get the left and right parenthese
  const auto l_par {buf.find_first_of('(', pos)};
  const auto r_par {buf.find_first_of(')', pos)};
  // Check unique left and right parenthese and their positions
  if(l_par == std::string::npos or 
     r_par == std::string::npos or 
     l_par > r_par or
     buf.find_last_of('(', l_par+1) != std::string::npos or 
     buf.find_last_of('r', r_par+1) != std::string::npos){
    return false; // Invalid
  }

  mod.name.resize(l_par-pos);
  buf.copy(mod.name.data(), l_par-pos);
  mod.name = std::regex_replace(mod.name, del_head_tailws, "$1");
  if(mod.name.empty() or not _is_word_valid(mod.name)){
    return false; // Invalid
  }
  
  // Extract ports
  pos = l_par + 1;
  while(pos < r_par){
    auto comma = buf.find_first_of(",)", pos);
    if(comma == pos){ // End of buf
      break;
    }
    std::string p(buf.data(), pos, comma-pos);
    p = std::regex_replace(p, del_head_tailws, "$1");
    if(p.empty() or mod.ports.find(p) != mod.ports.end()){
      return false; // Invalid
    }
    mod.ports.insert(std::move(p));
    pos = comma + 1;
  }
  return true;
}


// wire my_wire dependency;
inline bool Des::_keyword_wire(std::string_view buf, Module& mod){
  if(buf.find_first_of('\n') != std::string::npos){
    return false; // Invalid: should be a single line
  }

  if(buf.back() == ';'){
    buf = buf.substr(0, buf.size()-1);
  }

  std::regex_token_iterator<std::string_view::iterator> end_of_buf;
  std::regex ws_re ("\\s+");
  // Get keyword "wire"
  std::regex_token_iterator<std::string_view::iterator> iter(buf.begin(), buf.end(), ws_re, -1);
  if(iter == end_of_buf or iter->compare("wire") != 0){
    return false; // Invalid
  }

  // Get wire name
  if(++iter == end_of_buf){
    return false; // Invalid
  }
  std::string wire_name(iter->str());

  // Get wire type
  if(++iter == end_of_buf or 
    (iter->compare("stream") != 0 and iter->compare("dependency") != 0)){
    return false; // Invalid
  }
  std::string_view wire_type(iter->str());
  
  // Last check
  if(++iter != end_of_buf or 
    mod.dependency_wire.find(wire_name) != mod.dependency_wire.end() or 
    mod.stream_wire.find(wire_name) != mod.stream_wire.end()){
    return false; // Invalid
  }

  if(wire_type.compare("stream") == 0){
    mod.stream_wire.insert(std::move(wire_name));
  }
  else{
    mod.dependency_wire.insert(std::move(wire_name));
  }

  std::cout << "name = " << wire_name << std::endl;
  std::cout << "type = " << wire_type << std::endl;
  return true;
}



// input primary_input;
inline bool Des::_keyword_io(std::string_view buf, Module& mod){
  if(buf.find_first_of('\n') != std::string::npos){
    return false; // Invalid: should be a single line
  }

  if(buf.back() == ';'){
    buf = buf.substr(0, buf.size()-1);
  }

  std::regex_token_iterator<std::string_view::iterator> end_of_buf;
  std::regex ws_re ("\\s+");
  std::regex_token_iterator<std::string_view::iterator> iter(buf.begin(), buf.end(), ws_re, -1);
  if(iter == end_of_buf or 
    (iter->compare("input") != 0 and iter->compare("output") != 0)){
    return false; // Invalid
  }

  // Get keyword "input or output"
  std::string_view io_type(iter->str());

  // Get IO name
  if(++iter == end_of_buf){
    return false; // Invalid
  }

  std::string io_name(iter->str());

  // Last check
  if(++iter != end_of_buf or mod.ports.find(io_name) == mod.ports.end()){
    return false; // Invalid
  }
  mod.ports.erase(io_name);  

  if(io_type.compare("input") == 0){
    mod.inputs.insert(std::move(io_name));
  }
  else{
    mod.outputs.insert(std::move(io_name));
  }

  std::cout << "Type = " << io_type << "   Name = " << io_name << "\n";
  return true;
}



// PR    A(.i(in), .o(w));
inline void Des::_parse_cell(std::string_view buf){
  if(buf.back() == ';'){
    buf = buf.substr(0, buf.size()-1);
  }

  std::string::size_type pre_pos {0};
  std::string::size_type pos {0};
  if(pos = buf.find_first_of(" \t"); pos == std::string::npos){
    return ; // Invalid
  }

  // Get cell name
  std::string_view cell_name(&buf[0], pos);

  pre_pos = buf.find_first_not_of(" \t", pos);
  if(pre_pos == std::string::npos){
    return ; // Invalid
  }
  if(pos = buf.find_first_of(" (\t", pre_pos); pos == std::string::npos){
    return ; // Invalid
  }
  // Get instance name
  std::string_view inst_name(&buf[pre_pos], pos-pre_pos);

  std::string::size_type dot {0}; 
  std::string::size_type l_par {0}; 
  std::string::size_type r_par {pos}; 

  std::vector<std::string_view> port_names;
  std::vector<std::string_view> wire_names;

  while(1){
    if(dot = buf.find_first_of('.', r_par); dot == std::string::npos){
      break;  // Parse end
    }
    if(l_par = buf.find_first_of('(', dot); 
      l_par == std::string::npos or l_par == dot+1){
      return ;  // Invalid
    }
    if(r_par = buf.find_first_of(')', l_par); 
      r_par == std::string::npos or r_par == l_par+1){
      return ;  // Invalid
    }

    if(auto& p = port_names.emplace_back(&buf[dot+1], l_par-dot-1); 
       not _is_word_valid(p)){
      return ;  // Invalid
    }

    if(auto& w = wire_names.emplace_back(&buf[l_par+1], r_par-l_par-1); 
       not _is_word_valid(w)){
      return ;  // Invalid
    }
  }

  if(port_names.empty()){
    return ; // Invalid
  }

  std::cout << "Cell name = " << cell_name << '\n';
  std::cout << "Inst name = " << inst_name << '\n';
  for(size_t i=0; i<port_names.size(); i++){
    std::cout << port_names[i] << " : " << wire_names[i] << '\n';
  }
}


inline std::string Des::_match_keyword(std::string_view buf, size_t pos) const {
  static const std::vector<std::string> keywords 
    {"module", "input", "output", "wire", "endmodule"};
  if(auto iter=std::find(keywords.begin(), keywords.end(), buf); iter!=keywords.end()){
    return *iter;
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

