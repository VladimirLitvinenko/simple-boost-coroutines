#ifndef DOCUMENTPKG_H
#define DOCUMENTPKG_H

#include "core/documents/BytesDocument.h"
#include "core/admin/ByteStuffing.h"
#include <string>

namespace                       spo   {
namespace                       core  {
namespace                       docs  {

//------------------------------------------------------------------------------
template
<
    typename ByteT_,
    typename                    = typename std::enable_if
                                  <
                                    std::is_same< ByteT_, unsigned char >::value
                                    or
                                    std::is_same< ByteT_, char >::value
                                    or
                                    std::is_same< ByteT_, signed char >::value
                                  >::type
>
class SPO_CORE_EXPORT           DocumentPkg :
public                          spo::core::docs::BytesDocument< ByteT_ >
{
public:
  using base_class_t          = spo::core::docs::BytesDocument< ByteT_ >;
  using stuffinglist_t        = std::vector< spo::core::admin::ByteStuffing >;

  /**
   * @brief Конструктор DocumentPkg
   */
  DocumentPkg ()
    : DocumentPkg::base_class_t()
  {
    base_class_t::GetIDRef() = spo::core::docs::DOC_DOCPKG_ID;
  }

  /**
   * @brief Конструктор DocumentPkg
   * @param pkg
   */
  DocumentPkg ( const DocumentPkg & pkg )
    : DocumentPkg::DocumentPkg()
  {
    *this = pkg;
  }

  /**
   * @brief Конструктор DocumentPkg
   * @param pkg
   */
  DocumentPkg ( DocumentPkg && pkg )
    : DocumentPkg::DocumentPkg()
  {
    *this = std::move( pkg );
  }

  /**
   * @brief operator ()
   * @return
   */
  virtual DocumentPkg &  operator()()
  {
    return std::ref( * this );
  }
  /**
   * @brief operator =
   * @param pkg
   * @return
   */
  virtual DocumentPkg & operator= ( const DocumentPkg & pkg )
  {
    DocumentPkg::base_class_t::operator =( pkg );
    mStuffing = pkg.mStuffing;
    return std::ref( *this );
  }
  /**
   * @brief operator =
   * @param pkg
   * @return
   */
  virtual DocumentPkg & operator= ( DocumentPkg && pkg )
  {
    DocumentPkg::base_class_t::operator =( std::move( pkg ) );
    mStuffing.swap( pkg.mStuffing );
    return std::ref( *this );
  }

  /**
   * @brief IsValid
   * @return
   */
  bool IsValid () const override
  {
    return
        base_class_t::IsValid()
        and
        ( mStuffing.size() == 2 )
        and
        ( not mStuffing.at(0).GetFrom().empty() )
        and
        ( not mStuffing.at(0).GetTo().empty() )
        and
        ( not mStuffing.at(1).GetFrom().empty() )
        and
        ( not mStuffing.at(1).GetTo().empty() )
        ;
  }

  std::string GetHeader () const
  {
    return
        IsValid()
        ? mStuffing.at(0).GetFrom() + mStuffing.at(1).GetFrom()
        : std::string() ;
  }

  /**
   * @brief Метод HasHeader проверяет наличие заголовка пакета в наборе данных
   *        документа.
   * @param startIndex
   * @return
   */
  std::pair< bool, std::size_t > HasHeader( std::size_t startIndex = 0 ) const
  {
    std::pair< bool, std::size_t > retval{ IsValid(), std::string::npos };
    try
    {
      if( retval.first )
      {
        std::string header( GetHeader() );
        retval.first = not header.empty();
        if( retval.first )
        {
          retval.second = StrContent().find( header, startIndex );
          retval.first  = retval.second != std::string::npos;
        }
      }
    }
    catch( const std::exception & e )
    {
      retval = { false, std::string::npos };
      DUMP_EXCEPTION( e );
    }
    return retval;
  }

  /**
   * @brief HasHeader
   * @param iter
   * @return
   */
  template< typename IteratorT_ >
  std::pair< bool, IteratorT_ > HasHeader( const IteratorT_ & iter )
  {
    typedef IteratorT_ iter_end;

    std::pair< bool, IteratorT_ > retval =
        std::move(
          std::make_pair(
            IsValid(),
            iter_end() ) );

    try
    {
      if( retval.first )
      {
        std::string header( GetHeader() );
        retval.first = not header.empty();
        if( retval.first )
        {
          retval.second =
              std::search(
                iter,
                base_class_t::mContent.end(),
                header.begin(), header.end() );
          retval.first  = retval.second != base_class_t::mContent.end();
        }
      }
    }
    catch( const std::exception & e )
    {
      retval = std::move(
                 std::make_pair(
                   false,
                   iter_end() ) );
      DUMP_EXCEPTION( e );
    }
    return retval;
  }

  std::pair< bool, DocumentPkg<  ByteT_ > > GetPackage()
  {
    BEGIN_LOCK_SECTION_SELF_;

    std::pair< bool, DocumentPkg<  ByteT_ > > retval
    {
      false,
      DocumentPkg<  ByteT_ >()
    };
    auto header_validator =
        std::move(
          HasHeader<typename base_class_t::element_t::iterator>(
            base_class_t::mContent.begin() ) );

    if( header_validator.first )
    {
      auto futer_validator =
          HasHeader<typename base_class_t::element_t::iterator>(
            header_validator.second + GetHeader().size() );

      retval.first = futer_validator.first;
      if( retval.first )
      {
        retval.second.Add( header_validator.second, futer_validator.second - 1 );
        retval.first = not retval.second.IsEmpty();
        if( retval.first )
        {
          base_class_t::mContent.erase(
                base_class_t::mContent.begin(),
                futer_validator.second -1 );
        }
      }
    }
    return std::move( retval );

    END_LOCK_SECTION_;
  }

  std::vector< DocumentPkg<  ByteT_ > > GetPackages()
  {
    std::vector< DocumentPkg<  ByteT_ > > retval;
    return std::move( retval );
  }

  /**
   * @brief GetPrefix
   * @return
   */
  std::string GetPrefix () const
  {
    return
        IsValid()
        ? mStuffing.at(0).GetFrom()
        : std::string() ;
  }

  /**
   * @brief GetSuffix
   * @return
   */
  std::string GetSuffix () const
  {
    return
        IsValid()
        ? mStuffing.at(1).GetFrom()
        : std::string() ;
  }

  /**
   * @brief GetStuffing
   * @return
   */
  stuffinglist_t GetStuffing () const
  {
    return mStuffing;
  }

  std::string StrContent () const
  {
    std::string retval;
    retval.assign( base_class_t::mContent.begin(), base_class_t::mContent.end() );
    return std::move( retval );
  }

  /**
   * @brief FromByteArray
   * @param array
   * @return
   */
  bool FromByteArray ( const std::string & array ) override
  {
    return DocumentPkg::base_class_t::FromByteArray( Unstuff( array ) );
  }

  /**
   * @brief ToByteArray
   * @return
   */
  std::string ToByteArray ()  const override
  {
    return Stuff( DocumentPkg::base_class_t::ToByteArray() );
  }

protected:
  /**
   * @brief SetPrefix
   * @param prefix
   */
  void SetPrefix ( const std::string & prefix )
  {
    auto pref( GetPrefix() );
    for( auto & val_ref : mStuffing )
    {
      if( val_ref.GetFrom() == pref )
        val_ref.SetFrom( prefix );
    }
    pref = prefix;
  }
  /**
   * @brief SetSuffix
   * @param suffix
   */
  void SetSuffix ( const std::string & suffix )
  {
    auto suf( GetSuffix() );
    for( auto & val_ref : mStuffing )
    {
      if( val_ref.GetTo() == suf )
        val_ref.SetTo( suffix );
    }
    suf = suffix;
  }

private:
  mutable stuffinglist_t mStuffing
  {
    spo::core::admin::ByteStuffing( { std::string( 1, char(0xAA) ),
                                      std::string( 1, char(0xBB) ) + std::string( 1, char(0x00) ) } ),
    spo::core::admin::ByteStuffing( { std::string( 1, char(0xBB) ),
                                      std::string( 1, char(0xBB) ) + std::string( 1, char(0x01) ) } ),
  };

  /**
   * @brief Stuff
   * @param array
   * @return
   */
  std::string Stuff ( const std::string & array ) const
  {
    std::string retval( array );
    for( auto & val_ref : mStuffing )
    {
      retval = val_ref.Stuff( retval );
    }
    return retval;
  }
  /**
   * @brief Unstuff
   * @param array
   * @return
   */
  std::string Unstuff ( const std::string & array )
  {
    std::string retval( array );
    for( auto & val_ref : mStuffing )
    {
      retval = val_ref.Unstuff( retval );
    }
    return retval;
  }
};
//------------------------------------------------------------------------------

}// namespace                   cocs
}// namespace                   core
}// namespace                   spo

#endif // DOCUMENTPKG_H
