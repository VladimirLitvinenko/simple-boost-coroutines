/**
 * @file PasswdGenerator.h
 *
 * @brief Файл содержит описания структур и методов для работы с паролями
 *        хосте (ОС ПК).
 *
 * @author Владимир Н. Литвиненко.
 * @orhanization Санкт-Петербургский филиал ПАО "НПО "Стрела"
 * @copyright Все права защищены.
 */

#ifndef PASSWDGENERATOR_H
#define PASSWDGENERATOR_H

#include "core/admin/AdminGlobal.h"
#include "core/utils/CoreUtils.h"
#include <QRunnable>

#include <string>
#include <set>


namespace                       spo   {
namespace                       core  {
namespace                       admin {

//------------------------------------------------------------------------------

/**
 * @brief Константное выражение DEFAULT_PASSWD_MINLENGTH определяет минимальную
 *        допустимую длину пароля.
 * @return Минимальная допустимая длина пароля.
 */
constexpr std::size_t DEFAULT_PASSWD_MINLENGTH(){ return 6; }
/**
 * @brief Константное выражение DEFAULT_PASSWD_MAXLENGTH определяет максимальную
 *        допустимую длину пароля.
 * @return Максимальная допустимая длина пароля.
 */
constexpr std::size_t DEFAULT_PASSWD_MAXLENGTH(){ return 32; }

/**
 * @brief Тип PasswdGenFlag класса перечисления описывает признаки отбора символов
 * при создании пароля.
 *
 * @see PasswdGenerator::m_Flag
 */
enum class                      PasswdGenFlag
{
  Upper                       = 0x01, /** использовать буквы верхнего регистра английского алфавита */
  Lower                       = 0x02, /** использовать буквы нижнего регистра английского алфавита */
  Digit                       = 0x04, /** использовать цифры */
  Symbol                      = 0x08, /** использовать уствновленный набор служебных символов */
  WithoutAmbiguous            = 0x20, /** исключить набор символов неоднозначной интерпритации */
  WithoutVowels               = 0x40, /** исключить набор гласных бьукв и похожих цифр */
};

/**
 * @brief Тип PasswdGenFlags класса множества объединяет значения типа @a PasswdGenFlag.
 * @see PasswdGenerator::m_Flag
 */
using PasswdGenFlags          = std::set< PasswdGenFlag >;

/**
 * @brief Тип PasswdCallback определяет функтор обработки готового пароля.
 */
using PasswdCallback          = spo::simple_fnc_t< void, const std::string & >;

//------------------------------------------------------------------------------

/**
 * @brief Класс PasswdGenerator реализует технологию создания пароля.
 */
class SPO_CORE_EXPORT           PasswdGenerator :
public                          QRunnable
{
public:
  using base_calss_t          = QRunnable;

  /**/                          PasswdGenerator     () = default;
  virtual                     ~ PasswdGenerator     ();

  /**
   * @brief Метод IsComplete
   * @return
   */
  inline
  bool                          IsComplete          () const;

  /**
   * @brief Метод SetCallback
   * @param cb
   */
  void                          SetCallback         ( const PasswdCallback & cb );
  /**
   * @brief Метод ResetCallback
   */
  void                          ResetCallback       ();

  /**
   * @brief Метод GetDigits
   * @return
   */
  std::string                   GetDigits           () const;
  /**
   * @brief Метод GetUppers
   * @return
   */
  std::string                   GetUppers           () const;
  /**
   * @brief Метод GetLowers
   * @return
   */
  std::string                   GetLowers           () const;
  /**
   * @brief Метод GetSymbols
   * @return
   */
  std::string                   GetSymbols          () const;
  /**
   * @brief Метод GetAmbiguous
   * @return
   */
  std::string                   GetAmbiguous        () const;
  /**
   * @brief Метод GetVowels
   * @return
   */
  std::string                   GetVowels           () const;
  /**
   * @brief Метод GetLength
   * @return
   */
  std::size_t                   GetLength           () const;
  /**
   * @brief Метод SetLength
   * @param l
   */
  void                          SetLength           (const std::size_t & l );
  /**
   * @brief Метод GetRemoving
   * @return
   */
  std::string                   GetRemoving         () const;
  /**
   * @brief Метод SetRemoving
   * @param s
   */
  void                          SetRemoving         ( const std::string & s );
  /**
   * @brief Метод DigitsOn
   */
  void                          DigitsOn            ();
  /**
   * @brief Метод DigitsOff
   */
  void                          DigitsOff           ();
  /**
   * @brief Метод ContainsDigits
   * @return
   */
  bool                          ContainsDigits      () const;
  /**
   * @brief Метод UpperOn
   */
  void                          UpperOn             ();
  /**
   * @brief Метод UpperOff
   */
  void                          UpperOff            ();
  /**
   * @brief Метод ContainsUpper
   * @return
   */
  bool                          ContainsUpper       () const;
  /**
   * @brief Метод LowerOn
   */
  void                          LowerOn             ();
  /**
   * @brief Метод LowerOff
   */
  void                          LowerOff            ();
  /**
   * @brief Метод ContainsLower
   * @return
   */
  bool                          ContainsLower       () const;
  /**
   * @brief Метод SymbolOn
   */
  void                          SymbolOn            ();
  /**
   * @brief Метод SymbolOff
   */
  void                          SymbolOff           ();
  /**
   * @brief Метод ContainsSymbol
   * @return
   */
  bool                          ContainsSymbol      () const;
  /**
   * @brief Метод AmbiguousOn
   */
  void                          AmbiguousOn         ();
  /**
   * @brief Метод AmbiguousOff
   */
  void                          AmbiguousOff        ();
  /**
   * @brief Метод WithoutAmbiguous
   * @return
   */
  bool                          WithoutAmbiguous    () const;
  /**
   * @brief Метод VowelsOn
   */
  void                          VowelsOn            ();
  /**
   * @brief Метод VowelsOff
   */
  void                          VowelsOff           ();
  /**
   * @brief Метод WithoutVowels
   * @return
   */
  bool                          WithoutVowels       () const;
  /**
   * @brief Метод GetPassword
   * @return
   */
  std::string                   GetPassword         () const;
  /**
   * @brief Метод run
   */
  void                          run                 () override;

private:
  /**
   * @brief Атрибут m_Callback содержит внешний функтор обработки готового пароля.
   */
  PasswdCallback                m_Callback          { nullptr };
  /**
   * @brief Атрибут m_Digits содержит набор цифр, используемых при создании пароля.
   */
  std::string                   m_Digits            { "0123456789" };
  /**
   * @brief Атрибут m_Uppers содержит набор букв верхнего регистра, используемых
   * при создании пароля.
   */
  std::string                   m_Uppers            { "ABCDEFGHIJKLMNOPQRSTUVWXYZ" };
  /**
   * @brief Атрибут m_Lowers содержит набор букв нижнего регистра, используемых
   * при создании пароля.
   */
  std::string                   m_Lowers            { "abcdefghijklmnopqrstuvwxyz" };
  /**
   * @brief Атрибут m_Symbols содержит набор служебных, используемых при
   * создании пароля.
   */
  std::string                   m_Symbols           { "!#$%&*+,./:;<=>?@[\\]^_{|}~" };
  /**
   * @brief  Атрибут m_Ambiguous содержит набор элементов типа char c возможной
   * неоднозначной трактовкой.
   *
   * @note Неоднозначная трактовка возникает из-за затруднеения при идентификации
   * значения символа.
   * Например, при мелком шрифте консоли или недостаточной контрастности
   * монитора можно перепутать видимые символы 'Z' и '2' или 'I', 'l' и '1'.
   * Часто пользователи путают цифру '0' и букву 'O' и т.д.
   */
  std::string                   m_Ambiguous         { "B8G6I1l0OQDS5Z2" };
  /**
   * @brief Атрибут m_Vowels содержит набор  элементов типа char, обозначающие
   * глассные буквы и цифры на них похожие.
   */
  std::string                   m_Vowels            { "01aeiouyAEIOUY" };
  /**
   * @brief Атрибут m_Length содержит значение длинны в символах создаваемого пароля.
   */
  std::size_t                   m_Length            { DEFAULT_PASSWD_MINLENGTH() };
  /**
   * @brief Атрибут m_Removing содержит набор  элементов типа char, которые
   * необходимо икслючить при создании пароля.
   */
  std::string                   m_Removing;
  /**
   * @brief Атрибут m_Password содержит значение последней генерации пароля.
   */
  std::string                   m_Password;
  /**
   * @brief Атрибут m_Flag содержит набор признаков типа @a PasswdGenFlags,
   * применяемых при создании пароля.
   *
   * @note Начальное правило установлены как требования наличия пррописных,
   * строчных букв и хотя бы одной цифры.
   */
  PasswdGenFlags                m_Flag
  {
    PasswdGenFlag::Upper,
    PasswdGenFlag::Lower,
    PasswdGenFlag::Digit,
//    PasswdGenFlag::Symbol,
  };
  /**
   * @brief SwitchFlag
   * @param flag
   * @param toOn
   */
  void                          SwitchFlag          ( PasswdGenFlag flag, bool toOn );
  /**
   * @brief RemoveString
   * @param src
   * @param removing
   * @return
   */
  std::string                   RemoveString        ( const std::string & src, const std::string & removing );
  /**
   * @brief BuildString
   * @return
   */
  std::string                   BuildString         ();
  /**
   * @brief OneValueExists
   * @param s
   * @return
   */
  bool                          OneValueExists      ( std::string & s );
};

//------------------------------------------------------------------------------

}// namespace                   admin
}// namespace                   core
}// namespace                   spo

#endif // PASSWDGENERATOR_H
