#include "asio/AsioError.h"

namespace                         spo   {
namespace                         asio  {

AsioError &
AsioError::operator=( AsioError && err)
{
  m_Callbacks.swap( err.m_Callbacks );
  SetErrorCode( err.m_ErrorCode );
  err.m_ErrorCode.clear();
  return std::ref( * this );
}

AsioError &
AsioError::operator=( const AsioError& err )
{
  m_Callbacks   = err.m_Callbacks;
  SetErrorCode( err.m_ErrorCode );
  return std::ref( * this );
}

}// namespace                   asio
}// namespace                   spo
