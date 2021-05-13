#ifndef IOCHANNEL_H
#define IOCHANNEL_H

#include "core/documents/DocumentPkg.h"
#include "asio/SteadyTimer.h"

namespace                         spo   {
namespace                         asio  {


//------------------------------------------------------------------------------

/**
 * @brief Тип io_channel_action_t определяет метод (функтор) обработки данных
 * типа @a spo::socket_byffer_t.
 *
 * Значение шаблона ByteT_ указывает тип единицы хранения данных в буфере.
 * По умолчанию, тип - байт (unsigned char)
 */
template< typename                ByteT_            = unsigned char >
using io_channel_action_t       = spo::simple_fnc_t< bool, spo::core::docs::BytesDocument< ByteT_ > & >;

/**
 * @brief Тип buffer_actions_map определяет карту соответствий методов
 * @a io_channel_action_t обработки данных канала типа @a spo::asio::DataType
 * обмена данными.
 *
 * Значение шаблона ByteT_ указывает тип единицы хранения данных в буфере.
 * По умолчанию, тип - байт (unsigned char)
 */
template< typename                ByteT_            = unsigned char >
using buffer_actions_map        = std::map< short, io_channel_action_t< ByteT_ > >;

//------------------------------------------------------------------------------
/**
 * @brief Шаблонная структура IOChannel определяет канал передачи данных при
 * работе с объектами на основе технологии IPC с использованием Boost.Asio.
 *
 * Параметры шаблона:
 * @value ByteT_ тип единицы информации буфера обмена данными.
 *
 */
template< typename                ByteT_      = unsigned char >
struct                            IOChannel
{
  using action_t                = io_channel_action_t< ByteT_ >;
  using buffer_t                = spo::core::docs::BytesDocument< ByteT_ >;
  using bufferptr_t             = std::shared_ptr< buffer_t >;

  /**
   * @brief Атрибут m_Buffer содержит буфер обмена данными.
   */
  buffer_t                        m_Buffer;
  /**
   * @brief Атрибут m_Action содержит обработчик данных буфера обмена @a m_Buffer.
   */
  action_t                        m_Action;
  std::size_t                     m_BufferSize  { 512 };

  /**
    * @brief Конструктор IOChannel без параметров запрещен.
    */
  /**/                            IOChannel()  = delete;

  /**
   * @brief Конструктор IOChannel
   * @param socket    ссылка на сокет обмена данными;
   * @param action    действие по обработке данных в буфере обмена:
   * @param deadLine  время ожидания (таймаут) приема или передачи данных при обмене.
   */
  /**/                            IOChannel
  (
      action_t                    action,
      std::size_t                 bufferSize        = 512
  )
    : m_Action                    ( action )
    , m_BufferSize                ( bufferSize )
  {}

  /**
   * @brief Метод SetAction назначает действие (функтор) обработки данных буфера обмена @a m_Buffer
   * @param action действие (функтор)
   */
  void SetAction( IOChannel::action_t action ) BOOST_NOEXCEPT
  {
    BEGIN_LOCK_SECTION_( m_Buffer.MutexRef() );
    m_Action = action;
    END_LOCK_SECTION_
  }

  /**
   * @brief Метод Execute запускает на выполнение метод @a m_Action обработки
   * данных из буфера обмена.
   * @return Признак выполнения:
   * @value true действие определено и выполнено корректно
   * @value false действие не определено или выполнено некорректно
   */
  bool Execute()
  {
    return
        m_Action.operator bool() ?
          m_Action( BufferRef() ) :
          false;
  }

  /**
   * @brief Метод ActionExists проверяет наличие метода в атрибуте @a m_Action
   * @return Признак наличия:
   * @value true  атрибут @a m_Action содержит функтор работы с буфером данных;
   * @value false атрибут @a m_Action НЕ содержит функтор работы с буфером данных;
   */
  bool ActionExists  () const
  {
    return m_Action.operator bool();
  }

  /**
   * @brief Метод BufferRef возвращает ссылку на буфер с данными.
   * @return ссылка на буфер данных
   */
  buffer_t & BufferRef   ()
  {
    return std::ref( m_Buffer );
  }

  /**
   * @brief Метод BufferSize сообщает о разиере буфера с данными в определенных
   *        параметром ByteT_ шаблона единицах.
   * @return разиере буфера с данными.
   *
   * @warning Размер буфера содержит количество единичных величин!
   * Например буфер socket_byffer_t< char >( 10 ) и socket_byffer_t< ште >(10)
   * имеют одинаковое значение размера, а, именно, std::size_t(10).
   */
  std::size_t BufferSize() const
  {
    return m_BufferSize;
  }

  /**
   * @brief Метод SetBufferSize задает размер буфера данных в определенных
   * параметром ByteT_ шаблона единицах.
   * @param bSize значение размера буфера данных
   */
  void SetBufferSize( std::size_t bSize )
  {
    BEGIN_LOCK_SECTION_( m_Buffer.MutexRef() );
    m_BufferSize = bSize;
    END_LOCK_SECTION_;
  }

  /**
   * @brief Метод Clear очищаетданные в буфере.
   * Размер буфера устанавливается равным нулю.
   */
  void Clear       ()
  {
    BEGIN_LOCK_SECTION_( m_Buffer.MutexRef() );
    m_Buffer.ContentRef().clear();
    END_LOCK_SECTION_;
  }
};

//------------------------------------------------------------------------------

}// namespace                   asio
}// namespace                   spo

#endif // IOCHANNEL_H
