/**
  * @file AsioError.h
  * @brief Файл AsioError.h содержит объявление класса @a spo::asio::AsioError
  *        работы с кодами ошибок библиотеки Boost.Asio
  *
  * @author Vladimir N. Litvinenko ( mailto: )
  * @date 16.10.2019
  */

#ifndef ASIOERROR_H
#define ASIOERROR_H


#include "asio/AsioCommon.h"

namespace                         spo   {
namespace                         asio  {

//------------------------------------------------------------------------------
/**
 * @brief Класс AsioError предназначен для централизованной работы с кодами
 *        ошибки выполнения действий методов и классов библиотеки Boost.Asio.
 *
 * Класс определяет ассоциативный набор мтодов обратного вызова для заданного кода
 * ошибки перечисляемого типа @a boost::system::errc::errc_t.
 * Набор хранится в значении атрибута @a AsioError::m_Callbacks.
 *
 * Установка действий, ассоциируемых с кодами ошибки возможна динамически.
 * Для этого необходимо применить методы @ AsioError::SetErrorCallbacks
 * и @a AsioError::SetErrorCallback
 *
 * @see spo::asio::asio_errcallbacks_t, spo::asio::asio_errcallback_t
 */
class SPO_CORE_EXPORT             AsioError
{
private:
  /**
   * @brief Аргумент m_ErrorCode содержит значение кода ошибки выполнения действий
   *        методов и классов библиотеки Boost.Asio
   */
  error_t                    m_ErrorCode
  {
    boost::system::errc::make_error_code( boost::system::errc::success )
  };
  /**
   * @brief Аргумент m_Callbacks
   *
   * @see spo::asio::asio_errcallbacks_t, spo::asio::asio_errcallback_t
   */
  asio_errcallbacks_t             m_Callbacks;

public:
  /**
   * @brief Конструктор класса AsioError по-умолчанию
   */
  /**/                            AsioError           () = default;
  /**
   * @brief Конструктор класса AsioError с инициализацией ассоцияативного
   *        контейнера-карты обратных вызовов
   * @param callbacks ассоцияативный контейнер-карта обратных вызовов
   */
  /**/                            AsioError           ( const asio_errcallbacks_t & callbacks )
                                : AsioError::AsioError()
  {
    SetErrorCallbacks( callbacks );
  }
  virtual                       ~ AsioError           ()
  {
    ClearCallbacks();
  }

  /**
   * @brief Метод Code возвращает значение текущего код ошибки в формате
   *        типа @a spo::asio::error_t
   * @return Значение текущего кода ошибки в формате типа @a spo::asio::error_t
   */
  spo::asio::error_t         Code                () const
  {
    return m_ErrorCode;
  }
  /**
   * @brief Метод Value возвращает значение текущего код ошибки в формате
   *        перечисляемого типа @a boost::system::errc::errc_t
   * @return Значение текущего кода ошибки в формате перечисляемого
   *        типа @a boost::system::errc::errc_t
   */
  boost::system::errc::errc_t     Value               () const
  {
    return static_cast< boost::system::errc::errc_t >( m_ErrorCode.value() );
  }
  /**
   * @brief Метод SetErrorCallbacks
   * @param callbacks
   */
  void SetErrorCallbacks( const asio_errcallbacks_t & callbacks ) BOOST_NOEXCEPT
  {
    for( auto & mapped : callbacks )
      SetErrorCallback( mapped.first, mapped.second );
  }
  /**
   * @brief SetErrorCallback
   * @param errorCode
   * @param callback
   */
  void                            SetErrorCallback
  (
      const asio_errcallbacks_t::key_type     & errorCode,
      const asio_errcallbacks_t::mapped_type  & callback
  ) BOOST_NOEXCEPT
  {
    m_Callbacks[ errorCode ] = callback;
  }
  /**
   * @brief Execute
   * @param errorCode
   */
  void                            Execute
  (
      const asio_errcallbacks_t::key_type & errorCode,
      bool                                  async = true
  )
  {
    if( m_Callbacks.empty() )
      return;

    auto iter( m_Callbacks.find( errorCode ) );
    if( iter != m_Callbacks.end() )
    {
      auto ec( boost::system::errc::make_error_code( errorCode ) );
      DUMP_ASIO_ERROR( ec );
      if( async )
      {
        std::async( std::launch::async,
                    iter->second,
                    ec
        ).wait_for( std::chrono::milliseconds(0));
        return;
      }
      iter->second( ec );
    }
  }
  /**
   * @brief SetErrorCode
   * @param ec
   */
  virtual void                    SetErrorCode        ( boost::system::errc::errc_t ec )
  {
    m_ErrorCode = boost::system::errc::make_error_code( ec );
    if( m_ErrorCode != boost::system::errc::errc_t::success )
    {
      DUMP_ASIO_ERROR( m_ErrorCode );
    }
    Execute( ec );
  }
  virtual void                    SetErrorCode        ( const error_t & ec )
  {
    SetErrorCode( static_cast< boost::system::errc::errc_t >( ec.value() ) );
  }
  void                            Erase               ( const asio_errcallbacks_t::key_type & errorCode ) BOOST_NOEXCEPT
  {
    if( m_Callbacks.empty() )
      return;

    auto iter( m_Callbacks.find( errorCode ) );
    if( iter != m_Callbacks.end() )
    {
      m_Callbacks.erase( iter );
    }
  }
  void                            ClearCallbacks      () BOOST_NOEXCEPT
  {
    m_Callbacks.clear();
  }

  virtual AsioError & operator= ( const asio_errcallbacks_t & callbacks )
  {
    SetErrorCallbacks( callbacks );
    return std::ref( * this );
  }
  virtual AsioError & operator= ( const error_t & err )
  {
    SetErrorCode( err );
    return std::ref( * this );
  }
  virtual AsioError & operator= ( const AsioError & err ) ;
  virtual AsioError & operator= ( AsioError && err ) ;
};

//------------------------------------------------------------------------------

}// namespace                   asio
}// namespace                   spo

#endif // ASIOERROR_H
