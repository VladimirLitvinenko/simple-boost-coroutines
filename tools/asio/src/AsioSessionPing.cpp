//#include "asio/AsioSessionPing.h"

//namespace                       spo   {
//namespace                       asio  {

//AsioSessionPing::AsioSessionPing  ( icmp_socket_t && socket  )
//  : AsioSessionPing::base_class_t( socket.get_io_service() )
//  , m_Socket                    { std::move(socket)       }
//{
//}

//AsioSessionPing::~AsioSessionPing()
//{
//}

//bool AsioSessionPing::IsActive() const
//{
//  return m_Socket.is_open();
//}

//icmp_socket_t &
//AsioSessionPing::GetSocketRef()
//{
//  return std::ref( m_Socket );
//}

//std::size_t
//AsioSessionPing::GetByteAvailable() const
//{
//  return m_Socket.available();
//}

//void AsioSessionPing::Start()
//{
//  auto self( shared_from_this() );
//  SetReady( true );
//  boost::asio::spawn( StrandRef(), boost::bind( &AsioSessionPing::Run, self, _1 ) );
//  boost::asio::spawn( StrandRef(), boost::bind( &AsioSessionPing::TimeoutBreak, self, _1 ) );
//}

//void AsioSessionPing::Stop()
//{
//  SteadyTimerRef().cancel();

//  if( IsActive() )
//    m_Socket.shutdown( boost::asio::socket_base::shutdown_both );
//  m_Socket.close();
//  if( m_Deleter )
//  {
//    m_Deleter( asio_poolkey_t( this ) );
//  }
//}

//void AsioSessionPing::Run(boost::asio::yield_context yield)
//{
//  try
//  {
//    boost::asio::streambuf  request_buffer;
//    std::ostream            os( & request_buffer );

//    while ( IsReady() )
//    {
//      SteadyTimerRef().expires_from_now( std::chrono::milliseconds( 300 ) );

//      error_t  ec;
//      m_Socket.async_send_to( request_buffer.data(),
//                              m_Endpoint,
//                              yield[ ec ] );
//      if( ec.value() == boost::system::errc::success )
//      {
//        this->m_ReplyBuffer.consume( this->m_ReplyBuffer.size() );
//        this->m_Socket.async_receive_from(
//              this->m_ReplyBuffer.prepare( INT16_MAX ),
//              this->m_Endpoint,
//              [ this ]
//              (
//                  const error_t  ec,
//                  std::size_t
//              )
//              {
//                DUMP_BOOST_ERROR( ec );
//              } );
//      }
//    }
//  }
//  catch (std::exception& e )
//  {
//    DUMP_EXCEPTION( e );
//  }
//  Stop();
//}

//void AsioSessionPing::TimeoutBreak(boost::asio::yield_context yield)
//{
//  while( IsActive() and IsReady() )
//  {
//    boost::system::error_code ignored_ec;
//    SteadyTimerRef().async_wait( yield[ ignored_ec ] );
////    if( ignored_ec.value() != boost::system::errc::operation_canceled )
//    if ( SteadyTimerRef().expires_from_now() <= std::chrono::milliseconds( 0 ) )
//    {
//      Stop();
//    }
//  }
//}

//}// namespace                   asio
//}// namespace                   spo

