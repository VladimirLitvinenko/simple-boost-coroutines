#include "core/devices/Device.h"
#include "core/documents/DocumentLiterals.h"

namespace                       spo   {
namespace                       core  {
namespace                       dev   {

Device::Device()
  : Device::doc_class_t   ( spo::core::docs::DOC_DEVICE_ID )
  , mAction               ( core::udev::DeviceAction::None )
{

}

Device::Device
(
    const std::string             & name,
    const core::udev::DeviceAction& action,
    const spo::devprop_t          & enumerations
)
  : Device::Device()
{
  BEGIN_LOCK_SECTION_SELF_;
  SetName   ( name );
  SetAction ( action );
  SetContent( enumerations );
  END_LOCK_SECTION_;
}

Device::Device(const Device & dev )
  : Device::Device
    (
      const_cast<Device &>(dev).Name(),
      const_cast<Device &>(dev).Action(),
      const_cast<Device &>(dev).ContentRef()
    )
{
}

Device::Device( Device && rRef )
  : Device::Device ()
{
  *this = std::move( rRef );
}

Device::Device(const spo::devprop_t & prop )
  : Device::Device ()
{
  *this = prop;
}

Device::~Device()
{
}

std::pair
<
  spo::devprop_t::const_iterator,
  bool
>
Device::IsContains(const spo::devprop_t::key_type& pair)
const
{
  std::pair< spo::devprop_t::const_iterator, bool > retval
  {
        mContent.cend(),
        IsValid() and ( not mContent.empty() )
  };
  if( retval.second )
  {
    retval.first = mContent.find( pair );
    retval.second = retval.first != mContent.cend();
  }
  return retval;
}

bool
Device::IsContains( const spo::devprop_t::value_type & pair )
const
{
  auto is_key = IsContains( pair.first );
  if( is_key.second )
  {
    return is_key.first->second == pair.second;
  }
  return false;
}

bool
Device::IsStrongEqual( const Device & device )
const
{
  bool result( device.IsValid() and ( not device.mContent.empty() ) );
  for( auto iter( device.mContent.cbegin() );
       ( iter != device.mContent.cend() ) and result;
       ++iter
  )
  {
    result = IsContains( * iter );
  }
  return result;
}

//bool
//Device::IsEqualWithExclude(const Device & device, const spo::devprop_t & exclude )
//const
//{
//  bool result( IsStrongEqual( device) );
//  if( not result )
//  {
//    for( auto iter( exclude.cbegin() ); ( iter != exclude.cend() ) and ( not result ); ++iter )
//    {
//      result = const_cast< Device & >( device ).IsContains( * iter );
//    }
//  }
//  return result;
//}

Device &
Device::operator =(const Device & lRef )
{
  BEGIN_LOCK_SECTION_SELF_;
  SetName   ( const_cast<Device &>(lRef).Name()    );
  SetAction ( const_cast<Device &>(lRef).Action()  );
  FromByteArray( const_cast<Device &>(lRef).ToByteArray() );

  return *this;
  END_LOCK_SECTION_
}

Device &
Device::operator =(const spo::devprop_t& prop )
{
  ClearContent();
  return operator <<( prop );
}

Device &
Device::operator =( Device && rRef )
{
  BEGIN_LOCK_SECTION_SELF_;
  mName.swap( rRef.mName );
  SetAction ( rRef.Action()  );
  mContent.swap( rRef.mContent );
  rRef.ClearContent();
  rRef.mName.clear();
  rRef.mAction = core::udev::DeviceAction::None;

  return *this;
  END_LOCK_SECTION_
}

Device &
Device::operator <<
(
    const std::pair
    <
      const spo::devprop_t::key_type &,
      const spo::devprop_t::mapped_type &
    > & value
)
{
  BEGIN_LOCK_SECTION_SELF_;
  ContentRef().insert( { value.first, value.second } );
  return *this;
  END_LOCK_SECTION_
}

Device &
Device::operator <<( const spo::devprop_t & rMap )
{
  Append( rMap );
  return *this;
}

bool
Device::IsValid() const
{
  return Device::doc_class_t::IsValid() and ( not Name().empty() );
}

spo::strings_t
Device::StringsContent(const std::string & separator)
{
BEGIN_LOCK_SECTION_SELF_;
  spo::strings_t retval;
  if( doc_class_t::IsValid() )
  {
    for( spo::devprop_t::value_type pair : mContent )
    {
      retval.push_back(
            pair.first +
            separator +
            pair.second );
    }
  }
  return retval;
END_LOCK_SECTION_
}

void
Device::Append( const spo::devprop_t& vMap )
{
  BEGIN_LOCK_SECTION_SELF_;
  for( auto pair : vMap )
  {
    ContentRef().insert( { pair.first, pair.second } );
  }
  END_LOCK_SECTION_
}

bool
Device::FromByteArray( const std::string& data )
{
  BEGIN_LOCK_SECTION_SELF_;

  bool retval( not data.empty() );
  if( retval )
  {
    // list состоит из:
    // .at(0) - набор одиночных атрибутов в HEX-виде
    // .at(1) - карта свойств устройства в HEX-виде
    auto list = spo::core::utils::StrSplit(
                  spo::core::utils::StrFromHex( data ),
                  "\n");
    retval = list.size() == 2;

    if( retval )
    {
      // одиночные атрибуты
      auto list1 = spo::core::utils::StrSplit(
                    spo::core::utils::StrFromHex( list.at(0) ),
                    "\n");
      retval = list1.size() == 3;
      if( retval )
      {
        GetIDRef() = std::stoi( list1.at(0) );
        SetAction( static_cast< core::udev::DeviceAction >( list1.at(1).empty() ? 0 : std::stoi( list1.at(1) ) ) );
        SetName( list1.at(2) );

        // карта свойств устройства
        auto list2 = spo::core::utils::StrSplit(
                      spo::core::utils::StrFromHex( list.at(1) ),
                      "\n");
        spo::devprop_t infoMap;
        for( auto d : list2 )
        {
          auto pair = spo::core::utils::StrSplit( d , '=' );
          if( 2 == pair.size() )
          {
            infoMap[ pair[0] ] = pair[1];
          }
        }
        SetContent( infoMap );
      }
    }
  }
  return retval;

  END_LOCK_SECTION_
}

std::string
Device::ToByteArray()
const
{
  BEGIN_LOCK_SECTION_SELF_;

  std::string second_part;

  // карта свойств устройства
  for( auto & pair : mContent )
  {
    second_part += pair.first + "=" + pair.second + '\n';
  }

  return
      spo::core::utils::StrToHex
      (
        // одиночные атрибуты
        spo::core::utils::StrToHex(
          std::to_string( GetID() ) +
          "\n" +
          std::to_string( static_cast<int>( Action() ) ) +
          "\n" +
          Name() )
        + "\n" +
        // карта свойств устройства
        spo::core::utils::StrToHex(
          spo::core::utils::StrChop( second_part, 1 ) )
      );

  END_LOCK_SECTION_
}

void
Device::ClearContent()
{
  BEGIN_LOCK_SECTION_SELF_;
  ContentRef().clear();
  END_LOCK_SECTION_
}

void
Device::Dump()
{
  std::cout << DumpString().c_str() << std::endl;
}

std::string
Device::DumpString()
{
  BEGIN_LOCK_SECTION_SELF_;
  std::string retval = "Устройство " + Name() + "\nПараметры:\n";
  for( auto & maped_ref : ContentRef() )
  {
    retval += std::string("\t") +
              maped_ref.first +
              " : " +
              maped_ref.second +
              "\n";
  }
  return retval;
  END_LOCK_SECTION_
}

std::string
Device::Name() const
{
  return mName;
}

core::udev::DeviceAction
Device::Action() const
{
  return mAction;
}

void
Device::SetAction(const core::udev::DeviceAction& action)
BOOST_NOEXCEPT
{
  BEGIN_LOCK_SECTION_SELF_;
  mAction = action;
  END_LOCK_SECTION_
}

void
Device::SetName(const std::string & name )
{
  BEGIN_LOCK_SECTION_SELF_;
  mName = name;
  END_LOCK_SECTION_
}

}// namespace                   dev
}// namespace                   core
}// namespace                   spo
