#ifndef COREUTILS_H
#define COREUTILS_H

#include "core/admin/AdminGlobal.h"
#include "MainLiterals.h"
#include <vector>
#include <string>
#include <functional>
#include <future>
#include <cxxabi.h>
#include <boost/regex.hpp>
#include <iostream>

namespace                       spo   {
namespace                       core  {
namespace                       utils {

//------------------------------------------------------------------------------
using active_proc_t           = std::map< std::string, pid_t >;
using active_pid_t            = std::map< pid_t, std::string >;
using active_processes_t      = std::pair< active_pid_t, active_proc_t >;

//------------------------------------------------------------------------------
bool                            IsProcessDir( const std::string & dir );
active_processes_t              ActiveProcessMap();
std::pair< pid_t, bool >        ProcessIsActive( const std::string & procName );
std::pair< std::string, bool >  HostName();
bool                            SetSysyemWatches  ( int param, std::size_t value );
inline bool                     ExactMatch        ( const boost::regex & expList, const std::string & path )
{
  return boost::regex_match( path, expList );
}

std::pair
<
  std::string,  ///< каталог
  bool          ///< признак рекурсивного вчичсления
>                               RecurciveCalc     ( const std::string & filePath );

//------------------------------------------------------------------------------
/**
 * @brief   Шаблонный метод MapKeys возвращает набор ключей ассоциативного
 *          контейнер-карты типа @a std::map.
 * @ingroup Утилиты
 * @param   collection ассоциативный контейнер-карта типа @a std::map
 * @return  Контейнер-последовательность типа @a std::vector набора ключей из
 *          значений @a collection
 *
 * @see std::map, std::vector
 */
template< typename _Key, typename _Tp >
std::pair< std::vector< _Key >, bool > MapKeys( const std::map< _Key, _Tp > & collection )
{
  std::pair< std::vector< _Key >, bool >  retval
  {
    std::vector< _Key >(),
    collection.size() > 0
  };
  try
  {
    if( retval.second )
    {
      retval.first.reserve( collection.size() );
      std::for_each(  collection.cbegin(),
                      collection.cend(),
                      [ &retval ]( const std::pair< const _Key, _Tp > & value)
                      {
                        retval.first.push_back( value.first );
                      } );
    }
  }
  catch( const std::exception & e )
  {
    DUMP_EXCEPTION( e );
    return std::move( std::make_pair( std::vector< _Key >(), false ) );
  }
  return std::move( retval );
}

//------------------------------------------------------------------------------
template
<
    typename                    T_,
    typename = typename         std::enable_if< not std::is_void< T_ >::value >
>
std::string                     GetClassName ()
{
  static  auto    st        ( 0 );
          size_t  dlen      ( 0 );
  static  auto    mangled   ( typeid(T_).name() );
  static  char *  demangled { nullptr };

  demangled = abi::__cxa_demangle( mangled, demangled, &dlen, &st );

  return
      std::string(
        0 == st ?
          demangled :
          mangled );
}

template< typename T = int >
inline T                          leftShift( int incValue )
{
  return std::forward<const T>(1) << incValue;
}

template< typename T_ >
inline qint64                     as64Int( const T_ & value )
{
  return static_cast<qint64>(value);
}

/**
 * @brief Метод HashStdString формирует хэш-значение из STL строки типа @a std::string.
 * @param str исходная строка
 * @return хэш-значение.
 * @see std::size_t, std::hash, std::string
 */
inline
std::size_t                       HashStdString ( const std::string & str )
{
  return std::hash< std::string >()(str);
}

template< typename T = int >
inline T                          leftFill( const int & positions )
{
  T retval = static_cast<T>(0);
  for( auto i(0); i < positions; ++i )
    retval = static_cast<T>( static_cast<int>( retval ) | ( int(1) << i ) );
  return retval;
}

std::pair< std::string, bool >    MakePath                ( const std::string & path );
std::pair< std::string, bool >    MakeFile                ( const std::string & fileName );
inline bool                       RemoveFile              ( const std::string & fileName )
{
  boost::filesystem::path fn( fileName );
  return
      boost::filesystem::exists( fn ) ?
        boost::filesystem::remove( fn ) :
        true;
}

template
<
    typename T_                 = std::string,
    typename = typename           std::enable_if
                                  <
                                    std::is_same< T_, spo::strings_t >::value
                                    or
                                    std::is_same< T_, std::string >::value
                                  >
>
void                              Dump( const T_ & value )
{
  if( std::is_same< T_, spo::strings_t >::value )
  {
    if( value.empty() )
      return;
    std::string str_dump( value );
    std::replace( str_dump.begin(), str_dump.end(), '\n',' ');
    std::cout << str_dump.c_str() << std::endl;
  }
  else
  {
    for( auto & str : value )
    {
      Dump< decltype(str) >( str );
    }
  }
}

//------------------------------------------------------------------------------
spo::strings_t  StrSplit( const std::string & str, const std::string & splitter );

inline spo::strings_t
StrSplit( const std::string & str, char splitter )
{
  return StrSplit( str, std::string( 1, splitter ) );
}

std::string     StrJoin ( const spo::strings_t & strings, const std::string & splitter );

inline std::string
StrJoin ( const spo::strings_t & strings, char splitter )
{
  return StrJoin( strings, std::string( 1, splitter ) );
}

std::string     StrToHex   ( const std::string & str );
std::string     StrFromHex ( const std::string & str );
std::string     StrChop    ( const std::string & str, std::size_t endCount );
std::string     StrReplace ( const std::string & str, const std::string & str_from, const std::string & str_to );
inline bool     StrContains( const spo::strings_t & strings, const std::string & str )
{
  return
      std::find_if( const_cast<strings_t &>(strings).begin(),
                    const_cast<strings_t &>(strings).end(),
                    [ & str ]( const typename spo::strings_t::value_type & s )
                    {
                      return str == s;
                    } )
      !=
      const_cast<strings_t &>(strings).end();
}

inline bool     StrContains( const std::string & str, const std::string & part )
{
  return str.find( part.c_str() ) != std::string::npos;
}

}// namespace               utils
}// namespace               core
}// namespace               spo

#endif // COREUTILS_H
