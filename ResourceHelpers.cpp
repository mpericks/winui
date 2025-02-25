#include <windows.h>
#include <string>
#include "ResourceHelpers.h"

DWORD ResourceHelpers::OpenResource( const std::wstring& resource_name, const std::wstring& resource_type, void** resource_data_ptr_ptr, DWORD* size_ptr )
{
   HRSRC resource_handle = ::FindResourceW( ::GetModuleHandleW( NULL ), resource_name.c_str(), resource_type.c_str() ) ;
   if ( NULL == resource_handle  )
   {
      return ::GetLastError() ;
   }
   //don't use an auto handle object to wrap this handle. Even though it's an HGLOBAL, calling GlobalFree on it can crash.
   HGLOBAL global_mem_handle = ::LoadResource( ::GetModuleHandleW( NULL ), resource_handle ) ;
   if ( NULL == global_mem_handle )
   {
      return ::GetLastError() ;
   }
   void* resource_data = ::LockResource( global_mem_handle ) ;
   if ( NULL == resource_data )
   {
      return ERROR_UNIDENTIFIED_ERROR ;
   }
   DWORD data_size = ::SizeofResource( ::GetModuleHandleW( NULL ), resource_handle ) ;
   if ( 0 == data_size )
   {
      return ::GetLastError() ;
   }

   *resource_data_ptr_ptr = resource_data ;
   if (NULL != size_ptr )
   {
      *size_ptr = data_size ;
   }
   return ERROR_SUCCESS ;
}
HRESULT ResourceHelpers::StreamFromResource( const std::wstring& resource_name, const std::wstring& resource_type, IStream** stream_ptr_ptr )
{
   DWORD resource_size = 0 ;
   void* resource_data_ptr = NULL ;
   DWORD error = ResourceHelpers::OpenResource( resource_name, resource_type, &resource_data_ptr, &resource_size ) ;
   if ( ERROR_SUCCESS != error )
   {
      return MAKE_HRESULT( SEVERITY_ERROR, FACILITY_WIN32, error ) ;
   }
   HGLOBAL global_mem_handle = ::GlobalAlloc( GMEM_ZEROINIT, resource_size ) ;
   void* temp_ptr = ::GlobalLock( global_mem_handle ) ;
   if ( NULL == temp_ptr )
   {
      return MAKE_HRESULT( SEVERITY_ERROR, FACILITY_WIN32, ERROR_OUTOFMEMORY ) ;
   }
   ::memcpy( temp_ptr, resource_data_ptr, resource_size ) ;
   ::GlobalUnlock( global_mem_handle ) ;
   return ::CreateStreamOnHGlobal( global_mem_handle, true, stream_ptr_ptr ) ;
}
std::wstring ResourceHelpers::GetString( DWORD string_id )
{

   DWORD error_for_debugger = ERROR_SUCCESS ;
   //great example of Windows trivia. Have to mess with the string id to find it using FindResource
   int fake_string_id = (string_id >> 4) + 1;
   HRSRC resource_handle = ::FindResourceW( ::GetModuleHandleW( NULL ), MAKEINTRESOURCEW( fake_string_id ), RT_STRING ) ;
   if ( NULL == resource_handle  )
   {
      error_for_debugger = ::GetLastError() ;
      return std::wstring(L"") ;
   }
   DWORD data_size = ::SizeofResource( ::GetModuleHandleW( NULL ), resource_handle ) ;
   if ( 0 == data_size )
   {
      return std::wstring(L"") ;
   }

   std::wstring return_string ;
   DWORD string_char_count = data_size / sizeof (wchar_t) ;
   if ( 0 == string_char_count )
   {
      return std::wstring(L"") ;
  }

   wchar_t* resource_string = new wchar_t[ string_char_count + 1 ] ;
   ::ZeroMemory( resource_string, ( string_char_count + 1 )*sizeof(wchar_t) ) ; 
   if ( 0 != ::LoadStringW( ::GetModuleHandleW( NULL ), string_id, resource_string, string_char_count) )
   {
      return_string.assign( resource_string ) ;
   }
   delete[] resource_string ;
   return return_string ;

}