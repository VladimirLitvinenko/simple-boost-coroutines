/**
 * @file ByteStuffing.cpp
 *
 * @brief Файл содержит реализацию класса управления байт-стаффингом набора байт.
 *
 * @author Владимир Н. Литвиненко.
 * @orhanization Санкт-Петербургский филиал ПАО "НПО "Стрела"
 * @copyright Все права защищены.
 */

#include "core/admin/ByteStuffing.h"
#include "core/documents/DocumentLiterals.h"
#include "core/utils/CoreUtils.h"
#include <cerrno>
#include <QList>

namespace                       spo   {
namespace                       core  {
namespace                       admin {

ByteStuffing::ByteStuffing      (const std::pair<std::string, std::string>& p )
  : ByteStuffing::base_class_t  ( spo::core::docs::DOC_BYTESTUFFING_ID )
{
  *this = p;
}

ByteStuffing::ByteStuffing      ( std::pair<std::string, std::string> && p )
  : ByteStuffing::base_class_t  ( spo::core::docs::DOC_BYTESTUFFING_ID )
{
  *this = std::move( p );
}

ByteStuffing::ByteStuffing( const ByteStuffing & lValue)
  : ByteStuffing::base_class_t  ( spo::core::docs::DOC_BYTESTUFFING_ID )
{
  * this = lValue;
}

ByteStuffing::ByteStuffing( ByteStuffing && rValue )
  : ByteStuffing::base_class_t  ( spo::core::docs::DOC_BYTESTUFFING_ID )
{
  * this = std::move( rValue );
}

ByteStuffing::~ByteStuffing()
{
  ClearContent();
}

ByteStuffing &
ByteStuffing::operator=( const ByteStuffing & lValue )
{
  SetContent( lValue.mContent );
  return  * this;
}

ByteStuffing &
ByteStuffing::operator=( ByteStuffing && rValue)
{
  * this = rValue.mContent;
  rValue.ClearContent();
  return  * this;
}

ByteStuffing &
ByteStuffing::operator=(const std::pair<std::string, std::string>& p)
{
  SetContent( p );
  return * this;
}

ByteStuffing &
ByteStuffing::operator=( std::pair< std::string, std::string > && p)
{
  SetFrom ( std::move( p.first  ) );
  SetTo   ( std::move( p.second ) );
  return * this;
}

std::string
ByteStuffing::GetFrom() const
{
  return mContent.first;
}

void
ByteStuffing::SetFrom(const std::string & value)
{
  mContent.first = value;
}

void ByteStuffing::SetFrom( std::string && value)
{
  mContent.first.swap( value );
  value.clear();
}

std::string
ByteStuffing::GetTo() const
{
  return mContent.second;
}

void
ByteStuffing::SetTo( const std::string& value )
{
  mContent.second = value;
}

void ByteStuffing::SetTo( std::string && value)
{
  mContent.second.swap( value );
  value.clear();
}

std::string
ByteStuffing::Stuff( const std::string & value)
{
  return spo::core::utils::StrReplace(value, GetFrom(), GetTo() );
}

std::string
ByteStuffing::Stuff( std::string && value )
{
  return spo::core::utils::StrReplace( std::move( value ), GetFrom(), GetTo() );
}

std::string
ByteStuffing::Unstuff( const std::string & value )
{
  return spo::core::utils::StrReplace(value, GetTo(), GetFrom() );
}

std::string
ByteStuffing::Unstuff( std::string && value )
{
  return spo::core::utils::StrReplace( std::move( value ), GetTo(), GetFrom() );
}

bool
ByteStuffing::FromByteArray( const std::string & a )
{
  std::unique_lock<std::recursive_mutex> l( m_Mutex );
  errno = 0;
  std::string array( a );
  bool retval{ not a.empty() };
  if( not retval )
  {
    errno = ENODATA;
  }
  else
  {
    auto list = spo::core::utils::StrSplit( array, '\n');
    retval = ( not list.empty() ) and ( list.size() == 2 );
    if( retval )
    {
      SetFrom( list.at(0) );
      SetTo( list.at(1) );
    }
    else
    {
      errno = EBADMSG;
    }
  }
  return retval;
  UNUSED( l )
}

std::string
ByteStuffing::ToByteArray()
const
{
  return
      std::string() +
      mContent.first +
      '\n' +
      mContent.second
      ;
}

void
ByteStuffing::ClearContent()
{
  std::unique_lock<std::recursive_mutex> l( m_Mutex );
  ContentRef().first.clear();
  ContentRef().second.clear();
  UNUSED( l )
}

}// namespace                   admin
}// namespace                   core
}// namespace                   spo
