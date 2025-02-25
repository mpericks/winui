#include <windows.h>
#include <shlwapi.h>
#include <CommCtrl.h>
#include <windowsX.h>
#include <crtdbg.h>
#include <gdiplus.h>
#include <string>
#include <vector>
#include <sstream>
#include "thunker.h"
#include "GuiHelpers.h"
#include "ResourceHelpers.h"
#include "PngControl.h"

PngControl::PngControl(HWND parent_handle_init, const std::wstring& resource_name_init, const std::wstring& resource_type_init, HBRUSH background_brush_init ) : resource_name( resource_name_init ), resource_type( resource_type_init ), bitmap_ptr( NULL ), background_brush( background_brush_init )  
{
   Gdiplus::GdiplusStartupInput gdiplusStartupInput;
   ULONG_PTR           gdiplusToken;
   
   // Initialize GDI+.
   Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
   GuiHelpers::CreateCustomControl( this, std::wstring(L"Alien_PNG_Control"), (DWORD)'pngc', parent_handle_init ) ;
}

PngControl::~PngControl(void)
{
}

LRESULT PngControl::MessageHandler( HWND window_handle, UINT msg, WPARAM wParam, LPARAM lParam ) 
{
  UNREFERENCED_PARAMETER( lParam ) ;
  UNREFERENCED_PARAMETER( wParam ) ;
  UNREFERENCED_PARAMETER( window_handle ) ;
  LRESULT nRet = FALSE;
  switch (msg)
  {
    HANDLE_MSG(Handle(), WM_PAINT, OnPaint);
    case WM_GETDLGCODE: return 'pngc' ; break ;
    default:
     nRet = ::DefWindowProc( window_handle, msg, wParam, lParam ) ;
  }

  return nRet;
}

LRESULT PngControl::OnCreate( LPCREATESTRUCT create_info_ptr )
{
  UNREFERENCED_PARAMETER( create_info_ptr ) ;
  //background_brush = (HBRUSH)GetClassLong(this->ParentHandle(), GCL_HBRBACKGROUND);
  return TRUE ;
}

LRESULT PngControl::OnPaint(HWND hWnd)
{
   LRESULT ret = 0;

   PAINTSTRUCT ps;
   HDC hDC = ::BeginPaint(hWnd, &ps);
   if(hDC)
   {
	 RECT rc;
	 ::GetClientRect(hWnd, &rc);
	 POINT pt1, pt2;
	 pt1.x = rc.left;
	 pt1.y = rc.top;

	 pt2.x = rc.right;
	 pt2.y = rc.bottom;
	 ::ClientToScreen(hWnd, &pt1);
	 ::ClientToScreen(hWnd, &pt2);

	 HDC hDCMem = ::CreateCompatibleDC(hDC);
	 HBITMAP hBmpMem = ::CreateCompatibleBitmap(hDC, pt2.x - pt1.x, pt2.y - pt1.y);
	 ::SelectObject(hDCMem, hBmpMem);


   ::FillRect(hDCMem, &rc, background_brush );

	 ret = DrawGraphics(hDCMem, rc);

	 ::BitBlt(hDC, 0, 0, pt2.x - pt1.x, pt2.y - pt1.y, hDCMem, 0,0, SRCCOPY);
	 ::DeleteObject(hBmpMem);
	 ::DeleteDC(hDCMem);

    ::EndPaint(hWnd, &ps);
   }

   return ret;
}
LRESULT PngControl::DrawGraphics(HDC hDC, const RECT& rcClient)
{
  UNREFERENCED_PARAMETER( rcClient ) ; 
  LRESULT ret = 0;
   Gdiplus::Graphics    graphics(hDC);
   graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
   if ( NULL == bitmap_ptr )
   {
      IStream* png_stream_ptr ;
      if ( SUCCEEDED( ResourceHelpers::StreamFromResource( resource_name, resource_type, &png_stream_ptr ) ) )
      {
         bitmap_ptr = Gdiplus::Bitmap::FromStream( png_stream_ptr, false ) ;
      }
   }
   if ( NULL != bitmap_ptr && Gdiplus::Ok == bitmap_ptr->GetLastStatus() )
   {
     Gdiplus::RectF logo_image_rect( (Gdiplus::REAL)(0.0), (Gdiplus::REAL)(0.0), (Gdiplus::REAL)(bitmap_ptr->GetWidth() ), (Gdiplus::REAL)(bitmap_ptr->GetHeight() ) ) ;
       graphics.DrawImage( bitmap_ptr, logo_image_rect ) ;
   }
   return ret;
}
