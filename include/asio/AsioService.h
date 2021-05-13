#ifndef ASIOSERVICE_H
#define ASIOSERVICE_H

#include "asio/AsioError.h"

namespace                         spo   {
namespace                         asio  {

//------------------------------------------------------------------------------
/**
 * @typedef
 */
using io_service_callback_t     = spo::simple_fnc_t< void, void* >;

/**
 * @typedef
 */
using io_service_callbacks_t    = std::vector< std::pair< io_service_callback_t, void* > >;

/**
 * @brief The IoServiceActionType enum
 */
enum                              IoServiceActionType
{
  BeforeStart                   = 0,
  BeforeStop                    = 1,
  AfterStop                     = 2,
};

/**
 * @typedef
 */
using io_service_callbacks_map_t= std::map< IoServiceActionType, io_service_callbacks_t >;

//------------------------------------------------------------------------------
/**
 * @brief Класс-перечисление AsioState типа @a short объявляет значения
 *        возможных состояний сервера
 */
enum class                        AsioState : short
{
  Unknown                       = -1, ///< значение не определено (неопределённое состояние)
  Ok                            = 0,  ///< корректное состояние

  Exception                     = 1,

  ErrPortCount                  = 10,  ///< переполнение количества портов
  ErrChannelId                  = 11,  ///< ошибка идентификации канала данных

  ErrSocketCount                = 20, ///< переполнение допустимого количества
                                      ///< одновременно открытых сокетов

  ErrConnection                 = 30  ///< ошибка подключения сокета
};

//------------------------------------------------------------------------------
/**
 * @brief Класс AsioService
 */
class SPO_CORE_EXPORT             AsioService
{
public:
  /**/                            AsioService         ( const std::int64_t & timeoutMs = 10000 );
  /**/                            AsioService         ( const AsioService &             ) = delete;
  /**/                            AsioService         ( AsioService &&                  ) = delete;
  /**/                            AsioService         ( const spo::asio::io_service_t & ) = delete;
  /**/                            AsioService         ( spo::asio::io_service_t &&      ) = delete;
  virtual                       ~ AsioService         ()  {}

  virtual AsioService &           operator=           ( const AsioService&              ) = delete;
  virtual AsioService &           operator=           ( AsioService &&                  ) = delete;
  virtual AsioService &           operator=           ( const spo::asio::io_service_t & ) = delete;
  virtual AsioService &           operator=           ( spo::asio::io_service_t &&      ) = delete;

  static
  spo::asio::AsioService &
  Instance ( const std::int64_t & timeoutMs = 10000 )
  {
    static spo::asio::AsioService svc( timeoutMs );
    return std::ref( svc );
  }

  static
  spo::asio::error_t
  ExceptionError ()
  {
    AsioService::Instance().SetState( AsioState::Exception );
    AsioService::Instance().SetError( boost::system::errc::errc_t::owner_dead );
    return AsioService::Instance().ErrorCode();
  }

  /**
   * @brief Метод IsStateValid проверяет состояние сервера
   * @return Булево значение:
   * @value true  сервер в исправном состоянии;
   * @value false сервер неисправен.
   */
  bool IsStateValid () const BOOST_NOEXCEPT
  {
    return m_State == AsioState::Ok;
  }

  /**
   * @brief Метод ResetState сбрасывает сервер в неопределенное (начальное) состояние
   */
  void ResetState () BOOST_NOEXCEPT
  {
    m_State = AsioState::Unknown;
  }

  virtual bool                    IsValid             () const
  {
    return
        IsStateValid()
        and
        ( m_Error.Value() == boost::system::errc::success );
  }

  virtual bool                    IsActive            () const BOOST_NOEXCEPT
    { return m_Active; }

  std::int64_t                    CloseTimeout        () const
    { return m_TimeoutMs; }

  void                            SetCloseTimeout     ( const std::int64_t & timeoutMs = 10000 ) BOOST_NOEXCEPT
    { m_TimeoutMs = timeoutMs < 0 ? 0 : timeoutMs; }

  void                    SetError            ( const AsioError & error ) BOOST_NOEXCEPT
    { m_Error.SetErrorCode( error.Code() ); }

  void                    SetError            ( AsioError && error ) BOOST_NOEXCEPT
    { m_Error.SetErrorCode( std::move( error ).Code() ); }

  void                    SetError            ( const error_t & error ) BOOST_NOEXCEPT
    { m_Error.SetErrorCode( error ); }
  bool IsError            ( const error_t & error ) BOOST_NOEXCEPT
  {
    SetError( error );
    return not IsNoErr( ErrorCode() );
  }

  void SetError ( boost::system::errc::errc_t ec ) BOOST_NOEXCEPT
  {
    m_Error.SetErrorCode( ec );
  }

  void SetErrorCallbacks ( const asio_errcallbacks_t & callbacks ) BOOST_NOEXCEPT
  {
    m_Error.SetErrorCallbacks( callbacks );
  }
  void SetErrorCallback
  (
      const asio_errcallbacks_t::key_type & key,
      const asio_errcallbacks_t::mapped_type & action
  ) BOOST_NOEXCEPT
    { m_Error.SetErrorCallback( key, action ); }

  bool                            ActionExists              ( const IoServiceActionType & aType ) BOOST_NOEXCEPT;
  void                            AddBeforeStartCallback    ( io_service_callbacks_t::value_type action ) BOOST_NOEXCEPT;
  bool                            BeforeStartCallbackExists () BOOST_NOEXCEPT;
  void                            AddBeforeStopCallback     ( io_service_callbacks_t::value_type action ) BOOST_NOEXCEPT;
  void                            AddAfterStopCallback      ( io_service_callbacks_t::value_type action ) BOOST_NOEXCEPT;
  void                            ClearCallbacks            () BOOST_NOEXCEPT;

  bool                            Start               ();
  void                            Poll                ();
  void                            Stop                () BOOST_NOEXCEPT;

  error_t                         ErrorCode           () const
    { return m_Error.Code(); }

  boost::system::errc::errc_t     ErrorValue          () const
    { return m_Error.Value(); }

  io_service_t                  & ServiceRef          ()
  {
    return std::ref( m_Service );
  }

  /**
   * @brief Защищенный метод SetState назначает текущее состояние серверу
   * @param state значение назначаемого состояния
   */
  void SetState ( const AsioState state ) BOOST_NOEXCEPT
  {
    m_State = state;
  }

private:
  /**
   * @brief Атрибут m_Service
   */
  io_service_t                    m_Service;
  /**
   * @brief Атрибут m_WorkPtr
   */
  asio_workptr_t                  m_WorkPtr;
  /**
   * @brief Атрибут m_Error
   */
  AsioError                       m_Error;
  /**
   * @brief Атрибут m_Active
   */
  std::atomic_bool                m_Active            { false };
  /**
   * @brief Атрибут m_Actions
   */
  io_service_callbacks_map_t      m_Actions;
  /**
   * @brief Атрибут m_TimeoutMs
   */
  std::int64_t                    m_TimeoutMs         { 10000 };
  /**
   * @brief Атрибут m_State
   */
  AsioState                       m_State             { AsioState::Unknown };

  void                            RunServiceCallbacks ( const io_service_callbacks_map_t::key_type & key ) BOOST_NOEXCEPT;
  void                            SetDefaultErrorCallbacks () BOOST_NOEXCEPT;
  void                            RunService          () BOOST_NOEXCEPT;
  void                            RunThreadService    () BOOST_NOEXCEPT;
};

//------------------------------------------------------------------------------

using AsioServiceShared         = std::shared_ptr< AsioService >;

//------------------------------------------------------------------------------

}// namespace                   asio
}// namespace                   spo

#endif // ASIOSERVICE_H
