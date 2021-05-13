#include "asio/AsioService.h"
#include <boost/thread.hpp>
#include <boost/system/error_code.hpp>
#include <boost/utility/in_place_factory.hpp>

namespace                       spo   {
namespace                       asio  {

//------------------------------------------------------------------------------

AsioService::AsioService        ( const std::int64_t & timeoutMs )
  : m_TimeoutMs                 ( timeoutMs )
{
  SetDefaultErrorCallbacks();
}

bool
AsioService::ActionExists( const IoServiceActionType & aType )
BOOST_NOEXCEPT
{
  auto iter( m_Actions.find( aType ) );
  return
      iter != m_Actions.end()
              ? not iter->second.empty()
              : false;
}

void
AsioService::AddBeforeStartCallback( io_service_callbacks_t::value_type action)
BOOST_NOEXCEPT
{
  m_Actions[ BeforeStart ].push_back( action );
}

bool
AsioService::BeforeStartCallbackExists()
BOOST_NOEXCEPT
{
  return ActionExists( BeforeStart );
}

void
AsioService::AddBeforeStopCallback( io_service_callbacks_t::value_type action)
BOOST_NOEXCEPT
{
  m_Actions[ BeforeStop ].push_back( action );
}

void
AsioService::AddAfterStopCallback( io_service_callbacks_t::value_type  action )
BOOST_NOEXCEPT
{
  m_Actions[ AfterStop ].push_back( action );
}

void
AsioService::ClearCallbacks()
BOOST_NOEXCEPT
{
  m_Actions.clear();
}

bool
AsioService::Start()
{
  bool retval( IsActive() );
  RunServiceCallbacks( IoServiceActionType::BeforeStart );
  if( not retval )
  {
    try
    {
//      RunServiceCallbacks( IoServiceActionType::BeforeStart );
      retval = true;
      RunThreadService();
    }
    catch( const std::exception & e )
    {
      DUMP_EXCEPTION( e );
      retval = false;
      SetError( boost::system::errc::errc_t::connection_aborted );
    }
  }
  return retval;
}

void
AsioService::Poll()
{
  if( IsActive() )
    return;

  try
  {
    RunServiceCallbacks( IoServiceActionType::BeforeStart );
    m_Service.poll();
    RunServiceCallbacks( IoServiceActionType::AfterStop );
  }
  catch( const std::exception & e )
  {
    DUMP_EXCEPTION( e );
    SetError( boost::system::errc::errc_t::owner_dead );
  }
}

void
AsioService::Stop()
BOOST_NOEXCEPT
{
  try
  {
    RunServiceCallbacks( IoServiceActionType::BeforeStop );
    if( not IsActive() )
      return;

    m_WorkPtr.reset();

    auto stop_future = std::async
    (
      std::launch::async,
      [ this ]()
      {
        this->m_Service.stop();
        this->m_Service.reset();
      }
    );
    if( stop_future.wait_for( std::chrono::milliseconds( m_TimeoutMs ) )
        ==
        std::future_status::ready )
      stop_future.get();

//    if( IsActive() )
//      throw boost::system::errc::connection_aborted;
  }
  catch( const std::exception & e )
  {
    DUMP_EXCEPTION( e );
  }
}

void
AsioService::RunServiceCallbacks(const io_service_callbacks_map_t::key_type & key)
BOOST_NOEXCEPT
{
  auto iter( m_Actions.find( key ) );
  if( iter != m_Actions.end() )
  {
    for( auto i(iter->second.end()-1 ); i >= iter->second.begin(); --i )
    {
      (i->first)( i->second );
    }
  }
}

void
AsioService::SetDefaultErrorCallbacks()
BOOST_NOEXCEPT
{
  SetErrorCallbacks
      ( {
          { // обработка исключения exception
            boost::system::errc::errc_t::owner_dead,
            boost::bind( &AsioService::Stop, this )
          },
        } );
}

void
AsioService::RunService()
BOOST_NOEXCEPT
{
  if( IsActive() )
  {
    return;
  }

  auto tm = std::chrono::system_clock::to_time_t( std::chrono::system_clock::now() );
  std::string str( std::ctime( & tm ) );
  std::cout << "\n\t\t--- Try START AsioService, Time : " << str.c_str() << " ---\n";
  std::flush( std::cout );

  try
  {
    error_t ec;
    m_Active = true;
    m_WorkPtr = std::make_shared< asio_workuptr_t::element_type >( ServiceRef() );
    m_Service.run( ec );
    m_Service.reset();
    m_Active = false;

    std::cout << "\n\t\t--- AsioService STOPPED, Time : " << str.c_str() << " ---\n";
    std::flush( std::cout );

//    RunServiceCallbacks( IoServiceActionType::AfterStop );
    std::async( std::launch::async,
                &AsioService::RunServiceCallbacks,
                this,
                IoServiceActionType::AfterStop );
    return;
  }
  catch( const std::exception & e )
  {
    DUMP_EXCEPTION( e );
  }
  m_WorkPtr.reset();
  m_Active = false;
}

void
AsioService::RunThreadService()
BOOST_NOEXCEPT
{
  if( not IsActive() )
  {
    std::make_shared<threadptr_t::element_type>(
            boost::bind( & AsioService::RunService,
                         this ) )->detach();
    DUMP_INFO( "THREAD for ServiceRef().run() STARTED...");
  }
}

}// namespace                   asio
}// namespace                   spo
