#ifndef SDA_STATIC_LOGGER_HPP_
#define SDA_STATIC_LOGGER_HPP_

#include <sda/utility/logger.hpp>

namespace sda {

// Global declaration and macro usage.
//inline Logger<std::mutex> logger(env::log_file());
inline Logger<std::mutex> logger;

}; // end of namespace sda. ------------------------------------------------------------------------


#define SDA_LOG_REMOVE_FIRST_HELPER(N, ...) __VA_ARGS__
#define SDA_LOG_GET_FIRST_HELPER(N, ...) N
#define SDA_LOG_GET_FIRST(...) SDA_LOG_GET_FIRST_HELPER(__VA_ARGS__)
#define SDA_LOG_REMOVE_FIRST(...) SDA_LOG_REMOVE_FIRST_HELPER(__VA_ARGS__)

#define SDA_LOGTO(...) sda::logger.redir  (__VA_ARGS__)
#define SDA_LOG(...)   sda::logger.raw    (__VA_ARGS__)
#define SDA_LOGD(...)  sda::logger.debug  (__FILE__, __LINE__, __VA_ARGS__, '\n')
#define SDA_LOGI(...)  sda::logger.info   (__FILE__, __LINE__, __VA_ARGS__, '\n')
#define SDA_LOGW(...)  sda::logger.warning(__FILE__, __LINE__, __VA_ARGS__, '\n')
#define SDA_LOGE(...)  sda::logger.error  (__FILE__, __LINE__, __VA_ARGS__, '\n')
#define SDA_LOGF(...)  sda::logger.fatal  (__FILE__, __LINE__, __VA_ARGS__, '\n')

#define SDA_LOG_IF(...) if(SDA_LOG_GET_FIRST(__VA_ARGS__)) {          \
                         SDA_LOG(SDA_LOG_REMOVE_FIRST(__VA_ARGS__));  \
                       }

#define SDA_LOGD_IF(...) if(SDA_LOG_GET_FIRST(__VA_ARGS__)) {           \
                          SDA_LOGD(SDA_LOG_REMOVE_FIRST(__VA_ARGS__));  \
                        }

#define SDA_LOGI_IF(...) if(SDA_LOG_GET_FIRST(__VA_ARGS__)) {           \
                          SDA_LOGI(SDA_LOG_REMOVE_FIRST(__VA_ARGS__));  \
                        }

#define SDA_LOGW_IF(...) if(SDA_LOG_GET_FIRST(__VA_ARGS__)) {           \
                          SDA_LOGW(SDA_LOG_REMOVE_FIRST(__VA_ARGS__));  \
                        }

#define SDA_LOGE_IF(...) if(SDA_LOG_GET_FIRST(__VA_ARGS__)) {           \
                          SDA_LOGE(SDA_LOG_REMOVE_FIRST(__VA_ARGS__));  \
                        }

#define SDA_LOGF_IF(...) if(SDA_LOG_GET_FIRST(__VA_ARGS__)) {           \
                          SDA_LOGF(SDA_LOG_REMOVE_FIRST(__VA_ARGS__));  \
                        }

#define SDA_LOGW_RIF(...) if(SDA_LOG_GET_FIRST(__VA_ARGS__)) {           \
                           SDA_LOGW(SDA_LOG_REMOVE_FIRST(__VA_ARGS__));  \
                           return;                                     \
                         }

#define SDA_LOGE_RIF(...) if(SDA_LOG_GET_FIRST(__VA_ARGS__)) {           \
                           SDA_LOGE(SDA_LOG_REMOVE_FIRST(__VA_ARGS__));  \
                           return;                                     \
                         }

#endif

