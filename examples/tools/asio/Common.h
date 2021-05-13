#include "core/documents/DocumentPkg.h"
#include <string>

spo::core::docs::DocumentPkg<char> pkg;

static std::atomic_bool halfDuplexMode  { false };
static std::atomic_bool receiveComplete { true };

void SetHalfDuplexMode( bool value )
{
  halfDuplexMode.store( value );
}

bool DumpReceive ( spo::core::docs::BytesDocument<char> & data )
{
  bool retval( not data.IsEmpty() );
  if( retval )
  {
    pkg.Add( data.ContentRef(), data.Size() );

    {
      std::pair< bool, spo::core::docs::DocumentPkg< char > > true_pkg;
      do
      {
        true_pkg = std::move( pkg.GetPackage() );
        if(halfDuplexMode and not receiveComplete )
          receiveComplete = true_pkg.first;

        if( true_pkg.first )
        {
          std::cout << "\n\t-------------------- First pkg --------------------" << std::endl;
          std::cout << "SIZE: " << true_pkg.second.Size() << std::endl;
          std::cout << true_pkg.second.ContentRef().data() << std::endl;
        }
      }
      while( true_pkg.first );
    }

    std::cout << "\n\t-------------------- Received data --------------------" << std::endl;
    std::cout << pkg.ContentRef().data() << std::endl;
    std::flush( std::cout );
  }
  return retval;
}

bool DumpSend ( spo::core::docs::BytesDocument<char> & data )
{
  if( receiveComplete  )
  {
    auto tm = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::string tstr( std::ctime(& tm ) );
    std::string str(1, char( 0xAA ) );
    str += std::string( 1, char( 0xBB ) ) + std::string(  1000, 'S' );
//    str += std::string(1, char( 0xAA ) ) + std::string( 1, char( 0xBB ) ) + std::string(  1000, 'S' ) + str + "  " + tstr;
    str += tstr + "\n";

    data.FromByteArray(str);
    std::cout << "\n\t-------------------- Send --------------------" << std::endl;
    auto dumpstr( data.ToByteArray() );
    std::cout << "SIZE: " << dumpstr.size() << std::endl;
    std::cout << std::string( dumpstr.c_str(), 10 ) << "..." << (dumpstr.size() - 20) << "..." << std::string( dumpstr.c_str() + dumpstr.size() - 10, 10 ) << std::endl;
    std::flush( std::cout );
  }

  return not data.IsEmpty();
}
