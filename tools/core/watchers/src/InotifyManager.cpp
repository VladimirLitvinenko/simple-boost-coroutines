#include "core/watchers/InotifyManager.hpp"
#include "core/utils/CoreUtils.h"
#include "boost/filesystem.hpp"
#include <iostream>

namespace                       spo   {
namespace                       core  {
namespace                       watch {

//------------------------------------------------------------------------------

InotifyManager::InotifyManager()
{
  SetRunHandler(
        [ this ]()
        {
          for( auto event( inotifytools_next_event( 0 ) );
                            nullptr != event;
                            event = inotifytools_next_event( 0 ) )
          {
            this->ParceEvent( event );
            boost::this_thread::yield();
          }
        } );
}

InotifyManager::~InotifyManager() = default;

bool
InotifyManager::IsActive()
const
{
  return not m_Flags.empty();
}

bool
InotifyManager::IsValid()
const
{
  return
      m_Handlers.m_Run.operator bool()
      and
      ( not m_Actions.empty() );
}

void
InotifyManager::Run
(
    const notify_actions_t  & actions,
    const NotifyFlagset     & flags,
    const notify_pathes_t   & excluded,
    const std::string       & ignore_by_regex
)
{
  m_Actions = actions;
  m_Flags   = flags;

  if( not Init( excluded, ignore_by_regex ) )
  {
    if( m_Handlers.m_OnError )
      m_Handlers.m_OnError( EUCLEAN, strerror( EUCLEAN ) );
    return;
  }
  m_Handlers.m_Run();
}

std::string
InotifyManager::DumpStdString(const NotifyFlagset & flags )
{
  std::string retval;
  if( flags.find( NotifyFlag::Access ) != flags.end() )
    retval += "ACCESS ";
  if( flags.find( NotifyFlag::Modify ) != flags.end() )
    retval += "MODIFY ";
  if( flags.find( NotifyFlag::Attributes ) != flags.end() )
    retval += "ATTRIB ";
  if( flags.find( NotifyFlag::CloseAfterWrite ) != flags.end() )
    retval += "CLOSE_WRITE ";
  if( flags.find( NotifyFlag::CloseAfterNoWrite ) != flags.end() )
    retval += "CLOSE_NOWRITE ";
  if( flags.find( NotifyFlag::Open ) != flags.end() )
    retval += "OPEN ";
  if( flags.find( NotifyFlag::Close ) != flags.end() )
    retval += "CLOSE ";
  if( flags.find( NotifyFlag::MovedFrom ) != flags.end() )
    retval += "MOVED_FROM_PLACE ";
  if( flags.find( NotifyFlag::MovedTo ) != flags.end() )
    retval += "MOVED_TO_NEW_PLACE ";
  if( flags.find( NotifyFlag::Create ) != flags.end() )
    retval += "CREATE ";
  if( flags.find( NotifyFlag::Delete ) != flags.end() )
    retval += "DELETE ";
  if( flags.find( NotifyFlag::DeleteSelf ) != flags.end() )
    retval += "DELETE_SELF ";
  if( flags.find( NotifyFlag::MoveSelf ) != flags.end() )
    retval += "MOVE_SELF ";
  if( flags.find( NotifyFlag::NonBlock ) != flags.end() )
    retval += "NONBLOCK ";
  if( flags.find( NotifyFlag::FileWasIgnored ) != flags.end() )
    retval += "FILE_IGNORED ";
  return retval;
}

NotifyFlagset
InotifyManager::Flags()
const BOOST_NOEXCEPT
{
  return m_Flags;
}

void
InotifyManager::SetErrorHandler(const notify_error_handler_t & err_handler )
BOOST_NOEXCEPT
{
  m_Handlers.m_OnError = err_handler;
}

void
InotifyManager::SetRunHandler(const notify_run_handler_t& run_handler )
BOOST_NOEXCEPT
{
  m_Handlers.m_Run = run_handler;
}

void
InotifyManager::SetStopHandler(const notify_stop_handler_t & stop_handler )
BOOST_NOEXCEPT
{
  m_Handlers.m_OnStop = stop_handler;
}

void
InotifyManager::DumpFlags( const NotifyFlagset & flags )
{
  std::cout << "\tINotify Flags:" << DumpStdString(flags) << std::endl;
}

strings_t
InotifyManager::Pathes() const
{
  strings_t retval;
  for( auto & pair_ref : m_Actions )
    retval.push_back( pair_ref.first );
  return retval;
}

notify_actions_t
InotifyManager::Actions() const
{
  return m_Actions;
}

bool
InotifyManager::Init
(
    const notify_pathes_t & excluded,
    const std::string     & ignore_by_regex
)
BOOST_NOEXCEPT
{
  bool retval( ( not m_Actions.empty() ) or ( 0 != excluded.size() ) );
  errno = retval ? 0 : EINVAL;
  if( retval )
  {
    retval =  0 != inotifytools_initialize();
    errno = inotifytools_error();
  }

  if( retval )
  {
    inotifytools_initialize_stats();

    auto exc_size( excluded.size() );
    const char** excl_array( new const char*[ exc_size + 1 ] );
    try
    {
      for( std::size_t idx(0); idx < exc_size; idx++ )
      {
        excl_array[idx] = const_cast< char* >( const_cast<notify_pathes_t &>(excluded)[ idx ].data() );
      }
      excl_array[exc_size] = nullptr;

      for( auto & act_ref : m_Actions )
      {
        if( retval )
        {
          auto str_ptr = act_ref.first.c_str();
          retval = nullptr != str_ptr;
          if( not retval )
          {
            errno = ENODATA;
          }
          else
          {
            auto key( PrepareKey() );
            excluded.empty() ?
              inotifytools_watch_recursively( str_ptr, key ) :
              inotifytools_watch_recursively_with_exclude( str_ptr,
                                                           key,
                                                           excl_array );
            if( ( not ignore_by_regex.empty() ) and
                ( nullptr != ignore_by_regex.data() )
            )
            {
              retval =
                  0 != inotifytools_ignore_events_by_regex(
                         ignore_by_regex.c_str(),
                         IN_ALL_EVENTS );
            }
            errno = inotifytools_error();
          }
        }
      }
    }
    catch( const std::exception & e )
    {
      DUMP_EXCEPTION( e );
      errno = ENODATA;
    }
    delete[] excl_array;
  }
  if( ( not retval ) and ( m_Handlers.m_OnError ) )
    m_Handlers.m_OnError( errno, strerror( errno ) );

  retval = 0 == errno;
  return retval;
}

void
InotifyManager::ParceEvent( struct inotify_event * event )
BOOST_NOEXCEPT
{
  if( nullptr == event )
    return;

  std::array< char, PATH_MAX+1 > file_name;

  if( 0 == inotifytools_snprintf(
        file_name.data(),
        PATH_MAX,
        event,
        const_cast<char*>(std::string("%w%f").c_str()) )
    )
  {
    if( m_Handlers.m_OnError )
      m_Handlers.m_OnError( EFAULT, strerror( EFAULT ) );
    return;
  }

  boost::filesystem::path file( file_name.data() );
  for( auto & act_ref : m_Actions )
  {
    auto f_str( file.string() );
    if( f_str.find( act_ref.first ) != std::string::npos )
    {
      if( act_ref.second )
      {
        act_ref.second( f_str, PrepareFlags( event->mask ) );
      }
    }
  }
}

unsigned int
InotifyManager::PrepareKey()
{
  auto retval( static_cast<unsigned int>( IN_NONBLOCK | IN_IGNORED ) );
  for( auto & val_ref : m_Flags )
  {
    retval |= static_cast<unsigned int>(val_ref);
  }
  return retval;
}

NotifyFlagset
InotifyManager::PrepareFlags( const decltype( inotify_event::mask ) mask )
{
  return NotifyFlagset{ static_cast< NotifyFlag >( mask ) };
}

}// namespace                   watch
}// namespace                   spo
}// namespace                   core

