#include "core/devices/Devices.h"
#include "core/documents/DocumentLiterals.h"
#include "core/devices/DeviceEnumerationFactory.h"
#include <boost/filesystem.hpp>

namespace                       spo   {
namespace                       core  {
namespace                       dev   {

Devices::Devices()
  : Devices::doc_class_t( spo::core::docs::DOC_DEVICES_ID )
{
}

Devices::Devices( const Devices & d )
  : Devices::Devices()
{
  *this = d;
}

Devices::Devices( Devices && d )
  : Devices::Devices()
{
  *this = std::move( d );
}

Devices::~Devices()
{
}

bool
Devices::IsEmpty() const
{
  return mContent.empty();
}

void Devices::Scan()
{
  BEGIN_LOCK_SECTION_SELF_;
  ClearContent();
  for( auto pair : core::udev::DeviceEnumerationFactory::Enumerate () )
  {
    ( * this ) << pair.second;
  }
  END_LOCK_SECTION_
}

void Devices::Append(const spo::core::dev::Device & dev )
{
  BEGIN_LOCK_SECTION_SELF_;
    ContentRef().insert( { dev.Name(), dev } );
  END_LOCK_SECTION_
}

bool
Devices::FromByteArray(const std::string & data )
{
  BEGIN_LOCK_SECTION_SELF_;
    ClearContent();

    bool retval( ! data.empty() );
    if( retval )
    {
      for( auto & dev_info : spo::core::utils::StrSplit( spo::core::utils::StrFromHex( data ), '\n') )
      {
        if( dev_info.empty() )
          continue;

        spo::core::dev::Device dev;
        retval = dev.FromByteArray( dev_info );
        if( retval )
          Append( dev );
        else
        {
          ClearContent();
          break;
        }
      }
    }
    return retval;
  END_LOCK_SECTION_
}

std::string
Devices::ToByteArray()
const
{
  BEGIN_LOCK_SECTION_SELF_;
    std::string array;
    for( auto & val : mContent )
    {
      array += val.second.ToByteArray() + "\n";
    }
    return spo::core::utils::StrToHex( spo::core::utils::StrChop( array, 1 ) );
  END_LOCK_SECTION_
}

std::vector< spo::core::dev::Device >
Devices::Get(const spo::devprop_t::value_type& pair)
const
{
  BEGIN_LOCK_SECTION_SELF_;

  std::vector< spo::core::dev::Device > retval;
  for( auto & dev_ref : mContent )
  {
    std::pair
    <
      spo::devprop_t::const_iterator,
      bool
    > find_res = dev_ref.second.IsContains( pair.first );

    if( find_res.second )
    {
      if( spo::core::utils::StrContains( find_res.first->second, pair.second ) )
        retval.push_back( dev_ref.second );
    }
  }
  return retval;

  END_LOCK_SECTION_
}

Devices
Devices::Diff(const Devices & devices , const spo::devprop_t & prop)
{
  BEGIN_LOCK_SECTION_SELF_;
    Devices retval;
    for( auto & dev_ref : const_cast< Devices & >( devices ).mContent )
    {
      auto iter( mContent.find( dev_ref.first ) );
      if( iter == mContent.end() )
      {
        if( not Devices::ContainsPropery( dev_ref.second, prop ) )
          retval.Append( dev_ref.second );
      }
      else if( iter->second != dev_ref.second )
      {
        if( not Devices::ContainsPropery( iter->second, prop ) )
          retval.Append( iter->second );
      }
    }
    return std::move( retval );
  END_LOCK_SECTION_
}

Devices
Devices::Diff( const std::string & data, const spo::devprop_t & excluded )
{
  Devices d;
  return
      d.FromByteArray( data ) ?
        Diff( d, excluded ) :
        Devices();
}

Devices &
Devices::operator =(const Devices & document)
{
  BEGIN_LOCK_SECTION_SELF_;
    mContent = document.mContent;
    return * this;
  END_LOCK_SECTION_
}

Devices &
Devices::operator =( Devices && document)
{
  BEGIN_LOCK_SECTION_SELF_;
    mContent.swap( document.mContent );
    document.ClearContent();
    return * this;
  END_LOCK_SECTION_
}

void Devices::ClearContent()
{
  BEGIN_LOCK_SECTION_SELF_;
    ContentRef().clear();
    END_LOCK_SECTION_
}

std::string
Devices::DumpString()
{
  BEGIN_LOCK_SECTION_SELF_;
    std::string retval;
    for( auto & dev_ref : mContent )
    {
      retval += dev_ref.second.DumpString();
    }
    return retval;
  END_LOCK_SECTION_
}

void
Devices::Dump()
{
  std::cout << DumpString().c_str() << std::endl;
}

bool
Devices::ContainsPropery( const spo::core::dev::Device & device, const spo::devprop_t & prop )
{
  for( auto & p : const_cast< spo::devprop_t & >( prop ) )
  {
    if( const_cast< spo::core::dev::Device & >( device ).IsContains( p ) )
      return true;
  }
  return false;
}

//------------------------------------------------------------------------------

std::string
DevicesConfigPath()
{
  return spo::ADMIN_CONFIG_PATH;
}

std::string
DevicesConfigFile()
{
  return
      DevicesConfigPath() +
      DIR_SEPARATOR +
      spo::ARM_CONFIG_FILE;
}

bool
DevicesConfigFileExists()
{
  return
      boost::filesystem::exists(
        boost::filesystem::path(
          DevicesConfigFile() ) );
}

}// namespace                   dev
}// namespace                   core
}// namespace                   spo
