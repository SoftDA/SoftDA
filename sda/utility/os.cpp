#include <sda/utility/os.hpp>

namespace sda {

// Function: user_home
std::filesystem::path user_home() {

  auto home = ::getenv("HOME");

  if(home == nullptr) {
    home = ::getpwuid(::getuid())->pw_dir;
  }

  return home ? home : std::filesystem::current_path();
}


};  // end of namespace ot. -----------------------------------------------------------------------
