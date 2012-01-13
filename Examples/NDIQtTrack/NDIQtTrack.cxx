#if defined(_MSC_VER)
//  Warning about: identifier was truncated to '255' characters 
//  in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4284 )
#endif

#include <QApplication>

#include "NDIQtTrackGUI.h"

int main(int argc, char *argv[])
{
  // initialize the application.
  QApplication app(argc, argv);
  app.setOrganizationName("AIGS");
  app.setApplicationName("NDI Qt Track");

   // initialize the GUI.  Note the viewer inherits the GUI.
  NDIQtTrackGUI* mainWindow = NDIQtTrackGUI::New();

  mainWindow->show();

  return app.exec();
}