#pragma once

#include <vector>
#include <string>
#include <CommCtrl.h>
#include "thunker.h"

namespace GuiHelpers
{
   //"pinning" means to maintain the distance from a control's edge to the edge of the parent window
   class ResizeParameters
   {
   public:
      ResizeParameters() ;
      ~ResizeParameters() ;
   public:
      void PinTopLeft(UINT top_margin, UINT left_margin) ;
      void PinTopRight(UINT top_margin, UINT right_margin) ;
      void PinBottomRight(UINT bottom_margin, UINT right_margin ) ;
      void PinAll(UINT top_margin, UINT left_margin, UINT right_margin, UINT bottom_margin) ;
      void PinBottomAndCenterHorizontal( UINT bottom_margin ) ;
      void PinTopAndCenterHorizontal( UINT top_margin ) ;
      void PinTopAndSizeHorizontal( UINT top_margin, UINT left_margin, UINT right_margin ) ;
      bool IsPinned( UINT edge_id ) const ;
      bool IsCentered( UINT dimension_id ) const ;
      UINT RightBorder() const ;
      UINT TopBorder() const ;
      UINT BottomBorder() const ;
      UINT LeftBorder() const ;
   private:
      UINT left_border ;
      UINT right_border ;
      UINT top_border ;
      UINT bottom_border ;

      bool pinned_edges[4] ;
      bool centered_dimensions[2] ;
   public:
      static const UINT pinned_edge_top = 0 ;
      static const UINT pinned_edge_left = 1 ;
      static const UINT pinned_edge_right = 2 ;
      static const UINT pinned_edge_bottom = 3 ;

      static const UINT center_vertical = 0 ;
      static const UINT center_horizontal = 1 ;
   };
   class RichEditSupport
   {
   public:
     RichEditSupport() ;
     ~RichEditSupport() ;
     DWORD LoadRichTextFromResource( HWND control_handle, const std::wstring& resource_name, const std::wstring& resource_type ) ; 
     DWORD ReplaceVariable( HWND control_handle, const std::wstring& variable, const std::wstring& replacement_text ) ;
     DWORD DefaultWindowStyle() {return ES_MULTILINE | ES_READONLY | WS_CHILD | WS_VISIBLE ;} ;
   private:
     static DWORD CALLBACK RichTextLoadCallback( DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb ) ;
   private:
     HMODULE richedit_module_handle ;
   };
   class ISubView
   {
   public:
      virtual HWND Handle() = 0 ;
      virtual DWORD Show() = 0 ;
      virtual DWORD Hide() = 0 ;
   };
   class SubViewImpl : public ISubView
   {
   public:
      //overrides for ISubView
      virtual HWND Handle();
      virtual DWORD Show() ;
      virtual DWORD Hide() ;
      virtual LRESULT DialogProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) ;

   protected:
      HWND ParentHandle() ;
      void AddResizer( DWORD control_id, GuiHelpers::ResizeParameters parameters ) ;
   protected:
      virtual BOOL OnInitDialog() = 0;
      virtual LRESULT OnCommand(HWND hWnd, int nIdCtl, HWND hWndCtl, UINT nNotifyCode);
      virtual LRESULT MenuHandler(HWND hWnd, int nCommandId);
      virtual LRESULT OnControlCommandNotify( HWND control_handle, int control_id, UINT notify_code);
      virtual LRESULT OnSize(HWND hwnd, UINT state, int cx, int cy) ;
      virtual LRESULT OnCommonControlNotify( HWND control_handle, int control_id, NMHDR* notify_header_ptr ) ;
      virtual LRESULT OnUnhandledMessage( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )  ;
      void SetHandles( HWND parent, HWND self ) ;
   private:
      BOOL PreInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam) ;
   private:
      std::vector<std::pair< DWORD, GuiHelpers::ResizeParameters> > resize_parameters_array ;
      HWND window_handle;
      HWND parent_handle;
   };
   class CustomControl : public SubViewImpl
   {
   public:
       virtual LRESULT WindowProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam ) ;
       virtual LRESULT MessageHandler( HWND window_handle, UINT msg, WPARAM wParam, LPARAM lParam ) = 0;
   protected:
       virtual BOOL OnInitDialog() ;
        virtual BOOL OnNonClientCreate( LPCREATESTRUCT create_info_ptr ) ;
       virtual LRESULT OnCreate( LPCREATESTRUCT create_info_ptr ) = 0 ;
   private:
       LRESULT PreCreate( HWND hWnd, LPCREATESTRUCT create_info_ptr ) ;
       BOOL PreNonClientCreate(HWND window_handle, LPCREATESTRUCT lpCreateStruct) ;
   } ;
   typedef GuiHelpers::CustomControl MainWindowBaseImpl ;
   class MessageLoop
   {
   public:
      MessageLoop() {};
      ~MessageLoop() {};
   public:
      void RunMainApplicationLoop() ;
      void RunLocalLoopUntilEmpty() ;
      void AddAcceleratorTable( HACCEL accelerator_handle ) ;
      void AddDialogWindow( HWND dialog_handle ) ;
      void RemoveDialogWindow( HWND dialog_handle ) ;
   private:
      std::vector<HACCEL> accelerator_array ;
      std::vector<HWND> dialog_array ;
   };
   class DefaultDialogFont
   {
   public:
       DefaultDialogFont() ;
       ~DefaultDialogFont() ;
       HFONT Handle() ;
       void CopyLogfont( LOGFONT* logfont_ptr ) ;
   private:
       HFONT font_handle ;
       LOGFONTW logical_font_info ;
   };
   DWORD CALLBACK RegisterTextCallback( DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb ) ;
   DWORD ResizeChildFromParent( HWND child_handle, HWND parent_handle, const GuiHelpers::ResizeParameters& parameters ) ;
   DWORD ResizeChildFromParentDims( HWND window_handle, UINT width, UINT height, const GuiHelpers::ResizeParameters& parameters ) ;
   template <class message_handler> ATOM RegisterWindow( message_handler* pThis, const std::wstring& class_name ) 
   {
       UNREFERENCED_PARAMETER( pThis ) ;
       HINSTANCE instance_handle = ::GetModuleHandleW( NULL ) ;

      WNDCLASSEX wcex;

      wcex.cbSize = sizeof(WNDCLASSEX);

      wcex.style           = CS_HREDRAW | CS_VREDRAW;
      wcex.lpfnWndProc     = (WNDPROC)WM_Thunker<message_handler>::WM_Thunk;
      wcex.cbClsExtra      = 0;
      wcex.cbWndExtra      = 0;
      wcex.hInstance       = instance_handle ;
      wcex.hIcon           = NULL;
      wcex.hCursor         = ::LoadCursor(NULL, IDC_ARROW);
      wcex.hbrBackground   = (HBRUSH)(COLOR_WINDOW+1);
      wcex.lpszMenuName    = NULL;
      wcex.lpszClassName   = class_name.c_str() ;
      wcex.hIconSm         = NULL;

      return  RegisterClassEx(&wcex);
   }
   template <class view_class> DWORD CreateDefaultMainWindow( view_class* pThis, const std::wstring& class_name ) 
   {
      CoInitialize(NULL);
      INITCOMMONCONTROLSEX ccex = {sizeof(INITCOMMONCONTROLSEX), ICC_WIN95_CLASSES | ICC_STANDARD_CLASSES };
      InitCommonControlsEx(&ccex);

      HINSTANCE instance_handle = ::GetModuleHandleW( NULL ) ;


      ATOM atom = RegisterWindow( pThis, class_name ) ;

      if(atom)
      {
	     DWORD dwStyleEx = WS_EX_CONTROLPARENT | WS_EX_WINDOWEDGE | WS_EX_APPWINDOW;
         HWND window_handle = ::CreateWindowEx(
                                                dwStyleEx,
                                                (LPCTSTR)(INT_PTR)atom,
                                                NULL,
                                                WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_BORDER,
                                                CW_USEDEFAULT,
                                                CW_USEDEFAULT,
                                                CW_USEDEFAULT,
                                                CW_USEDEFAULT,
                                                NULL,
                                                NULL,
                                                instance_handle,
                                                (LPVOID) pThis
                                                );
         if ( NULL == window_handle )
         {
            return ::GetLastError() ;
         }
      }
      else
      {
         DWORD error = ::GetLastError() ;
         _RPTF1(_CRT_ERROR, "RegisterClass Failed %d", error );
         return error ;
      } 
      return ERROR_SUCCESS ;
   };
   template<class sub_view_class> DWORD CreateSubView( DWORD dialog_id, HWND parent_handle, sub_view_class* pThis )
   {
      HWND window_handle = ::CreateDialogParamW( ::GetModuleHandleW( NULL ), 
                                                 MAKEINTRESOURCE(dialog_id),
                                                 parent_handle, 
                                                 (DLGPROC)DLG_Thunker<sub_view_class>::DLG_Thunk, 
                                                 (LPARAM)pThis);
      if (NULL == window_handle)
      {
         DWORD error =  ::GetLastError() ;
         _RPTF1(_CRT_ERROR, "Can't create view. CreateDialogParam(...) failed with error %d", error );
         return error;
      }
      return ERROR_SUCCESS ;
   };

   template <class message_handler> HWND CreateCustomControl( message_handler* pThis, const std::wstring& class_name, DWORD control_id, HWND parent_handle ) 
   {
       HINSTANCE instance_handle = ::GetModuleHandleW( NULL ) ;

       ATOM atom = RegisterWindow( pThis, class_name ) ;
       UNREFERENCED_PARAMETER( atom ) ;
       HWND window_handle = NULL ;
       DWORD error = ERROR_SUCCESS ;
       DWORD dwStyleEx = 0;
       window_handle = ::CreateWindowEx(
                                         dwStyleEx,
                                         (LPCTSTR)(INT_PTR)class_name.c_str(),
                                         NULL,
                                         WS_CHILD | WS_VISIBLE,
                                         CW_USEDEFAULT,
                                         CW_USEDEFAULT,
                                         CW_USEDEFAULT,
                                         CW_USEDEFAULT,
                                         parent_handle,
                                         (HMENU) control_id,
                                         instance_handle,
                                         (LPVOID) pThis
                                       );
       if ( NULL == window_handle )
       {
         error = ::GetLastError() ;
         _RPTF1(_CRT_ERROR, "CreateWindow Failed %d", error );
       }
       return window_handle ;    
   }
   HWND CreateChildControl( const std::wstring& class_name, DWORD control_id, HWND parent_handle, DWORD style, DWORD style_ex ) ;
   void CenterRectToMonitor(RECT* prc) ;
};