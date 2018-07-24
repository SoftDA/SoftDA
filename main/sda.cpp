#include <sda/headerdef.hpp>
#include <sda/des/des.hpp>
#include <cassert>

int main(){
  sda::Des parser;
  assert(parser.parse_module("/home/clin99/SoftDA/example/darpa-idea/flow.des"));
  assert(parser.parse_module("/home/clin99/SoftDA/example/darpa-idea/signoff.des"));

  const auto& modules = parser.get_all_modules();
  for(const auto& iter : modules){
    std::cout << "Detail " << iter.first << "\n";
    auto str = parser.dump_module(iter.first);
    std::cout << str << "\n";
  }

  //parser._keyword_module("abcd");
  //parser._keyword_module("modu");
  //parser._keyword_module("module");
  //parser._keyword_module("module ()");
  //exit(1);
  //parser._keyword_module("module M1()");
  //parser._keyword_module("module M2(a, b, cdef, ghi         )");


  //std::string s("module M3(a, \n b\n\n\n\n)");
  //parser._keyword_module(s);

  //parser._keyword_wire("wire abc dependency          ;");
  //parser._keyword_wire("wire abc stream          ;");
  //parser._keyword_wire("wire     def stream          ;");


  // parser._keyword_io("input abcde;");
  // parser._keyword_io("output  123415;");
  // parser._keyword_io("input       a41no1;");
  

  //parser._parse_cell("PR    A(.i(in), .o(w));");
}
