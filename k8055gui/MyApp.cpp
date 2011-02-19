//#include "MyApp.h"

#include <wx/wx.h>
#include "MyFrame.h"
class MyApp: public wxApp
{
    virtual bool OnInit();
};

DECLARE_APP(MyApp)


IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
    MyFrame *frame = new MyFrame( _T("K8055 USB Experiment Interface Board"), wxPoint(50,50), wxSize(635,340) );
    frame->Show(TRUE);
    SetTopWindow(frame);
    return TRUE;
} 
