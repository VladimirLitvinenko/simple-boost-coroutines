#include "core/devices/DeviceEnumerationFactory.h"
#include "core/devices/DevicesLiterals.h"

#include <QDebug>
#include <libudev.h>
#include <memory>

namespace                       spo   {
namespace                       core  {
namespace                       udev  {

devenumeration_t
DeviceEnumerationFactory::Enumerate()
{
  devenumeration_t enumerations;

  udev::udev_t        device        ( nullptr );
  udev::enumerate_t   enumerate     ( nullptr );
  udev::listentry_t * devices       ( nullptr );
  udev::listentry_t * dev_list_entry( nullptr );

  device = ::udev_new();
  if( nullptr == device  )
  {
    return enumerations;
  }

  try
  {
    /* create enumerate object */
    enumerate = ::udev_enumerate_new(device);
    if( nullptr == enumerate )
    {
      return enumerations;
    }

    udev_enumerate_add_match_subsystem( enumerate, BLOCK_ID_PATH.toStdString().c_str() );
    udev_enumerate_scan_devices       ( enumerate );

    devices = ::udev_enumerate_get_list_entry(enumerate);
    if( nullptr == devices)
    {
      return enumerations;
    }

    udev_list_entry_foreach( dev_list_entry, devices )
    {
      std::string name = udev_list_entry_get_name( dev_list_entry );
      auto dev( udev_device_new_from_syspath( device, name.data() ) );

      if( nullptr != dev )
      {
//        if(
////           ( strncmp(udev_device_get_devtype(dev), "partition", 9) != 0 )
////           and
//           ( ! QString(udev_device_get_sysname(dev)).contains("loop") )
//        )
        {
          std::string node_name = udev_device_get_devnode( dev );

          auto list_sys = udev_device_get_sysattr_list_entry( dev);
          while ( nullptr != list_sys )
          {
            std::string n = udev_list_entry_get_name(list_sys);
            list_sys = udev_list_entry_get_next(list_sys);
          }

          spo::devprop_t proprty_map;
          auto list_entry = udev_device_get_properties_list_entry(dev);

          while ( nullptr != list_entry )
          {
            proprty_map[ udev_list_entry_get_name(list_entry) ] =
                udev_list_entry_get_value(list_entry);
            list_entry = udev_list_entry_get_next(list_entry);
          }
          enumerations[ node_name ] = std::move( spo::core::dev::Device(
                                      node_name,
                                      core::udev::DeviceAction::None,
                                      proprty_map ) );
        }
        ::udev_device_unref( dev) ;
      }
    }
    ::udev_enumerate_unref( enumerate );
  }
  catch( const std::exception & e)
  {
    ::udev_enumerate_unref( enumerate );
    enumerations.clear();
    DUMP_EXCEPTION( e );
  }
  ::udev_unref( device );

  return enumerations;
}

spo::devprop_t
DeviceEnumerationFactory::FindEnumeration
(
    const devenumeration_t  & enumerations,
    std::string             & deviceName
)
{
  auto key_iter ( enumerations.find( deviceName ) );
  return
      ( key_iter != enumerations.end() )
      and
      key_iter->second.IsValid() ?
        key_iter->second.Content():
        spo::devprop_t();
}

}// namespace                   udev
}// namespace                   spo
}// namespace                   core
