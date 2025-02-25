#pragma once
#include "GuiHelpers.h"
#include <gdiplus.h>

class PngControl : public GuiHelpers::CustomControl
{
public:
  PngControl(HWND parent_handle_init, const std::wstring& resource_name_init, const std::wstring& resource_type_init, HBRUSH background_brush_init);
  ~PngControl(void);
public: 
  LRESULT MessageHandler( HWND window_handle, UINT msg, WPARAM wParam, LPARAM lParam ) ;
private:
  virtual LRESULT OnCreate( LPCREATESTRUCT create_info_ptr ) ;
  LRESULT OnPaint(HWND hWnd);
  LRESULT DrawGraphics(HDC hDC, const RECT& rcClient);
private:
  std::wstring resource_name ;
  std::wstring resource_type ;
  Gdiplus::Bitmap* bitmap_ptr ;
  HBRUSH background_brush ;
};
