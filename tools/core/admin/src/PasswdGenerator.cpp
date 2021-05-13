/**
 * @file PasswdGenerator.сзз
 *
 * @brief Файл содержит реализации методов для работы с паролями
 *        хосте (ОС ПК).
 *
 * @author Владимир Н. Литвиненко.
 * @orhanization Санкт-Петербургский филиал ПАО "НПО "Стрела"
 * @copyright Все права защищены.
 */

#include "core/admin/PasswdGenerator.h"

#include <boost/bind.hpp>

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>

namespace                       spo   {
namespace                       core  {
namespace                       admin {

/* Borrowed/adapted from e2fsprogs's UUID generation code */
/**
 * @brief Статический метод get_random_fd
 * @return
 */
static int get_random_fd()
{
  struct timeval	tv;
  static int	fd = -2;

  if (fd == -2) {
    ::gettimeofday(&tv, 0);
    fd = open("/dev/urandom", O_RDONLY);
    if (fd == -1)
      fd = open("/dev/random", O_RDONLY | O_NONBLOCK);
  }
  return fd;
}

/**
 * @brief Метод pw_random_number создает псевдослучайный номер.
 * @param max_num значение максимальной границы номера.
 * @return
 */
int pw_random_number( std::size_t max_num )
{
  unsigned int rand_num;
  int i, fd = get_random_fd();
  int lose_counter = 0, nbytes = sizeof(rand_num);
  char *cp = (char *) &rand_num;

  if( fd >= 0 )
  {
    while( nbytes > 0 )
    {
      i = read(fd, cp, nbytes);
      if( (i < 0)
          and
          ( (errno == EINTR) || (errno == EAGAIN))
      )
      {
        continue;
      }
      if( i <= 0 )
      {
        if (lose_counter++ == 8)
          break;
        continue;
      }
      nbytes -= i;
      cp += i;
      lose_counter = 0;
    }
  }

  Q_ASSERT( nbytes == 0 );

  return (rand_num % max_num);
}

PasswdGenerator::~PasswdGenerator()
{

}

bool PasswdGenerator::IsComplete() const
{
  return not m_Password.empty();
}

void
PasswdGenerator::SetCallback(const PasswdCallback & f )
{
  m_Callback = boost::bind( f, _1 );
}

void PasswdGenerator::ResetCallback()
{
  m_Callback = nullptr/*[]( const std::string &){}*/;
}

std::string
PasswdGenerator::GetDigits()
const
{
  return m_Digits;
}

std::string
PasswdGenerator::GetUppers()
const
{
  return m_Uppers;
}

std::string
PasswdGenerator::GetLowers()
const
{
  return m_Lowers;
}

std::string
PasswdGenerator::GetSymbols()
const
{
  return m_Symbols;
}

std::string
PasswdGenerator::GetAmbiguous()
const
{
  return m_Ambiguous;
}

std::string
PasswdGenerator::GetVowels()
const
{
  return m_Vowels;
}

std::string
PasswdGenerator::GetPassword()
const
{
  return m_Password;
}

void PasswdGenerator::run()
{
  m_Password.clear();

  std::string tmp{ BuildString() };

  bool run_callback( true );
  if( ! tmp.empty() )
  {
    auto len ( tmp.size() );
    for( std::size_t i( 0 ); i < m_Length; i++ )
    {
      m_Password.push_back( tmp[ pw_random_number( len ) ] );
    }

    if( IsComplete() )
    {
      run_callback =
          not ( ( ContainsSymbol() and ( not OneValueExists( m_Symbols ) ) )
                or
                ( ContainsDigits() and ( not OneValueExists( m_Digits ) ) ) );

      if( not run_callback )
      { // не выполняются условия по флагам. Пересоздание пароля.
        // обратный вызов m_Callback будет применен только на последнем шаге рекурсии.
        run();
      }
    }
  }

  if( m_Callback and run_callback )
    m_Callback( GetPassword() );
}

std::string
PasswdGenerator::GetRemoving()
const
{
  return m_Removing;
}

void
PasswdGenerator::SetRemoving(const std::string & str )
{
  m_Removing = str;
}

void
PasswdGenerator::SwitchFlag(PasswdGenFlag flag, bool toOn)
{
  if( toOn )
    m_Flag.insert ( flag );
  else
    m_Flag.erase  ( flag );
}

std::string
PasswdGenerator::RemoveString(const std::string & src, const std::string & removing)
{
  std::string retval( src );
  if( ( ! removing.empty() ) and ( ! retval.empty() ) )
  {
    for( auto c : removing )
    {
      auto pos( retval.find( c ) );
      if( pos != std::string::npos )
      {
        retval.erase( pos, 1 );
      }
    }
  }
  return std::move( retval );
}

std::string
PasswdGenerator::BuildString()
{
  std::string retval;

  if( ContainsDigits() )
    retval += m_Digits;

  if( ContainsUpper() )
    retval += m_Uppers;

  if( ContainsLower() )
    retval += m_Lowers;

  if( ContainsSymbol() )
    retval += m_Symbols;

  if( WithoutAmbiguous() )
    retval = RemoveString( retval, m_Ambiguous );

  if( WithoutVowels() )
    retval = RemoveString( retval, m_Vowels );

  return std::move( RemoveString( retval, m_Removing ) );
}

bool PasswdGenerator::OneValueExists( std::string & str_ref )
{
  bool retval( false );
  if( IsComplete() and ( not str_ref.empty() ) )
  {
    for( std::size_t i(0);
         ( i < str_ref.size() )
         and
         ( not retval );
         i++
    )
    {
      auto ch = str_ref.at(i);
      retval = m_Password.find( ch ) != std::string::npos;
    }
  }
  return retval;
}

std::size_t
PasswdGenerator::GetLength()
const
{
  return m_Length;
}

void
PasswdGenerator::SetLength(const std::size_t & Length)
{
  m_Length =
      Length >= DEFAULT_PASSWD_MINLENGTH() ?
        Length :
        DEFAULT_PASSWD_MINLENGTH();
}

void PasswdGenerator::DigitsOn()
{
  SwitchFlag( PasswdGenFlag::Digit, true );
}

void PasswdGenerator::DigitsOff()
{
  SwitchFlag( PasswdGenFlag::Digit, false );
}

bool PasswdGenerator::ContainsDigits() const
{
  return m_Flag.find( PasswdGenFlag::Digit ) != m_Flag.cend();
}

void PasswdGenerator::UpperOn()
{
  SwitchFlag( PasswdGenFlag::Upper, true );
}

void PasswdGenerator::UpperOff()
{
  SwitchFlag( PasswdGenFlag::Upper, false );
}

bool PasswdGenerator::ContainsUpper() const
{
  return m_Flag.find( PasswdGenFlag::Upper ) != m_Flag.cend();
}

void PasswdGenerator::LowerOn()
{
  SwitchFlag( PasswdGenFlag::Lower, true );
}

void PasswdGenerator::LowerOff()
{
  SwitchFlag( PasswdGenFlag::Lower, false );
}

bool PasswdGenerator::ContainsLower() const
{
  return m_Flag.find( PasswdGenFlag::Lower ) != m_Flag.cend();
}

void PasswdGenerator::SymbolOn()
{
  SwitchFlag( PasswdGenFlag::Symbol, true );
}

void PasswdGenerator::SymbolOff()
{
  SwitchFlag( PasswdGenFlag::Symbol, false );
}

bool PasswdGenerator::ContainsSymbol() const
{
  return m_Flag.find( PasswdGenFlag::Symbol ) != m_Flag.cend();
}

void PasswdGenerator::AmbiguousOn()
{
  SwitchFlag( PasswdGenFlag::WithoutAmbiguous, false );
}

void PasswdGenerator::AmbiguousOff()
{
  SwitchFlag( PasswdGenFlag::WithoutAmbiguous, true );
}

bool PasswdGenerator::WithoutAmbiguous() const
{
  return m_Flag.find( PasswdGenFlag::WithoutAmbiguous ) != m_Flag.cend();
}

void PasswdGenerator::VowelsOn()
{
  SwitchFlag( PasswdGenFlag::WithoutVowels, false );
}

void PasswdGenerator::VowelsOff()
{
  SwitchFlag( PasswdGenFlag::WithoutVowels, true );
}

bool PasswdGenerator::WithoutVowels() const
{
  return m_Flag.find( PasswdGenFlag::WithoutVowels ) != m_Flag.cend();
}

}// namespace                   admin
}// namespace                   core
}// namespace                   spo
