#pragma once

bool LoadGdiPlusBitmapResource(LPCTSTR pName, 
                               LPCTSTR pType, 
                               HMODULE hInst, 
                               Gdiplus::Bitmap** bitmap_out)
{

    HRSRC hResource = ::FindResource(hInst, pName, pType);
    if (!hResource)
        return false;
    
    DWORD imageSize = ::SizeofResource(hInst, hResource);
    if (!imageSize)
        return false;

    const void* pResourceData = ::LockResource(::LoadResource(hInst, 
                                              hResource));
    if (!pResourceData)
        return false;

    HGLOBAL hBuffer  = ::GlobalAlloc(GMEM_MOVEABLE, imageSize);
    if (hBuffer)
    {
      void* pBuffer = ::GlobalLock(hBuffer);
      if (pBuffer)
      {
        CopyMemory(pBuffer, pResourceData, imageSize);

        IStream* pStream = NULL;
        if (::CreateStreamOnHGlobal(hBuffer, FALSE, &pStream) == S_OK)
        {
          *bitmap_out = Gdiplus::Bitmap::FromStream(pStream);
          pStream->Release();
          if (*bitmap_out)
          { 
            if ( (*bitmap_out)->GetLastStatus() == Gdiplus::Ok)
            {
              return true;
            }

            delete *bitmap_out;
            *bitmap_out = NULL;
          }
        }
        *bitmap_out = NULL;
        ::GlobalUnlock(hBuffer);
      }
      ::GlobalFree(hBuffer);
    }
    return false;
}