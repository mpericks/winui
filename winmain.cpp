// Defines the entry point for the application.
//

#include <windows.h>
#include <objbase.h>
#include <combaseapi.h>
#include <heapapi.h>
#include <sal.h>
#include <intsafe.h>

#include "GuiHelpers.h"
#include "ApplicationTypes.h"

int APIENTRY wWinMain(
                      _In_ HINSTANCE hInstance,
                      _In_opt_ HINSTANCE hPrevInstance,
                      _In_ LPWSTR lpCmdLine,
                      _In_ int nShowCmd
                     )
{
    
   UNREFERENCED_PARAMETER( lpCmdLine ) ;
   UNREFERENCED_PARAMETER( hPrevInstance ) ;
   UNREFERENCED_PARAMETER( nShowCmd ) ;

   int nRet = 0;
   if (FAILED(::CoInitialize(NULL)))
   {
       return -1;
   }

   ::HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);
   GuiHelpers::MessageLoop message_loop ;
   ApplicationWindow main_window( &message_loop );

   if( main_window.Handle() )
   {
      message_loop.AddDialogWindow( main_window.Handle() ) ;
      if (message_loop.HasGameIdleFunction())
      {
          message_loop.RunGameApplicationWithIdleCallback();
      }
      else
      {
          message_loop.RunMainApplicationLoop();
      }
   }
   ::CoUninitialize() ;
   return nRet;
}

