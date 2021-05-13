#ifndef ASIOSIGNALS_H
#define ASIOSIGNALS_H

#include "asio/AsioService.h"
#include "asio/AsioService.h"

namespace                       spo   {
namespace                       asio  {

//------------------------------------------------------------------------------
class SPO_CORE_EXPORT             AsioSignals
{
public:
  explicit                        AsioSignals( const spo::asio::AsioService & );
  virtual                       ~ AsioSignals();

  error_t                    AddSignal           ( int signalNumber );
  error_t                    RemoveSignal        ( int signalNumber );
  virtual error_t            AddSignalHandler
  (
      int                         signalNumber,
      const asio_signal_action_t& method
  );
  virtual error_t            AddSignalHandler
  (
      int                         signalNumber,
      asio_signal_action_t     && method
  );
  void                            SetSignalsDefault   ();

private:
  spo::asio::AsioService        & m_ServiceRef;
  asio_signal_t                   m_Signals;
  asio_signal_actions_t           m_SignalActions;

  void                            SignalHandler
  (
      const error_t        & error,        // Result of operation.
      int                         signal_number // Indicates which signal occurred.
  );
};

//------------------------------------------------------------------------------

}// namespace                   asio
}// namespace                   spo

#endif // ASIOSIGNALS_H
