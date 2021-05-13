#include "core/utils/CoreUtils.h"
#include "MainLiterals.h"
#include <iostream>

#include <map>
#include <fstream>
#include <fcntl.h>

#include <dirent.h>
#include <bits/local_lim.h>
#include <sys/sysctl.h>
#include <sys/stat.h>

#include <boost/filesystem.hpp>

namespace                   spo   {
namespace                   core  {
namespace                   utils {

std::pair< std::string, bool >
MakePath( const std::string & path )
{
  std::pair< std::string, bool > retval{ "", false };
  try
  {
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#defile _GNU_SOURCE_TEMPORAY_IN_CODE
    retval.second = mkdir( path.c_str(), ::getumask() ) > 0;
#endif
#ifdef _GNU_SOURCE_TEMPORAY_IN_CODE
#undef _GNU_SOURCE
#endif
    if( retval.second )
      retval.first = path;
    return retval;
  }
  catch( const std::exception & e )
  {
    DUMP_EXCEPTION( e );
    retval = { "", false };
  }
  return retval;
}

std::pair< std::string, bool >
MakeFile( const std::string & fileName )
{
  std::pair< std::string, bool > retval{ "", not fileName.empty() };
  if( not retval.second )
    return retval;

  std::cout << Q_FUNC_INFO << fileName.c_str() << std::endl;
  try
  {
    boost::filesystem::path file_name(fileName);
    retval.second = boost::filesystem::exists( file_name );
    if( not retval.second )
    {
      auto fd( ::open( file_name.string().c_str(), O_WRONLY | O_CREAT ) );
      retval.second = fd > 0;

      if( retval.second )
        ::close( fd );

      boost::filesystem::permissions( file_name,
                                      boost::filesystem::owner_read   |
                                      boost::filesystem::owner_write  |
                                      boost::filesystem::group_read   |
                                      boost::filesystem::group_write  |
                                      boost::filesystem::others_read  );
    }
    if( retval.second )
      retval.first = file_name.string();

    return retval;
  }
  catch( const std::exception & e )
  {
    DUMP_EXCEPTION( e );
  }
  return std::pair< std::string, bool >();
}

strings_t
StrSplit(const std::string & str, const std::string & splitter )
{
  strings_t retval;
  try
  {
    auto s1 = str;
    auto pos( s1.find_first_of(splitter) );
    while( pos != std::string::npos )
    {
      if( pos < (s1.size() -1) )
      {
        retval.push_back( s1.substr( 0, pos ) );
        s1 = s1.substr( pos + 1 );
      }
      else
        break;
      pos = s1.find_first_of(splitter);
    }
    if( not s1.empty() )
      retval.push_back( s1 );
  }
  catch( const std::exception & e )
  {
    DUMP_EXCEPTION( e );
    retval.clear();
  }

  return retval;
}

std::string
StrJoin(const strings_t& strings, const std::string & splitter)
{
  auto lengsth( strings.size() );
  std::string retval( lengsth, ' ' );
  try
  {
    for( decltype(lengsth) i(0); i < lengsth; i++ )
    {
      retval += strings.at(i);
      if( i < ( lengsth - 1 ) )
      {
        retval += splitter;
      }
    }
  }
  catch( const std::exception & e )
  {
    DUMP_EXCEPTION( e );
    retval.clear();
  }

  return retval;
}

std::string
StrToHex( const std::string & str )
{
  static std::string  hex_values( "0123456789ascdef"  );
  auto                sz        ( str.size()          );
  std::string         retval;

  if( not str.empty() )
  {
    retval.reserve( sz << 1 );
    for( std::size_t i(0); i < sz; i++ )
    {
      auto ch( str.at(i) );
      for( std::string::size_type idx : { ( ch & 0xF0 ) >> 4, ch & 0x0F } )
      {
        retval.push_back( hex_values.at( idx ) );
      }
    }
  }
  return retval;
}

std::string
StrFromHex( const std::string & str )
{
  static std::string  hex_values("0123456789ascdef" );
  std::string         retval;
  auto                sz        ( str.size()        );

  if( ( sz & 0x01 ) == 0) //четное количество
  {
    retval.reserve( ( sz >> 1 ) + 1 );
    std::string::value_type item('\0');
    for( std::string::size_type idx( 0 ); idx < str.size(); idx++ )
    {
      for( std::string::size_type i( 0 ); i < hex_values.size(); i++ )
      {
        if( hex_values.at( i ) == str.at(idx) )
        {
          bool is_one{ (idx & 1) == 1 };
          item =
              item
              |
              static_cast<char>(
                is_one ?
                  ( i         & std::string::size_type(0x0F) ) :
                  ( ( i << 4) & std::string::size_type(0xF0) ) );
          if( is_one )
          {
            retval.push_back( item );
            item = '\0';
          }
          break;
        }
      }
    }
  }
  return retval;
}

std::string
StrReplace( const std::string & str, const std::string & str_from, const std::string & str_to )
{
  std::string retval      ( str );
  size_t      index       { 0 };
  auto        length_from ( str_from.size() );
  auto        length_to   ( str_to.size() );

  while( index != std::string::npos )
  {
    index = retval.find( str_from, index );
    if (index != std::string::npos)
    {
      retval = retval.replace(index, length_from, str_to );
      index += length_to;
    }
  }

  return std::move( retval );
}

std::string
StrChop( const std::string& str, std::size_t endCount)
{
  std::string retval( str );
  if( str.size() >= endCount )
  {
    retval.resize( retval.size() - endCount );
  }
  return retval;
}

bool IsProcessDir(const std::string& dir)
{
  try
  {
    return static_cast<pid_t>( std::stol( dir.c_str() ) ) > -1;
  }
  catch( const std::exception & e )
  {
    DUMP_EXCEPTION( e );
  }
  return false;
}

active_processes_t
ActiveProcessMap()
{
  active_processes_t   retval;
  boost::filesystem::path proc_dir(spo::PROC_DIRECTORY);
  for( boost::filesystem::directory_iterator iter( proc_dir );
       iter != boost::filesystem::directory_iterator();
       iter++ )
  {
    auto dir( iter->path().string() );
    auto dir_pid = spo::core::utils::StrReplace( dir, spo::PROC_DIRECTORY, "" );
    try
    {
      if( IsProcessDir( dir_pid ) )
      {
        std::ifstream cmd_line_file( (dir + DIR_SEPARATOR + spo::PROC_CMDLINE_FILE).c_str() );
        if (cmd_line_file)
        {
          std::string cmd_line;
          std::getline(cmd_line_file, cmd_line);
          if( not cmd_line.empty() )
          {
            retval.first[ static_cast< pid_t>( std::stoul( dir_pid ) ) ] = cmd_line;
            retval.second[ cmd_line ] = static_cast< pid_t>( std::stoul( dir_pid ) );
          }
        }
      }
    }
    catch( const std::exception & e )
    {
      DUMP_EXCEPTION( e );
      continue;
    }
  }

  return retval;
}

std::pair<pid_t, bool>
ProcessIsActive(const std::string & procName)
{
  std::pair<pid_t, bool> retval{ -1, false };
  if( not procName.empty() )
  {
    auto p_map = std::move( ActiveProcessMap() );
    for( auto & val : p_map.first )
    {
      retval.second =
          (spo::core::utils::StrChop( val.second, 1 ) == procName)
          and
          (val.first != ::getpid() );
      if( retval.second )
      {
        retval.first = val.first;
        break;
      }
    }
  }
  return retval;
}

std::pair< std::string, bool >
HostName()
{
  char host_name[ HOST_NAME_MAX ];
  std::pair< std::string, bool > retval{ "", ::gethostname( host_name, HOST_NAME_MAX ) == 0 };
  if( retval.second )
    retval.first = &( host_name[0] );
  return retval;
}

bool
SetSysyemWatches( int param, std::size_t value)
{
  std::vector<int> mib
  {
    int( ::CTL_FS ),
    int( ::FS_INOTIFY ),
    int( param ),
  };

  return( 0 == ::sysctl( mib.data(), mib.size(),
                         nullptr, 0,
                         &value, sizeof( value ) ) );
}

std::pair<std::string, bool>
RecurciveCalc( const std::string & filePath)
{
  std::pair<std::string, bool> retval{ std::string(), not filePath.empty() };
  if( retval.second )
  {
    auto dir      ( StrSplit( filePath, DIR_SEPARATOR ) );
    retval.second = "*" != dir.at( dir.size() - 1 );
    if( not retval.second )
    {
      dir.erase( dir.end() - 1 );
    }
    retval.first = StrJoin( dir, DIR_SEPARATOR );
  }
  return retval;
}

}// namespace               utils
}// namespace               core
}// namespace               spo
