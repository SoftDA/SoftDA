#ifndef SDA_UTILITY_LAMBDA_HPP_
#define SDA_UTILITY_LAMBDA_HPP_

namespace sda {

template <typename T>
struct CopyOnMove {

  CopyOnMove(T&& rhs) : object(std::move(rhs)) {}
  CopyOnMove(const CopyOnMove& other) : object(std::move(other.object)) {}

  T& get() { return object; }
  
  mutable T object; 
};


};  // end of namespace sda. -----------------------------------------------------------------------

#endif
