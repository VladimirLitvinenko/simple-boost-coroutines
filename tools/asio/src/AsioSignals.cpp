#include "asio/AsioSignals.h"

namespace                       spo   {
namespace                       asio  {

AsioSignals::AsioSignals        ( const spo::asio::AsioService & service )
  : m_ServiceRef                ( const_cast< spo::asio::AsioService & >( service ) )
  , m_Signals                   ( const_cast< spo::asio::AsioService & >( service ).ServiceRef() )
{

}

AsioSignals::~AsioSignals()
{

}

error_t AsioSignals::AddSignal(int signalNumber)
{
  error_t err;
  err = this->m_Signals.add( signalNumber, err );
  return err;
}

error_t
AsioSignals::RemoveSignal( int signalNumber )
{
  error_t err;
  auto iter( m_SignalActions.find( signalNumber) );
  if( iter != m_SignalActions.end() )
  {
    m_SignalActions.erase( iter );
  }
  err = this->m_Signals.remove( signalNumber, err );
  return err;
}

error_t
AsioSignals::AddSignalHandler
(
    int                           signalNumber,
    const asio_signal_action_t  & method
)
{
  auto err = AddSignal( signalNumber );
  if( IsNoErr( err ) )
  {
    m_SignalActions[ signalNumber ] = method;
  }
  return err;
}

error_t
AsioSignals::AddSignalHandler
(
    int                     signalNumber,
    asio_signal_action_t && method
)
{
  auto err = AddSignal( signalNumber );
  if( IsNoErr( err ) )
  {
    m_SignalActions[ signalNumber ] = std::move( method );
  }
  return err;
}

void AsioSignals::SetSignalsDefault()
{
  auto lambda_stop = [this]( int , error_t& e)
  {
    m_ServiceRef.Stop();
    return e;
  };
  auto lambda_restart = [this]( int , error_t& e)
  {
    m_ServiceRef.Stop();
    m_ServiceRef.Start();
    return e;
  };
  auto lambda_start = [this]( int , error_t& e)
  {
    m_ServiceRef.Start();
    return e;
  };
  auto lambda_exit = [this]( int , error_t& e)
  {
    m_ServiceRef.Stop();
    exit( IsNoErr(e) ? 0 : 1 );
    return e;
  };

  AddSignalHandler( SIGHUP  , lambda_restart );
  AddSignalHandler( SIGINT  , lambda_stop );
  AddSignalHandler( SIGQUIT , lambda_stop );
  AddSignalHandler( SIGTRAP , lambda_exit );
  AddSignalHandler( SIGABRT , lambda_exit );
  AddSignalHandler( SIGBUS  , lambda_restart );
  AddSignal( SIGFPE ); // ignore
  AddSignalHandler( SIGSEGV , lambda_exit );
  AddSignalHandler( SIGPIPE , lambda_restart );
  AddSignalHandler( SIGTERM , lambda_stop );

  AddSignalHandler( SIGSTOP , lambda_stop );
  AddSignalHandler( SIGCONT , lambda_start );
  AddSignal( SIGTSTP );
  AddSignal( SIGTTIN ); // ignore
  AddSignal( SIGTTOU ); // ignore
  AddSignal( SIGURG );
  AddSignalHandler( SIGXCPU , lambda_restart );
  AddSignal( SIGXFSZ );

  AddSignal( SIGVTALRM );
  AddSignal( SIGPROF );

  AddSignal( SIGWINCH );
  AddSignal( SIGPOLL );
  AddSignal( SIGIO );
  AddSignalHandler( SIGPWR  , lambda_stop );
  AddSignalHandler( SIGSYS  , lambda_stop );

  m_Signals.async_wait( boost::bind( &AsioSignals::SignalHandler, this, _1, _2 ) );
}

void
AsioSignals::SignalHandler(const error_t& error, int signal_number)
{
  if( IsNoErr( error) )
  {
    auto iter( m_SignalActions.find( signal_number ) );
    if( iter != m_SignalActions.end() )
    {
      error_t e( error );
      e = iter->second( signal_number, e );
    }
  }
  m_Signals.async_wait( boost::bind( &AsioSignals::SignalHandler, this, _1, _2 ) );
}

}// namespace                   asio
}// namespace                   spo
