#ifndef BYTESDOCUMENT_H
#define BYTESDOCUMENT_H

#include "core/documents/IDocument.h"
#include <boost/asio/streambuf.hpp>
#include "asio/AsioCommon.h"

namespace                         spo   {
namespace                         core  {
namespace                         docs  {

//------------------------------------------------------------------------------
/**
 * @brief Шаблонн BytesDocument определяет класс работы с типизированным набором байт.
 * @typedef ByteT_ тип представления байта
 *
 * Расширяет определение класса @a spo::core::docs::Document
 *
 * @note тип ByteT_ представления байта может быть одним из предопределенных типов языка:
 * @l char символ (включает знак в 0-м байте);
 * @l unsigned char символ без знака;
 * @l signed char явное определение симфола со знаком.
 *
 * @see spo::core::docs::Document, spo::core::docs::IDocument
 */
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
class SPO_CORE_EXPORT             BytesDocument :
public                            spo::core::docs::Document< spo::socket_byffer_t< ByteT_ > >
{
public:
  using doc_class_t             = spo::core::docs::Document< spo::socket_byffer_t< ByteT_ > >;
  using self                    = BytesDocument;

  /**/                            BytesDocument       ()
                                : doc_class_t         ( spo::core::docs::DOC_BYTEARRAY_ID )
  {}
  /**/                            BytesDocument       ( typename doc_class_t::element_t::size_type size )
                                : self::BytesDocument ()
  {
    doc_class_t::mContent.resize( size );
  }
  /**/                            BytesDocument       ( int id )
                                : doc_class_t         ( id )
  {}
  /**/                            BytesDocument       ( const BytesDocument<ByteT_> & doc )
                                : self::BytesDocument ()
  {
    * this = doc;
  }
  /**/                            BytesDocument       ( BytesDocument<ByteT_> && doc )
                                : self::BytesDocument ()
  {
    * this = std::move( doc );
  }
  virtual                       ~ BytesDocument       ()  {}

  /**
   * @brief operator =
   * @param doc
   * @return
   */
  virtual BytesDocument<ByteT_> & operator = ( const BytesDocument<ByteT_> & doc )
  {
    BEGIN_LOCK_SECTION_SELF_;
    doc_class_t::mContent =
        const_cast< BytesDocument<ByteT_> & >(doc).mContent;
    return std::ref( * this );
    END_LOCK_SECTION_
  }

  /**
   * @brief Size
   * @return
   */
  std::size_t Size () const
  {
    return doc_class_t::mContent.size();
  }

  /**
   * @brief IsEmpty
   * @return
   */
  bool IsEmpty() const
  {
    return doc_class_t::mContent.empty();
  }

  /**
   * @brief Swap
   * @param doc
   */
  void Swap ( BytesDocument<ByteT_> & doc )
  {
    BEGIN_LOCK_SECTION_SELF_;
    doc_class_t::mContent.swap( doc.mContent );
    END_LOCK_SECTION_
  }

  /**
   * @brief operator =
   * @param doc
   * @return
   */
  virtual BytesDocument<ByteT_> & operator = ( BytesDocument<ByteT_> && doc )
  {
    BEGIN_LOCK_SECTION_SELF_;
    Swap( doc );
    return std::ref( * this );
    END_LOCK_SECTION_
  }

  /**
   * @brief operator =
   * @param str
   * @return
   */
  virtual BytesDocument<ByteT_> & operator = ( const std::string & str )
  {
    BEGIN_LOCK_SECTION_SELF_;
    ClearContent();
    Add( str );
    return std::ref( * this );
    END_LOCK_SECTION_
  }

  /**
   * @brief FromByteArray
   * @param array
   * @return
   */
  bool FromByteArray ( const std::string & array ) override
  {
    bool retval { not array.empty() };
    BEGIN_LOCK_SECTION_SELF_;
    try
    {
      ClearContent();
      if( retval )
      {
        doc_class_t::mContent.assign( array.cbegin(), array.cend() );
        retval = not IsEmpty();
      }
    }
    catch( const std::exception & e )
    {
      retval = false;
      DUMP_EXCEPTION( e );
    }
    return retval;
    END_LOCK_SECTION_
  }

  /**
   * @brief ToByteArray
   * @return
   */
  std::string ToByteArray () const override
  {
    std::string retval;
    BEGIN_LOCK_SECTION_SELF_;
    try
    {
      retval = retval.assign( doc_class_t::mContent.cbegin(), doc_class_t::mContent.cend() );
    }
    catch( const std::exception & e )
    {
      retval.clear();
      DUMP_EXCEPTION( e );
    }

    return retval;
    END_LOCK_SECTION_
  }

  /**
   * @brief ClearContent
   */
  void ClearContent () override
  {
    BEGIN_LOCK_SECTION_SELF_;
    doc_class_t::mContent.clear();
    END_LOCK_SECTION_
  }

  /**
   * @brief Add
   * @param v
   * @param v_size
   */
  virtual void Add ( const typename doc_class_t::element_t & v, std::size_t v_size = 0 )
  {
    BEGIN_LOCK_SECTION_SELF_;
    try
    {
      auto sz   ( v.size() );
      auto v_sz ( v_size == 0 ? sz : ( v_size < sz ? v_size : sz ) );
      doc_class_t::ContentRef().reserve( doc_class_t::ContentRef().size() + v_sz );
      for( std::size_t i(0); i < v_sz; i++ )
        doc_class_t::ContentRef().push_back( v.at(i) );
    }
    catch( const std::exception & e )
    {
      DUMP_EXCEPTION( e )
    }
    END_LOCK_SECTION_
  }

  /**
   * @brief Add
   * @param data_ptr
   * @param v_size
   */
  virtual void Add ( const void * data_ptr, std::size_t v_size )
  {
    BEGIN_LOCK_SECTION_SELF_;
    try
    {
      doc_class_t::ContentRef().reserve( doc_class_t::ContentRef().size() + v_size );
      for( std::size_t i(0); i < v_size; i++ )
      {
        doc_class_t::ContentRef().push_back(
              *( reinterpret_cast< ByteT_* >(
                   const_cast< void* >( data_ptr ) ) + i ) );
      }
    }
    catch( const std::exception & e )
    {
      DUMP_EXCEPTION( e )
    }
    END_LOCK_SECTION_
  }

  virtual void Add ( const typename doc_class_t::element_t::iterator & iterFirst,
                     const typename doc_class_t::element_t::iterator & iterLast )
  {
    BEGIN_LOCK_SECTION_SELF_;
    try
    {
      doc_class_t::ContentRef().reserve( doc_class_t::ContentRef().size() + std::distance( iterFirst, iterLast ) );
      for( auto iter(iterFirst); iter <= iterLast; iter++ )
        doc_class_t::ContentRef().push_back( *iter );
    }
    catch( const std::exception & e )
    {
      DUMP_EXCEPTION( e )
    }
    END_LOCK_SECTION_
  }

//  virtual void Add ( const boost::asio::streambuf::mutable_buffers_type & buf )
//  {
//    BEGIN_LOCK_SECTION_SELF_;
//    try
//    {
//      auto buf_size( boost::asio::detail::buffer_size_helper(buf) );
//      doc_class_t::ContentRef().reserve( buf_size );

//      auto ptr      ( boost::asio::detail::buffer_cast_helper( buf ) );
//      for( std::size_t i( 0 ); i < buf_size; i++ )
//        doc_class_t::ContentRef().push_back( *( reinterpret_cast<ByteT_*>( ptr )+ i ) );
//    }
//    catch( const std::exception & e )
//    {
//      DUMP_EXCEPTION( e )
//    }
//    END_LOCK_SECTION_
//  }

  virtual void Add ( const std::string & str )
  {
    BEGIN_LOCK_SECTION_SELF_;
    try
    {
      std::vector< ByteT_> data( str.size(), '0' );
      Add( data, str.size() );
    }
    catch( const std::exception & e )
    {
      DUMP_EXCEPTION( e )
    }
    END_LOCK_SECTION_
  }

  virtual void Remove ( std::size_t idx_from,  std::size_t idx_to )
  {
    BEGIN_LOCK_SECTION_SELF_;
    try
    {
      if( ( idx_from < doc_class_t::mContent.size() )
          and
          ( idx_to < doc_class_t::mContent.size() )
          and
          ( idx_from < idx_to ) )
      {
        doc_class_t::mContent.erase( doc_class_t::mContent.begin() + idx_from,
                                     doc_class_t::mContent.begin() + idx_from + idx_to );
      }
    }
    catch( const std::exception & e )
    {
      DUMP_EXCEPTION( e )
    }
    END_LOCK_SECTION_
  }

  /**
   * @brief Метод ToStream
   * @param buffer
   * @param size
   * @return
   */
  bool ToStream ( boost::asio::streambuf & buffer, std::size_t size ) const
  {
    BEGIN_LOCK_SECTION_SELF_;
//    std::cout << std::boolalpha << base_class_t::HasHeader() << std::endl;
    auto retval ( false );
    try
    {
      buffer.prepare( size );
      auto data( ToByteArray() );
      retval = 0 < data.size();
      if( retval )
      {
// 1-й вариант:
//        os.write( data.c_str(), data.size() );
// 2-й вариант:
        std::ostream os( & buffer );
        os << std::skipws << data;
      }
    }
    catch( const std::exception & e )
    {
      retval = false;
//      UNUSED( spo::asio::AsioService::ExceptionError() );
      DUMP_EXCEPTION( e );
    }
    return retval;
    END_LOCK_SECTION_;
  }

  /**
   * @brief Метод FromStream
   * @param buff
   * @return
   */
  bool FromStream ( boost::asio::streambuf & buff )
  {
    BEGIN_LOCK_SECTION_SELF_;
    auto retval( 0 < buff.size() );
    if( retval )
    {
      try
      {
        std::istream is( & buff );
        decltype( doc_class_t::mContent ) content( buff.size() );
// 1-й вариант:
//        std::skipws( is );
//        content.assign(
//              std::istreambuf_iterator< ByteT_ >( is ),
//              std::istreambuf_iterator< ByteT_ >() );
//
// 2-й вариант:
        is >> std::skipws >> content.data();

        Add( content);
//        retval = FromByteArray( doc_class_t::mContent.data() );
      }
      catch( const std::exception & e )
      {
        retval = false;
//        UNUSED( spo::asio::AsioService::ExceptionError() );
        DUMP_EXCEPTION( e );
      }
    }
    END_LOCK_SECTION_;
    return retval;
  }
};

//------------------------------------------------------------------------------

}// namespace                     docs
}// namespace                     core
}// namespace                     spo

#endif // BYTESDOCUMENT_H
