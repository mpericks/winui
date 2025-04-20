#include <windows.h>
#include <richedit.h>
#include <CommCtrl.h>
#include <windowsx.h>
#include <crtdbg.h>
#include <string>
#include <vector>
#include <algorithm>
#include "TextHelpers.h"
#include "ResourceHelpers.h"
#include "thunker.h"
#include "GuiHelpers.h"

#pragma comment(lib, "Comctl32.lib")

GuiHelpers::ResizeParameters::ResizeParameters()
{
   ::ZeroMemory( &pinned_edges[0], sizeof ( pinned_edges ) ) ;
   ::ZeroMemory( &centered_dimensions[0], sizeof ( centered_dimensions ) ) ;
   left_border = 0 ;
   right_border = 0 ;
   top_border = 0 ;
   bottom_border = 0 ;
}
GuiHelpers::ResizeParameters::~ResizeParameters()
{

}
void GuiHelpers::ResizeParameters::PinTopLeft(UINT top_margin, UINT left_margin)
{
   pinned_edges[pinned_edge_top] = pinned_edges[pinned_edge_left] = true ;
   pinned_edges[pinned_edge_bottom] = pinned_edges[pinned_edge_right] = false ;
   top_border = top_margin ;
   left_border = left_margin ;
}
void GuiHelpers::ResizeParameters::PinTopRight(UINT top_margin, UINT right_margin)
{
   pinned_edges[pinned_edge_top] = pinned_edges[pinned_edge_right] = true ;
   pinned_edges[pinned_edge_bottom] = pinned_edges[pinned_edge_left] = false ;
   top_border = top_margin ;
   right_border = right_margin ;
}
void GuiHelpers::ResizeParameters::PinBottomRight(UINT bottom_margin, UINT right_margin )
{
   pinned_edges[pinned_edge_bottom] = pinned_edges[pinned_edge_right] = true ;
   pinned_edges[pinned_edge_top] = pinned_edges[pinned_edge_left] = false ;
   right_border = right_margin ;
   bottom_border = bottom_margin ;
}
void GuiHelpers::ResizeParameters::PinAll(UINT top_margin, UINT left_margin, UINT right_margin, UINT bottom_margin) 
{
   pinned_edges[0] = pinned_edges[1] = pinned_edges[2] = pinned_edges[3] = true ;
   right_border = right_margin ;
   bottom_border = bottom_margin ;
   top_border = top_margin ;
   left_border = left_margin ;
}
void GuiHelpers::ResizeParameters::PinBottomAndCenterHorizontal( UINT bottom_margin )
{
   ::ZeroMemory( &pinned_edges[0], sizeof ( pinned_edges ) ) ;
   ::ZeroMemory( &centered_dimensions[0], sizeof ( centered_dimensions ) ) ;
   pinned_edges[pinned_edge_bottom] = true ;
   centered_dimensions[ center_horizontal ] = true ;
   bottom_border = bottom_margin ;
}
void GuiHelpers::ResizeParameters::PinTopAndCenterHorizontal( UINT top_margin )
{
   ::ZeroMemory( &pinned_edges[0], sizeof ( pinned_edges ) ) ;
   ::ZeroMemory( &centered_dimensions[0], sizeof ( centered_dimensions ) ) ;
   pinned_edges[pinned_edge_top] = true ;
   centered_dimensions[ center_horizontal ] = true ;
   top_border = top_margin ;
}
void GuiHelpers::ResizeParameters::PinTopAndSizeHorizontal( UINT top_margin, UINT left_margin, UINT right_margin )
{
   ::ZeroMemory( &pinned_edges[0], sizeof ( pinned_edges ) ) ;
   ::ZeroMemory( &centered_dimensions[0], sizeof ( centered_dimensions ) ) ;
   pinned_edges[pinned_edge_top] = true ;
   pinned_edges[pinned_edge_right ] = pinned_edges[pinned_edge_left ] = true ;
   top_border = top_margin ;
   left_border = left_margin ;
   right_border = right_margin ;
}
bool GuiHelpers::ResizeParameters::IsPinned( UINT edge_id ) const
{
   if ( 4 > edge_id )
   {
      return pinned_edges[ edge_id ] ;
   }
   else
   {
      _RPTF0( _CRT_ASSERT, "Out of range index for pinned edge query in resized params" ) ;
   }
   return false;
}
bool GuiHelpers::ResizeParameters::IsCentered( UINT dimension_id ) const
{
   if ( 2 > dimension_id )
   {
      return centered_dimensions[ dimension_id ] ;
   }
   else
   {
      _RPTF0( _CRT_ASSERT, "Out of range index for centered dimension query in resized params" ) ;
   }
   return false ;
}
UINT GuiHelpers::ResizeParameters::RightBorder() const { return right_border; }
UINT GuiHelpers::ResizeParameters::TopBorder() const {return top_border; }
UINT GuiHelpers::ResizeParameters::BottomBorder() const { return bottom_border; }
UINT GuiHelpers::ResizeParameters::LeftBorder() const { return left_border; } 

///////////////////////////////////////////////////////////

////////////////////////////////////////////////////////
DWORD GuiHelpers::ResizeChildFromParent( HWND child_handle, 
                                       HWND parent_handle, 
                                       const GuiHelpers::ResizeParameters& resize_params )
{
   //get parent width and height
   RECT size_rect = {0,0,0,0};
   ::GetClientRect( parent_handle, &size_rect ) ;
   UINT width = size_rect.right - size_rect.left ;
   UINT height = size_rect.bottom - size_rect.top ;
   return GuiHelpers::ResizeChildFromParentDims( child_handle, width, height, resize_params ) ;
}
DWORD GuiHelpers::ResizeChildFromParentDims( HWND window_handle, 
                                           UINT width, UINT height, 
                                           const GuiHelpers::ResizeParameters& parameters)
{
   RECT position_rect = {0,0,0,0} ;
   if ( parameters.IsPinned( GuiHelpers::ResizeParameters::pinned_edge_right ) )
   {
      position_rect.right = width - parameters.RightBorder() ;
   }
   if ( parameters.IsPinned( GuiHelpers::ResizeParameters::pinned_edge_left ) )
   {
      position_rect.left = parameters.LeftBorder() ;
   }
   if ( parameters.IsPinned( GuiHelpers::ResizeParameters::pinned_edge_top ) )
   {
      position_rect.top = parameters.TopBorder() ;
   }
   if ( parameters.IsPinned( GuiHelpers::ResizeParameters::pinned_edge_bottom ) )
   {
      position_rect.bottom = height - parameters.BottomBorder() ;
   }
   
   RECT subview_rect ;
   ::GetWindowRect( window_handle, &subview_rect ) ;
   LONG subview_width = subview_rect.right - subview_rect.left ;
   LONG subview_height = subview_rect.bottom - subview_rect.top ;

   if ( !parameters.IsPinned( GuiHelpers::ResizeParameters::pinned_edge_right ) )
   {
      //right edge isn't pinned
      if ( parameters.IsPinned( GuiHelpers::ResizeParameters::pinned_edge_left ) )
      {
         //left edge is pinnned
         position_rect.right = position_rect.left + subview_width ;
      }
      else
      {
         //left nor right is pinned stay where we are
         position_rect.right = subview_rect.right ;
      }
   }
   if ( !parameters.IsPinned( GuiHelpers::ResizeParameters::pinned_edge_left ) )
   {
      //left edge isn't pinned
      if ( parameters.IsPinned( GuiHelpers::ResizeParameters::pinned_edge_right ) )
      {
         //right edge is pinnned
         position_rect.left = position_rect.right - subview_width ;
      }
      else
      {
         //left nor right is pinned stay where we are
         position_rect.right = subview_rect.right ;
      }
   }
   if (!parameters.IsPinned( GuiHelpers::ResizeParameters::pinned_edge_top ) )
   {
      //top is not pinned
      if (parameters.IsPinned( GuiHelpers::ResizeParameters::pinned_edge_bottom ))
      {
         //bottom is pinned
         position_rect.top = position_rect.bottom - subview_height ;
      }
      else
      {
         //bottom nor top is pinned
         position_rect.top = subview_rect.top ;
      }

   }
   if (!parameters.IsPinned( GuiHelpers::ResizeParameters::pinned_edge_bottom ) )
   {
      //bottom is not pinned
      if (parameters.IsPinned( GuiHelpers::ResizeParameters::pinned_edge_top ) )
      {
         //top is pinned
         position_rect.bottom = position_rect.top + subview_height ;
      }
      else
      {
         //bottom nor top is pinned
         position_rect.bottom = subview_rect.bottom ;
      }

   }
   if ( parameters.IsCentered( GuiHelpers::ResizeParameters::center_horizontal ) )
   {
      if(  !parameters.IsPinned( GuiHelpers::ResizeParameters::pinned_edge_left ) && 
           !parameters.IsPinned( GuiHelpers::ResizeParameters::pinned_edge_right ) ) 
      {
         UINT equal_margins = ( width - subview_width ) / 2 ;
         position_rect.left = equal_margins ;
         position_rect.right = position_rect.left + subview_width ;
      }
      else
      {
         _RPTF0( _CRT_ASSERT, "Can't pin left or right if you want to center when resizing" ) ;  
      }
   }
   if ( parameters.IsCentered( GuiHelpers::ResizeParameters::center_vertical ) )
   {
      if(  !parameters.IsPinned( GuiHelpers::ResizeParameters::pinned_edge_top ) && 
           !parameters.IsPinned( GuiHelpers::ResizeParameters::pinned_edge_bottom ) ) 
      {
         UINT equal_margins = ( height - subview_height ) / 2 ;
         position_rect.top = equal_margins ;
         position_rect.bottom = position_rect.top + subview_height ;
      }
      else
      {
         _RPTF0( _CRT_ASSERT, "Can't pin top or bottom if you want to center when resizing" ) ;  
      }

   }
   LONG subview_new_width = position_rect.right - position_rect.left ;
   LONG subview_new_height = position_rect.bottom - position_rect.top ;

   //make sure no one is outside of the client area
   if ( (UINT)position_rect.bottom > height ) 
   {
      position_rect.bottom = height ;
   }
   if ( (UINT)position_rect.right > width )
   {
      position_rect.right = width ;
   }
   if ( position_rect.top < 0 )
   {
      position_rect.top = 0 ;
   }
   if ( position_rect.left < 0 )
   {
      position_rect.left = 0 ;
   }
   ::SetWindowPos( window_handle, 
                   ::GetParent( window_handle ), 
                   position_rect.left, 
                   position_rect.top, 
                   subview_new_width, 
                   subview_new_height, 
                   SWP_NOZORDER ) ;
   
   return ERROR_SUCCESS ;
}
///////////////////////////////////////////////////////

//////////////////////////////////////////////////////
HWND GuiHelpers::CreateChildControl( const std::wstring& class_name, DWORD control_id, HWND parent_handle, DWORD style, DWORD style_ex )
{
	return ::CreateWindowExW( style_ex, 
							   class_name.c_str(), 
							   NULL, 
							   style, 
							   CW_USEDEFAULT, 
							   CW_USEDEFAULT, 
							   CW_USEDEFAULT, 
							   CW_USEDEFAULT, 
							   parent_handle, 
							   (HMENU)control_id, 
							   ::GetModuleHandle( NULL ), 
							   NULL 
							   ) ;
}
///////////////////////////////////////////////

//////////////////////////////////////////////
GuiHelpers::RichEditSupport::RichEditSupport()
{
  richedit_module_handle = ::LoadLibraryW(L"RICHED20.DLL") ;
  if (NULL == richedit_module_handle )
  {
    DWORD error =  ::GetLastError() ;
    _RPT0(_CRT_ERROR, "Can't create rich edit control for register view. riched20.dll not found");
    throw(error);
    return;
  }
}
GuiHelpers::RichEditSupport::~RichEditSupport()
{
   if (NULL != richedit_module_handle )
   {
      ::FreeLibrary( richedit_module_handle ) ;
   }
}
DWORD CALLBACK GuiHelpers::RichEditSupport::RichTextLoadCallback( DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb )
{
    IStream* stream_ptr = (IStream*) dwCookie ;
    HRESULT hr = stream_ptr->Read( pbBuff, cb, (ULONG*)pcb ) ;
    if ( SUCCEEDED(hr) )
    {
        return 0 ;
    }
    return hr ;
}
DWORD GuiHelpers::RichEditSupport::LoadRichTextFromResource( HWND control_handle, const std::wstring& resource_name, const std::wstring& resource_type )
{
  IStream* stream_ptr = NULL ;
  HRESULT hr = ResourceHelpers::StreamFromResource( resource_name, resource_type, &stream_ptr ) ;
  if ( SUCCEEDED(hr) )
  {
    EDITSTREAM edit_stream_info ;
    edit_stream_info.pfnCallback = GuiHelpers::RichEditSupport::RichTextLoadCallback ;
    edit_stream_info.dwCookie = (DWORD_PTR) stream_ptr ;
    edit_stream_info.dwError = 0 ;
    ::SendMessage( control_handle, EM_STREAMIN, SF_RTF, (LPARAM)&edit_stream_info ) ;
    stream_ptr->Release() ;
    return ERROR_SUCCESS ;
  }
  return hr ;
}
DWORD GuiHelpers::RichEditSupport::ReplaceVariable( HWND rich_edit_handle, const std::wstring& variable, const std::wstring& replacement_text ) 
{
  DWORD return_value = ERROR_NOT_FOUND ;
  std::string replacement_text_narrow = utf8_encode( replacement_text ) ;
  FINDTEXTEX find_text_struct ;
  find_text_struct.chrg.cpMin = 0 ;
  find_text_struct.chrg.cpMax = -1 ;
  find_text_struct.lpstrText = variable.c_str() ;
  find_text_struct.chrgText.cpMin = 0 ;
  find_text_struct.chrgText.cpMax = -1 ;
  ::SendMessage( rich_edit_handle, EM_FINDTEXTEXW, FR_DOWN | FR_MATCHCASE , (LPARAM)&find_text_struct ) ;
  while ( find_text_struct.chrgText.cpMax != -1 )
  {
    return_value = ERROR_SUCCESS ;
    SETTEXTEX set_text_struct ;
    set_text_struct.flags = ST_SELECTION;
    set_text_struct.codepage = CP_UTF8 ;//WTF?

    ::SendMessage( rich_edit_handle, EM_SETSEL, find_text_struct.chrgText.cpMin, find_text_struct.chrgText.cpMax ) ;
    ::SendMessage( rich_edit_handle, EM_SETTEXTEX, (WPARAM)&set_text_struct, (LPARAM)replacement_text_narrow.c_str() ) ;
    find_text_struct.chrg.cpMin = find_text_struct.chrgText.cpMax ;
    find_text_struct.chrg.cpMax = -1 ;
    ::SendMessage( rich_edit_handle, EM_FINDTEXTEXW, FR_DOWN | FR_MATCHCASE , (LPARAM)&find_text_struct ) ;
  }
  return return_value ;
}
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
HWND GuiHelpers::SubViewImpl::Handle()
{ 
   if ( ! ::IsWindow( window_handle ) )
   {
      _RPTF0( _CRT_ASSERT, "Asking for bogus window handle. Are you asking for it before you set it?" ) ;
   }
   return window_handle;
}
HWND GuiHelpers::SubViewImpl::ParentHandle()
{
    return parent_handle ;
}
DWORD GuiHelpers::SubViewImpl::Show() 
{
   ::ShowWindow( this->Handle(), SW_SHOW ) ;
   ::SetFocus( Handle() ) ;
   return ERROR_SUCCESS ;
}
DWORD GuiHelpers::SubViewImpl::Hide() 
{
   ::ShowWindow( this->Handle(), SW_HIDE ) ;
   return ERROR_SUCCESS ;
}
void GuiHelpers::SubViewImpl::AddResizer( DWORD control_id, GuiHelpers::ResizeParameters parameters )
{
    resize_parameters_array.push_back( std::make_pair( control_id, parameters ) ) ;
}
LRESULT GuiHelpers::SubViewImpl::DialogProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch (msg)
  {
    HANDLE_MSG(hWnd, WM_INITDIALOG, PreInitDialog);
    HANDLE_MSG(hWnd, WM_COMMAND, OnCommand);
    HANDLE_MSG( hWnd, WM_SIZE, OnSize ) ;
    HANDLE_MSG( hWnd, WM_NOTIFY, OnCommonControlNotify ) ;
  }
  return OnUnhandledMessage( hWnd, msg, wParam, lParam );
}
LRESULT GuiHelpers::SubViewImpl::OnUnhandledMessage( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam ) 
{ 
  UNREFERENCED_PARAMETER(hWnd) ;
  UNREFERENCED_PARAMETER(msg) ;
  UNREFERENCED_PARAMETER(wParam) ;
  UNREFERENCED_PARAMETER(lParam) ;
  return 0; 
}
BOOL GuiHelpers::SubViewImpl::PreInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
    UNREFERENCED_PARAMETER( hwndFocus ) ;
    UNREFERENCED_PARAMETER( lParam ) ;
    SetHandles( ::GetParent( hwnd ), hwnd ) ;
    return OnInitDialog( ) ;
}
void GuiHelpers::SubViewImpl::SetHandles( HWND parent, HWND self )
{
    parent_handle = parent ;
    window_handle = self ;
}
LRESULT GuiHelpers::SubViewImpl::OnCommand(HWND hWnd, int nIdCtl, HWND hWndCtl, UINT nNotifyCode)
{
   LRESULT nRet = FALSE;
   switch(nNotifyCode)
   {
      case 0:
         if (NULL == hWndCtl)
         {
          nRet = MenuHandler(hWnd, nIdCtl);
         }
         else
         {
           nRet = OnControlCommandNotify( hWndCtl, nIdCtl, nNotifyCode );
         }
         break;
      case 1:
         nRet = TRUE;//AccelHandler(hWnd, nIdCtl) ;
         break;
      default:
         nRet = OnControlCommandNotify( hWndCtl, nIdCtl, nNotifyCode );
         break;
   }
   return nRet;
}

LRESULT GuiHelpers::SubViewImpl::MenuHandler(HWND hWnd, int nCommandId)
{
  UNREFERENCED_PARAMETER( hWnd ) ;
  UNREFERENCED_PARAMETER( nCommandId ) ;
  return FALSE;
}

LRESULT GuiHelpers::SubViewImpl::OnControlCommandNotify( HWND hWndCtl, int nIdCtl, UINT nNotifyCode)
{
   UNREFERENCED_PARAMETER( hWndCtl ) ;
   UNREFERENCED_PARAMETER( nNotifyCode ) ;
   UNREFERENCED_PARAMETER (nIdCtl) ;

  return FALSE;
}
LRESULT GuiHelpers::SubViewImpl::OnSize(HWND hwnd, UINT state, int cx, int cy)
{
   UNREFERENCED_PARAMETER( hwnd ) ;
   UNREFERENCED_PARAMETER( state ) ;
   for ( UINT i = 0 ; i < resize_parameters_array.size() ; i++ )
   {
      HWND control_handle = ::GetDlgItem( window_handle, resize_parameters_array[i].first ) ;
      GuiHelpers::ResizeChildFromParentDims( control_handle, cx, cy, resize_parameters_array[i].second ) ;
   }
   return TRUE ;
}
LRESULT GuiHelpers::SubViewImpl::OnCommonControlNotify( HWND hWnd, int control_id, NMHDR* notify_header_ptr )
{
   UNREFERENCED_PARAMETER( hWnd ) ;
   UNREFERENCED_PARAMETER( control_id ) ;
   UNREFERENCED_PARAMETER( notify_header_ptr ) ;
   return FALSE ;
}
//////////////////////////////////////////////

/////////////////////////////////////////////
LRESULT GuiHelpers::CustomControl::WindowProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam ) 
{
  switch (msg)
  {
    HANDLE_MSG(hWnd, WM_NCCREATE, PreNonClientCreate);
    HANDLE_MSG(hWnd, WM_CREATE, PreCreate);
    HANDLE_MSG(hWnd, WM_COMMAND, OnCommand);
    HANDLE_MSG( hWnd, WM_SIZE, OnSize ) ;
    HANDLE_MSG( hWnd, WM_NOTIFY, OnCommonControlNotify ) ;
  }
  return MessageHandler( hWnd, msg, wParam, lParam ) ;
}
LRESULT GuiHelpers::CustomControl::PreCreate( HWND hWnd, LPCREATESTRUCT create_info_ptr ) 
{
    SetHandles( create_info_ptr->hwndParent, hWnd ) ;
    return OnCreate( create_info_ptr ) ;    
}
BOOL GuiHelpers::CustomControl::PreNonClientCreate(HWND window_handle, LPCREATESTRUCT create_info_ptr)
{
    SetHandles( create_info_ptr->hwndParent, window_handle ) ;
    return OnNonClientCreate( create_info_ptr );
}
BOOL GuiHelpers::CustomControl::OnNonClientCreate( LPCREATESTRUCT lpCreateStruct )
{
  return TRUE ;
}
//never called. just to satisfy pure virtual of base
BOOL GuiHelpers::CustomControl::OnInitDialog() 
{ 
    return FALSE; 
}
//////////////////////////////////////////////

/////////////////////////////////////////////
void GuiHelpers::MessageLoop::RunMainApplicationLoop()
{
   MSG msg;

   while ( ::GetMessage( &msg, NULL, 0, 0) )
   {
      bool accel_handled = false ;
      for ( UINT i = 0 ; i < accelerator_array.size() ; i++ )
      {
        for ( UINT win = 0 ; win < dialog_array.size() ; win++ )
        {
          accel_handled |= ::TranslateAcceleratorW( dialog_array.at(win), accelerator_array[i], &msg) ? true : false ;
        }
      }

      bool dialog_handled = false ;
      for ( UINT i = 0 ; i < dialog_array.size() ; i++ )
      {
         dialog_handled = ::IsDialogMessageW( dialog_array[i], &msg ) ? true : false  ;
      }

      if ( !accel_handled && !dialog_handled )
      {
         ::TranslateMessage(&msg);
         ::DispatchMessageW(&msg);
      }
   }
}

void GuiHelpers::MessageLoop::RunLocalLoopUntilEmpty()
{
   MSG msg ;

   while ( ::PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE ) )
   {
      bool accel_handled = false ;
      for ( UINT i = 0 ; i < accelerator_array.size() ; i++ )
      {
        for ( UINT win = 0 ; win < dialog_array.size() ; win++ )
        {
          accel_handled |= ::TranslateAcceleratorW( dialog_array.at(win), accelerator_array[i], &msg) ? true : false ;
        }
      }

      bool dialog_handled = false ;
      for ( UINT i = 0 ; i < dialog_array.size() ; i++ )
      {
         dialog_handled = ::IsDialogMessageW( dialog_array[i], &msg ) ? true : false  ;
      }

      if ( !accel_handled && !dialog_handled )
      {
         ::TranslateMessage(&msg);
         ::DispatchMessageW(&msg);
      }
   }
   if (HasGameIdleFunction())
   {
       m_idle_function(msg.message == WM_QUIT);
   }
}

void GuiHelpers::MessageLoop::RunGameApplicationWithIdleCallback()
{
    if (!m_idle_function)
    {
        _RPTF0(_CRT_ERROR, L"no function stored in the idle callback in the message loop");
        return;
    }

    bool bQuit = false;
    quit_request = false;
    while (!bQuit && !quit_request)
    {
        MSG msg;

        if (::PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
            {
                bQuit = true;
            }
            bool accel_handled = false;
            for (UINT i = 0; i < accelerator_array.size(); i++)
            {
                for (UINT win = 0; win < dialog_array.size(); win++)
                {
                    accel_handled |= ::TranslateAcceleratorW(dialog_array.at(win), accelerator_array[i], &msg) ? true : false;
                }
            }

            bool dialog_handled = false;
            for (UINT i = 0; i < dialog_array.size(); i++)
            {
                dialog_handled = ::IsDialogMessageW(dialog_array[i], &msg) ? true : false;
            }

            if (!accel_handled && !dialog_handled)
            {
                ::TranslateMessage(&msg);
                ::DispatchMessageW(&msg);
            }
        }
        if (m_idle_function)
        {
            m_idle_function(bQuit || quit_request);
        }
    }
}

void GuiHelpers::MessageLoop::SetGameIdleFunction(std::function<void(bool)> idle_function)
{
    m_idle_function = idle_function;
}

bool GuiHelpers::MessageLoop::HasGameIdleFunction()
{
    return (bool)m_idle_function;
}

void GuiHelpers::MessageLoop::Quit()
{
    quit_request = true;
    PostQuitMessage(0);
}

void GuiHelpers::MessageLoop::AddAcceleratorTable( HACCEL accelerator_handle )
{
   std::vector<HACCEL>::const_iterator finder ;
   finder = std::find( accelerator_array.begin(), accelerator_array.end(), accelerator_handle ) ;
   if ( finder == accelerator_array.end() || ! accelerator_array.size() ) 
   {
      accelerator_array.push_back( accelerator_handle ) ;
   }      
}

void GuiHelpers::MessageLoop::AddDialogWindow( HWND dialog_handle ) 
{
   std::vector<HWND>::const_iterator finder ;
   finder = std::find( dialog_array.begin(), dialog_array.end(), dialog_handle ) ;
   if ( finder == dialog_array.end() || ! dialog_array.size() ) 
   {
      dialog_array.push_back( dialog_handle ) ;
   }      
}

void GuiHelpers::MessageLoop::RemoveDialogWindow( HWND dialog_handle )
{
   std::vector<HWND>::const_iterator new_end = std::remove( dialog_array.begin(), dialog_array.end(), dialog_handle ) ;
   if ( new_end != dialog_array.end() )
   {
      dialog_array.erase( new_end, dialog_array.end() ) ; 
   }
}
////////////////////////////////////////////////////

////////////////////////////////////////////////////

GuiHelpers::DefaultDialogFont::DefaultDialogFont()
{
   NONCLIENTMETRICSW info_structure ;
   info_structure.cbSize = sizeof ( NONCLIENTMETRICSW ) ;
   ::SystemParametersInfoW( SPI_GETNONCLIENTMETRICS, info_structure.cbSize, &info_structure, 0 ) ;
   font_handle = ::CreateFontIndirectW( &info_structure.lfMessageFont ) ;
   ::memcpy( &logical_font_info, &info_structure.lfMessageFont, sizeof (LOGFONTW) ) ;
}
GuiHelpers::DefaultDialogFont::~DefaultDialogFont() 
{
    ::DeleteObject( font_handle ) ;
}
HFONT GuiHelpers::DefaultDialogFont::Handle() 
{
    return font_handle ;
}
void GuiHelpers::DefaultDialogFont::CopyLogfont( LOGFONTW* logfont_ptr ) 
{
    ::memcpy( logfont_ptr, &logical_font_info, sizeof (LOGFONTW) ) ;
}
//////////////////////////////////////////////////////

//////////////////////////////////////////////////////
void GuiHelpers::CenterRectToMonitor(RECT* prc) //, UINT flags
{
    HMONITOR hMonitor;
    MONITORINFO mi;
    RECT        rc;
    int         w = prc->right  - prc->left;
    int         h = prc->bottom - prc->top;

    // 
    // get the nearest monitor to the passed rect. 
    // 
    hMonitor = MonitorFromRect(prc, MONITOR_DEFAULTTONEAREST);

    // 
    // get the work area or entire monitor rect. 
    // 
    mi.cbSize = sizeof(mi);
    GetMonitorInfo(hMonitor, &mi);

    //if (flags & MONITOR_WORKAREA)
        rc = mi.rcWork;
    //else
        //rc = mi.rcMonitor;

    // 
    // center or clip the passed rect to the monitor rect 
    // 
    //if (flags & MONITOR_CENTER)
    //{
        prc->left   = rc.left + (rc.right  - rc.left - w) / 2;
        prc->top    = rc.top  + (rc.bottom - rc.top  - h) / 2;
        prc->right  = prc->left + w;
        prc->bottom = prc->top  + h;
    //}
    //else
    //{
        //prc->left   = max(rc.left, min(rc.right-w,  prc->left));
        //prc->top    = max(rc.top,  min(rc.bottom-h, prc->top));
        //prc->right  = prc->left + w;
        //prc->bottom = prc->top  + h;
    //}
}
