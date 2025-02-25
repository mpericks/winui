#pragma once

namespace ResourceHelpers
{
   DWORD OpenResource( const std::wstring& resource_name, const std::wstring& resource_type, void** resource_data_ptr_ptr, DWORD* size_ptr ) ;
   HRESULT StreamFromResource( const std::wstring& resource_name, const std::wstring& resource_type, IStream** stream_ptr_ptr ) ;
   std::wstring GetString( DWORD string_id ) ;
};
