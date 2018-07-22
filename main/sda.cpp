#include <sda/headerdef.hpp>
#include <sda/des/des.hpp>

int main(){
  sda::Des parser;
  parser._keyword_module("abcd");
  parser._keyword_module("modu");
  parser._keyword_module("module");
  parser._keyword_module("module ()");
  exit(1);
  parser._keyword_module("module M1()");
  parser._keyword_module("module M2(a, b, cdef, ghi         )");


  std::string s("module M3(a, \n b\n\n\n\n)");
  parser._keyword_module(s);

}
