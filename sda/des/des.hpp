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

  enum class Keyword{
    NONE = 0,
    MODULE,  
    INPUT, 
    OUTPUT, 
    WIRE, 
    ENDMODULE
  };

  struct Instance{
    Instance() = default;

    std::string name;
    std::string module_name;

    std::unordered_map<std::string, std::string> pin2wire;
    std::unordered_map<std::string, std::string> wire2pin;
  };

  struct Module{
    Module() = default;

    std::string name;

    std::unordered_set<std::string> ports;
    // port name, inst name
    std::unordered_map<std::string_view, std::string> inputs;
    std::unordered_map<std::string_view, std::string> outputs;

    // wire name, <inst 1 name, inst 2 name>
    std::unordered_map<std::string, std::pair<std::string, std::string>> dependency_wire;   
    std::unordered_map<std::string, std::pair<std::string, std::string>> stream_wire;

    std::unordered_map<std::string, Instance> instances;
  };

  struct Vertex{
    Vertex() = default;
    std::string module_name;
    std::unordered_set<std::string> edges;
  };

  struct Edge{
    std::string name;
    std::string from;
    std::string to;
  };

  struct Graph{
    Graph() = default;
    std::unordered_set<std::string> pi;
    std::unordered_set<std::string> po;

    std::unordered_map<std::string, Vertex> vertices;
    std::unordered_map<std::string, Edge> edges;

    void add_graph(const Graph&);
  };

  public:
    bool parse_module(const std::filesystem::path&);

    std::string dump_module(const std::string&) const;
    const std::unordered_map<std::string, Module>& get_all_modules() const;

    void build_graph();

  private:

    std::unordered_map<std::string_view, Vertex> _libs;
    std::unordered_map<std::string_view, Graph> _graphs;
    void _build_graph(const std::string&);

    std::unordered_map<std::string, Module> _modules;

    Keyword _match_keyword(std::string_view, size_t = 0) const;

    bool _next_valid_char(std::string_view, size_t&);
    bool _keyword_module(std::string_view, Module&);
    bool _keyword_wire(std::string_view, Module&);
    bool _keyword_io(std::string_view, Module&);

    bool _parse_cell(std::string_view, Module&);

    std::vector<std::string> _split_on_space(std::string&);

    bool _is_word_valid(std::string_view);

    std::regex _del_head_tail_ws {"^[ \t\n]+|[ \t\n]+$"};
    std::regex _ws_re {"\\s+"};
};

inline void Des::Graph::add_graph(const Graph& other){
}


inline const std::unordered_map<std::string, Des::Module>& Des::get_all_modules() const {
  return _modules;
}


inline std::string Des::dump_module(const std::string& module_name) const {
  if(_modules.find(module_name) == _modules.end()){
    return std::string();
  }
  
  std::string str;
  auto m = _modules.at(module_name);

  str.append("module ").append(m.name).append("(");
  for(const auto& p: m.ports){
    str.append(p).append(",\n");
  }
  if(not m.ports.empty()){
    str.erase(str.size()-2);
  }
  str.append(");\n");

  for(const auto& p: m.inputs){
    str.append("input ").append(p.first).append(";\n");
  }

  for(const auto& p: m.outputs){
    str.append("output ").append(p.first).append(";\n");
  }

  for(const auto& p: m.dependency_wire){
    str.append("wire ").append(p.first).append("dependency;\n");
  }

  for(const auto& p: m.stream_wire){
    str.append("wire ").append(p.first).append("stream;\n");
  }

  for(const auto&[k ,v]: m.instances){
    str.append(v.module_name).append(1, ' ').append(k).append(1, '(');
    for(const auto& [p, w]: v.pin2wire){
      str.append(1, '.').append(p).append(1, '(').append(w).append("), ");
    }
    str.erase(str.size()-2);
    str.append(");\n");
  }

  return str.append("endmodule\n");
}


// Function: _is_word_valid 
// Check the chars must be either alphabets or digits or underscore
// and the first char must be an alphabet or underscore.
inline bool Des::_is_word_valid(std::string_view sv){
  static constexpr auto is_char_valid = [](char c){return (std::isalnum(c) or c == '_');};
  return not sv.empty() and std::all_of(sv.begin(), sv.end(), is_char_valid) and 
         (std::isalpha(sv[0]) or sv[0] == '_');
}



inline std::vector<std::string> Des::_split_on_space(std::string& s){
  return std::vector<std::string>(std::sregex_token_iterator(s.begin(), s.end(), _ws_re, -1), {});
}

inline bool Des::_keyword_module(std::string_view buf, Module& mod){
  size_t pos {6};  // Skip keyword "module"

  // Get the name of the module
  if(auto ret=buf.find_first_not_of(" \t\n", pos); 
    ret == std::string::npos or ret == pos){
    return false; // Invalid
  }
  else{
    pos = ret;
  }

  // Get the left and right parenthese
  const auto l_par {buf.find_first_of('(', pos)};
  const auto r_par {buf.find_first_of(')', pos)};

  // Check unique left and right parenthese and their positions
  if(l_par == std::string::npos or 
     r_par == std::string::npos or 
     l_par > r_par or
     buf.find_first_of('(', l_par+1) != std::string::npos or 
     buf.find_first_of(')', r_par+1) != std::string::npos){
    return false; // Invalid
  }

  // Get module name 
  mod.name.resize(l_par-pos);
  buf.copy(mod.name.data(), l_par-pos, pos);
  mod.name = std::regex_replace(mod.name, _del_head_tail_ws, "$1");
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
    p = std::regex_replace(p, _del_head_tail_ws, "$1");
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
  // Get keyword "wire"
  std::regex_token_iterator<std::string_view::iterator> iter(buf.begin(), buf.end(), _ws_re, -1);
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
    mod.stream_wire.insert({std::move(wire_name), {}});
  }
  else{
    mod.dependency_wire.insert({std::move(wire_name), {}});
  }

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
  std::regex_token_iterator<std::string_view::iterator> iter(buf.begin(), buf.end(), _ws_re, -1);
  if(iter == end_of_buf or 
    (iter->compare("input") != 0 and iter->compare("output") != 0)){
    return false; // Invalid
  }

  // Get keyword "input/output"
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

  if(io_type.compare("input") == 0){
    mod.inputs.insert({*mod.ports.find(io_name), {}});
  }
  else{
    mod.outputs.insert({*mod.ports.find(io_name), {}});
  }

  return true;
}



// PR    A(.i(in), .o(w));
inline bool Des::_parse_cell(std::string_view buf, Module& mod){
  if(buf.back() == ';'){
    buf = buf.substr(0, buf.size()-1);
  }

  std::string::size_type pre_pos {0};
  std::string::size_type pos {0};
  if(pos = buf.find_first_of(" \t"); pos == std::string::npos){
    return false; // Invalid
  }

  // Get cell name 
  if(not _is_word_valid({&buf[0], pos})){
    return false;
  }
  std::string module_name(&buf[0], pos);

  // Move cursors to the beg/end of instance name
  if(pre_pos = buf.find_first_not_of(" \t", pos); pre_pos == std::string::npos){
    return false; // Invalid
  }
  if(pos = buf.find_first_of(" (\t", pre_pos); pos == std::string::npos){
    return false; // Invalid
  }

  // Get instance name 
  if(not _is_word_valid({&buf[pre_pos], pos-pre_pos})){
    return false;
  }
  std::string inst_name(&buf[pre_pos], pos-pre_pos);

  auto [inst, inserted] = mod.instances.insert({inst_name, Instance()});
  if(not inserted){
    return false;
  }
  inst->second.name = std::move(inst_name);
  inst->second.module_name = std::move(module_name);

  std::string::size_type dot {0}; 
  std::string::size_type l_par {0}; 
  std::string::size_type r_par {pos}; 


  // A lambda to set the pin names of an edge
  auto set_pin_name = [](std::pair<std::string, std::string>& e, std::string& pin){
    if(e.first.empty()){
      e.first = pin;
    }
    else{
      e.second = pin;
    }
  };

  while(1){
    if(dot = buf.find_first_of('.', r_par); dot == std::string::npos){
      break;  // Parse end
    }
    if(l_par = buf.find_first_of('(', dot); 
      l_par == std::string::npos or l_par == dot+1){
      return false;  // Invalid
    }
    if(r_par = buf.find_first_of(')', l_par); 
      r_par == std::string::npos or r_par == l_par+1){
      return false;  // Invalid
    }

    std::string pin_name({&buf[dot+1], l_par-dot-1});
    std::string wire_name({&buf[l_par+1], r_par-l_par-1}); // A wire could be an input or output

    pin_name = std::regex_replace(pin_name, _del_head_tail_ws, "$1");
    wire_name = std::regex_replace(wire_name, _del_head_tail_ws, "$1");

    // Check the pin name and wire name
    if(not _is_word_valid(pin_name) or 
       not _is_word_valid(wire_name)){ 
      return false;
    }

    // Check wire should exist. (wire is always declared before the inst)
    if(mod.dependency_wire.find(wire_name) == mod.dependency_wire.end()and 
       mod.stream_wire.find(wire_name) == mod.stream_wire.end() and 
       mod.inputs.find(wire_name) == mod.inputs.end() and 
       mod.outputs.find(wire_name) == mod.outputs.end()){
      return false;
    }


    if(mod.inputs.find(wire_name) != mod.inputs.end()){
      mod.inputs.at(wire_name) = inst->second.name;
    }
    else if(mod.outputs.find(wire_name) != mod.outputs.end()){
      mod.outputs.at(wire_name) = inst->second.name;
    }
    else if(mod.stream_wire.find(wire_name) != mod.stream_wire.end()){
      set_pin_name(mod.stream_wire.at(wire_name), inst->second.name);
    }
    else{
      set_pin_name(mod.dependency_wire.at(wire_name), inst->second.name);
    }

    inst->second.pin2wire.insert({pin_name, wire_name});
    inst->second.wire2pin.insert({wire_name, pin_name});
  }

  if(inst->second.pin2wire.empty()){
    return false; // Invalid
  }
  return true;
}


inline Des::Keyword Des::_match_keyword(std::string_view buf, size_t pos) const {
  static const std::unordered_map<std::string_view, Keyword> keywords = {
    {"module",    Keyword::MODULE},
    {"input",     Keyword::INPUT},
    {"output",    Keyword::OUTPUT},
    {"wire",      Keyword::WIRE},
    {"endmodule", Keyword::ENDMODULE}
  };

  for(const auto& iter: keywords){
    if(buf.compare(pos, iter.first.size(), iter.first) == 0){
      return iter.second;
    }
  }

  return Keyword::NONE;
}


inline bool Des::_next_valid_char(std::string_view buf, size_t& pos){
  std::string::size_type ret;
  while(pos < buf.size()){
    switch(buf[pos]){
      case '\n':
      case ' ':
      case '\t': 
        ret=buf.find_first_not_of(" \t\n", pos+1);  // Skip all whitespace and newline
        if(ret == std::string::npos){
          return false;
        }
        pos = ret;
        break;
      case '#':
        if(ret=buf.find_first_of("\n", pos+1); ret == std::string::npos){
          return false;
        }
        pos = ret+1;
        break;
      case '/':
        if(pos >= buf.size()-1){
          return false;
        }
        switch(buf[pos+1]){
          case '/':
            if(ret=buf.find_first_of("\n", pos+1); ret == std::string::npos){  // Skip current line
              return true;
            }
            pos = ret+1;
            break;
          case '*':
            if(ret=buf.find("*/", pos+1); ret == std::string::npos){
              return false;
            }
            pos = ret+2;
            break;
          default:
            return false;
        }
        break;
      default:
        return true;
        break;    
    }
  }
  return false;
}

inline bool Des::parse_module(const std::filesystem::path &p){
  if(not std::filesystem::exists(p)){
    return false;
  }

  std::ifstream ifs(p);
  if(not ifs.good()){
    return false;
  }

  // Read the file to a local buffer.
  ifs.seekg(0, std::ios::end);
  std::string buffer(ifs.tellg(), ' ');
  ifs.seekg(0);
  ifs.read(&buffer[0], buffer.size());

  Module mod;
  size_t pos {0};
  while(pos < buffer.size()){
    // Skip whitespace and comments
    if(not _next_valid_char(buffer, pos)){
      printf("fail not next valid\n");
      return false;
    }

    // Retrieve keyword
    auto keyword=_match_keyword(buffer, pos);

    // Find semicolon position 
    const auto semicol_pos {buffer.find_first_of(';', pos)};
    if(keyword != Keyword::ENDMODULE and semicol_pos == std::string::npos){
      return false;
    }

    // Process current line
    switch(keyword){
      case Keyword::MODULE:
        if(not _keyword_module({&buffer[pos], semicol_pos-pos}, mod)){
          std::cout << std::string_view(&buffer[pos], semicol_pos-pos) << "\n";
          printf("fail module\n");
          return false;
        }
        printf("ok module\n");
        pos = semicol_pos+1;
        break;
      case Keyword::INPUT:
      case Keyword::OUTPUT:           
        if(not _keyword_io({&buffer[pos], semicol_pos-pos}, mod)){ 
          printf("fail io\n");
          return false;
        }
        printf("ok io\n");
        pos = semicol_pos+1;
        break;
      case Keyword::WIRE:
        if(not _keyword_wire({&buffer[pos], semicol_pos-pos}, mod)){
          printf("fail wire\n");
          return false;
        }
        pos = semicol_pos+1;
        break;
      case Keyword::ENDMODULE:
        pos = buffer.size();
        break;
      default:
        if(not _parse_cell({&buffer[pos], semicol_pos-pos}, mod)){
          printf("fail cell\n");
          std::cout << std::string_view(&buffer[pos], semicol_pos-pos) << "\n";
          return false;
        }
        pos = semicol_pos+1;
        break;
    }
  }
  _modules.insert({mod.name, std::move(mod)});
  return true;
}


// ----------------------------------------------------------------------------------------------- 


inline void Des::build_graph(){
  // Collect lib graphs
  for(const auto& m: _modules){
    for(const auto& inst: m.second.instances){
      if(_modules.find(inst.second.module_name) == _modules.end() and 
        _libs.find(inst.second.module_name) == _libs.end()){
        _libs.insert({inst.second.module_name, {}});
        //for(const auto &iter: inst.second.pin2wire){
        //  _libs.at(inst.second.module_name).edges.insert(iter.first);
        //}
      }
    }
  }

  // Recursively build graph for each module
  for(const auto& m: _modules){
    if(_graphs.find(m.first) == _graphs.end()){
      _build_graph(m.first);
    }
  }
}



template <typename T>
void prefix_key(std::string prefix, std::unordered_map<std::string, T>& m){
  for(auto& iter: m){
    auto nh = m.extract(iter.first);
    nh.key() = prefix+iter.first;
    m.insert(move(nh));
  }
}


inline void Des::_build_graph(const std::string& module_name){
  _graphs.insert({module_name, {}});
  auto& g {_graphs.find(module_name)->second};
  auto& m {_modules.find(module_name)->second};

  // 1. Iterate through the module's instances
  // 2. For each instance, check it's type ?
  //      Lib -> get the vertex 
  //      Module -> has graph ?
  //                Yes:  
  //                No:   Recursive build its graph 
  std::unordered_map<std::string, Graph> subgraphs;

  auto collect_subgraphs {
    [&](const Instance& inst){ 
      if(_modules.find(inst.module_name) == _modules.end()){
        // This is a lib cell 
        if(g.vertices.find(inst.name) == g.vertices.end()){
          g.vertices.insert({inst.name, _libs.at(inst.module_name)});
        }
      }
      else{
        // This is a module
        if(subgraphs.find(inst.name) == subgraphs.end()){
          // If the module's graph does not exist, build it recursively 
          if(_graphs.find(inst.module_name) == _graphs.end()){
            _build_graph(inst.module_name);
          }
          subgraphs.insert({inst.name, _graphs.at(inst.module_name)});
        }
      }
    }
  };

  for(const auto& kvp: m.inputs){
    const auto& inst {m.instances.at(kvp.second)};
    collect_subgraphs(inst);
    g.pi.emplace(kvp.first);
  }

  for(const auto& kvp: m.outputs){
    const auto& inst {m.instances.at(kvp.second)};
    collect_subgraphs(inst);
    g.po.emplace(kvp.first);
  }

  for(const auto& kvp: m.dependency_wire){
    const auto& [inst1, inst2] = kvp.second;
    collect_subgraphs(m.instances.at(inst1));
    collect_subgraphs(m.instances.at(inst2));
  }

  for(const auto& kvp: m.stream_wire){
    const auto& [inst1, inst2] = kvp.second;
    collect_subgraphs(m.instances.at(inst1));
    collect_subgraphs(m.instances.at(inst2));
  }


  // Iterate through wires to build connection of graph 
  

}



};  // end of namespace sda. ----------------------------------------------------------------------


#endif

