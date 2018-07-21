#ifndef SDA_UTILITY_OS_HPP_
#define SDA_UTILITY_OS_HPP_

#include <unistd.h>
#include <pwd.h>
#include <experimental/filesystem>

namespace std {
  namespace filesystem = experimental::filesystem;
};

namespace ot {

// Function: user_home
std::filesystem::path user_home();


};  // end of namespace ot. -----------------------------------------------------------------------


#endif
