#ifndef SDA_UTILITY_UTILITY_HPP_
#define SDA_UTILITY_UTILITY_HPP_

#include <sda/utility/lambda.hpp>
#include <sda/utility/logger.hpp>
#include <sda/utility/tokenizer.hpp>
#include <sda/utility/index.hpp>
#include <sda/utility/iterator.hpp>
#include <sda/utility/os.hpp>
#include <sda/utility/scope_guard.hpp>
#include <sda/utility/CLI11.hpp>

// Miscellaneous
namespace sda {

// Resize a container to fit a given size
template <typename T>
void resize_to_fit(size_t N, T& v) {
  auto sz = v.size();
  for(; sz < N; sz = (sz==0) ? 32 : (sz << 1));
  v.resize(sz);
}

// Resize containers to fit a given size
template <typename... T, std::enable_if_t<(sizeof...(T)>1), void>* = nullptr>
void resize_to_fit(size_t N, T&...vs) {
  (resize_to_fit(N, vs), ...);
}

};

#endif
