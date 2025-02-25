//thunker.h

#pragma once

template< typename WM_Impl >
class WM_Thunker
{
public:
   static LRESULT CALLBACK WM_Thunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
   {
      LRESULT nRet = FALSE;
      LPCREATESTRUCT lpCreate = 0;
      WM_Impl* pImpl = 0;
      switch (msg)
      {
         case WM_NCCREATE:
            lpCreate = (LPCREATESTRUCT) lParam;
            pImpl = (WM_Impl*)lpCreate->lpCreateParams;
            if(pImpl)
            {
               if(::SetProp(hWnd, TEXT("MPE_THIS"), (HANDLE)pImpl))
               {
                  nRet = pImpl->WindowProc(hWnd, msg, wParam, lParam);
               }
            }
         break;
         default:
            pImpl = (WM_Impl*)::GetProp(hWnd, TEXT("MPE_THIS"));
            if(pImpl)
            {
               nRet = pImpl->WindowProc(hWnd, msg, wParam, lParam);
            }
         break;
      }

      return nRet;
   };
};

template< typename DLG_Impl >
class DLG_Thunker
{
public:
   static INT_PTR CALLBACK DLG_Thunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
   {
      INT_PTR nRet = FALSE;
      DLG_Impl* pImpl = 0;
      switch (msg)
      {
         case WM_INITDIALOG:
            pImpl = (DLG_Impl*)lParam;
            if(pImpl)
            {
               if(::SetProp(hWnd, TEXT("MPE_THIS"), (HANDLE)pImpl))
               {
                  nRet = pImpl->DialogProc(hWnd, msg, wParam, lParam);
               }
            }
         break;
         default:
            pImpl = (DLG_Impl*)::GetProp(hWnd, TEXT("MPE_THIS"));
            if(pImpl)
            {
               nRet = pImpl->DialogProc(hWnd, msg, wParam, lParam);
            }
         break;
      }

      return nRet;
   };
};
