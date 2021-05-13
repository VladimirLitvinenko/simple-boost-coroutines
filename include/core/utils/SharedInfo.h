#ifndef SHARED_INFO_H
#define SHARED_INFO_H

#include "core/admin/AdminGlobal.h"
#include "core/utils/CoreUtils.h"
#include "MainLiterals.h"
#include <boost/thread.hpp>

namespace                           spo   {
namespace                           core  {
namespace                           utils {

//------------------------------------------------------------------------------

class SPO_CORE_EXPORT               SharedInfo
{
public:
  enum                              Mode                : quint64
  {
    NoMask                        = quint64    ( 0 ),
    ErrorMask                     = quint64(1) << ( sizeof(Mode) * 8 - 1 ),
    ExceptionMask                 = quint64(1) << ( sizeof(Mode) * 8 - 1 ),
  };

  enum                              Status              : quint64
  {
    Ok                            = static_cast<quint64>( Mode::NoMask ),
    Overflow                      = 1
                                    |
                                    static_cast<quint64>(Mode::ExceptionMask),

    NoMemory                      = 2
                                    |
                                    static_cast<quint64>(Mode::ExceptionMask),

    InvalidArgument               = 4
                                    |
                                    static_cast<quint64>(Mode::ExceptionMask),

    MaxStatusValue                = quint64(1) << ( sizeof( quint32 ) * 8 ),
  };

  using status_handler_t          = std::function< void( SharedInfo::Status ) >;

private:
  std::atomic< Status >             m_Status              { Status::Ok };
  status_handler_t                  m_StatusHandler       { nullptr };

protected:
  /**/                              SharedInfo          () = default;

public:

  /**/                              SharedInfo          ( const SharedInfo & );
  /**/                              SharedInfo          ( SharedInfo && );

  virtual SharedInfo &              operator=           ( const SharedInfo & );
  virtual SharedInfo &              operator=           ( SharedInfo && );

  void                              SetStatusHandler    ( SharedInfo::status_handler_t handler ) BOOST_NOEXCEPT;

  virtual bool                      IsValid             () const;

  SharedInfo::Status                GetStatus           () const;
  void                              SetStatus           ( const SharedInfo::Status & s );
  void                              SetMode             ( const SharedInfo::Mode & mode );
  SharedInfo::Mode                  GetMode             () const;
  std::string                       StatusString        ();
  std::size_t                       MaxSize             () const;
  void                              SetMaxSize          ( const std::size_t & maxSize = INT16_MAX );

private:
  std::size_t                       m_MaxSize           = INT16_MAX;
  static std::map< quint64, std::string > m_ModeStrings;
  static std::map< quint64, std::string > m_StatusStrings;
};

//------------------------------------------------------------------------------

}// namespace                       utils
}// namespace                       asio
}// namespace                       spo

#endif // SHARED_INFO_H
