/**
 * @file ByteStuffing.h
 *
 * @brief Файл содержит описания класса управления байт-стаффингом набора байт.
 *
 * @author Владимир Н. Литвиненко.
 * @orhanization Санкт-Петербургский филиал ПАО "НПО "Стрела"
 * @copyright Все права защищены.
 */

#ifndef BYTESTUFFING_H
#define BYTESTUFFING_H

#include "core/admin/AdminGlobal.h"
#include "core/documents/IDocument.h"
#include <string>

namespace                       spo   {
namespace                       core  {
namespace                       admin {

//------------------------------------------------------------------------------
/**
 * @brief Тип bytestuffing_t определяет пару последовательностей символов (байт)
 *        к применению алгоритма байт-стаффинга.
 */
using bytestuffing_t          = std::pair
                                <
                                  std::string, ///< значение заменяемой последовательности байт
                                  std::string  ///< значение последовательности байт для замены
                                >;

//------------------------------------------------------------------------------

/**
 * @brief Класс ByteStuffing определяет набор байт и правила замены байт для
 * обозначения начала пакета (и конца) байт.
 *
 * Байт-стаффинг применяется при передаче пакетов данных по программным и
 * физическим каналам передачи данных.
 */
class SPO_CORE_EXPORT           ByteStuffing :
public                          spo::core::docs::Document< bytestuffing_t >
{
public:
  /**
   * @brief Тип base_class_t определяет класс Документ типа @a spo::core::docs::Document
   *        как базовый для данного класса.
   */
  using base_class_t          = spo::core::docs::Document< bytestuffing_t >;

  /**
   * @brief Конструктор ByteStuffing принимает дvalue ссылку на пару значений
   *        строк, интерпритируемых как набор байт. Первая строка содержит
   *        значения для атрибута @a m_From, а вторая -- для атрибута @a m_To.
   */
  /**/                          ByteStuffing      ( const std::pair< std::string, std::string > & );
  /**
   * @brief Конструктор ByteStuffing принимает rvalue ссылку на пару значений
   *        строк, интерпритируемых как набор байт. Первая строка содержит
   *        значения для атрибута @a m_From, а вторая -- для атрибута @a m_To.
   */
  /**/                          ByteStuffing      ( std::pair< std::string, std::string > && );
  /**
   * @brief Конструктор копирования экземпляра класса ByteStuffing .
   */
  /**/                          ByteStuffing      ( const ByteStuffing & );
  /**
   * @brief Конструктор перемещения экземпляра класса ByteStuffing .
   */
  /**/                          ByteStuffing      ( ByteStuffing && );
  virtual                     ~ ByteStuffing      ();

  /**
   * @brief Метод operator = определяет оператор копирования экземпляра класса ByteStuffing.
   * @param lRef Ссылка на экземпляр класса для копирования.
   * @return Ссылка на текущий экземпляр класса.
   */
  virtual ByteStuffing &        operator=         ( const ByteStuffing &  lValue ) ;
  /**
   * @brief Метод operator = определяет оператор перемещения экземпляра класса ByteStuffing.
   * @param lRef Ссылка на экземпляр класса для копирования.
   * @return Ссылка на текущий экземпляр класса.
   */
  virtual ByteStuffing &        operator=         ( ByteStuffing &&  rValue ) ;
  /**
   * @brief Метод operator = определяет оператор присвоения значений для
   *        атрибутов @a m_From и @a m_To сооответствующими значениями
   *        пары строк std::pair::first и std::pair::second параметра оператора.
   * @param p Ссылка на константную пару значений строк:
   * @li std::pair::first  строка (набор символов const char) для инициализации
   *      атрибута @a m_From;
   * @li std::pair::second строка (набор символов const char) для инициализации
   *      атрибута @a m_To.
   * @return Ссылка на текущий экземпляр класса.
   */
  virtual ByteStuffing &        operator=         ( const std::pair< std::string, std::string > & p ) ;
  /**
   * @brief Метод operator = определяет оператор перемещения значений для
   *        атрибутов @a m_From и @a m_To сооответствующими значениями
   *        пары строк std::pair::first и std::pair::second параметра оператора.
   * @param p Ссылка на rvalue ссылку пары значений строк:
   * @li std::pair::first  строка (набор символов const char) для инициализации
   *      атрибута @a m_From;
   * @li std::pair::second строка (набор символов const char) для инициализации
   *      атрибута @a m_To.
   * @return Ссылка на текущий экземпляр класса.
   */
  virtual ByteStuffing &        operator=         ( std::pair< std::string, std::string > && p ) ;

  /**
   * @brief Метод GetFrom возвлащает копию значения шаблона байт для преобразования.
   * @return Массив байт - копию значения шаблона байт для преобразования.
   */
  std::string                   GetFrom           () const;
  virtual void                  SetFrom           ( const std::string & value );
  /**
   * @brief Метод SetFrom устанавливает значения шаблона байт для замены
   *        перемещением значения атрибута @a m_From
   * @param value rvalue ссылка на строку символов (байт) как новое значение
   *              атрибута @a m_From.
   */
  virtual void                  SetFrom           ( std::string && value );
  /**
   * @brief Метод GetTo возвлащает копию значения шаблона байт для замены
   *        значений, равных значению атрибута @a m_From
   * @return
   */
  std::string                   GetTo             () const;
  virtual void                  SetTo             ( const std::string & value );
  /**
   * @brief Метод SetTo устанавливает значения шаблона байт замены
   *        перемещением значения атрибута @a m_To
   * @param value rvalue-ссылка на строку символов (байт) как новое значение
   *              атрибута @a m_To.
   */
  virtual void                  SetTo             ( std::string && value );

  virtual std::string           Stuff             ( const std::string & value );
  /**
   * @brief Метод Stuff производит операцию байт-стаффинга для строки символов (байт).
   * @param value rvalue-константная ссылка на преобразуемую строку симолов
   * @return Массив байт после завершения операцию байт-стаффинга.
   *
   * При обнаружении в стпроке значения @a value последовательности байт,
   * равной значению аттрибута @a m_From производится замена данной последовательности
   * значениями из массива байт аттрибута @a m_To.
   */
  virtual std::string           Stuff             ( std::string && value );

  virtual std::string           Unstuff           ( const std::string & value );
  /**
   * @brief Метод Unstuff производит обратное методу @a Stuff преобразование
   * строки (восстановление значений).
   * @param value rvalue-константная ссылка на восстанавливаемую строку.
   * @return Массив байт после завершения операцию восстановления.
   *
   * При обнаружении в строке значения @a value последовательности байт,
   * равной значению аттрибута @a m_To производится замена данной последовательности
   * значениями из массива байт аттрибута @a m_From.
   */
  virtual std::string           Unstuff           ( std::string && value );

  /**
   * @brief Метод FromByteArray перегружает абстрактный метод @a base_class_t::FromByteArray
   *        класса-предка.
   * @param a lvalue-константная ссылка на массив байт упаковки экземпляра класса.
   * @return Признак успешного преобразования:
   * @value true  преобразование прошло успешно;
   * @value false в результате преобразования возникли ошибки.
   *
   * При возникновении ошибки системная переменная errno принимает значения:
   * @value ENODATA параметр не содержит данных;
   * @value EBADMSG структура данных параметра неверна;
   *
   * Cтруктура данных параметра: <first ByteArray (from)><'\n'><second QByteArray (to)>
   *
   */
  bool                          FromByteArray     ( const std::string & a ) override;
  /**
   * @brief Метод ToByteArray перегружает абстрактный метод @a base_class_t::ToByteArray
   * Метод производит упаковку значений Документа в формат
   * <first ByteArray (from)><'\n'><second QByteArray (to)> как массив байт
   * типа std::string
   * @return Упакованный массив байт типа QByteArray.
   */
  std::string                   ToByteArray       () const override;
  /**
   * @brief Метод ClearContent перегружает абстрактный метод @a base_class_t::ClearContent.
   * Медод отвечает за очистку данных Документа типа @a base_class_t
   */
  void                          ClearContent      () override;
};

//------------------------------------------------------------------------------

}// namespace                   admin
}// namespace                   core
}// namespace                   spo

#endif // BYTESTUFFING_H
