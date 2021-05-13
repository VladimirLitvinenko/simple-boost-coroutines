#ifndef STEADYTIMER_H
#define STEADYTIMER_H

#include "asio/AsioCommon.h"

namespace                         spo   {
namespace                         asio  {

//------------------------------------------------------------------------------
/**
 * @brief The SteadyTimer class
 */
class SPO_CORE_EXPORT             SteadyTimer
{
public:
  /**/                            SteadyTimer   ( io_service_t & service )
                                  : m_Timer     ( service )
  {}
  /**/                            SteadyTimer   ( io_service_t & service, const boost::int64_t deadLine )
                                : SteadyTimer::SteadyTimer( service )
  {
    m_Deadline.store(
        deadLine > -1 ?
          deadLine :
          0 );
  }

  asio_steady_timer_t           & TimerRef            () { return std::ref( m_Timer ); }
  boost::int64_t                  Deadline            () { return m_Deadline; }
  void                            SetDeadLine         ( const boost::int64_t & deadline )
    { m_Deadline = deadline; }
  bool                            IsActive            () const
    { return m_Active; }
  void                            Start               ()
  {
    if( not IsActive() )
    {
      m_Timer.expires_from_now( std::chrono::milliseconds( Deadline() ) );
      m_Active = true;
    }
  }
  void                            Stop                ()
  {
    m_Timer.cancel();
    m_Active = false;
  }
  bool                            IsExpired           ( boost::system::error_code ignored_ec ) const
  {
    return
        IsActive() ?
          ( m_Timer.expires_from_now() <= std::chrono::milliseconds( 0 ) )
          and
          ( ignored_ec != boost::system::errc::operation_canceled ) :
          true;
  }

private:
  asio_steady_timer_t             m_Timer;
  std::atomic<boost::int64_t>     m_Deadline          { boost::int64_t( ASIO_DEADLINE_DEFAULT ) };
  std::atomic_bool                m_Active            { false };
};

//------------------------------------------------------------------------------
using deadline_timer_shared_t   = std::shared_ptr< SteadyTimer >;

//------------------------------------------------------------------------------
}// namespace                   asio
}// namespace                   spo


#endif // STEADYTIMER_H
