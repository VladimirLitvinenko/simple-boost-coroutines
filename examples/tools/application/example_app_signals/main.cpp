#include "ui/app/ServiceQtGui.h"
//#include <QCoreApplication>
#include <iostream>

//#define SET_SERVICE_STATUS_ALL
#define SET_SERVICE_STATUS_LIST

int main(int argc, char *argv[])
{
  spo::ui::ServiceQtGui a(argc, argv);
//  spo::app::AppWithSignals< QCoreApplication > a(argc, argv);

  a.SetStateHandler( spo::app::AppState::ErrorParamValue, []( const spo::app::AppState & st )
  {
    std::cout << "Error params =" << (int)st << std::endl;
  } );
  a.SetStateHandler( spo::app::AppState::Ok, []( const spo::app::AppState & st )
  {
    std::cout << "OK =" << (int)st << std::endl;
  } );

#ifdef SET_SERVICE_STATUS_ALL
  std::cout << "Defined macros SET_SERVICE_STATUS_ALL" << std::endl;
  a.SetStatusHandlers(
  {
          { static_cast< short >( spo::app::SignalStatus::Ignored ),
            []( int sN )
            {
              std::cout << "Ignored signal:" << sN << std::endl;
            }
          },
          { static_cast< short >( spo::app::SignalStatus::Default ),
            []( int sN )
            {
              std::cout << "Default signal:" << sN << std::endl;
            }
          },
          { static_cast< short >( spo::app::SignalStatus::Attached ),
            []( int sN )
            {
              std::cout << "Attached signal:" << sN << std::endl;
            }
          },
          { static_cast< short >( spo::app::SignalStatus::Processed ),
            []( int sN )
            {
              std::cout << "Processed signal:" << sN << std::endl;
            }
          },
    } );
#elif defined( SET_SERVICE_STATUS_LIST )
  std::cout << "Defined macros SET_SERVICE_STATUS_LIST" << std::endl;
  a.SetStatusHandlers(
  {
          { static_cast< short >( spo::app::SignalStatus::Ignored ),
            []( int sN )
            {
              std::cout << "Ignored signal:" << sN << std::endl;
            }
          },
          { static_cast< short >( spo::app::SignalStatus::Default ),
            []( int sN )
            {
              std::cout << "Default signal:" << sN << std::endl;
            }
          },
          { static_cast< short >( spo::app::SignalStatus::Attached ),
            []( int sN )
            {
              std::cout << "Attached signal:" << sN << std::endl;
            }
          },
          { static_cast< short >( spo::app::SignalStatus::Processed ),
            []( int sN )
            {
              std::cout << "Processed signal:" << sN << std::endl;
            }
          },
          { static_cast< short >( spo::app::SignalStatus::Sended ),
            []( int sN )
            {
              std::cout << "Signal sebded:" << sN << std::endl;
            }
          },
    } );
#else
  std::cout << "Undefined macros SET_SERVICE_STATUS_ALL" << std::endl;
  a.SetStatusHandler( spo::app::SignalStatus::Ignored,
                      []( int sN )
  {
    std::cout << "Ignored signal:" << sN << std::endl;
  } );
  a.SetStatusHandler( spo::app::SignalStatus::Default,
                    []( int sN )
  {
    std::cout << "Default signal:" << sN << std::endl;
  } );
  a.SetStatusHandler( spo::app::SignalStatus::Attached,
                    []( int sN )
  {
    std::cout << "Attached signal:" << sN << std::endl;
  } );
  a.SetStatusHandler( spo::app::SignalStatus::Processed,
                    []( int sN )
  {
    std::cout << "Processed signal:" << sN << std::endl;
  } );
#endif

  a.AddIgnore( SIGINT );
  a.SendSignal( SIGINT );

  a.SetState( spo::app::AppState::ErrorParamValue );
  a.SetState( spo::app::AppState::Ok );

  a.AddDefault( SIGBUS );

  a.AddIgnore( SIGBUS );
  a.SendSignal( SIGBUS );

  a.AddAction( SIGALRM,
               []( int sn )
  {
    std::cout << "\t\tALARM. Signal: " << sn << std::endl;
  } );
  a.AddAction( SIGBUS,
               []( int sn )
  {
    std::cout << "\t\tBUS. Signal: " << sn << std::endl;
  } );

//  a.SetStdOutEnable( true );
  a.SendSignal( SIGALRM );
  a.SendSignal( SIGBUS );

  return a.exec();
}
