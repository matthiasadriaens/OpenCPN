/***************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  OpenCPN Main frame
 * Author:   David Register
 *
 ***************************************************************************
 *   Copyright (C) 2010 by David S. Register                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,  USA.         *
 **************************************************************************/
#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
#include "wx/wx.h"
#endif //precompiled headers
#ifdef __WXMSW__
//#include "c:\\Program Files\\visual leak detector\\include\\vld.h"
#endif


// Include CrashRpt Header
#ifdef OCPN_USE_CRASHRPT
#include "CrashRpt.h"
#include <new.h>
#endif

#include "wx/print.h"
#include "wx/printdlg.h"
#include "wx/artprov.h"
#include <wx/intl.h>
#include <wx/listctrl.h>
#include <wx/aui/aui.h>
#include <version.h> //Gunther
#include <wx/dialog.h>
#include <wx/progdlg.h>
#include <wx/clrpicker.h>

#include <wx/dialog.h>

#include "dychart.h"

#ifdef __WXMSW__
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <psapi.h>
#endif

#include "chart1.h"
#include "chcanv.h"
#include "chartdb.h"
#include "navutil.h"
#include "styles.h"
#include "routeman.h"
#include "statwin.h"
#include "concanv.h"
#include "options.h"
#include "about.h"
#include "thumbwin.h"
#include "tcmgr.h"
#include "ais.h"
#include "chartimg.h"               // for ChartBaseBSB
#include "routeprop.h"
#include "toolbar.h"
#include "compasswin.h"
#include "datastream.h"
#include "OCPN_DataStreamEvent.h"
#include "multiplexer.h"
#include "routeprintout.h"
#include "Select.h"
#include "FontMgr.h"
#include "NMEALogWindow.h"
#include "Layer.h"
#include "NavObjectCollection.h"
#include "AISTargetListDialog.h"
#include "AISTargetAlertDialog.h"
#include "AIS_Decoder.h"
#include "OCP_DataStreamInput_Thread.h"
#include "TrackPropDlg.h"
#include "gshhs.h"
#include "cutil.h"
#include "routemanagerdialog.h"
#include "pluginmanager.h"
#include "AIS_Target_Data.h"
#include "OCPN_Base.h"
#include "OCPN_Printout.h"
#include "OCPN_Dialogs.h"
#include "OCPN_Functions.h"

#ifdef ocpnUSE_GL
#include "glChartCanvas.h"
#endif

#include <wx/image.h>

#ifdef __WXOSX__
#include "macutils.h"
#endif

#ifdef USE_S57
#include "cm93.h"
#include "s52plib.h"
#include "s57chart.h"
#include "cpl_csv.h"
#endif

#ifdef __WXMSW__
//#define __MSVC__LEAK
#ifdef __MSVC__LEAK
#include "Stackwalker.h"
#endif
#endif

#include <wx/jsonreader.h>

#ifdef OCPN_USE_PORTAUDIO
#include "portaudio.h"
#endif

#ifdef __OCPN__ANDROID__
#include "androidUTIL.h"
#endif

WX_DECLARE_OBJARRAY(wxDialog *, MyDialogPtrArray);

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY( ArrayOfCDI );
WX_DEFINE_OBJARRAY( ArrayOfRect );
WX_DEFINE_OBJARRAY( MyDialogPtrArray );

#ifdef __WXMSW__
void RedirectIOToConsole();
extern SetSysColors_t            pSetSysColors;
extern GetSysColor_t             pGetSysColor;
#endif

//------------------------------------------------------------------------------
//      Static variable definition
//------------------------------------------------------------------------------

extern FILE               *flog;
extern wxLog              *logger;
extern wxLog              *Oldlogger;
extern bool               g_bFirstRun;
extern wxString           glog_file;
extern wxString           gConfig_File;
extern wxString           gExe_path;

extern bool               g_unit_test_1;
extern bool               g_start_fullscreen;
extern bool               g_rebuild_gl_cache;

extern MyFrame            *gFrame;

extern ChartCanvas        *cc1;
ConsoleCanvas             *console;
extern StatWin            *stats;
wxWindowList              AppActivateList;

extern MyConfig           *pConfig;

ChartBase                 *Current_Vector_Ch;
ChartBase                 *Current_Ch;
extern ChartDB            *ChartData;
extern ChartStack         *pCurrentStack;
wxString                  *pdir_list[20];
int                       g_restore_stackindex;
extern int                g_restore_dbindex;
double                    g_ChartNotRenderScaleFactor;

RouteList                 *pRouteList;
LayerList                 *pLayerList;
bool                      g_bIsNewLayer;
int                       g_LayerIdx;
bool                      g_bLayerViz;

extern Select             *pSelect;
extern Select             *pSelectTC;
extern Select             *pSelectAIS;

extern Routeman           *g_pRouteMan;
extern WayPointman        *pWayPointMan;
MarkInfoImpl              *pMarkPropDialog;
RouteProp                 *pRoutePropDialog;
TrackPropDlg              *pTrackPropDialog;
RouteManagerDialog        *pRouteManagerDialog;
GoToPositionDialog        *pGoToPositionDialog;

extern double             gLat, gLon, gCog, gSog, gHdt, gHdm, gVar;
extern double             vLat, vLon;
extern double             initial_scale_ppm;
double                    initial_rotation;

int                       g_nbrightness;

extern bool               bDBUpdateInProgress;

extern ThumbWin           *pthumbwin;
TCMgr                     *ptcmgr;


bool                      bDrawCurrentValues;
extern bool               b_novicemode;

extern wxString           g_PrivateDataDir;
extern wxString           g_SData_Locn;
extern wxString           *pChartListFileName;
extern wxString           *pWorldMapLocation;
extern wxString           *pInit_Chart_Dir;
wxString                  g_csv_locn;
wxString                  g_SENCPrefix;
wxString                  g_UserPresLibData;
extern wxString           g_Plugin_Dir;
wxString                  g_VisibleLayers;
wxString                  g_InvisibleLayers;

extern wxString           str_version_major;
extern wxString           str_version_minor;
extern wxString           str_version_patch;

#ifdef ocpnUSE_GL
extern CompressionWorkerPool   *g_CompressorPool;
#endif

bool                      g_bcompression_wait;

wxString                  g_uploadConnection;

int                       user_user_id;
int                       file_user_id;

extern int                quitflag;
int                       g_tick;
extern int                       g_mem_total, g_mem_used, g_mem_initial;

bool                      s_bSetSystemTime;

extern wxString                  *phost_name;

extern unsigned int       malloc_max;

wxArrayOfConnPrm          *g_pConnectionParams;
//OCP_GARMIN_Thread         *pGARMIN_Thread;

extern wxDateTime         g_start_time;
extern wxDateTime         g_loglast_time;
OCPN_Sound                bells_sound[2];

OCPN_Sound                g_anchorwatch_sound;

extern RoutePoint         *pAnchorWatchPoint1;
extern RoutePoint         *pAnchorWatchPoint2;
extern double             AnchorPointMinDist;
bool                      AnchorAlertOn1, AnchorAlertOn2;
bool                      g_bCruising;

ChartDummy                *pDummyChart;

ocpnToolBarSimple*        g_toolbar;
extern ocpnStyle::StyleManager*  g_StyleManager;

// Global print data, to remember settings during the session
wxPrintData               *g_printData = (wxPrintData*) NULL ;

// Global page setup data
wxPageSetupData*          g_pageSetupData = (wxPageSetupData*) NULL;

extern bool               g_bShowOutlines;
bool                      g_bShowDepthUnits;
bool                      g_bDisplayGrid;  // Flag indicating weather the lat/lon grid should be displayed
extern bool               g_bShowChartBar;
bool                      g_bShowActiveRouteHighway;
int                       g_nNMEADebug;
int                       g_nAWDefault;
int                       g_nAWMax;
bool                      g_bPlayShipsBells;
bool                      g_bFullscreenToolbar;
bool                      g_bShowLayers;
bool                      g_bTransparentToolbar;
bool                      g_bPermanentMOBIcon;
bool                      g_bTempShowMenuBar;

int                       g_iSDMMFormat;
int                       g_iDistanceFormat;
int                       g_iSpeedFormat;

int                       g_iNavAidRadarRingsNumberVisible;
float                     g_fNavAidRadarRingsStep;
int                       g_pNavAidRadarRingsStepUnits;
int                       g_iWaypointRangeRingsNumber;
float                     g_fWaypointRangeRingsStep;
int                       g_iWaypointRangeRingsStepUnits;
wxColour                  g_colourWaypointRangeRingsColour;
bool                      g_bWayPointPreventDragging;
bool                      g_bConfirmObjectDelete;

extern ColorScheme        global_color_scheme;
int                       Usercolortable_index;
wxArrayPtrVoid            *UserColorTableArray;
wxArrayPtrVoid            *UserColourHashTableArray;
wxColorHashMap            *pcurrent_user_color_hash;

extern int                gps_watchdog_timeout_ticks;
extern int                sat_watchdog_timeout_ticks;

extern int                gGPS_Watchdog;
extern bool               bGPSValid;

extern int                gHDx_Watchdog;
extern int                gHDT_Watchdog;
extern int                gVAR_Watchdog;
extern bool               g_bHDT_Rx;
extern bool               g_bVAR_Rx;

extern int                gSAT_Watchdog;
int                       g_SatsInView;
bool                      g_bSatValid;

bool                      g_bDebugCM93;
bool                      g_bDebugS57;

bool                      g_bfilter_cogsog;
int                       g_COGFilterSec;
int                       g_SOGFilterSec;

int                       g_ChartUpdatePeriod;
int                       g_SkewCompUpdatePeriod;

extern int                g_lastClientRectx;
extern int                g_lastClientRecty;
extern int                g_lastClientRectw;
extern int                g_lastClientRecth;
extern double             g_display_size_mm;
double                    g_config_display_size_mm;

#ifdef USE_S57
extern s52plib            *ps52plib;
S57ClassRegistrar         *g_poRegistrar;
extern s57RegistrarMgr    *g_pRegistrarMan;

CM93OffsetDialog          *g_pCM93OffsetDialog;
#endif

#ifdef __WXOSX__
#include "macutils.h"
#endif

// begin rms
#if defined( USE_S57) || defined ( __WXOSX__ )
#ifdef __WXMSW__
#ifdef USE_GLU_TESS
#ifdef USE_GLU_DLL
// end rms
extern bool               s_glu_dll_ready;
extern HINSTANCE          s_hGLU_DLL; // Handle to DLL
#endif
#endif
#endif
#endif

double                    g_ownship_predictor_minutes;
double                    g_ownship_HDTpredictor_miles;

int                       g_current_arrow_scale;

extern OCPN_Base          *g_pBASE;

Multiplexer               *g_pMUX;

extern AIS_Decoder        *g_pAIS;

AISTargetAlertDialog      *g_pais_alert_dialog_active;
extern AISTargetQueryDialog      *g_pais_query_dialog_active;

int                       g_S57_dialog_sx, g_S57_dialog_sy;

extern int                g_nframewin_x;
extern int                g_nframewin_y;
extern int                g_nframewin_posx;
extern int                g_nframewin_posy;
extern bool               g_bframemax;

bool                      g_bAutoAnchorMark;

wxRect                    g_blink_rect;
extern double             g_PlanSpeed;
extern wxDateTime         g_StartTime;
extern int                g_StartTimeTZ;
extern IDX_entry          *gpIDX;
extern int                gpIDXn;
long                      gStart_LMT_Offset;

extern wxArrayString             *pMessageOnceArray;

FILE                      *s_fpdebug;
bool                      bAutoOpen;
bool                      bFirstAuto;

bool                      g_bUseRMC;
bool                      g_bUseGLL;

int                       g_nCacheLimit;
extern int                g_memCacheLimit;
bool                      g_bGDAL_Debug;

double                    g_VPRotate; // Viewport rotation angle, used on "Course Up" mode
extern bool               g_bCourseUp;
int                       g_COGAvgSec; // COG average period (sec.) for Course Up Mode
double                    g_COGAvg;
bool                      g_bLookAhead;
extern bool               g_bskew_comp;
extern bool               g_bopengl;
bool                      g_bShowFPS;
bool                      g_bsmoothpanzoom;
bool                      g_fog_overzoom;
double                    g_overzoom_emphasis_base;
bool                      g_oz_vector_scale;

extern int                g_nCOMPortCheck;

bool                      g_b_legacy_input_filter_behaviour;  // Support original input filter process or new process

bool                      g_bbigred;

extern PlugInManager      *g_pi_manager;

bool                      g_bDebugGPSD;

extern bool               g_bFullScreenQuilt;
extern bool               g_bQuiltEnable;
bool                      g_bQuiltStart;

extern bool               g_bportable;

extern bool               g_bdisable_opengl;

extern ChartGroupArray    *g_pGroupArray;
extern int                g_GroupIndex;

wxString                  g_GPS_Ident;

wxProgressDialog          *s_ProgDialog;

S57QueryDialog            *g_pObjectQueryDialog;

extern wxArrayString      TideCurrentDataSet;
wxString                  g_TCData_Dir;

bool                      g_boptionsactive;
options                   *g_options;
int                       options_lastPage = 0;
wxPoint                   options_lastWindowPos( 0,0 );
wxSize                    options_lastWindowSize( 0,0 );

wxToolBarToolBase         *m_pAISTool;

int                       g_nAIS_activity_timer;

bool                      g_bEnableZoomToCursor;

bool                      g_bTrackActive;
extern bool               g_bTrackCarryOver;
extern bool               g_bDeferredStartTrack;
extern bool               g_bTrackDaily;
bool                      g_bHighliteTracks;
int                       g_route_line_width;
int                       g_track_line_width;
wxString                  g_default_wp_icon;

Track                     *g_pActiveTrack;
double                    g_TrackIntervalSeconds;
double                    g_TrackDeltaDistance;
int                       g_nTrackPrecision;

int                       g_total_NMEAerror_messages;

int                       g_cm93_zoom_factor;
CM93DSlide                *pCM93DetailSlider;
bool                      g_bShowCM93DetailSlider;
int                       g_cm93detail_dialog_x, g_cm93detail_dialog_y;

bool                      g_bUseGreenShip;

extern wxString           g_AW1GUID;
extern wxString           g_AW2GUID;

bool                      g_b_overzoom_x; // Allow high overzoom

int                       g_OwnShipIconType;
double                    g_n_ownship_length_meters;
double                    g_n_ownship_beam_meters;
double                    g_n_gps_antenna_offset_y;
double                    g_n_gps_antenna_offset_x;
int                       g_n_ownship_min_mm;

double                    g_n_arrival_circle_radius;

int                       g_nautosave_interval_seconds;

bool                      g_bPreserveScaleOnX;

about                     *g_pAboutDlg;

extern wxLocale                  *plocale_def_lang;
extern wxString                  g_locale;
extern bool                      g_b_assume_azerty;

bool                      g_bUseRaster;
bool                      g_bUseVector;
bool                      g_bUseCM93;

int                       g_click_stop;

int                       g_MemFootSec;
int                       g_MemFootMB;

ArrayOfInts               g_quilt_noshow_index_array;

wxStaticBitmap            *g_pStatBoxTool;

bool                      g_bquiting;
int                       g_BSBImgDebug;

AISTargetListDialog       *g_pAISTargetList;

bool                      g_bGarminHostUpload;

extern wxAuiManager       *g_pauimgr;
wxAuiDefaultDockArt       *g_pauidockart;

bool                      g_blocale_changed;

RoutePrintSelection       *pRoutePrintSelection;

wxMenu                    *g_FloatingToolbarConfigMenu;
wxString                  g_toolbarConfig = _T("XXXXXXXXXXXXXXXXXXXXXXXXXXXXX");
ocpnFloatingToolbarDialog *g_FloatingToolbarDialog;
extern ocpnFloatingCompassWindow *g_FloatingCompassDialog;

extern int                g_toolbar_x;
extern int                g_toolbar_y;
long                      g_toolbar_orient;
wxRect                    g_last_tb_rect;
float                     g_toolbar_scalefactor;

MyDialogPtrArray          g_MacShowDialogArray;
bool                      g_benable_rotate;

bool                      g_bShowMag;
double                    g_UserVar;
bool                      g_bMagneticAPB;


//                        OpenGL Globals
int                       g_GPU_MemSize;

bool                      g_bserial_access_checked;


char bells_sound_file_name[2][12] = { "1bells.wav", "2bells.wav" };

int                       portaudio_initialized; //Belongs to OCPN_Sound

static char nmea_tick_chars[] = { '|', '/', '-', '\\', '|', '/', '-', '\\' };
static int tick_idx;

int               g_sticky_chart;

extern wxString OpenCPNVersion; //Gunther

extern int n_NavMessageShown;
extern wxString g_config_version_string;

extern bool      g_btouch;
extern bool      g_bresponsive;

bool             b_inCompressAllCharts;
bool             g_bexpert;
int              g_chart_zoom_modifier;

int              g_NMEAAPBPrecision;

wxString         g_TalkerIdText;

bool             g_bEmailCrashReport;
bool             g_bAdvanceRouteWaypointOnArrivalOnly;

wxArrayString    g_locale_catalog_array;

#ifdef __MSVC__
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__ )
#define new DEBUG_NEW
#endif

#if !defined(NAN)
static const long long lNaN = 0xfff8000000000000;
#define NAN (*(double*)&lNaN)
#endif

//------------------------------------------------------------------------------
//    PNG Icon resources
//------------------------------------------------------------------------------

#ifdef __WXGTK__
#include "bitmaps/opencpn.xpm"
#endif

//------------------------------------------------------------------------------
//              Local constants
//------------------------------------------------------------------------------
enum {
    ID_PIANO_DISABLE_QUILT_CHART = 32000, ID_PIANO_ENABLE_QUILT_CHART
};

#include <wx/power.h>

//------------------------------------------------------------------------------
// MyFrame
//------------------------------------------------------------------------------

//      Frame implementation
BEGIN_EVENT_TABLE(MyFrame, wxFrame) EVT_CLOSE(MyFrame::OnCloseWindow)
EVT_MENU(wxID_EXIT, MyFrame::OnExit)
EVT_SIZE(MyFrame::OnSize)
EVT_MOVE(MyFrame::OnMove)
EVT_MENU(-1, MyFrame::OnToolLeftClick)
EVT_TIMER(INIT_TIMER, MyFrame::OnInitTimer)
EVT_TIMER(FRAME_TIMER_1, MyFrame::OnFrameTimer1)
EVT_TIMER(FRAME_TC_TIMER, MyFrame::OnFrameTCTimer)
EVT_TIMER(FRAME_COG_TIMER, MyFrame::OnFrameCOGTimer)
EVT_TIMER(MEMORY_FOOTPRINT_TIMER, MyFrame::OnMemFootTimer)
EVT_TIMER(BELLS_TIMER, MyFrame::OnBellsTimer)
EVT_ACTIVATE(MyFrame::OnActivate)
EVT_MAXIMIZE(MyFrame::OnMaximize)
EVT_COMMAND(wxID_ANY, wxEVT_COMMAND_TOOL_RCLICKED, MyFrame::RequestNewToolbarArgEvent)
EVT_ERASE_BACKGROUND(MyFrame::OnEraseBackground)
#ifdef wxHAS_POWER_EVENTS
EVT_POWER_SUSPENDING(MyFrame::OnSuspending)
EVT_POWER_SUSPENDED(MyFrame::OnSuspended)
EVT_POWER_SUSPEND_CANCEL(MyFrame::OnSuspendCancel)
EVT_POWER_RESUME(MyFrame::OnResume)
#endif // wxHAS_POWER_EVENTS
END_EVENT_TABLE()

// My frame constructor
MyFrame::MyFrame( wxFrame *frame, const wxString& title, const wxPoint& pos, const wxSize& size,
        long style ) :
        wxFrame( frame, -1, title, pos, size, style ) //wxSIMPLE_BORDER | wxCLIP_CHILDREN | wxRESIZE_BORDER)
//wxCAPTION | wxSYSTEM_MENU | wxRESIZE_BORDER
{
    m_ulLastNEMATicktime = 0;
    m_pStatusBar = NULL;
    m_pMenuBar = NULL;
    g_toolbar = NULL;
    m_toolbar_scale_tools_shown = false;
    piano_ctx_menu = NULL;

    //      Redirect the initialization timer to this frame
    InitTimer.SetOwner( this, INIT_TIMER );
    m_iInitCount = 0;
    
    //      Redirect the global heartbeat timer to this frame
    FrameTimer1.SetOwner( this, FRAME_TIMER_1 );

    //      Redirect the Tide/Current update timer to this frame
    FrameTCTimer.SetOwner( this, FRAME_TC_TIMER );

    //      Redirect the COG Averager timer to this frame
    FrameCOGTimer.SetOwner( this, FRAME_COG_TIMER );

    //      Redirect the Memory Footprint Management timer to this frame
    MemFootTimer.SetOwner( this, MEMORY_FOOTPRINT_TIMER );

    //      Redirect the Bells timer to this frame
    BellsTimer.SetOwner( this, BELLS_TIMER );

#ifdef __OCPN__ANDROID__
//    m_PrefTimer.SetOwner( this, ANDROID_PREF_TIMER );
//    Connect( m_PrefTimer.GetId(), wxEVT_TIMER, wxTimerEventHandler( MyFrame::OnPreferencesResultTimer ), NULL, this );
#endif
    
    //      Set up some assorted member variables
    nRoute_State = 0;
    m_bTimeIsSet = false;
    m_bdefer_resize = false;

    //    Clear the NMEA Filter tables
    for( int i = 0; i < MAX_COGSOG_FILTER_SECONDS; i++ ) {
        COGFilterTable[i] = 0.;
        SOGFilterTable[i] = 0.;
    }
    m_COGFilterLast = 0.;
    m_last_bGPSValid = false;

    gHdt = NAN;
    gHdm = NAN;
    gVar = NAN;
    gSog = NAN;
    gCog = NAN;
    m_fixtime = 0;

    m_bpersistent_quilt = false;

    m_ChartUpdatePeriod = 1;                  // set the default (1 sec.) period

//    Establish my children
    g_pMUX = new Multiplexer();
    
    g_pMUX->SetAISHandler(g_pAIS);
    g_pMUX->SetGPSHandler(this);
    
    //  Create/connect a dynamic event handler slot
    Connect( wxEVT_OCPN_DATASTREAM, (wxObjectEventFunction) (wxEventFunction) &MyFrame::OnEvtOCPN_NMEA );

    bFirstAuto = true;

    //  Create/connect a dynamic event handler slot for OCPN_MsgEvent(s) coming from PlugIn system
    Connect( wxEVT_OCPN_MSG, (wxObjectEventFunction) (wxEventFunction) &MyFrame::OnEvtPlugInMessage );

    Connect( wxEVT_OCPN_THREADMSG, (wxObjectEventFunction) (wxEventFunction) &MyFrame::OnEvtTHREADMSG );

    
    //        Establish the system icons for the frame.

#ifdef __WXMSW__
    SetIcon( wxICON(0) );           // this grabs the first icon in the integrated MSW resource file
#endif

#if defined(__WXGTK__) || defined(__WXQT__)
    wxIcon app_icon(opencpn);          // This comes from opencpn.xpm inclusion above
    SetIcon(app_icon);
#endif

#ifdef __WXMSW__

//    Establish the entry points in USER32.DLL for system color control

    wxDynamicLibrary dllUser32( _T("user32.dll") );

    pSetSysColors = (SetSysColors_t) dllUser32.GetSymbol( wxT("SetSysColors") );
    pGetSysColor = (GetSysColor_t) dllUser32.GetSymbol( wxT("GetSysColor") );

    SaveSystemColors();
#endif

    g_FloatingToolbarConfigMenu = new wxMenu();

    m_next_available_plugin_tool_id = ID_PLUGIN_BASE;

    m_COGFilterLast = 0.;

    g_sticky_chart = -1;
    m_BellsToPlay = 0;
}

MyFrame::~MyFrame()
{
    FrameTimer1.Stop();
    delete ChartData;
    delete pCurrentStack;

//      Free the Route List
    wxRouteListNode *node = pRouteList->GetFirst();

    while( node ) {
        Route *pRouteDelete = node->GetData();
        delete pRouteDelete;

        node = node->GetNext();
    }
    delete pRouteList;
    delete g_FloatingToolbarConfigMenu;
}

void MyFrame::OnEraseBackground( wxEraseEvent& event )
{
}

void MyFrame::OnMaximize( wxMaximizeEvent& event )
{
    g_click_stop = 0;
#ifdef __WXOSX__
    event.Skip();
#endif
}

void MyFrame::OnActivate( wxActivateEvent& event )
{
//    Code carefully in this method.
//    It is called in some unexpected places,
//    such as on closure of dialogs, etc.

    if( cc1 ) cc1->SetFocus();       // This seems to be needed for MSW, to get key and wheel events
                                     // after minimize/maximize.

#ifdef __WXOSX__
    if(event.GetActive())
    {
        SurfaceToolbar();
        
        if(stats)
            stats->Show(g_bShowChartBar);
    }
#endif

    event.Skip();
}

ColorScheme MyFrame::GetColorScheme()
{
    return global_color_scheme;
}

void MyFrame::SetAndApplyColorScheme( ColorScheme cs )
{
    global_color_scheme = cs;

    wxString SchemeName;
    switch( cs ){
        case GLOBAL_COLOR_SCHEME_DAY:
            SchemeName = _T("DAY");
            break;
        case GLOBAL_COLOR_SCHEME_DUSK:
            SchemeName = _T("DUSK");
            break;
        case GLOBAL_COLOR_SCHEME_NIGHT:
            SchemeName = _T("NIGHT");
            break;
        default:
            SchemeName = _T("DAY");
            break;
    }

    g_StyleManager->GetCurrentStyle()->SetColorScheme( cs );
    cc1->GetWorldBackgroundChart()->SetColorScheme( cs );

    //Search the user color table array to find the proper hash table
    Usercolortable_index = 0;
    for( unsigned int i = 0; i < UserColorTableArray->GetCount(); i++ ) {
        colTable *ct = (colTable *) UserColorTableArray->Item( i );
        if( SchemeName.IsSameAs( *ct->tableName ) ) {
            Usercolortable_index = i;
            break;
        }
    }

#ifdef USE_S57
    if( ps52plib ) ps52plib->SetPLIBColorScheme( SchemeName );
#endif

    //    Set up a pointer to the proper hash table
    pcurrent_user_color_hash = (wxColorHashMap *) UserColourHashTableArray->Item(
            Usercolortable_index );

    SetSystemColors( cs );

    if( cc1 ) cc1->SetColorScheme( cs );

    if( pWayPointMan ) pWayPointMan->SetColorScheme( cs );

    if( ChartData ) ChartData->ApplyColorSchemeToCachedCharts( cs );

    if( stats ) stats->SetColorScheme( cs );

    if( console ) console->SetColorScheme( cs );

    if( g_pRouteMan ) g_pRouteMan->SetColorScheme( cs );

    if( pMarkPropDialog ) pMarkPropDialog->SetColorScheme( cs );

    //    For the AIS target query dialog, we must rebuild it to incorporate the style desired for the colorscheme selected
    if( g_pais_query_dialog_active ) {
        bool b_isshown = g_pais_query_dialog_active->IsShown();
        int n_mmsi = g_pais_query_dialog_active->GetMMSI();
        if( b_isshown ) g_pais_query_dialog_active->Show( false );              // dismiss it

        g_pais_query_dialog_active->Close();

        g_pais_query_dialog_active = new AISTargetQueryDialog();
        g_pais_query_dialog_active->Create( this, -1, _( "AIS Target Query" ),
                g_pAIS->AlertDlgPosition() );
        g_pais_query_dialog_active->SetMMSI( n_mmsi );
        g_pais_query_dialog_active->UpdateText();
        if( b_isshown ) g_pais_query_dialog_active->Show();
    }

    if( pRouteManagerDialog ) pRouteManagerDialog->SetColorScheme();

    if( g_pAISTargetList ) g_pAISTargetList->SetColorScheme();

    if( g_pObjectQueryDialog ) g_pObjectQueryDialog->SetColorScheme();

    ApplyGlobalColorSchemetoStatusBar();

    UpdateToolbar( cs );

    if( g_pi_manager ) g_pi_manager->SetColorSchemeForAllPlugIns( cs );
}

void MyFrame::ApplyGlobalColorSchemetoStatusBar( void )
{
    if( m_pStatusBar != NULL ) {
        m_pStatusBar->SetBackgroundColour(GetGlobalColor(_T("UIBDR")));    //UINFF
        m_pStatusBar->ClearBackground();

        int styles[] = { wxSB_FLAT, wxSB_FLAT, wxSB_FLAT, wxSB_FLAT, wxSB_FLAT, wxSB_FLAT };
        m_pStatusBar->SetStatusStyles( m_StatusBarFieldCount, styles );
        int widths[] = { -6, -5, -5, -3, -4 };
        m_pStatusBar->SetStatusWidths( m_StatusBarFieldCount, widths );
    }
}

void MyFrame::DestroyMyToolbar()
{
    if( g_FloatingToolbarDialog ) {
        g_FloatingToolbarDialog->DestroyToolBar();
        g_toolbar = NULL;
    }
}

bool _toolbarConfigMenuUtil( int toolid, wxString tipString )
{
    wxMenuItem* menuitem;

    if( toolid == ID_MOB && g_bPermanentMOBIcon ) return true;

    // Item ID trickery is needed because the wxCommandEvents for menu item clicked and toolbar button
    // clicked are 100% identical, so if we use same id's we can't tell the events apart.

    int idOffset = ID_PLUGIN_BASE - ID_ZOOMIN + 100;  // Hopefully no more than 100 plugins loaded...
    int menuItemId = toolid + idOffset;

    menuitem = g_FloatingToolbarConfigMenu->FindItem( menuItemId );

    if( menuitem ) {
        return menuitem->IsChecked();
    }

    menuitem = g_FloatingToolbarConfigMenu->AppendCheckItem( menuItemId, tipString );
    menuitem->Check( g_toolbarConfig.GetChar( toolid - ID_ZOOMIN ) == _T('X') );
    return menuitem->IsChecked();
}

ocpnToolBarSimple *MyFrame::CreateAToolbar()
{
    ocpnToolBarSimple *tb = NULL;
    wxToolBarToolBase* newtool;

    if( g_FloatingToolbarDialog )
        tb = g_FloatingToolbarDialog->GetToolbar();
    if( !tb )
        return 0;

    ocpnStyle::Style* style = g_StyleManager->GetCurrentStyle();

    wxString tipString;

    CheckAndAddPlugInTool( tb );
    tipString = wxString( _("Zoom In") ) << _T(" (+)");
    if( _toolbarConfigMenuUtil( ID_ZOOMIN, tipString ) )
        tb->AddTool( ID_ZOOMIN, _T("zoomin"),
            style->GetToolIcon( _T("zoomin"), TOOLICON_NORMAL ), tipString, wxITEM_NORMAL );

    CheckAndAddPlugInTool( tb );
    tipString = wxString( _("Zoom Out") ) << _T(" (-)");
    if( _toolbarConfigMenuUtil( ID_ZOOMOUT, tipString ) )
        tb->AddTool( ID_ZOOMOUT, _T("zoomout"),
            style->GetToolIcon( _T("zoomout"), TOOLICON_NORMAL ), tipString, wxITEM_NORMAL );

    m_toolbar_scale_tools_shown = pCurrentStack && pCurrentStack->b_valid
            && ( pCurrentStack->nEntry > 1 );

    CheckAndAddPlugInTool( tb );
    tipString = wxString( _("Shift to Larger Scale Chart") ) << _T(" (F7)");
    if( _toolbarConfigMenuUtil( ID_STKDN, tipString ) ) {
        newtool = tb->AddTool( ID_STKDN, _T("scin"),
                style->GetToolIcon( _T("scin"), TOOLICON_NORMAL ), tipString, wxITEM_NORMAL );
        newtool->Enable( m_toolbar_scale_tools_shown );
    }

    CheckAndAddPlugInTool( tb );
    tipString = wxString( _("Shift to Smaller Scale Chart") ) << _T(" (F8)");
    if( _toolbarConfigMenuUtil( ID_STKUP, tipString ) ) {
        newtool = tb->AddTool( ID_STKUP, _T("scout"),
                style->GetToolIcon( _T("scout"), TOOLICON_NORMAL ), tipString, wxITEM_NORMAL );
        newtool->Enable( m_toolbar_scale_tools_shown );
    }

    CheckAndAddPlugInTool( tb );
    tipString = wxString( _("Create Route") ) << _T(" (Ctrl-R)");
    if( _toolbarConfigMenuUtil( ID_ROUTE, tipString ) )
        tb->AddTool( ID_ROUTE, _T("route"),
            style->GetToolIcon( _T("route"), TOOLICON_NORMAL ),
            style->GetToolIcon( _T("route"), TOOLICON_TOGGLED ), wxITEM_CHECK, tipString );

    CheckAndAddPlugInTool( tb );
    tipString = wxString( _("Auto Follow") ) << _T(" (F2)");
    if( _toolbarConfigMenuUtil( ID_FOLLOW, tipString ) )
        tb->AddTool( ID_FOLLOW, _T("follow"),
            style->GetToolIcon( _T("follow"), TOOLICON_NORMAL ),
            style->GetToolIcon( _T("follow"), TOOLICON_TOGGLED ), wxITEM_CHECK, tipString );

    CheckAndAddPlugInTool( tb );
    tipString = _("Options");
    if( _toolbarConfigMenuUtil( ID_SETTINGS, tipString ) )
        tb->AddTool( ID_SETTINGS, _T("settings"),
            style->GetToolIcon( _T("settings"), TOOLICON_NORMAL ), tipString, wxITEM_NORMAL );

    CheckAndAddPlugInTool( tb );
    tipString = wxString( _("Show ENC Text") ) << _T(" (T)");
    if( _toolbarConfigMenuUtil( ID_ENC_TEXT, tipString ) )
        tb->AddTool( ID_ENC_TEXT, _T("text"),
            style->GetToolIcon( _T("text"), TOOLICON_NORMAL ),
            style->GetToolIcon( _T("text"), TOOLICON_TOGGLED ), wxITEM_CHECK, tipString );

    m_pAISTool = NULL;
    CheckAndAddPlugInTool( tb );
    tipString = _("Hide AIS Targets");          // inital state is on
    if( _toolbarConfigMenuUtil( ID_AIS, tipString ) )
        m_pAISTool = tb->AddTool( ID_AIS, _T("AIS"), style->GetToolIcon( _T("AIS"), TOOLICON_NORMAL ),
                                  style->GetToolIcon( _T("AIS"), TOOLICON_DISABLED ),
                                  wxITEM_NORMAL, tipString );

    CheckAndAddPlugInTool( tb );
    tipString = _("Show Currents");
    if( _toolbarConfigMenuUtil( ID_CURRENT, tipString ) )
        tb->AddTool( ID_CURRENT, _T("current"),
            style->GetToolIcon( _T("current"), TOOLICON_NORMAL ), tipString, wxITEM_CHECK );

    CheckAndAddPlugInTool( tb );
    tipString = _("Show Tides");
    if( _toolbarConfigMenuUtil( ID_TIDE, tipString ) )
        tb->AddTool( ID_TIDE, _T("tide"),
            style->GetToolIcon( _T("tide"), TOOLICON_NORMAL ), tipString, wxITEM_CHECK );

    CheckAndAddPlugInTool( tb );
    tipString = _("Print Chart");
    if( _toolbarConfigMenuUtil( ID_PRINT, tipString ) )
        tb->AddTool( ID_PRINT, _T("print"),
            style->GetToolIcon( _T("print"), TOOLICON_NORMAL ), tipString, wxITEM_NORMAL );

    CheckAndAddPlugInTool( tb );
    tipString = _("Route & Mark Manager");
    if( _toolbarConfigMenuUtil( ID_ROUTEMANAGER, tipString ) )
        tb->AddTool( ID_ROUTEMANAGER,
            _T("route_manager"), style->GetToolIcon( _T("route_manager"), TOOLICON_NORMAL ),
            tipString, wxITEM_NORMAL );

    CheckAndAddPlugInTool( tb );
    tipString = _("Enable Tracking");
    if( _toolbarConfigMenuUtil( ID_TRACK, tipString ) )
        tb->AddTool( ID_TRACK, _T("track"),
            style->GetToolIcon( _T("track"), TOOLICON_NORMAL ),
            style->GetToolIcon( _T("track"), TOOLICON_TOGGLED ), wxITEM_CHECK, tipString );

    CheckAndAddPlugInTool( tb );
    tipString = wxString( _("Change Color Scheme") ) << _T(" (F5)");
    if( _toolbarConfigMenuUtil( ID_COLSCHEME, tipString ) ){
        tb->AddTool( ID_COLSCHEME,
            _T("colorscheme"), style->GetToolIcon( _T("colorscheme"), TOOLICON_NORMAL ),
            tipString, wxITEM_NORMAL );
        tb->SetToolTooltipHiViz( ID_COLSCHEME, true );  // cause the Tooltip to always be visible, whatever
                                                        //  the colorscheme
    }

    CheckAndAddPlugInTool( tb );
    tipString = _("About OpenCPN");
    if( _toolbarConfigMenuUtil( ID_ABOUT, tipString ) )
        tb->AddTool( ID_ABOUT, _T("help"),
            style->GetToolIcon( _T("help"), TOOLICON_NORMAL ), tipString, wxITEM_NORMAL );

    //      Add any PlugIn toolbar tools that request default positioning
    AddDefaultPositionPlugInTools( tb );

    //  And finally add the MOB tool
    tipString = wxString( _("Drop MOB Marker") ) << _(" (Ctrl-Space)");
    if( _toolbarConfigMenuUtil( ID_MOB, tipString ) )
        tb->AddTool( ID_MOB, _T("mob_btn"),
                     style->GetToolIcon( _T("mob_btn"), TOOLICON_NORMAL ), tipString, wxITEM_NORMAL );


// Realize() the toolbar
    g_FloatingToolbarDialog->Realize();

//      Set up the toggle states

    if( cc1 ) {
        //  Re-establish toggle states
        tb->ToggleTool( ID_CURRENT, cc1->GetbShowCurrent() );
        tb->ToggleTool( ID_TIDE, cc1->GetbShowTide() );
    }

    if( pConfig && cc1 )
        tb->ToggleTool( ID_FOLLOW, cc1->m_bFollow );

#ifdef USE_S57
    if( ( pConfig ) && ( ps52plib ) ) if( ps52plib->m_bOK ) tb->ToggleTool( ID_ENC_TEXT,
            ps52plib->GetShowS57Text() );
#endif

    wxString initiconName;
    if( g_pAIS->ShowAIS() ) {
        tb->SetToolShortHelp( ID_AIS, _("Hide AIS Targets") );
        initiconName = _T("AIS");
    }
    else {
        tb->SetToolShortHelp( ID_AIS, _("Show AIS Targets") );
        initiconName = _T("AIS_Disabled");
    }
    tb->SetToolNormalBitmapEx( m_pAISTool, initiconName );
    m_lastAISiconName = initiconName;

    tb->ToggleTool( ID_TRACK, g_bTrackActive );
    
    //  Set PlugIn tool toggle states
    ArrayOfPlugInToolbarTools tool_array = g_pi_manager->GetPluginToolbarToolArray();
    for( unsigned int i = 0; i < tool_array.GetCount(); i++ ) {
        PlugInToolbarToolContainer *pttc = tool_array.Item( i );
        if( !pttc->b_viz )
            continue;
        
        if( pttc->kind == wxITEM_CHECK )
            tb->ToggleTool( pttc->id, pttc->b_toggle );
    }
    

    SetStatusBarPane( -1 );                   // don't show help on status bar

    return tb;
}

bool MyFrame::CheckAndAddPlugInTool( ocpnToolBarSimple *tb )
{
    if( !g_pi_manager ) return false;

    bool bret = false;
    int n_tools = tb->GetToolsCount();

    //    Walk the PlugIn tool spec array, checking the requested position
    //    If a tool has been requested by a plugin at this position, add it
    ArrayOfPlugInToolbarTools tool_array = g_pi_manager->GetPluginToolbarToolArray();

    for( unsigned int i = 0; i < tool_array.GetCount(); i++ ) {
        PlugInToolbarToolContainer *pttc = tool_array.Item( i );
        if( pttc->position == n_tools ) {
            wxBitmap *ptool_bmp;

            switch( global_color_scheme ){
                case GLOBAL_COLOR_SCHEME_DAY:
                    ptool_bmp = pttc->bitmap_day;
                    ;
                    break;
                case GLOBAL_COLOR_SCHEME_DUSK:
                    ptool_bmp = pttc->bitmap_dusk;
                    break;
                case GLOBAL_COLOR_SCHEME_NIGHT:
                    ptool_bmp = pttc->bitmap_night;
                    break;
                default:
                    ptool_bmp = pttc->bitmap_day;
                    ;
                    break;
            }

            tb->AddTool( pttc->id, wxString( pttc->label ), *( ptool_bmp ),
                    wxString( pttc->shortHelp ), pttc->kind );
            bret = true;
        }
    }

    //    If we added a tool, call again (recursively) to allow for adding adjacent tools
    if( bret ) while( CheckAndAddPlugInTool( tb ) ) { /* nothing to do */
    }

    return bret;
}

bool MyFrame::AddDefaultPositionPlugInTools( ocpnToolBarSimple *tb )
{
    if( !g_pi_manager ) return false;

    bool bret = false;

    //    Walk the PlugIn tool spec array, checking the requested position
    //    If a tool has been requested by a plugin at this position, add it
    ArrayOfPlugInToolbarTools tool_array = g_pi_manager->GetPluginToolbarToolArray();

    for( unsigned int i = 0; i < tool_array.GetCount(); i++ ) {
        PlugInToolbarToolContainer *pttc = tool_array.Item( i );

        //      Tool is currently tagged as invisible
        if( !pttc->b_viz )
            continue;

        if( pttc->position == -1 )                  // PlugIn has requested default positioning
                {
            wxBitmap *ptool_bmp;

            switch( global_color_scheme ){
                case GLOBAL_COLOR_SCHEME_DAY:
                    ptool_bmp = pttc->bitmap_day;
                    ;
                    break;
                case GLOBAL_COLOR_SCHEME_DUSK:
                    ptool_bmp = pttc->bitmap_dusk;
                    break;
                case GLOBAL_COLOR_SCHEME_NIGHT:
                    ptool_bmp = pttc->bitmap_night;
                    break;
                default:
                    ptool_bmp = pttc->bitmap_day;
                    ;
                    break;
            }

            tb->AddTool( pttc->id, wxString( pttc->label ), *( ptool_bmp ),
                    wxString( pttc->shortHelp ), pttc->kind );
            bret = true;
        }
    }
    return bret;
}

void MyFrame::RequestNewToolbar()
{
    bool b_reshow = true;
    if( g_FloatingToolbarDialog ) {
        b_reshow = g_FloatingToolbarDialog->IsShown();

        float ff = fabs(g_FloatingToolbarDialog->GetScaleFactor() - g_toolbar_scalefactor);
        if(ff > 0.01f){
            DestroyMyToolbar();
            delete g_FloatingToolbarDialog;
            g_FloatingToolbarDialog = NULL;
        }
    }

    if( !g_FloatingToolbarDialog ) {
        g_FloatingToolbarDialog = new ocpnFloatingToolbarDialog( cc1,
             wxPoint( g_toolbar_x, g_toolbar_y ), g_toolbar_orient, g_toolbar_scalefactor );
    }

    if( g_FloatingToolbarDialog ) {
        if( g_FloatingToolbarDialog->IsToolbarShown() )
            DestroyMyToolbar();

        g_toolbar = CreateAToolbar();
        g_FloatingToolbarDialog->RePosition();
        g_FloatingToolbarDialog->SetColorScheme( global_color_scheme );
        g_FloatingToolbarDialog->Show( b_reshow );
    }
    
#ifdef __OCPN__ANDROID__
    DoChartUpdate();
#endif    

}

//      Update inplace the current toolbar with bitmaps corresponding to the current color scheme
void MyFrame::UpdateToolbar( ColorScheme cs )
{
    if( g_FloatingToolbarDialog ) {
        g_FloatingToolbarDialog->SetColorScheme( cs );

        if( g_FloatingToolbarDialog->IsToolbarShown() ) {
            DestroyMyToolbar();
            g_toolbar = CreateAToolbar();
        }
    }

    if( g_FloatingCompassDialog ) g_FloatingCompassDialog->SetColorScheme( cs );

    if( g_toolbar ) {
        //  Re-establish toggle states
        g_toolbar->ToggleTool( ID_FOLLOW, cc1->m_bFollow );
        g_toolbar->ToggleTool( ID_CURRENT, cc1->GetbShowCurrent() );
        g_toolbar->ToggleTool( ID_TIDE, cc1->GetbShowTide() );
    }

    return;
}

void MyFrame::EnableToolbar( bool newstate )
{
    if( g_toolbar ) {
        g_toolbar->EnableTool( ID_ZOOMIN, newstate );
        g_toolbar->EnableTool( ID_ZOOMOUT, newstate );
        g_toolbar->EnableTool( ID_STKUP, newstate );
        g_toolbar->EnableTool( ID_STKDN, newstate );
        g_toolbar->EnableTool( ID_ROUTE, newstate );
        g_toolbar->EnableTool( ID_FOLLOW, newstate );
        g_toolbar->EnableTool( ID_SETTINGS, newstate );
        g_toolbar->EnableTool( ID_ENC_TEXT, newstate );
        g_toolbar->EnableTool( ID_CURRENT, newstate );
        g_toolbar->EnableTool( ID_TIDE, newstate );
        g_toolbar->EnableTool( ID_ABOUT, newstate );
        g_toolbar->EnableTool( ID_TBEXIT, newstate );
        g_toolbar->EnableTool( ID_TBSTAT, newstate );
        g_toolbar->EnableTool( ID_PRINT, newstate );
        g_toolbar->EnableTool( ID_COLSCHEME, newstate );
        g_toolbar->EnableTool( ID_ROUTEMANAGER, newstate );
        g_toolbar->EnableTool( ID_TRACK, newstate );
        g_toolbar->EnableTool( ID_AIS, newstate );
    }
}

void MyFrame::SetToolbarScale()
{
    //  Get the basic size of a tool icon
    ocpnStyle::Style* style = g_StyleManager->GetCurrentStyle();
    wxSize style_tool_size = style->GetToolSize();

    g_toolbar_scalefactor = 1.0;
    if(g_bresponsive ){
        //      Adjust the scale factor so that the basic tool size is xx millimetres, assumed square
        float target_size = 9.0;                // mm

        float basic_tool_size_mm = style_tool_size.x / cc1->GetPixPerMM();
        g_toolbar_scalefactor =  target_size / basic_tool_size_mm;
        g_toolbar_scalefactor = wxMax(g_toolbar_scalefactor, 1.0);

        //  Round to the nearest "quarter", to avoid rendering artifacts
        g_toolbar_scalefactor = wxRound( g_toolbar_scalefactor * 4.0 )/ 4.0;
    }
}

// Intercept menu commands
void MyFrame::OnExit( wxCommandEvent& event )
{
    quitflag++;                             // signal to the timer loop

}

static bool b_inCloseWindow;

void MyFrame::OnCloseWindow( wxCloseEvent& event )
{
    //    It is possible that double clicks on application exit box could cause re-entrance here
    //    Not good, and don't need it anyway, so simply return.
    if( b_inCloseWindow ) {
//            wxLogMessage(_T("opencpn::MyFrame re-entering OnCloseWindow"));
        return;
    }

    //  If the multithread chart compressor engine is running, cancel the close command
    if( b_inCompressAllCharts ) {
        return;
    }

    if( bDBUpdateInProgress )
        return;

    b_inCloseWindow = true;

    ::wxSetCursor( wxCURSOR_WAIT );

    // If we happen to have the measure tool open on Ctrl-Q quit
    cc1->CancelMeasureRoute();

    // We save perspective before closing to restore position next time
    // Pane is not closed so the child is not notified (OnPaneClose)
    if( g_pAISTargetList ) {
        wxAuiPaneInfo &pane = g_pauimgr->GetPane( g_pAISTargetList );
        g_pAIS->set_TargetListAUIPerspective( g_pauimgr->SavePaneInfo( pane ) );
        g_pauimgr->DetachPane( g_pAISTargetList );
    }

    pConfig->SetPath( _T ( "/AUI" ) );
    pConfig->Write( _T ( "AUIPerspective" ), g_pauimgr->SavePerspective() );

    g_bquiting = true;

#ifndef __OCPN__ANDROID__    
#ifdef ocpnUSE_GL
    // cancel compression jobs
    if(g_bopengl && g_CompressorPool){
        g_CompressorPool->PurgeJobList();

        if(g_CompressorPool->GetRunningJobCount())
            g_bcompression_wait = true;
    }
#endif

    if( cc1 ) {
//        cc1->SetCursor( wxCURSOR_WAIT );

        cc1->Refresh( true );
        cc1->Update();
        wxYield();
    }
#endif

    //   Save the saved Screen Brightness
    RestoreScreenBrightness();

    //    Deactivate the PlugIns
    if( g_pi_manager ) {
        g_pi_manager->DeactivateAllPlugIns();
    }

    wxLogMessage( _T("opencpn::MyFrame exiting cleanly.") );

    quitflag++;

    FrameTimer1.Stop();

    /*
     Automatically drop an anchorage waypoint, if enabled
     On following conditions:
     1.  In "Cruising" mode, meaning that speed has at some point exceeded 3.0 kts.
     2.  Current speed is less than 0.5 kts.
     3.  Opencpn has been up at least 30 minutes
     4.  And, of course, opencpn is going down now.
     5.  And if there is no anchor watch set on "anchor..." icon mark           // pjotrc 2010.02.15
     */
    if( g_bAutoAnchorMark ) {
        bool watching_anchor = false;                                           // pjotrc 2010.02.15
        if( pAnchorWatchPoint1 )                                               // pjotrc 2010.02.15
        watching_anchor = ( pAnchorWatchPoint1->GetIconName().StartsWith( _T("anchor") ) ); // pjotrc 2010.02.15
        if( pAnchorWatchPoint2 )                                               // pjotrc 2010.02.15
        watching_anchor |= ( pAnchorWatchPoint2->GetIconName().StartsWith( _T("anchor") ) ); // pjotrc 2010.02.15

        wxDateTime now = wxDateTime::Now();
        wxTimeSpan uptime = now.Subtract( g_start_time );

        if( !watching_anchor && ( g_bCruising ) && ( gSog < 0.5 )
                && ( uptime.IsLongerThan( wxTimeSpan( 0, 30, 0, 0 ) ) ) )     // pjotrc 2010.02.15
                {
            //    First, delete any single anchorage waypoint closer than 0.25 NM from this point
            //    This will prevent clutter and database congestion....

            wxRoutePointListNode *node = pWayPointMan->GetWaypointList()->GetFirst();
            while( node ) {
                RoutePoint *pr = node->GetData();
                if( pr->GetName().StartsWith( _T("Anchorage") ) ) {
                    double a = gLat - pr->m_lat;
                    double b = gLon - pr->m_lon;
                    double l = sqrt( ( a * a ) + ( b * b ) );

                    // caveat: this is accurate only on the Equator
                    if( ( l * 60. * 1852. ) < ( .25 * 1852. ) ) {
                        pConfig->DeleteWayPoint( pr );
                        pSelect->DeleteSelectablePoint( pr, SELTYPE_ROUTEPOINT );
                        delete pr;
                        break;
                    }
                }

                node = node->GetNext();
            }

            wxString name = now.Format();
            name.Prepend( _("Anchorage created ") );
            RoutePoint *pWP = new RoutePoint( gLat, gLon, _T("anchorage"), name, GPX_EMPTY_STRING );
            pWP->m_bShowName = false;
            pWP->m_bIsolatedMark = true;

            pConfig->AddNewWayPoint( pWP, -1 );       // use auto next num
        }
    }

    FrameTimer1.Stop();
    FrameCOGTimer.Stop();

    g_bframemax = IsMaximized();

    //    Record the current state of tracking
//    g_bTrackCarryOver = g_bTrackActive;

    TrackOff();

    if( pCurrentStack ) {
        g_restore_stackindex = pCurrentStack->CurrentStackEntry;
        g_restore_dbindex = pCurrentStack->GetCurrentEntrydbIndex();
    }

    if( g_FloatingToolbarDialog ) {
        wxPoint tbp = g_FloatingToolbarDialog->GetPosition();
        wxPoint tbp_incanvas = cc1->ScreenToClient( tbp );
        g_toolbar_x = tbp_incanvas.x;
        g_toolbar_y = tbp_incanvas.y;
        g_toolbar_orient = g_FloatingToolbarDialog->GetOrient();
    }

    pConfig->UpdateSettings();
    pConfig->UpdateNavObj();

//    pConfig->m_pNavObjectChangesSet->Clear();
    delete pConfig->m_pNavObjectChangesSet;

    //Remove any leftover Routes and Waypoints from config file as they were saved to navobj before
    pConfig->DeleteGroup( _T ( "/Routes" ) );
    pConfig->DeleteGroup( _T ( "/Marks" ) );
    pConfig->Flush();

    delete g_printData;
    delete g_pageSetupData;

    if( g_pAboutDlg ) g_pAboutDlg->Destroy();

//      Explicitely Close some children, especially the ones with event handlers
//      or that call GUI methods

#ifdef USE_S57
    if( g_pCM93OffsetDialog ) g_pCM93OffsetDialog->Destroy();
#endif

    if(g_FloatingToolbarDialog)
        g_FloatingToolbarDialog->Destroy();
    g_FloatingToolbarDialog = NULL;

    if( g_pAISTargetList ) {
        g_pAISTargetList->Disconnect_decoder();
        g_pAISTargetList->Destroy();
    }

    if( g_FloatingCompassDialog ) g_FloatingCompassDialog->Destroy();
    g_FloatingCompassDialog = NULL;

    //      Delete all open charts in the cache
    cc1->EnablePaint(false);
    if( ChartData )
        ChartData->PurgeCache();




#ifndef __OCPN__ANDROID__
    SetStatusBar( NULL );
#endif    
    stats = NULL;

    if( pRouteManagerDialog ) {
        pRouteManagerDialog->Destroy();
        pRouteManagerDialog = NULL;
    }

    cc1->Destroy();
    cc1 = NULL;

    g_pauimgr->UnInit();
    delete g_pauimgr;
    g_pauimgr = NULL;
    //    Unload the PlugIns
    //      Note that we are waiting until after the canvas is destroyed,
    //      since some PlugIns may have created children of canvas.
    //      Such a PlugIn must stay intact for the canvas dtor to call DestoryChildren()
    if( g_pi_manager ) {
        g_pi_manager->UnLoadAllPlugIns();
        delete g_pi_manager;
        g_pi_manager = NULL;
    }

    if( g_pAIS ) {
        if(g_pMUX)
            g_pMUX->SetAISHandler(NULL);
        delete g_pAIS;
        g_pAIS = NULL;
    }

    delete g_pMUX;

    //  Clear some global arrays, lists, and hash maps...
    for ( size_t i = 0; i < g_pConnectionParams->Count(); i++ )
    {
        ConnectionParams *cp = g_pConnectionParams->Item(i);
        delete cp;
    }
    delete g_pConnectionParams;
    
    if(pLayerList){
        LayerList::iterator it;
        while(pLayerList->GetCount()){
            Layer *lay = pLayerList->GetFirst()->GetData();
            delete lay;                 // automatically removes the layer from list, see Layer dtor
        }
    }
    
    MsgPriorityHash::iterator it;
    for( it = NMEA_Msg_Hash.begin(); it != NMEA_Msg_Hash.end(); ++it ){
        NMEA_Msg_Container *pc = it->second;
        delete pc;
    }
    NMEA_Msg_Hash.clear();
    
    pthumbwin = NULL;

    NMEALogWindow::Shutdown();
    
    g_FloatingToolbarDialog = NULL;
    g_bTempShowMenuBar = false;
    
    this->Destroy();
    
    gFrame = NULL;

#ifndef __OCPN__ANDROID__    
    #define THREAD_WAIT_SECONDS  5
#ifdef ocpnUSE_GL
    // The last thing we do is finish the compression threads.
    // This way the main window is already invisible and to the user
    // it appears to have finished rather than hanging for several seconds
    // while the compression threads exit
    if(g_bopengl && g_CompressorPool){
        wxDateTime now = wxDateTime::Now();
        time_t stall = now.GetTicks();
        time_t end = stall + THREAD_WAIT_SECONDS;

        while(stall < end ) {
            wxDateTime later = wxDateTime::Now();
            stall = later.GetTicks();

            if(!g_CompressorPool->GetRunningJobCount())
                break;
            wxYield();
            wxSleep(1);
        }
    }
#endif
#endif


#ifdef __OCPN__ANDROID__
    wxTheApp->OnExit();
#endif

}

void MyFrame::OnMove( wxMoveEvent& event )
{
    if( g_FloatingToolbarDialog ) g_FloatingToolbarDialog->RePosition();

    if( stats && stats->IsVisible()) stats->RePosition();

    UpdateGPSCompassStatusBox( );

    if( console && console->IsShown() ) PositionConsole();

//    Somehow, this method does not work right on Windows....
//      g_nframewin_posx = event.GetPosition().x;
//      g_nframewin_posy = event.GetPosition().y;

    g_nframewin_posx = GetPosition().x;
    g_nframewin_posy = GetPosition().y;
}

void MyFrame::ProcessCanvasResize( void )
{
    if( stats ) {
        stats->ReSize();
        stats->RePosition();
    }

    if( g_FloatingToolbarDialog ) {
        g_FloatingToolbarDialog->RePosition();
        g_FloatingToolbarDialog->SetGeometry();
        g_FloatingToolbarDialog->Realize();
        g_FloatingToolbarDialog->RePosition();

    }

    UpdateGPSCompassStatusBox( );

    if( console && console->IsShown() ) PositionConsole();
}

void MyFrame::OnSize( wxSizeEvent& event )
{
    ODoSetSize();
}

void MyFrame::ODoSetSize( void )
{
    int x, y;
    GetClientSize( &x, &y );

//      Resize the children

    if( m_pStatusBar ) {
        //  Maybe resize the font so the text fits in the boxes

        wxRect stat_box;
        m_pStatusBar->GetFieldRect( 0, stat_box );
        // maximum size is 1/28 of the box width, or the box height - whicever is less
        int max_font_size = wxMin( (stat_box.width / 28), (stat_box.height) );

        wxFont sys_font = *wxNORMAL_FONT;
        int try_font_size = sys_font.GetPointSize();

#ifdef __WXOSX__
        int min_font_size = 10; // much less than 10pt is unreadably small on OS X
        try_font_size += 1;     // default to 1pt larger than system UI font
#else
        int min_font_size = 7;  // on Win/Linux the text does not shrink quite so fast
        try_font_size += 2;     // default to 2pt larger than system UI font
#endif

        // get the user's preferred font, or if none set then the system default with the size overridden
        wxFont* templateFont = FontMgr::Get().GetFont( _("StatusBar"), try_font_size );
        int font_size = templateFont->GetPointSize();

        font_size = wxMin( font_size, max_font_size );  // maximum to fit in the statusbar boxes
        font_size = wxMax( font_size, min_font_size );  // minimum to stop it being unreadable

        wxFont *pstat_font = wxTheFontList->FindOrCreateFont( font_size,
              wxFONTFAMILY_SWISS, templateFont->GetStyle(), templateFont->GetWeight(), false,
              templateFont->GetFaceName() );

        m_pStatusBar->SetFont( *pstat_font );
#ifdef __WXQT__
        m_pStatusBar->SetMinHeight( pstat_font->GetPointSize() + 10 );
#endif
        
    }

    int cccw = x;
    int ccch = y;

    if( cc1 ) {
        int cur_width, cur_height;
        cc1->GetSize( &cur_width, &cur_height );
        if( ( cur_width != cccw ) || ( cur_height != ccch ) ) {
            if( g_pauimgr->GetPane( cc1 ).IsOk() )
                g_pauimgr->GetPane( cc1 ).BestSize( cccw, ccch );
            else
                cc1->SetSize( 0, 0, cccw, ccch );
        }
    }


    if( g_FloatingToolbarDialog ) {
        wxSize oldSize = g_FloatingToolbarDialog->GetSize();
        g_FloatingToolbarDialog->RePosition();
        g_FloatingToolbarDialog->SetGeometry();
        g_FloatingToolbarDialog->Realize();

        if( oldSize != g_FloatingToolbarDialog->GetSize() )
            g_FloatingToolbarDialog->Refresh( false );

        g_FloatingToolbarDialog->RePosition();

    }


    UpdateGPSCompassStatusBox( true );

    if( console ) PositionConsole();

    if( stats ) {
        stats->ReSize();
        stats->FormatStat();
        stats->RePosition();
    }

//  Update the stored window size
    GetSize( &x, &y );
    g_nframewin_x = x;
    g_nframewin_y = y;

//  Inform the PlugIns
    if( g_pi_manager ) g_pi_manager->SendResizeEventToAllPlugIns( x, y );

//  Force redraw if in lookahead mode
    if( g_bLookAhead ) {
        if( g_bCourseUp ) DoCOGSet();
        else
            DoChartUpdate();
    }

    if( pthumbwin )
        pthumbwin->SetMaxSize( cc1->GetParent()->GetSize() );

    //  Reset the options dialog size logic
    options_lastWindowSize = wxSize(0,0);
    options_lastWindowPos = wxPoint(0,0);

    if( pRouteManagerDialog && pRouteManagerDialog->IsShown() ){
        pRouteManagerDialog->Centre();
    }

}

void MyFrame::PositionConsole( void )
{
    if( NULL == cc1 ) return;
    //    Reposition console based on its size and chartcanvas size
    int ccx, ccy, ccsx, ccsy, consx, consy;
    cc1->GetSize( &ccsx, &ccsy );
    cc1->GetPosition( &ccx, &ccy );

    console->GetSize( &consx, &consy );

    wxPoint screen_pos = ClientToScreen( wxPoint( ccx + ccsx - consx - 2, ccy + 45 ) );
    console->Move( screen_pos );
}

void MyFrame::UpdateAllFonts()
{
    if( console ) {
        console->UpdateFonts();
        //    Reposition console
        PositionConsole();
    }

    //  Close and destroy any persistent dialogs, so that new fonts will be utilized
    if( g_pais_query_dialog_active ) {
        g_pais_query_dialog_active->Destroy();
        g_pais_query_dialog_active = NULL;
    }

    if( pRoutePropDialog ) {
        pRoutePropDialog->Destroy();
        pRoutePropDialog = NULL;
    }

    if( pTrackPropDialog ) {
        pTrackPropDialog->Destroy();
        pTrackPropDialog = NULL;
    }

    if( pMarkPropDialog ) {
        pMarkPropDialog->Destroy();
        pMarkPropDialog = NULL;
    }

    if( g_pObjectQueryDialog ) {
        g_pObjectQueryDialog->Destroy();
        g_pObjectQueryDialog = NULL;
    }


    if( pWayPointMan ) pWayPointMan->ClearRoutePointFonts();

    cc1->Refresh();
}

void MyFrame::SetGroupIndex( int index )
{
    int new_index = index;
    if( index > (int) g_pGroupArray->GetCount() ) new_index = 0;

    bool bgroup_override = false;
    int old_group_index = new_index;

    if( !CheckGroup( new_index ) ) {
        new_index = 0;
        bgroup_override = true;
    }

    //    Get the currently displayed chart native scale, and the current ViewPort
    int current_chart_native_scale = cc1->GetCanvasChartNativeScale();
    ViewPort vp = cc1->GetVP();

    g_GroupIndex = new_index;

    //  Invalidate the "sticky" chart on group change, since it might not be in the new group
    g_sticky_chart = -1;

    //    We need a new chartstack and quilt to figure out which chart to open in the new group
    cc1->UpdateCanvasOnGroupChange();

    int dbi_hint = cc1->FindClosestCanvasChartdbIndex( current_chart_native_scale );

    double best_scale = cc1->GetBestStartScale(dbi_hint, vp);

    cc1->SetVPScale( best_scale );

    if(cc1->GetQuiltMode())
        dbi_hint = cc1->GetQuiltReferenceChartIndex();

    //    Refresh the canvas, selecting the "best" chart,
    //    applying the prior ViewPort exactly
    ChartsRefresh( dbi_hint, vp, true );

    //    Message box is deferred so that canvas refresh occurs properly before dialog
    if( bgroup_override ) {
        wxString msg( _("Group \"") );
        msg += GetGroupName( old_group_index );
        msg += _("\" is empty, switching to \"All Active Charts\" group.");

        OCPNMessageBox( this, msg, _("OpenCPN Group Notice"), wxOK );
    }
}

void MyFrame::OnToolLeftClick( wxCommandEvent& event )
{
    if( s_ProgDialog ) return;

    switch( event.GetId() ){
        case ID_MENU_SCALE_OUT:
        case ID_STKUP:
            DoStackDelta( 1 );
            DoChartUpdate();
            break;

        case ID_MENU_SCALE_IN:
        case ID_STKDN:
            DoStackDelta( -1 );
            DoChartUpdate();
            break;

        case ID_MENU_ZOOM_IN:
        case ID_ZOOMIN: {
            cc1->ZoomCanvas( 2.0, false );
            DoChartUpdate();
            break;
        }

        case ID_MENU_ZOOM_OUT:
        case ID_ZOOMOUT: {
            cc1->ZoomCanvas( 0.5, false );
            DoChartUpdate();
            break;
        }

        case ID_MENU_ROUTE_NEW:
        case ID_ROUTE: {
            if( 0 == nRoute_State ){
                nRoute_State = 1;
                cc1->SetCursor( *cc1->pCursorPencil );
                SetToolbarItemState( ID_ROUTE, true );
            }
            else {
                cc1->FinishRoute();
                SetToolbarItemState( ID_ROUTE, false );
            }

            break;
        }

        case ID_MENU_TOOL_MEASURE: {
            cc1->StartMeasureRoute();
            break;
        }

        case ID_MENU_MARK_BOAT: {
            cc1->DropMarker(true);
            break;
        }

        case ID_MENU_MARK_CURSOR: {
            cc1->DropMarker(false);
            break;
        }

        case ID_MENU_NAV_FOLLOW:
        case ID_FOLLOW: {
            TogglebFollow();
            break;
        }

        case ID_MENU_CHART_OUTLINES: {
            ToggleChartOutlines();
            break;
        }

        case ID_MENU_CHART_QUILTING: {
            ToggleQuiltMode();
            break;
        }

        case ID_MENU_UI_CHARTBAR: {
            ToggleStats();
            break;
        }
            
#ifdef USE_S57
        case ID_MENU_ENC_TEXT:
        case ID_ENC_TEXT: {
            ToggleENCText();
            break;
        }
        case ID_MENU_ENC_LIGHTS: {
            ToggleLights();
            break;
        }
        case ID_MENU_ENC_SOUNDINGS: {
            ToggleSoundings();
            break;
        }
        case ID_MENU_ENC_ANCHOR: {
            ToggleAnchor();
            break;
        }
#endif

        case ID_MENU_AIS_TARGETS:
        case ID_AIS: {
            g_pAIS->set_ShowAIS( !g_pAIS->ShowAIS() );

            if( g_toolbar ) {
                wxString iconName;
                if( g_pAIS->ShowAIS() ) {
                    g_toolbar->SetToolShortHelp( ID_AIS, _("Hide AIS Targets") );
                    iconName = _T("AIS");
                } else {
                    g_toolbar->SetToolShortHelp( ID_AIS, _("Show AIS Targets") );
                    iconName = _T("AIS_Disabled");
                }

                if( m_pAISTool ) {
                    g_toolbar->SetToolNormalBitmapEx( m_pAISTool, iconName );
                    g_toolbar->Refresh();
                    m_lastAISiconName = iconName;
                }
            }

            SetMenubarItemState( ID_MENU_AIS_TARGETS, g_pAIS->ShowAIS() );

            cc1->ReloadVP();

            break;
        }

        case ID_MENU_AIS_TARGETLIST: {
            if ( cc1 ) cc1->ShowAISTargetList();
            break;
        }

        case ID_MENU_AIS_TRACKS: {
            g_pAIS->set_ShowTracks( !g_pAIS->ShowTracks() );
            SetMenubarItemState( ID_MENU_AIS_TRACKS, g_pAIS->ShowTracks() );
            break;
        }

        case ID_MENU_AIS_CPADIALOG: {
            g_pAIS->set_CPAAlert( !g_pAIS->CPAAlert() );
            SetMenubarItemState( ID_MENU_AIS_CPADIALOG, g_pAIS->CPAAlert() );
            break;
        }

        case ID_MENU_AIS_CPASOUND: {
            g_pAIS->set_CPAAlertAudio( !g_pAIS->CPAAlertAudio() );
            SetMenubarItemState( ID_MENU_AIS_CPADIALOG, g_pAIS->CPAAlertAudio() );
            break;
        }

        case wxID_PREFERENCES:
        case ID_SETTINGS: {
            DoSettings();
            break;
        }

        case ID_MENU_UI_FULLSCREEN: {
            ToggleFullScreen();
            break;
        }

        case ID_MENU_SHOW_CURRENTS:
        case ID_CURRENT: {
            LoadHarmonics();

            if( ptcmgr->IsReady() ) {
                cc1->SetbShowCurrent( !cc1->GetbShowCurrent() );
                SetToolbarItemState( ID_CURRENT, cc1->GetbShowCurrent() );
                wxString tip = _("Show Currents");
                if(cc1->GetbShowCurrent())
                    tip = _("Hide Currents");
                if( g_toolbar )
                    g_toolbar->SetToolShortHelp( ID_CURRENT, tip );
                
                SetMenubarItemState( ID_MENU_SHOW_CURRENTS, cc1->GetbShowCurrent() );
                cc1->ReloadVP();
            } else {
                wxLogMessage( _T("Chart1::Event...TCMgr Not Available") );
                cc1->SetbShowCurrent( false );
                SetToolbarItemState( ID_CURRENT, false );
                SetMenubarItemState( ID_MENU_SHOW_CURRENTS, false );
            }

            if( cc1->GetbShowCurrent() ) {
                FrameTCTimer.Start( TIMER_TC_VALUE_SECONDS * 1000, wxTIMER_CONTINUOUS );
                cc1->SetbTCUpdate( true );                        // force immediate update
            } else
                FrameTCTimer.Stop();

            cc1->Refresh( false );

            break;

        }

        case ID_MENU_SHOW_TIDES:
        case ID_TIDE: {
            LoadHarmonics();

            if( ptcmgr->IsReady() ) {
                cc1->SetbShowTide( !cc1->GetbShowTide() );
                SetToolbarItemState( ID_TIDE, cc1->GetbShowTide() );
                wxString tip = _("Show Tides");
                if(cc1->GetbShowTide())
                    tip = _("Hide Tides");
                if( g_toolbar )
                    g_toolbar->SetToolShortHelp( ID_TIDE, tip );
                
                SetMenubarItemState( ID_MENU_SHOW_TIDES, cc1->GetbShowTide() );
                cc1->ReloadVP();
            } else {
                wxLogMessage( _("Chart1::Event...TCMgr Not Available") );
                cc1->SetbShowTide( false );
                SetToolbarItemState( ID_TIDE, false );
                SetMenubarItemState( ID_MENU_SHOW_TIDES, false );
            }

            if( cc1->GetbShowTide() ) {
                FrameTCTimer.Start( TIMER_TC_VALUE_SECONDS * 1000, wxTIMER_CONTINUOUS );
                cc1->SetbTCUpdate( true );                        // force immediate update
            } else
                FrameTCTimer.Stop();

            cc1->Refresh( false );

            break;

        }

        case wxID_ABOUT:
        case ID_ABOUT: {
            if( !g_pAboutDlg ) g_pAboutDlg = new about( this, &g_SData_Locn );

            g_pAboutDlg->Update();
            g_pAboutDlg->Show();

            break;
        }

        case wxID_HELP: {
            LaunchLocalHelp();
            break;
        }

        case ID_PRINT: {
            DoPrint();
            break;
        }

        case ID_MENU_UI_COLSCHEME:
        case ID_COLSCHEME: {
            ToggleColorScheme();
            break;
        }

        case ID_TBEXIT: {
            Close();
            break;
        }

        case ID_MENU_OQUIT: {
            Close();
            break;
        }
        
        case ID_MENU_ROUTE_MANAGER:
        case ID_ROUTEMANAGER: {
            if( NULL == pRouteManagerDialog )         // There is one global instance of the Dialog
            pRouteManagerDialog = new RouteManagerDialog( cc1 );

            pRouteManagerDialog->UpdateRouteListCtrl();
            pRouteManagerDialog->UpdateTrkListCtrl();
            pRouteManagerDialog->UpdateWptListCtrl();
            pRouteManagerDialog->UpdateLayListCtrl();
            pRouteManagerDialog->Show();

            //    Required if RMDialog is not STAY_ON_TOP
#ifdef __WXOSX__
            pRouteManagerDialog->Centre();
            pRouteManagerDialog->Raise();
#endif
            break;
        }

        case ID_MENU_NAV_TRACK:
        case ID_TRACK: {
            if( !g_bTrackActive ) {
                TrackOn();
                g_bTrackCarryOver = true;
            } else {
                TrackOff( true );
                g_bTrackCarryOver = false;
                cc1->Refresh(true);
            }
            break;
        }

        case ID_MENU_CHART_NORTHUP:
        case ID_MENU_CHART_COGUP:
        case ID_TBSTATBOX: {
            ToggleCourseUp();
            break;
        }

        case ID_MENU_MARK_MOB:
        case ID_MOB: {
            ActivateMOB();
            break;
        }

        default: {
            //        Look for PlugIn tools
            //        If found, make the callback.
            //        TODO Modify this to allow multiple tools per plugin
            if( g_pi_manager ) {
                ArrayOfPlugInToolbarTools tool_array = g_pi_manager->GetPluginToolbarToolArray();
                for( unsigned int i = 0; i < tool_array.GetCount(); i++ ) {
                    PlugInToolbarToolContainer *pttc = tool_array.Item( i );
                    if( event.GetId() == pttc->id ) {
                        if( pttc->m_pplugin ) pttc->m_pplugin->OnToolbarToolCallback( pttc->id );
                        return; // required to prevent event.Skip() being called
                    }
                }
            }

            // If we didn't handle the event, allow it to bubble up to other handlers.
            // This is required for the system menu items (Hide, etc) on OS X to work.
            // This must only be called if we did NOT handle the event, otherwise it
            // stops the menu items from working on Windows.
            event.Skip();

            break;
        }

    }         // switch

}

void MyFrame::DoSettings()
{
    bool bnewtoolbar = !( DoOptionsDialog() == 0 );

    //              Apply various system settings
    ApplyGlobalSettings( true, bnewtoolbar );                 // flying update

    if( g_FloatingToolbarDialog )
        g_FloatingToolbarDialog->RefreshFadeTimer();

    if( cc1->GetbShowCurrent() || cc1->GetbShowTide() )
        LoadHarmonics();
    
    //  The chart display options may have changed, especially on S57 ENC,
    //  So, flush the cache and redraw
    cc1->ReloadVP();
}


void MyFrame::ToggleStats()
{
    if( stats ) {
        if( stats->IsShown() ){
            stats->Hide();
            g_bShowChartBar = false;
        }
        else {
            stats->Move(0,0);
            stats->RePosition();
            stats->Show();
            gFrame->Raise();
            DoChartUpdate();
            UpdateControlBar();
            g_bShowChartBar = true;
        }
        Refresh();
        
        SetMenubarItemState( ID_MENU_UI_CHARTBAR, g_bShowChartBar );
        
    }
}

void MyFrame::ToggleColorScheme()
{
    ColorScheme s = GetColorScheme();
    int is = (int) s;
    is++;
    s = (ColorScheme) is;
    if( s == N_COLOR_SCHEMES ) s = GLOBAL_COLOR_SCHEME_RGB;

    SetAndApplyColorScheme( s );
}

void MyFrame::ToggleFullScreen()
{
    bool to = !IsFullScreen();
    long style = wxFULLSCREEN_NOBORDER | wxFULLSCREEN_NOCAPTION;; // | wxFULLSCREEN_NOMENUBAR;

    ShowFullScreen( to, style );
    UpdateToolbar( global_color_scheme );
    UpdateControlBar();
    Layout();
}

void MyFrame::ActivateMOB( void )
{
    //    The MOB point
    wxDateTime mob_time = wxDateTime::Now();
    wxString mob_label( _( "MAN OVERBOARD" ) );
    mob_label += _T(" at ");
    mob_label += mob_time.FormatTime();

    RoutePoint *pWP_MOB = new RoutePoint( gLat, gLon, _T ( "mob" ), mob_label, GPX_EMPTY_STRING );
    pWP_MOB->m_bKeepXRoute = true;
    pWP_MOB->m_bIsolatedMark = true;
    pSelect->AddSelectableRoutePoint( gLat, gLon, pWP_MOB );
    pConfig->AddNewWayPoint( pWP_MOB, -1 );       // use auto next num


    if( bGPSValid && !wxIsNaN(gCog) && !wxIsNaN(gSog) ) {
        //    Create a point that is one mile along the present course
        double zlat, zlon;
        ll_gc_ll( gLat, gLon, gCog, 1.0, &zlat, &zlon );

        RoutePoint *pWP_src = new RoutePoint( zlat, zlon, g_default_wp_icon,
                wxString( _( "1.0 NM along COG" ) ), GPX_EMPTY_STRING );
        pSelect->AddSelectableRoutePoint( zlat, zlon, pWP_src );

        Route *temp_route = new Route();
        pRouteList->Append( temp_route );

        temp_route->AddPoint( pWP_src );
        temp_route->AddPoint( pWP_MOB );

        pSelect->AddSelectableRouteSegment( gLat, gLon, zlat, zlon, pWP_src, pWP_MOB, temp_route );

        temp_route->m_RouteNameString = _("Temporary MOB Route");
        temp_route->m_RouteStartString = _("Assumed 1 Mile Point");
        ;
        temp_route->m_RouteEndString = mob_label;

        temp_route->m_bDeleteOnArrival = false;

        temp_route->SetRouteArrivalRadius( -1.0 );                    // never arrives

        temp_route->RebuildGUIDList();         // ensure the GUID list is intact and good

        if( g_pRouteMan->GetpActiveRoute() ) g_pRouteMan->DeactivateRoute();
        g_pRouteMan->ActivateRoute( temp_route, pWP_MOB );

        wxJSONValue v;
        v[_T("GUID")] = temp_route->m_GUID;
        wxString msg_id( _T("OCPN_MAN_OVERBOARD") );
        g_pi_manager->SendJSONMessageToAllPlugins( msg_id, v );
    }

    if( pRouteManagerDialog && pRouteManagerDialog->IsShown() ) {
        pRouteManagerDialog->UpdateRouteListCtrl();
        pRouteManagerDialog->UpdateWptListCtrl();
    }

    cc1->InvalidateGL();
    cc1->Refresh( false );

    wxString mob_message( _( "MAN OVERBOARD" ) );
    mob_message += _T(" Time: ");
    mob_message += mob_time.Format();
    mob_message += _T("  Position: ");
    mob_message += toSDMM( 1, gLat );
    mob_message += _T("   ");
    mob_message += toSDMM( 2, gLon );
    wxLogMessage( mob_message );

}
void MyFrame::TrackOn( void )
{
    g_bTrackActive = true;
    g_pActiveTrack = new Track();

    pRouteList->Append( g_pActiveTrack );
    if(pConfig)
        pConfig->AddNewRoute( g_pActiveTrack, 0 );

    g_pActiveTrack->Start();

    SetToolbarItemState( ID_TRACK, g_bTrackActive );
    if( g_toolbar )
        g_toolbar->SetToolShortHelp( ID_TRACK, _("Disable Tracking") );
    
    SetMenubarItemState( ID_MENU_NAV_TRACK, g_bTrackActive );

    if( pRouteManagerDialog && pRouteManagerDialog->IsShown() )
    {
        pRouteManagerDialog->UpdateTrkListCtrl();
        pRouteManagerDialog->UpdateRouteListCtrl();
    }

    wxJSONValue v;
    wxDateTime now;
    now = now.Now().ToUTC();
    wxString name = g_pActiveTrack->m_RouteNameString;
    if(name.IsEmpty())
    {
        RoutePoint *rp = g_pActiveTrack->GetPoint( 1 );
        if( rp && rp->GetCreateTime().IsValid() )
            name = rp->GetCreateTime().FormatISODate() + _T(" ") + rp->GetCreateTime().FormatISOTime();
        else
            name = _("(Unnamed Track)");
    }
    v[_T("Name")] = name;
    v[_T("GUID")] = g_pActiveTrack->m_GUID;
    wxString msg_id( _T("OCPN_TRK_ACTIVATED") );
    g_pi_manager->SendJSONMessageToAllPlugins( msg_id, v );
}

Track *MyFrame::TrackOff( bool do_add_point )
{
    Track *return_val = g_pActiveTrack;

    if( g_pActiveTrack )
    {
        wxJSONValue v;
        wxString msg_id( _T("OCPN_TRK_DEACTIVATED") );
        v[_T("GUID")] = g_pActiveTrack->m_GUID;
        g_pi_manager->SendJSONMessageToAllPlugins( msg_id, v );

        g_pActiveTrack->Stop( do_add_point );

        if( g_pActiveTrack->GetnPoints() < 2 ) {
            g_pRouteMan->DeleteRoute( g_pActiveTrack );
            return_val = NULL;
        }
        else {
            if( g_bTrackDaily ) {
                Track *pExtendTrack = g_pActiveTrack->DoExtendDaily();
                if(pExtendTrack) {
                    g_pRouteMan->DeleteRoute( g_pActiveTrack );
                    return_val = pExtendTrack;
                }
            }
        }
    }

    g_pActiveTrack = NULL;

    g_bTrackActive = false;

    if( pRouteManagerDialog && pRouteManagerDialog->IsShown() )
    {
        pRouteManagerDialog->UpdateTrkListCtrl();
        pRouteManagerDialog->UpdateRouteListCtrl();
    }

    SetToolbarItemState( ID_TRACK, g_bTrackActive );
    if( g_toolbar )
        g_toolbar->SetToolShortHelp( ID_TRACK, _("Enable Tracking") );
    SetMenubarItemState( ID_MENU_NAV_TRACK, g_bTrackActive );

    return return_val;
}

void MyFrame::TrackMidnightRestart( void )
{
    if( !g_pActiveTrack )
        return;

    Track *pPreviousTrack = TrackOff( true );
    TrackOn();

    //  Set the restarted track's current state such that the current track point's attributes match the
    //  attributes of the last point of the track that was just stopped at midnight.

    if( pPreviousTrack ) {
        RoutePoint *pMidnightPoint = pPreviousTrack->GetLastPoint();
        g_pActiveTrack->AdjustCurrentTrackPoint(pMidnightPoint);
    }

    if( pRouteManagerDialog && pRouteManagerDialog->IsShown() ) {
        pRouteManagerDialog->UpdateTrkListCtrl();
        pRouteManagerDialog->UpdateRouteListCtrl();
    }
}

void MyFrame::ToggleCourseUp( void )
{
    g_bCourseUp = !g_bCourseUp;

    if( g_bCourseUp ) {
        //    Stuff the COGAvg table in case COGUp is selected
        double stuff = 0.;
        if( !wxIsNaN(gCog) ) stuff = gCog;

        if( g_COGAvgSec > 0) {
            for( int i = 0; i < g_COGAvgSec; i++ )
                COGTable[i] = stuff;
        }
        g_COGAvg = stuff;
        gFrame->FrameCOGTimer.Start( 100, wxTIMER_CONTINUOUS );
    } else
        cc1->SetVPRotation(0); /* reset to north up */

    SetMenubarItemState( ID_MENU_CHART_COGUP, g_bCourseUp );
    SetMenubarItemState( ID_MENU_CHART_NORTHUP, !g_bCourseUp );

    if(m_pMenuBar)
        m_pMenuBar->SetLabel( ID_MENU_CHART_NORTHUP, _("North Up Mode") );
    
    DoCOGSet();
    UpdateGPSCompassStatusBox( true );
    DoChartUpdate();
    cc1->ReloadVP();
}

void MyFrame::ToggleENCText( void )
{
#ifdef USE_S57
    if( ps52plib ) {
        ps52plib->SetShowS57Text( !ps52plib->GetShowS57Text() );
        SetToolbarItemState( ID_ENC_TEXT, ps52plib->GetShowS57Text() );
        wxString tip = _("Show ENC Text (T)");
        if(ps52plib->GetShowS57Text())
            tip = _("Hide ENC Text (T)");
        if( g_toolbar )
            g_toolbar->SetToolShortHelp( ID_ENC_TEXT, tip );
        
        SetMenubarItemState( ID_MENU_ENC_TEXT, ps52plib->GetShowS57Text() );
        cc1->ReloadVP();
    }

#endif
}

void MyFrame::ToggleSoundings( void )
{
#ifdef USE_S57
    if( ps52plib ) {
        ps52plib->SetShowSoundings( !ps52plib->GetShowSoundings() );
        SetMenubarItemState( ID_MENU_ENC_SOUNDINGS, ps52plib->GetShowSoundings() );
        cc1->ReloadVP();
    }
#endif
}

bool MyFrame::ToggleLights( bool doToggle, bool temporary )
{
    bool oldstate = true;
    OBJLElement *pOLE = NULL;
    
#ifdef USE_S57
    if( ps52plib ) {
        for( unsigned int iPtr = 0; iPtr < ps52plib->pOBJLArray->GetCount(); iPtr++ ) {
            pOLE = (OBJLElement *) ( ps52plib->pOBJLArray->Item( iPtr ) );
            if( !strncmp( pOLE->OBJLName, "LIGHTS", 6 ) ) {
                oldstate = pOLE->nViz != 0;
                break;
            }
        }
    }

    oldstate &= !ps52plib->IsObjNoshow("LIGHTS");
    
    if( doToggle ){
        if(oldstate)                            // On, going off
            ps52plib->AddObjNoshow("LIGHTS");
        else{                                   // Off, going on
            if(pOLE)
                pOLE->nViz = 1;
            ps52plib->RemoveObjNoshow("LIGHTS");
        }
        
        SetMenubarItemState( ID_MENU_ENC_LIGHTS, !oldstate );
    }

    if( doToggle ) {
        if( ! temporary ) {
            ps52plib->GenerateStateHash();
            cc1->ReloadVP();
        }
    }
    

#endif
    return oldstate;
}

void MyFrame::ToggleRocks( void )
{
#ifdef USE_S57
    if( ps52plib ) {
        int vis =  0;
        // Need to loop once for UWTROC, which is our "master", then for
        // other categories, since order is unknown?
        for( unsigned int iPtr = 0; iPtr < ps52plib->pOBJLArray->GetCount(); iPtr++ ) {
            OBJLElement *pOLE = (OBJLElement *) ( ps52plib->pOBJLArray->Item( iPtr ) );
            if( !strncmp( pOLE->OBJLName, "UWTROC", 6 ) ) {
                pOLE->nViz = !pOLE->nViz;
                vis = pOLE->nViz;
            }
        }
        for( unsigned int iPtr = 0; iPtr < ps52plib->pOBJLArray->GetCount(); iPtr++ ) {
            OBJLElement *pOLE = (OBJLElement *) ( ps52plib->pOBJLArray->Item( iPtr ) );
            if( !strncmp( pOLE->OBJLName, "OBSTRN", 6 ) ) {
                pOLE->nViz = vis;
            }
            if( !strncmp( pOLE->OBJLName, "WRECKS", 6 ) ) {
                pOLE->nViz = vis;
            }
        }
        ps52plib->GenerateStateHash();
        cc1->ReloadVP();
    }
#endif
}

void MyFrame::ToggleAnchor( void )
{
#ifdef USE_S57
    if( ps52plib ) {
        int old_vis =  0;
        OBJLElement *pOLE = NULL;
        
        if(  MARINERS_STANDARD == ps52plib->GetDisplayCategory()){
            // Need to loop once for SBDARE, which is our "master", then for
            // other categories, since order is unknown?
            for( unsigned int iPtr = 0; iPtr < ps52plib->pOBJLArray->GetCount(); iPtr++ ) {
                OBJLElement *pOLE = (OBJLElement *) ( ps52plib->pOBJLArray->Item( iPtr ) );
                if( !strncmp( pOLE->OBJLName, "SBDARE", 6 ) ) {
                    old_vis = pOLE->nViz;
                    break;
                }
            }
        }
        else if(OTHER == ps52plib->GetDisplayCategory())
            old_vis = true;

        const char * categories[] = { "ACHBRT", "ACHARE", "CBLSUB", "PIPARE", "PIPSOL", "TUNNEL" };
        unsigned int num = sizeof(categories) / sizeof(categories[0]);
        
        old_vis &= !ps52plib->IsObjNoshow("SBDARE");
        
        if(old_vis){                            // On, going off
            ps52plib->AddObjNoshow("SBDARE");
            for( unsigned int c = 0; c < num; c++ ) {
                ps52plib->AddObjNoshow(categories[c]);
            }
        }    
        else{                                   // Off, going on
            if(pOLE)
                pOLE->nViz = 1;
            ps52plib->RemoveObjNoshow("SBDARE");
            for( unsigned int c = 0; c < num; c++ ) {
                ps52plib->RemoveObjNoshow(categories[c]);
            }
            
            unsigned int cnt = 0;
            for( unsigned int iPtr = 0; iPtr < ps52plib->pOBJLArray->GetCount(); iPtr++ ) {
                OBJLElement *pOLE = (OBJLElement *) ( ps52plib->pOBJLArray->Item( iPtr ) );
                for( unsigned int c = 0; c < num; c++ ) {
                    if( !strncmp( pOLE->OBJLName, categories[c], 6 ) ) {
                        pOLE->nViz = 1;         // force on
                        cnt++;
                        break;
                    }
                }
                if( cnt == num ) break;
            }
            
        }

        SetMenubarItemState( ID_MENU_ENC_ANCHOR, !old_vis );
        
        ps52plib->GenerateStateHash();
        cc1->ReloadVP();

    }
#endif
}

void MyFrame::TogglebFollow( void )
{
    if( !cc1->m_bFollow ) SetbFollow();
    else
        ClearbFollow();
}

void MyFrame::SetbFollow( void )
{
    cc1->m_bFollow = true;
    SetToolbarItemState( ID_FOLLOW, true );
    SetMenubarItemState( ID_MENU_NAV_FOLLOW, true );

    DoChartUpdate();
    cc1->ReloadVP();
}

void MyFrame::ClearbFollow( void )
{
    //    Center the screen on the GPS position, for lack of a better place
    vLat = gLat;
    vLon = gLon;

    cc1->m_bFollow = false;
    SetToolbarItemState( ID_FOLLOW, false );
    SetMenubarItemState( ID_MENU_NAV_FOLLOW, false );

    DoChartUpdate();
    cc1->ReloadVP();
}

void MyFrame::ToggleChartOutlines( void )
{
    if( !g_bShowOutlines ) g_bShowOutlines = true;
    else
        g_bShowOutlines = false;

    cc1->Refresh( false );

#ifdef ocpnUSE_GL         // opengl renders chart outlines as part of the chart this needs a full refresh
    if( g_bopengl )
        cc1->GetglCanvas()->Invalidate();
#endif

    SetMenubarItemState( ID_MENU_CHART_OUTLINES, g_bShowOutlines );
}

void MyFrame::SetMenubarItemState( int item_id, bool state )
{
    if( m_pMenuBar ) {
        bool enabled = m_pMenuBar->IsEnabled( item_id );
        m_pMenuBar->Enable( item_id, false );
        m_pMenuBar->Check( item_id, state );
        m_pMenuBar->Enable( item_id, enabled );
     }
}

void MyFrame::SetToolbarItemState( int tool_id, bool state )
{
    if( g_toolbar ) g_toolbar->ToggleTool( tool_id, state );
}

void MyFrame::SetToolbarItemBitmaps( int tool_id, wxBitmap *bmp, wxBitmap *bmpRollover )
{
    if( g_toolbar ) {
        g_toolbar->SetToolBitmaps( tool_id, bmp, bmpRollover );
        wxRect rect = g_toolbar->GetToolRect( tool_id );
        g_toolbar->RefreshRect( rect );
    }
}

void MyFrame::ApplyGlobalSettings( bool bFlyingUpdate, bool bnewtoolbar )
{
    //             ShowDebugWindow as a wxStatusBar
    m_StatusBarFieldCount = 5;

#ifdef __WXMSW__
    UseNativeStatusBar( false );              // better for MSW, undocumented in frame.cpp
#endif

    if( pConfig->m_bShowStatusBar ) {
        if( !m_pStatusBar ) {
            m_pStatusBar = CreateStatusBar( m_StatusBarFieldCount, 0 );   // No wxST_SIZEGRIP needed
            ApplyGlobalColorSchemetoStatusBar();
        }

    } else {
        if( m_pStatusBar ) {
            m_pStatusBar->Destroy();
            m_pStatusBar = NULL;
            SetStatusBar( NULL );
        }
    }

    SendSizeEvent();               
    
    /*
     * Menu Bar - add or remove it if necessary, and update the state of the menu items
     */
#ifdef __WXOSX__
    bool showMenuBar = true;    // the menu bar is always visible in OS X
#else
    bool showMenuBar = pConfig->m_bShowMenuBar; // get visibility from options

    if (!showMenuBar && g_bTempShowMenuBar)     // allows pressing alt to temporarily show
        showMenuBar = true;
#endif

    if ( showMenuBar ) {
        if ( !m_pMenuBar ) {    // add the menu bar if it is enabled
            m_pMenuBar = new wxMenuBar();
            RegisterGlobalMenuItems();
            SetMenuBar(m_pMenuBar); // must be after RegisterGlobalMenuItems for wx to populate the OS X App Menu correctly
        }
        UpdateGlobalMenuItems(); // update the state of the menu items (checkmarks etc)
    } else {
        if ( m_pMenuBar ) {     // remove the menu bar if it is disabled
            SetMenuBar( NULL );
            m_pMenuBar->Destroy();
            m_pMenuBar = NULL;
        }
    }

    SendSizeEvent();               
    
    if( bFlyingUpdate ) {
        if( pConfig->m_bShowCompassWin ) {
            if(!g_FloatingCompassDialog) {
                g_FloatingCompassDialog = new ocpnFloatingCompassWindow( cc1 );
                if( g_FloatingCompassDialog ) g_FloatingCompassDialog->UpdateStatus( true );
            }
        } else if(g_FloatingCompassDialog) {
            g_FloatingCompassDialog->Destroy();
            g_FloatingCompassDialog = NULL;
        }
    }

    if( bnewtoolbar ) UpdateToolbar( global_color_scheme );

}


wxString _menuText( wxString name, wxString shortcut ) {
    wxString menutext;
    menutext << name << _T("\t") << shortcut;
    return menutext;
}

void MyFrame::RegisterGlobalMenuItems()
{
    if ( !m_pMenuBar ) return;  // if there isn't a menu bar

    wxMenu *nav_menu = new wxMenu();
    nav_menu->AppendCheckItem( ID_MENU_NAV_FOLLOW, _menuText(_("Auto Follow"), _T("Ctrl-A")) );
    nav_menu->AppendCheckItem( ID_MENU_NAV_TRACK, _("Enable Tracking") );
    nav_menu->AppendSeparator();
    nav_menu->AppendRadioItem( ID_MENU_CHART_NORTHUP, _("North Up Mode") );
    nav_menu->AppendRadioItem( ID_MENU_CHART_COGUP, _("Course Up Mode") );
    nav_menu->AppendSeparator();
    nav_menu->Append( ID_MENU_ZOOM_IN, _menuText(_("Zoom In"), _T("+")) );
    nav_menu->Append( ID_MENU_ZOOM_OUT, _menuText(_("Zoom Out"), _T("-")) );
    nav_menu->AppendSeparator();
    nav_menu->Append( ID_MENU_SCALE_IN, _menuText(_("Larger Scale Chart"), _T("Ctrl-Left")) );
    nav_menu->Append( ID_MENU_SCALE_OUT, _menuText(_("Smaller Scale Chart"), _T("Ctrl-Right")) );
#ifndef __WXOSX__
    nav_menu->AppendSeparator();
    nav_menu->Append( ID_MENU_OQUIT, _menuText(_("Exit OpenCPN"), _T("Ctrl-Q")) );
#endif    
    m_pMenuBar->Append( nav_menu, _("&Navigate") );


    wxMenu* view_menu = new wxMenu();
    view_menu->AppendCheckItem( ID_MENU_CHART_QUILTING, _menuText(_("Enable Chart Quilting"), _T("Q")) );
    view_menu->AppendCheckItem( ID_MENU_CHART_OUTLINES, _menuText(_("Show Chart Outlines"), _T("O")) );
    view_menu->AppendCheckItem( ID_MENU_UI_CHARTBAR, _menuText(_("Show Chart Bar"), _T("Ctrl-B")) );
#ifdef USE_S57
    view_menu->AppendSeparator();
    view_menu->AppendCheckItem( ID_MENU_ENC_TEXT, _menuText(_("Show ENC Text"), _T("T")) );
    view_menu->AppendCheckItem( ID_MENU_ENC_LIGHTS, _menuText(_("Show ENC Lights"), _T("L")) );
    view_menu->AppendCheckItem( ID_MENU_ENC_SOUNDINGS, _menuText(_("Show ENC Soundings"), _T("S")) );
    view_menu->AppendCheckItem( ID_MENU_ENC_ANCHOR, _menuText(_("Show ENC Anchoring Info"), _T("A")) );
#endif
    view_menu->AppendSeparator();
    view_menu->AppendCheckItem( ID_MENU_SHOW_TIDES, _("Show Tides") );
    view_menu->AppendCheckItem( ID_MENU_SHOW_CURRENTS, _("Show Currents") );
    view_menu->AppendSeparator();
    view_menu->Append( ID_MENU_UI_COLSCHEME, _menuText(_("Change Color Scheme"), _T("C")) );
    view_menu->AppendSeparator();
#ifdef __WXOSX__
    view_menu->Append(ID_MENU_UI_FULLSCREEN, _menuText(_("Enter Full Screen"), _T("RawCtrl-Ctrl-F")) );
#else
    view_menu->Append(ID_MENU_UI_FULLSCREEN, _menuText(_("Enter Full Screen"), _T("F11")) );
#endif
    m_pMenuBar->Append( view_menu, _("&View") );


    wxMenu* ais_menu = new wxMenu();
    ais_menu->AppendCheckItem( ID_MENU_AIS_TARGETS, _("Show AIS Targets") );
    ais_menu->AppendCheckItem( ID_MENU_AIS_TRACKS, _("Show Target Tracks") );
    ais_menu->AppendCheckItem( ID_MENU_AIS_CPADIALOG, _("Show CPA Alert Dialogs") );
    ais_menu->AppendCheckItem( ID_MENU_AIS_CPASOUND, _("Sound CPA Alarms") );
    ais_menu->AppendSeparator();
    ais_menu->Append( ID_MENU_AIS_TARGETLIST, _("AIS Target List...") );
    m_pMenuBar->Append( ais_menu, _("&AIS") );


    wxMenu* tools_menu = new wxMenu();
    tools_menu->Append( ID_MENU_TOOL_MEASURE, _menuText(_("Measure Distance"), _T("M")) );
    tools_menu->AppendSeparator();
    tools_menu->Append( ID_MENU_ROUTE_MANAGER, _("Route && Mark Manager...") );
    tools_menu->Append( ID_MENU_ROUTE_NEW, _menuText(_("Create Route"), _T("Ctrl-R")) );
    tools_menu->AppendSeparator();
    tools_menu->Append( ID_MENU_MARK_BOAT, _menuText(_("Drop Mark at Boat"), _T("Ctrl-O")) );
    tools_menu->Append( ID_MENU_MARK_CURSOR, _menuText(_("Drop Mark at Cursor"), _T("Ctrl-M")) );
    tools_menu->AppendSeparator();
#ifdef __WXOSX__
    tools_menu->Append( ID_MENU_MARK_MOB, _menuText(_("Drop MOB Marker"), _T("RawCtrl-Space")) ); // NOTE Cmd+Space is reserved for Spotlight
    tools_menu->AppendSeparator();
    tools_menu->Append( wxID_PREFERENCES, _menuText(_("Preferences..."), _T("Ctrl-,")) );
#else
    tools_menu->Append( ID_MENU_MARK_MOB, _menuText(_("Drop MOB Marker"), _T("Ctrl-Space")) );
    tools_menu->AppendSeparator();
    tools_menu->Append( wxID_PREFERENCES, _menuText(_("Options..."), _T("Ctrl-,")) );
#endif

    m_pMenuBar->Append( tools_menu, _("&Tools") );
    wxMenu* help_menu = new wxMenu();
    help_menu->Append( wxID_ABOUT, _("About OpenCPN") );
    help_menu->Append( wxID_HELP, _("OpenCPN Help") );
    m_pMenuBar->Append( help_menu, _("&Help") );


    // Set initial values for menu check items and radio items
    UpdateGlobalMenuItems();
}

void MyFrame::UpdateGlobalMenuItems()
{
    if ( !m_pMenuBar ) return;  // if there isn't a menu bar

    if ( pConfig ) m_pMenuBar->FindItem( ID_MENU_NAV_FOLLOW )->Check( pConfig->st_bFollow );
    m_pMenuBar->FindItem( ID_MENU_CHART_NORTHUP )->Check( !g_bCourseUp );
    m_pMenuBar->FindItem( ID_MENU_CHART_COGUP )->Check( g_bCourseUp );
    m_pMenuBar->FindItem( ID_MENU_NAV_TRACK )->Check( g_bTrackActive );
    m_pMenuBar->FindItem( ID_MENU_CHART_OUTLINES )->Check( g_bShowOutlines );
    m_pMenuBar->FindItem( ID_MENU_CHART_QUILTING )->Check( g_bQuiltEnable );
    m_pMenuBar->FindItem( ID_MENU_UI_CHARTBAR )->Check( g_bShowChartBar );
    m_pMenuBar->FindItem( ID_MENU_AIS_TARGETS )->Check( g_pAIS->ShowAIS() );
    m_pMenuBar->FindItem( ID_MENU_AIS_TRACKS )->Check( g_pAIS->ShowTracks() );
    m_pMenuBar->FindItem( ID_MENU_AIS_CPADIALOG )->Check( g_pAIS->CPAAlert() );
    m_pMenuBar->FindItem( ID_MENU_AIS_CPASOUND )->Check( g_pAIS->CPAAlertAudio() );
#ifdef USE_S57
    if( ps52plib ) {
        m_pMenuBar->FindItem( ID_MENU_ENC_TEXT )->Check( ps52plib->GetShowS57Text() );
        m_pMenuBar->FindItem( ID_MENU_ENC_SOUNDINGS )->Check( ps52plib->GetShowSoundings() );

        bool light_state = false;
        if( ps52plib ) {
            for( unsigned int iPtr = 0; iPtr < ps52plib->pOBJLArray->GetCount(); iPtr++ ) {
                OBJLElement *pOLE = (OBJLElement *) ( ps52plib->pOBJLArray->Item( iPtr ) );
                if( !strncmp( pOLE->OBJLName, "LIGHTS", 6 ) ) {
                    light_state = (pOLE->nViz == 1);
                    break;
                }
            }
        }
        m_pMenuBar->FindItem( ID_MENU_ENC_LIGHTS )->Check( (!ps52plib->IsObjNoshow("LIGHTS")) && light_state );
        
        m_pMenuBar->FindItem( ID_MENU_ENC_ANCHOR )->Check( !ps52plib->IsObjNoshow("SBDARE") );
    }
#endif
}

void MyFrame::SubmergeToolbarIfOverlap( int x, int y, int margin )
{
    if( g_FloatingToolbarDialog ) {
        wxRect rect = g_FloatingToolbarDialog->GetScreenRect();
        rect.Inflate( margin );
        if( rect.Contains( x, y ) ) g_FloatingToolbarDialog->Submerge();
    }
}

void MyFrame::SubmergeToolbar( void )
{
    if( g_FloatingToolbarDialog ) g_FloatingToolbarDialog->Submerge();
}

void MyFrame::SurfaceToolbar( void )
{
    if( g_FloatingToolbarDialog && g_FloatingToolbarDialog->IsToolbarShown() ) {
        if( IsFullScreen() ) {
            if( g_bFullscreenToolbar ) {
                g_FloatingToolbarDialog->Surface();
            }
        } else
            g_FloatingToolbarDialog->Surface();
    }
    gFrame->Raise();
}

void MyFrame::JumpToPosition( double lat, double lon, double scale )
{
    vLat = lat;
    vLon = lon;
    cc1->m_bFollow = false;

    //  is the current chart available at the target location?
    int currently_selected_index = pCurrentStack->GetCurrentEntrydbIndex();

    //  If not, then select the smallest scale chart at the target location (may be empty)
    ChartData->BuildChartStack( pCurrentStack, lat, lon );
    if(!pCurrentStack->DoesStackContaindbIndex(currently_selected_index)){
        pCurrentStack->CurrentStackEntry = pCurrentStack->nEntry - 1;
        int selected_index = pCurrentStack->GetCurrentEntrydbIndex();
        if( cc1->GetQuiltMode() )
            cc1->SetQuiltRefChart( selected_index );
    }

    if( !cc1->GetQuiltMode() ) {
        cc1->SetViewPoint( lat, lon, scale, Current_Ch->GetChartSkew() * PI / 180., cc1->GetVPRotation() );
    } else {
        cc1->SetViewPoint( lat, lon, scale, 0, cc1->GetVPRotation() );
    }

    cc1->ReloadVP();

    SetToolbarItemState( ID_FOLLOW, false );

    if( g_pi_manager ) {
        g_pi_manager->SendViewPortToRequestingPlugIns( cc1->GetVP() );
    }
}

int MyFrame::DoOptionsDialog()
{
    g_boptionsactive = true;

    ::wxBeginBusyCursor();
    g_options = new options( this, -1, _("Options") );
    ::wxEndBusyCursor();

//    Set initial Chart Dir
    g_options->SetInitChartDir( *pInit_Chart_Dir );

//      Pass two working pointers for Chart Dir Dialog
    g_options->SetCurrentDirList( ChartData->GetChartDirArray() );
    ArrayOfCDI *pWorkDirArray = new ArrayOfCDI;
    g_options->SetWorkDirListPtr( pWorkDirArray );

//      Pass a ptr to MyConfig, for updates
    g_options->SetConfigPtr( pConfig );

    g_options->SetInitialSettings();

    bDBUpdateInProgress = true;

    bPrevQuilt = g_bQuiltEnable;
    bPrevFullScreenQuilt = g_bFullScreenQuilt;
    bPrevOGL = g_bopengl;

    prev_locale = g_locale;

    bool b_sub = false;
    if( g_FloatingToolbarDialog && g_FloatingToolbarDialog->IsShown() ) {
        wxRect bx_rect = g_options->GetScreenRect();
        wxRect tb_rect = g_FloatingToolbarDialog->GetScreenRect();
        if( tb_rect.Intersects( bx_rect ) ) b_sub = true;

        if( b_sub ) g_FloatingToolbarDialog->Submerge();
    }

#if defined(__WXOSX__) || defined(__WXQT__)
    if(stats) stats->Hide();
#endif

    g_options->SetInitialPage(options_lastPage );

    if(!g_bresponsive){
        g_options->lastWindowPos = options_lastWindowPos;
        if( options_lastWindowPos != wxPoint(0,0) ) {
            g_options->Move( options_lastWindowPos );
            g_options->SetSize( options_lastWindowSize );
        } else {
            g_options->Center();
        }
    }
    else {

        wxSize canvas_size = cc1->GetSize();
        wxPoint canvas_pos = cc1->GetPosition();
        wxSize fitted_size = g_options->GetSize();;

        fitted_size.x = wxMin(fitted_size.x, canvas_size.x);
        fitted_size.y = wxMin(fitted_size.y, canvas_size.y);

        g_options->SetSize( fitted_size );
        int xp = (canvas_size.x - fitted_size.x)/2;
        int yp = (canvas_size.y - fitted_size.y)/2;

        wxPoint xxp = ClientToScreen(canvas_pos);
        g_options->Move(xxp.x + xp, xxp.y + yp);

    }

    if( g_FloatingToolbarDialog)
        g_FloatingToolbarDialog->DisableTooltips();

    int rr = g_options->ShowModal();

    if( g_FloatingToolbarDialog)
        g_FloatingToolbarDialog->EnableTooltips();

    options_lastPage = g_options->lastPage;
    options_lastWindowPos = g_options->lastWindowPos;
    options_lastWindowSize = g_options->lastWindowSize;

    if( b_sub ) {
        SurfaceToolbar();
        cc1->SetFocus();
    }

#ifdef __WXGTK__
    Raise();                      // I dunno why...
#endif

    bool ret_val = false;
    rr = g_options->GetReturnCode();
    if( rr ) {
        ProcessOptionsDialog( rr, g_options );
        ret_val = true;
    }

    delete pWorkDirArray;

    SetToolbarScale();
    RequestNewToolbar();

    bDBUpdateInProgress = false;
    if( g_FloatingToolbarDialog ) {
        if( IsFullScreen() && !g_bFullscreenToolbar )
            g_FloatingToolbarDialog->Submerge();
    }

#if defined(__WXOSX__) || defined(__WXQT__)
    if(stats)
        stats->Show();
#endif

    Refresh( false );

    g_boptionsactive = false;

    delete g_options;
    g_options = NULL;

    return ret_val;
}

double  GetDisplaySizeMM();

int MyFrame::ProcessOptionsDialog( int rr, options* dialog )
{
    //    Capture the name of the currently open chart
    wxString chart_file_name;
    if( cc1->GetQuiltMode() ) {
        int dbi = cc1->GetQuiltRefChartdbIndex();
        chart_file_name = ChartData->GetDBChartFileName( dbi );
    } else
        if( Current_Ch )
            chart_file_name = Current_Ch->GetFullPath();

    ArrayOfCDI *pWorkDirArray = dialog->GetWorkDirListPtr();

    if( ( rr & VISIT_CHARTS )
            && ( ( rr & CHANGE_CHARTS ) || ( rr & FORCE_UPDATE ) || ( rr & SCAN_UPDATE ) ) ) {

       UpdateChartDatabaseInplace( *pWorkDirArray, ( ( rr & FORCE_UPDATE ) == FORCE_UPDATE ),
                true, *pChartListFileName );

        //    Re-open the last open chart
        int dbii = ChartData->FinddbIndex( chart_file_name );
        ChartsRefresh( dbii, cc1->GetVP() );
    }

    if( ( rr & LOCALE_CHANGED ) || ( rr & STYLE_CHANGED ) ) {
        if( ( prev_locale != g_locale ) || ( rr & STYLE_CHANGED ) ) {
            OCPNMessageBox(NULL, _("Please restart OpenCPN to activate language or style changes."),
                    _("OpenCPN Info"), wxOK | wxICON_INFORMATION );
            if( rr & LOCALE_CHANGED ) g_blocale_changed = true;;
        }
    }

    bool b_groupchange = false;
    if( ( ( rr & VISIT_CHARTS )
            && ( ( rr & CHANGE_CHARTS ) || ( rr & FORCE_UPDATE ) || ( rr & SCAN_UPDATE ) ) )
            || ( rr & GROUPS_CHANGED ) ) {
        b_groupchange = ScrubGroupArray();
        ChartData->ApplyGroupArray( g_pGroupArray );
        SetGroupIndex( g_GroupIndex );
    }

    if( rr & GROUPS_CHANGED || b_groupchange) {
        pConfig->DestroyConfigGroups();
        pConfig->CreateConfigGroups( g_pGroupArray );
    }

    if( rr & TIDES_CHANGED ) {
        LoadHarmonics();
    }

    pConfig->UpdateSettings();

    if( g_pActiveTrack ) {
        g_pActiveTrack->SetPrecision( g_nTrackPrecision );
    }

    if( ( bPrevQuilt != g_bQuiltEnable ) || ( bPrevFullScreenQuilt != g_bFullScreenQuilt ) ) {
        cc1->SetQuiltMode( g_bQuiltEnable );
        SetupQuiltMode();
    }

    if( g_bCourseUp ) {
        //    Stuff the COGAvg table in case COGUp is selected
        double stuff = 0.;
        if( !wxIsNaN(gCog) ) stuff = gCog;
        if( g_COGAvgSec > 0 ) {
            for( int i = 0; i < g_COGAvgSec; i++ )
                COGTable[i] = stuff;
        }

        g_COGAvg = stuff;

        DoCOGSet();
    }

    //    Stuff the Filter tables
    double stuffcog = 0.;
    double stuffsog = 0.;
    if( !wxIsNaN(gCog) ) stuffcog = gCog;
    if( !wxIsNaN(gSog) ) stuffsog = gSog;

    for( int i = 0; i < MAX_COGSOG_FILTER_SECONDS; i++ ) {
        COGFilterTable[i] = stuffcog;
        SOGFilterTable[i] = stuffsog;
    }
    m_COGFilterLast = stuffcog;

    if(cc1)
        SetChartUpdatePeriod( cc1->GetVP() );              // Pick up changes to skew compensator

     if(rr & GL_CHANGED){
        //    Refresh the chart display, after flushing cache.
        //      This will allow all charts to recognise new OpenGL configuration, if any
        int dbii = ChartData->FinddbIndex( chart_file_name );
        ChartsRefresh( dbii, cc1->GetVP(), true );
    }
    
    if(g_config_display_size_mm > 0){
        g_display_size_mm = g_config_display_size_mm;
    }
    else{
        g_display_size_mm = wxMax(100, GetDisplaySizeMM());
    }
        
    cc1->SetDisplaySizeMM( g_display_size_mm );
    
    if(stats){
        stats->Show(g_bShowChartBar);
        if(g_bShowChartBar){
            stats->Move(0,0);
            stats->RePosition();
            gFrame->Raise();
            DoChartUpdate();
            UpdateControlBar();
            Refresh();
        }
    }
        
    return 0;
}

void MyFrame::LaunchLocalHelp( void ) {
#if wxUSE_XLOCALE || !wxCHECK_VERSION(3,0,0)
    
    wxString def_lang_canonical = wxLocale::GetLanguageInfo( wxLANGUAGE_DEFAULT )->CanonicalName;

    wxString help_locn = g_SData_Locn + _T("doc/help_");

    wxString help_try = help_locn + def_lang_canonical + _T(".html");

    if( ! ::wxFileExists( help_try ) ) {
        help_try = help_locn + _T("en_US") + _T(".html");

        if( ! ::wxFileExists( help_try ) ) {
            help_try = help_locn + _T("web") + _T(".html");
        }

        if( ! ::wxFileExists( help_try ) ) return;
    }

    wxLaunchDefaultBrowser(wxString( _T("file:///") ) + help_try );
#endif    
}


wxString MyFrame::GetGroupName( int igroup )
{
    ChartGroup *pGroup = g_pGroupArray->Item( igroup - 1 );
    return pGroup->m_group_name;
}

bool MyFrame::CheckGroup( int igroup )
{
    if( igroup == 0 ) return true;              // "all charts" is always OK

    ChartGroup *pGroup = g_pGroupArray->Item( igroup - 1 );

    if( !pGroup->m_element_array.GetCount() )   //  truly empty group is OK
        return true;

    bool b_chart_in_group = false;

    for( unsigned int j = 0; j < pGroup->m_element_array.GetCount(); j++ ) {
        wxString element_root = pGroup->m_element_array.Item( j )->m_element_name;

        for( unsigned int ic = 0; ic < (unsigned int) ChartData->GetChartTableEntries(); ic++ ) {
            ChartTableEntry *pcte = ChartData->GetpChartTableEntry( ic );
            wxString chart_full_path( pcte->GetpFullPath(), wxConvUTF8 );

            if( chart_full_path.StartsWith( element_root ) ) {
                b_chart_in_group = true;
                break;
            }
        }

        if( b_chart_in_group ) break;
    }

    return b_chart_in_group;                           // this group is empty

}

bool MyFrame::ScrubGroupArray()
{
    //    For each group,
    //    make sure that each group element (dir or chart) references at least oneitem in the database.
    //    If not, remove the element.

    bool b_change = false;
    unsigned int igroup = 0;
    while( igroup < g_pGroupArray->GetCount() ) {
        bool b_chart_in_element = false;
        ChartGroup *pGroup = g_pGroupArray->Item( igroup );

        for( unsigned int j = 0; j < pGroup->m_element_array.GetCount(); j++ ) {
            wxString element_root = pGroup->m_element_array.Item( j )->m_element_name;

            for( unsigned int ic = 0; ic < (unsigned int) ChartData->GetChartTableEntries();
                    ic++ ) {
                ChartTableEntry *pcte = ChartData->GetpChartTableEntry( ic );
                wxString chart_full_path( pcte->GetpFullPath(), wxConvUTF8 );

                if( chart_full_path.StartsWith( element_root ) ) {
                    b_chart_in_element = true;
                    break;
                }
            }

            if( !b_chart_in_element )             // delete the element
            {
                ChartGroupElement *pelement = pGroup->m_element_array.Item( j );
                pGroup->m_element_array.RemoveAt( j );
                delete pelement;
                b_change = true;
            }
        }

        igroup++;                                 // next group
    }

    return b_change;
}

// Flav: This method reloads all charts for convenience
void MyFrame::ChartsRefresh( int dbi_hint, ViewPort &vp, bool b_purge )
{
    if( !ChartData ) return;

    ::wxBeginBusyCursor();

    bool b_run = FrameTimer1.IsRunning();

    FrameTimer1.Stop();                  // stop other asynchronous activity

    cc1->InvalidateQuilt();
    cc1->SetQuiltRefChart( -1 );

    Current_Ch = NULL;

    delete pCurrentStack;
    pCurrentStack = NULL;

    if( b_purge ) ChartData->PurgeCache();

    //    Build a new ChartStack
    pCurrentStack = new ChartStack;
    ChartData->BuildChartStack( pCurrentStack, vLat, vLon );

    if( -1 != dbi_hint ) {
        if( cc1->GetQuiltMode() ) {
            pCurrentStack->SetCurrentEntryFromdbIndex( dbi_hint );
            cc1->SetQuiltRefChart( dbi_hint );
        } else {
            //      Open the saved chart
            ChartBase *pTentative_Chart;
            pTentative_Chart = ChartData->OpenChartFromDB( dbi_hint, FULL_INIT );

            if( pTentative_Chart ) {
                /* Current_Ch is always NULL here, (set above) should this go before that? */
                if( Current_Ch ) Current_Ch->Deactivate();

                Current_Ch = pTentative_Chart;
                Current_Ch->Activate();

                pCurrentStack->CurrentStackEntry = ChartData->GetStackEntry( pCurrentStack,
                        Current_Ch->GetFullPath() );
            } else
                SetChartThumbnail( dbi_hint );       // need to reset thumbnail on failed chart open
        }

        //          Refresh the Piano Bar
        if( stats ) {
            ArrayOfInts piano_active_chart_index_array;
            piano_active_chart_index_array.Add( pCurrentStack->GetCurrentEntrydbIndex() );
            stats->pPiano->SetActiveKeyArray( piano_active_chart_index_array );

            stats->Refresh( true );
        }

    } else {
        //    Select reference chart from the stack, as though clicked by user
        //    Make it the smallest scale chart on the stack
        pCurrentStack->CurrentStackEntry = pCurrentStack->nEntry - 1;
        int selected_index = pCurrentStack->GetCurrentEntrydbIndex();
        cc1->SetQuiltRefChart( selected_index );
    }

    //    Validate the correct single chart, or set the quilt mode as appropriate
    SetupQuiltMode();

    cc1->ReloadVP();

    UpdateControlBar();

    UpdateGPSCompassStatusBox( );

    cc1->SetCursor( wxCURSOR_ARROW );

    if( b_run ) FrameTimer1.Start( TIMER_GFRAME_1, wxTIMER_CONTINUOUS );

    ::wxEndBusyCursor();

}

bool MyFrame::UpdateChartDatabaseInplace( ArrayOfCDI &DirArray, bool b_force, bool b_prog,
        const wxString &ChartListFileName )
{
    bool b_run = FrameTimer1.IsRunning();
    FrameTimer1.Stop();                  // stop other asynchronous activity

    cc1->InvalidateQuilt();
    cc1->SetQuiltRefChart( -1 );
    ChartData->PurgeCache();

    Current_Ch = NULL;

    delete pCurrentStack;
    pCurrentStack = NULL;

    ::wxBeginBusyCursor();

    wxProgressDialog *pprog = NULL;
    if( b_prog ) {
        wxString longmsg = _("OpenCPN Chart Update");
        longmsg += _T("..........................................................................");
        pprog = new wxProgressDialog( _("OpenCPN Chart Update"), longmsg,
                100, this,
                wxPD_SMOOTH | wxPD_ELAPSED_TIME | wxPD_ESTIMATED_TIME | wxPD_REMAINING_TIME );
    }


    wxLogMessage( _T("   ") );
    wxLogMessage( _T("Starting chart database Update...") );
    ChartData->Update( DirArray, b_force, pprog );
    ChartData->SaveBinary(ChartListFileName);
    wxLogMessage( _T("Finished chart database Update") );
    wxLogMessage( _T("   ") );

    delete pprog;

    ::wxEndBusyCursor();

    pConfig->UpdateChartDirs( DirArray );

    if( b_run ) FrameTimer1.Start( TIMER_GFRAME_1, wxTIMER_CONTINUOUS );

    return true;
}

void MyFrame::ToggleQuiltMode( void )
{
    if( cc1 ) {
        bool cur_mode = cc1->GetQuiltMode();

        if( !cc1->GetQuiltMode() )
            cc1->SetQuiltMode( true );
        else
            if( cc1->GetQuiltMode() ) {
                cc1->SetQuiltMode( false );
                g_sticky_chart = cc1->GetQuiltReferenceChartIndex();
            }


        if( cur_mode != cc1->GetQuiltMode() ){
            SetupQuiltMode();
            DoChartUpdate();
            Refresh();
        }
    }
    g_bQuiltEnable = cc1->GetQuiltMode();
}

void MyFrame::SetQuiltMode( bool bquilt )
{
    if( cc1 ) cc1->SetQuiltMode( bquilt );
}

bool MyFrame::GetQuiltMode( void )
{
    if( cc1 ) return cc1->GetQuiltMode();
    else
        return false;
}

void MyFrame::SetupQuiltMode( void )
{

    if( cc1->GetQuiltMode() )                               // going to quilt mode
    {
        ChartData->LockCache();

        stats->pPiano->SetNoshowIndexArray( g_quilt_noshow_index_array );

        ocpnStyle::Style* style = g_StyleManager->GetCurrentStyle();

        stats->pPiano->SetVizIcon( new wxBitmap( style->GetIcon( _T("viz") ) ) );
        stats->pPiano->SetInVizIcon( new wxBitmap( style->GetIcon( _T("redX") ) ) );
        stats->pPiano->SetTMercIcon( new wxBitmap( style->GetIcon( _T("tmercprj") ) ) );
        stats->pPiano->SetSkewIcon( new wxBitmap( style->GetIcon( _T("skewprj") ) ) );

        stats->pPiano->SetRoundedRectangles( true );

        //    Select the proper Ref chart
        int target_new_dbindex = -1;
        if( pCurrentStack ) {
            target_new_dbindex = pCurrentStack->GetCurrentEntrydbIndex();

            if(-1 != target_new_dbindex){
                if( !cc1->IsChartQuiltableRef( target_new_dbindex ) ){
                    
                    int proj = ChartData->GetDBChartProj(target_new_dbindex);
                    int type = ChartData->GetDBChartType(target_new_dbindex);

                // walk the stack up looking for a satisfactory chart
                    int stack_index = pCurrentStack->CurrentStackEntry;

                    while((stack_index < pCurrentStack->nEntry-1) && (stack_index >= 0)) {
                        int proj_tent = ChartData->GetDBChartProj( pCurrentStack->GetDBIndex(stack_index));
                        int type_tent = ChartData->GetDBChartType( pCurrentStack->GetDBIndex(stack_index));

                        if(cc1->IsChartQuiltableRef(pCurrentStack->GetDBIndex(stack_index))){
                            if((proj == proj_tent) && (type_tent == type)){
                                target_new_dbindex = pCurrentStack->GetDBIndex(stack_index);
                                break;
                            }
                        }
                        stack_index++;
                    }
                }
            }
        }

        if( cc1->IsChartQuiltableRef( target_new_dbindex ) )
            SelectQuiltRefdbChart( target_new_dbindex, false );        // Try not to allow a scale change
        else
            SelectQuiltRefdbChart( -1 );

        Current_Ch = NULL;                  // Bye....

        SetChartThumbnail( -1 );            //Turn off thumbnails for sure

        //  Re-qualify the quilt reference chart selection
        cc1->AdjustQuiltRefChart(  );

    } else                                                  // going to SC Mode
    {
        ArrayOfInts empty_array;
        stats->pPiano->SetActiveKeyArray( empty_array );
        stats->pPiano->SetNoshowIndexArray( empty_array );
        stats->pPiano->SetSubliteIndexArray( empty_array );
        stats->pPiano->SetVizIcon( NULL );
        stats->pPiano->SetInVizIcon( NULL );

        ocpnStyle::Style* style = g_StyleManager->GetCurrentStyle();

        stats->pPiano->SetTMercIcon( new wxBitmap( style->GetIcon( _T("tmercprj") ) ) );
        stats->pPiano->SetSkewIcon( new wxBitmap( style->GetIcon( _T("skewprj") ) ) );

        stats->pPiano->SetRoundedRectangles( false );

    }

    //    When shifting from quilt to single chart mode, select the "best" single chart to show
    if( !cc1->GetQuiltMode() ) {
        if( ChartData && ChartData->IsValid() ) {
            ChartData->UnLockCache();
            ChartData->UnLockAllCacheCharts();

            double tLat, tLon;
            if( cc1->m_bFollow == true ) {
                tLat = gLat;
                tLon = gLon;
            } else {
                tLat = vLat;
                tLon = vLon;
            }

            if( !Current_Ch ) {

                // Build a temporary chart stack based on tLat, tLon
                ChartStack TempStack;
                ChartData->BuildChartStack( &TempStack, tLat, tLon, g_sticky_chart );

                //    Iterate over the quilt charts actually shown, looking for the largest scale chart that will be in the new chartstack....
                //    This will (almost?) always be the reference chart....

                ChartBase *Candidate_Chart = NULL;
                int cur_max_scale = (int) 1e8;

                ChartBase *pChart = cc1->GetFirstQuiltChart();
                while( pChart ) {
                    //  Is this pChart in new stack?
                    int tEntry = ChartData->GetStackEntry( &TempStack, pChart->GetFullPath() );
                    if( tEntry != -1 ) {
                        if( pChart->GetNativeScale() < cur_max_scale ) {
                            Candidate_Chart = pChart;
                            cur_max_scale = pChart->GetNativeScale();
                        }
                    }
                    pChart = cc1->GetNextQuiltChart();
                }

                Current_Ch = Candidate_Chart;

                //    If the quilt is empty, there is no "best" chart.
                //    So, open the smallest scale chart in the current stack
                if( NULL == Current_Ch ) {
                    Current_Ch = ChartData->OpenStackChartConditional( &TempStack,
                            TempStack.nEntry - 1, true, CHART_TYPE_DONTCARE,
                            CHART_FAMILY_DONTCARE );
                }
            }

            //  Invalidate all the charts in the quilt,
            // as any cached data may be region based and not have fullscreen coverage
            cc1->InvalidateAllQuiltPatchs();

            if( Current_Ch ) {
                int dbi = ChartData->FinddbIndex( Current_Ch->GetFullPath() );
                ArrayOfInts one_array;
                one_array.Add( dbi );
                stats->pPiano->SetActiveKeyArray( one_array );
            }

        }
        //    Invalidate the current stack so that it will be rebuilt on next tick
        if( pCurrentStack ) pCurrentStack->b_valid = false;
    }

}

void MyFrame::ClearRouteTool()
{
    if( g_toolbar ) g_toolbar->ToggleTool( ID_ROUTE, false );
}

void MyFrame::DoStackDown( void )
{
    DoStackDelta( -1 );
}

void MyFrame::DoStackUp( void )
{
    DoStackDelta( 1 );
}

void MyFrame::DoStackDelta( int direction )
{
    if( !cc1->GetQuiltMode() ) {
        int current_stack_index = pCurrentStack->CurrentStackEntry;
        if( (current_stack_index + direction) >= pCurrentStack->nEntry )
            return;
        if( (current_stack_index + direction) < 0 )
            return;

        if( m_bpersistent_quilt /*&& g_bQuiltEnable*/ ) {
            int new_dbIndex = pCurrentStack->GetDBIndex(current_stack_index + direction );

            if( cc1->IsChartQuiltableRef( new_dbIndex ) ) {
                ToggleQuiltMode();
                SelectQuiltRefdbChart( new_dbIndex );
                m_bpersistent_quilt = false;
            }
        }
        else {
            SelectChartFromStack( current_stack_index + direction );
        }
    } else {
        ArrayOfInts piano_chart_index_array = cc1->GetQuiltExtendedStackdbIndexArray();
        int refdb = cc1->GetQuiltRefChartdbIndex();

        //      Find the ref chart in the stack
        int current_index = -1;
        for(unsigned int i=0 ; i < piano_chart_index_array.Count() ; i++){
            if(refdb == piano_chart_index_array.Item( i )){
                current_index = i;
                break;
            }
        }
        if(current_index == -1)
            return;

        const ChartTableEntry &ctet = ChartData->GetChartTableEntry( refdb );
        int target_family= ctet.GetChartFamily();

        int new_index = -1;
        int check_index = current_index + direction;
        bool found = false;
        int check_dbIndex = -1;
        int new_dbIndex = -1;

        //      When quilted. switch within the same chart family
        while(!found && (unsigned int)check_index < piano_chart_index_array.Count() && (check_index >= 0)){
            check_dbIndex = piano_chart_index_array.Item( check_index );
            const ChartTableEntry &cte = ChartData->GetChartTableEntry( check_dbIndex );
            if(target_family == cte.GetChartFamily()){
                found = true;
                new_index = check_index;
                new_dbIndex = check_dbIndex;
                break;
            }

            check_index += direction;
        }

        if(!found)
            return;


        if( !cc1->IsChartQuiltableRef( new_dbIndex ) ) {
            ToggleQuiltMode();
            SelectdbChart( new_dbIndex );
            m_bpersistent_quilt = true;
        } else {
            SelectQuiltRefChart( new_index );
        }
    }

    UpdateGlobalMenuItems(); // update the state of the menu items (checkmarks etc)
    cc1->SetQuiltChartHiLiteIndex( -1 );

    cc1->ReloadVP();
}

// Defered initialization for anything that is not required to render the initial frame
// and takes a while to initialize.  This gets opencpn up and running much faster.
void MyFrame::OnInitTimer(wxTimerEvent& event)
{
    switch(m_iInitCount++) {
    case 0:
        // Load the waypoints.. both of these routines are very slow to execute which is why
        // they have been to defered until here
        pWayPointMan = new WayPointman();
        pConfig->LoadNavObjects();
        break;

    case 1:
        // Connect Datastreams
        for ( size_t i = 0; i < g_pConnectionParams->Count(); i++ )
        {
            ConnectionParams *cp = g_pConnectionParams->Item(i);
            if( cp->bEnabled ) {

#ifdef __unix__
                if( cp->GetDSPort().Contains(_T("Serial"))) {
                    if( ! g_bserial_access_checked ){
                        if( !CheckSerialAccess() ){
                        }
                        g_bserial_access_checked = true;
                    }
                }
#endif

                dsPortType port_type = cp->IOSelect;
                DataStream *dstr = new DataStream( g_pMUX,
                                                   cp->GetDSPort(),
                                                   wxString::Format(wxT("%i"),cp->Baudrate),
                                                   port_type,
                                                   cp->Priority,
                                                   cp->Garmin
                    );
                dstr->SetInputFilter(cp->InputSentenceList);
                dstr->SetInputFilterType(cp->InputSentenceListType);
                dstr->SetOutputFilter(cp->OutputSentenceList);
                dstr->SetOutputFilterType(cp->OutputSentenceListType);
                dstr->SetChecksumCheck(cp->ChecksumCheck);

                cp->b_IsSetup = true;

                g_pMUX->AddStream(dstr);
            }
        }

        console = new ConsoleCanvas( gFrame );                    // the console
        console->SetColorScheme( global_color_scheme );
        break;

    default:
        g_pi_manager->LoadAllPlugIns( g_Plugin_Dir, true, false );

        RequestNewToolbar();

        //   Notify all the AUI PlugIns so that they may syncronize with the Perspective
        g_pi_manager->NotifyAuiPlugIns();
        g_pi_manager->ShowDeferredBlacklistMessages(); //  Give the use dialog on any blacklisted PlugIns
        g_pi_manager->CallLateInit();

        InitTimer.Stop(); // Initialization complete
    }

    cc1->InvalidateGL();
    Refresh();
}

//    Manage the application memory footprint on a periodic schedule
void MyFrame::OnMemFootTimer( wxTimerEvent& event )
{
    MemFootTimer.Stop();

    int memsize = GetApplicationMemoryUse();

    g_MemFootMB = 100;
    printf( "Memsize: %d  \n", memsize );
    // The application memory usage has exceeded the target, so try to manage it down....
    if( memsize > ( g_MemFootMB * 1000 ) ) {
        if( ChartData && cc1 ) {
            //    Get a local copy of the cache info
            wxArrayPtrVoid *pCache = ChartData->GetChartCache();
            unsigned int nCache = pCache->GetCount();
            CacheEntry *pcea = new CacheEntry[nCache];

            for( unsigned int i = 0; i < nCache; i++ ) {
                CacheEntry *pce = (CacheEntry *) ( pCache->Item( i ) );
                pcea[i] = *pce;                  //ChartBase *Ch = (ChartBase *)pce->pChart;
            }

            if( nCache > 1 ) {
                //    Bubble Sort the local cache entry array
                bool b_cont = true;
                while( b_cont ) {
                    b_cont = false;
                    for( unsigned int i = 0; i < nCache - 1; i++ ) {
                        if( pcea[i].RecentTime > pcea[i + 1].RecentTime ) {
                            CacheEntry tmp = pcea[i];
                            pcea[i] = pcea[i + 1];
                            pcea[i + 1] = tmp;
                            b_cont = true;
                            break;
                        }
                    }
                }

                //    Free up some chart cache entries until the memory footprint target is realized

                unsigned int idelete = 0;                 // starting at top. which is oldest
                unsigned int idelete_max = pCache->GetCount();

                //    How many can be deleted?
                unsigned int minimum_cache = 1;
                if( cc1->GetQuiltMode() ) minimum_cache = cc1->GetQuiltChartCount();

                while( ( memsize > ( g_MemFootMB * 1000 ) )
                        && ( pCache->GetCount() > minimum_cache ) && ( idelete < idelete_max ) ) {
                    int memsizeb = memsize;

                    ChartData->DeleteCacheChart( (ChartBase *) pcea[idelete].pChart );
                    idelete++;
                    memsize = GetApplicationMemoryUse();
                    printf( "delete, before: %d  after: %d\n", memsizeb, memsize );
                }

            }

            delete[] pcea;
        }

    }

//      MemFootTimer.Start(wxMax(g_MemFootSec * 1000, 60 * 1000), wxTIMER_CONTINUOUS);
    MemFootTimer.Start( 9000, wxTIMER_CONTINUOUS );
}

// play an arbitrary number of bells by using 1 and 2 bell sounds
void MyFrame::OnBellsTimer(wxTimerEvent& event)
{
    int bells = wxMin(m_BellsToPlay, 2);

    if(bells <= 0)
        return;

    if( !bells_sound[bells - 1].IsOk() )            // load the bells sound
    {
        wxString soundfile = _T("sounds");
        g_pBASE->appendOSDirSlash( &soundfile );
        soundfile += wxString( bells_sound_file_name[bells - 1], wxConvUTF8 );
        soundfile.Prepend( g_SData_Locn );
        bells_sound[bells - 1].Create( soundfile );
        if( !bells_sound[bells - 1].IsOk() ) {
            wxLogMessage( _T("Failed to load bells sound file: ") + soundfile );
            return;
        }

        wxLogMessage( _T("Using bells sound file: ") + soundfile );
    }

    bells_sound[bells - 1].Play();
    m_BellsToPlay -= bells;
    
    BellsTimer.Start(2000, wxTIMER_ONE_SHOT);
}

int ut_index;

void MyFrame::OnFrameTimer1( wxTimerEvent& event )
{

    if( s_ProgDialog ) {
        return;
    }

    if( g_unit_test_1 ) {
//            if((0 == ut_index) && GetQuiltMode())
//                  ToggleQuiltMode();

        cc1->m_bFollow = false;
        if( g_toolbar ) g_toolbar->ToggleTool( ID_FOLLOW, cc1->m_bFollow );

        if( ChartData ) {
            if( ut_index < ChartData->GetChartTableEntries() ) {
                const ChartTableEntry *cte = &ChartData->GetChartTableEntry( ut_index );
                double lat = ( cte->GetLatMax() + cte->GetLatMin() ) / 2;
                double lon = ( cte->GetLonMax() + cte->GetLonMin() ) / 2;

                vLat = lat;
                vLon = lon;

                cc1->SetViewPoint( lat, lon );

                if( cc1->GetQuiltMode() ) {
                    if( cc1->IsChartQuiltableRef( ut_index ) ) SelectQuiltRefdbChart( ut_index );
                } else
                    SelectdbChart( ut_index );

                double ppm = cc1->GetCanvasScaleFactor() / cte->GetScale();
                ppm /= 2;
                cc1->SetVPScale( ppm );

                cc1->ReloadVP();

                ut_index++;
            }
        }
    }
    g_tick++;

#ifdef __WXOSX__
    //    To fix an ugly bug ?? in wxWidgets for Carbon.....
    //    Or, maybe this is the way Macs work....
    //    Hide some non-UI Dialogs if the application is minimized....
    //    They will be re-Show()-n in MyFrame::OnActivate()
    if(IsIconized())
    {
        if(g_FloatingToolbarDialog) {
            if(g_FloatingToolbarDialog->IsShown())
            g_FloatingToolbarDialog->Submerge();
        }

        AppActivateList.Clear();
        if(cc1){
            for ( wxWindowList::iterator it = cc1->GetChildren().begin(); it != cc1->GetChildren().end(); ++it ) {
                if( (*it)->IsShown() ) {
                    (*it)->Hide();
                    AppActivateList.Append(*it);
                }
            }
        }

        if(gFrame){
            for ( wxWindowList::iterator it = gFrame->GetChildren().begin(); it != gFrame->GetChildren().end(); ++it ) {
                if( (*it)->IsShown() ) {
                    if( !(*it)->IsKindOf( CLASSINFO(ChartCanvas) ) ) {
                        (*it)->Hide();
                        AppActivateList.Append(*it);
                    }
                }
            }
        }
#if 0
        if(console && console->IsShown()) {
            console->Hide();
        }

        if(g_FloatingCompassDialog && g_FloatingCompassDialog->IsShown()) {
            g_FloatingCompassDialog->Hide();
        }

        if(stats && stats->IsShown()) {
            stats->Hide();
        }
#endif
    }
#endif

//      Listen for quitflag to be set, requesting application close
    if( quitflag ) {
        wxLogMessage( _T("Got quitflag from SIGUSR1") );
        FrameTimer1.Stop();
        Close();
        return;
    }

    if( bDBUpdateInProgress ) return;

    FrameTimer1.Stop();

    //  If tracking carryover was found in config file, enable tracking as soon as
    //  GPS become valid
    if(g_bDeferredStartTrack){
        if(!g_bTrackActive){
            if(bGPSValid){
                gFrame->TrackOn();
                g_bDeferredStartTrack = false;
            }
        }
        else {                                  // tracking has been manually activated
            g_bDeferredStartTrack = false;
        }
    }

//  Update and check watchdog timer for GPS data source
    gGPS_Watchdog--;
    if( gGPS_Watchdog <= 0 ) {
        bGPSValid = false;
        if( gGPS_Watchdog == 0  ){
            wxString msg;
            msg.Printf( _T("   ***GPS Watchdog timeout at Lat:%g   Lon: %g"), gLat, gLon );
            wxLogMessage(msg);
        }
        gSog = NAN;
        gCog = NAN;
    }

//  Update and check watchdog timer for Mag Heading data source
    gHDx_Watchdog--;
    if( gHDx_Watchdog <= 0 ) {
        gHdm = NAN;
        if( g_nNMEADebug && ( gHDx_Watchdog == 0 ) ) wxLogMessage(
                _T("   ***HDx Watchdog timeout...") );
    }

//  Update and check watchdog timer for True Heading data source
    gHDT_Watchdog--;
    if( gHDT_Watchdog <= 0 ) {
        g_bHDT_Rx = false;
        gHdt = NAN;
        if( g_nNMEADebug && ( gHDT_Watchdog == 0 ) ) wxLogMessage(
                _T("   ***HDT Watchdog timeout...") );
    }

    //  Update and check watchdog timer for Magnetic Variation data source
    gVAR_Watchdog--;
    if( gVAR_Watchdog <= 0 ) {
        g_bVAR_Rx = false;
        if( g_nNMEADebug && ( gVAR_Watchdog == 0 ) ) wxLogMessage(
            _T("   ***VAR Watchdog timeout...") );
    }
    //  Update and check watchdog timer for GSV (Satellite data)
    gSAT_Watchdog--;
    if( gSAT_Watchdog <= 0 ) {
        g_bSatValid = false;
        g_SatsInView = 0;
        if( g_nNMEADebug && ( gSAT_Watchdog == 0 ) ) wxLogMessage(
                _T("   ***SAT Watchdog timeout...") );
    }

    //    Build and send a Position Fix event to PlugIns
    if( g_pi_manager )
    {
        GenericPosDatEx GPSData;
        GPSData.kLat = gLat;
        GPSData.kLon = gLon;
        GPSData.kCog = gCog;
        GPSData.kSog = gSog;
        GPSData.kVar = gVar;
        GPSData.kHdm = gHdm;
        GPSData.kHdt = gHdt;
        GPSData.nSats = g_SatsInView;

        GPSData.FixTime = m_fixtime;

        g_pi_manager->SendPositionFixToAllPlugIns( &GPSData );
    }

    //   Check for anchorwatch alarms                                 // pjotrc 2010.02.15
    if( pAnchorWatchPoint1 ) {
        double dist;
        double brg;
        DistanceBearingMercator( pAnchorWatchPoint1->m_lat, pAnchorWatchPoint1->m_lon, gLat, gLon,
                &brg, &dist );
        double d = g_nAWMax;
        ( pAnchorWatchPoint1->GetName() ).ToDouble( &d );
        d = AnchorDistFix( d, AnchorPointMinDist, g_nAWMax );
        bool toofar = false;
        bool tooclose = false;
        if( d >= 0.0 ) toofar = ( dist * 1852. > d );
        if( d < 0.0 ) tooclose = ( dist * 1852 < -d );

        if( tooclose || toofar ) AnchorAlertOn1 = true;
        else
            AnchorAlertOn1 = false;
    } else
        AnchorAlertOn1 = false;

    if( pAnchorWatchPoint2 ) {
        double dist;
        double brg;
        DistanceBearingMercator( pAnchorWatchPoint2->m_lat, pAnchorWatchPoint2->m_lon, gLat, gLon,
                &brg, &dist );

        double d = g_nAWMax;
        ( pAnchorWatchPoint2->GetName() ).ToDouble( &d );
        d = AnchorDistFix( d, AnchorPointMinDist, g_nAWMax );
        bool toofar = false;
        bool tooclose = false;
        if( d >= 0 ) toofar = ( dist * 1852. > d );
        if( d < 0 ) tooclose = ( dist * 1852 < -d );

        if( tooclose || toofar ) AnchorAlertOn2 = true;
        else
            AnchorAlertOn2 = false;
    } else
        AnchorAlertOn2 = false;

    if( (pAnchorWatchPoint1 || pAnchorWatchPoint2) && !bGPSValid )
        AnchorAlertOn1 = true;

//  Send current nav status data to log file on every half hour   // pjotrc 2010.02.09

    wxDateTime lognow = wxDateTime::Now();   // pjotrc 2010.02.09
    int hourLOC = lognow.GetHour();
    int minuteLOC = lognow.GetMinute();
    lognow.MakeGMT();
    int minuteUTC = lognow.GetMinute();
    int second = lognow.GetSecond();

    wxTimeSpan logspan = lognow.Subtract( g_loglast_time );
    if( ( logspan.IsLongerThan( wxTimeSpan( 0, 30, 0, 0 ) ) ) || ( minuteUTC == 0 )
            || ( minuteUTC == 30 ) ) {
        if( logspan.IsLongerThan( wxTimeSpan( 0, 1, 0, 0 ) ) ) {
            wxString day = lognow.FormatISODate();
            wxString utc = lognow.FormatISOTime();
            wxString navmsg = _T("LOGBOOK:  ");
            navmsg += day;
            navmsg += _T(" ");
            navmsg += utc;
            navmsg += _T(" UTC ");

            if( bGPSValid ) {
                wxString data;
                data.Printf( _T(" GPS Lat %10.5f Lon %10.5f "), gLat, gLon );
                navmsg += data;

                wxString cog;
                if( wxIsNaN(gCog) ) cog.Printf( _T("COG ----- ") );
                else
                    cog.Printf( _T("COG %10.5f "), gCog );

                wxString sog;
                if( wxIsNaN(gSog) ) sog.Printf( _T("SOG -----  ") );
                else
                    sog.Printf( _T("SOG %6.2f ") + getUsrSpeedUnit(), toUsrSpeed( gSog ) );

                navmsg += cog;
                navmsg += sog;
            } else {
                wxString data;
                data.Printf( _T(" DR Lat %10.5f Lon %10.5f"), gLat, gLon );
                navmsg += data;
            }
            wxLogMessage( navmsg );
            g_loglast_time = lognow;

            if( hourLOC == 0 && minuteLOC == 0 && g_bTrackDaily )
                TrackMidnightRestart();

            int bells = ( hourLOC % 4 ) * 2;     // 2 bells each hour
            if( minuteLOC != 0 ) bells++;       // + 1 bell on 30 minutes
            if( !bells ) bells = 8;     // 0 is 8 bells

            if( g_bPlayShipsBells && ( ( minuteLOC == 0 ) || ( minuteLOC == 30 ) ) ) {
                m_BellsToPlay = bells;
                BellsTimer.Start(5, wxTIMER_ONE_SHOT);
            }
        }
    }

//      Update the Toolbar Status windows and lower status bar the first time watchdog times out
    if( ( gGPS_Watchdog == 0 ) || ( gSAT_Watchdog == 0 ) ) {
        wxString sogcog( _T("SOG --- ") + getUsrSpeedUnit() + _T(" COG ---\u00B0") );
        if( GetStatusBar() ) SetStatusText( sogcog, STAT_FIELD_SOGCOG );

        gCog = 0.0;                                 // say speed is zero to kill ownship predictor
    }

    if( cc1 ) {
#if !defined(__WXGTK__) && !defined(__WXQT__)
        double cursor_lat, cursor_lon;
        cc1->GetCursorLatLon( &cursor_lat, &cursor_lon );
        cc1->SetCursorStatus(cursor_lat, cursor_lon);
#endif
    }
//      Update the chart database and displayed chart
    bool bnew_view = false;

//    Do the chart update based on the global update period currently set
//    If in COG UP mode, the chart update is handled by COG Update timer
    if( !g_bCourseUp && ( 0 == m_ChartUpdatePeriod-- ) ) {
        bnew_view = DoChartUpdate();
        m_ChartUpdatePeriod = g_ChartUpdatePeriod;
    }

    nBlinkerTick++;
    if( cc1 )
        cc1->DrawBlinkObjects();

//      Update the active route, if any
    if( g_pRouteMan->UpdateProgress() ) {
        //    This RefreshRect will cause any active routepoint to blink
        if( g_pRouteMan->GetpActiveRoute() ) cc1->RefreshRect( g_blink_rect, false );
    }
#if 0 // too slow, my computer hiccups, this takes nearly a second on some machines.
//  Instead we should save the current configuration only when it needs to be saved.
    if( 0 == ( g_tick % ( g_nautosave_interval_seconds ) ) ) {
        pConfig->UpdateSettings();
        pConfig->UpdateNavObj();
    }
#endif

//  Force own-ship drawing parameters
    cc1->SetOwnShipState( SHIP_NORMAL );

    if( cc1->GetQuiltMode() ) {
        double erf = cc1->GetQuiltMaxErrorFactor();
        if( erf > 0.02 ) cc1->SetOwnShipState( SHIP_LOWACCURACY );
    } else {
        if( Current_Ch ) {
            if( Current_Ch->GetChart_Error_Factor() > 0.02 ) cc1->SetOwnShipState(
                    SHIP_LOWACCURACY );
        }
    }

    if( !bGPSValid )
        cc1->SetOwnShipState( SHIP_INVALID );

    if( bGPSValid != m_last_bGPSValid ) {
        if(!g_bopengl)
            cc1->UpdateShips();

        bnew_view = true;                  // force a full Refresh()
        m_last_bGPSValid = bGPSValid;
    }

    //    If any PlugIn requested dynamic overlay callbacks, force a full canvas refresh
    //    thus, ensuring at least 1 Hz. callback.
    bool brq_dynamic = false;
    if( g_pi_manager ) {
        ArrayOfPlugIns *pplugin_array = g_pi_manager->GetPlugInArray();
        for( unsigned int i = 0; i < pplugin_array->GetCount(); i++ ) {
            PlugInContainer *pic = pplugin_array->Item( i );
            if( pic->m_bEnabled && pic->m_bInitState ) {
                if( pic->m_cap_flag & WANTS_DYNAMIC_OPENGL_OVERLAY_CALLBACK ) {
                    brq_dynamic = true;
                    break;
                }
            }
        }

        if( brq_dynamic )
            bnew_view = true;
    }

    FrameTimer1.Start( TIMER_GFRAME_1, wxTIMER_CONTINUOUS );

    if(g_bopengl) {
#ifdef ocpnUSE_GL
        if(m_fixtime - cc1->GetglCanvas()->m_last_render_time > 0)
            bnew_view = true;
        
        if( AnyAISTargetsOnscreen( cc1->GetVP() ) )
            bnew_view = true;
        
        if(bnew_view) /* full frame in opengl mode */
            cc1->Refresh(false);
#endif
    } else {
//  Invalidate the ChartCanvas window appropriately
//    In non-follow mode, invalidate the rectangles containing the AIS targets and the ownship, etc...
//    In follow mode, if there has already been a full screen refresh, there is no need to check ownship or AIS,
//       since they will be always drawn on the full screen paint.

        if( ( !cc1->m_bFollow ) || g_bCourseUp ) {
            cc1->UpdateShips();
            cc1->UpdateAIS();
            cc1->UpdateAlerts();
        } else {
            if( !bnew_view )                    // There has not been a Refresh() yet.....
            {
                cc1->UpdateAIS();
                cc1->UpdateAlerts();
            }
        }
    }

    if( g_pais_query_dialog_active && g_pais_query_dialog_active->IsShown() ) g_pais_query_dialog_active->UpdateText();

    // Refresh AIS target list every 5 seconds to avoid blinking
    if( g_pAISTargetList && ( 0 == ( g_tick % ( 5 ) ) ) ) g_pAISTargetList->UpdateAISTargetList();

    //  Pick up any change Toolbar status displays
    UpdateGPSCompassStatusBox();
    UpdateAISTool();

    if( console && console->IsShown() ) {
//            console->Raise();
        console->RefreshConsoleData();
    }

    //  This little hack fixes a problem seen with some UniChrome OpenGL drivers
    //  We need a deferred resize to get glDrawPixels() to work right.
    //  So we set a trigger to generate a resize after 5 seconds....
    //  See the "UniChrome" hack elsewhere
    if( m_bdefer_resize ) {
        if( 0 == ( g_tick % ( 5 ) ) ) {
            printf( "___RESIZE\n" );
            SetSize( m_defer_size );
            g_pauimgr->Update();
            m_bdefer_resize = false;
        }
    }
}

double MyFrame::GetTrueOrMag(double a)
{
    if( g_bShowMag ){
        if(!wxIsNaN(gVar)){
            if((a - gVar) >360.)
                return (a - gVar - 360.);
            else
                return ((a - gVar) >= 0.) ? (a - gVar) : (a - gVar + 360.);
        }
        else{
            if((a - g_UserVar) >360.)
                return (a - g_UserVar - 360.);
            else
                return ((a - g_UserVar) >= 0.) ? (a - g_UserVar) : (a - g_UserVar + 360.);
        }
    }
    else
        return a;
}

void MyFrame::TouchAISActive( void )
{
    ocpnStyle::Style* style = g_StyleManager->GetCurrentStyle();

    if( m_pAISTool ) {
        if( ( !g_pAIS->IsAISSuppressed() ) && ( !g_pAIS->IsAISAlertGeneral() ) ) {
            g_nAIS_activity_timer = 5;                // seconds

            wxString iconName = _T("AIS_Normal_Active");
            if( g_pAIS->IsAISAlertGeneral() ) iconName = _T("AIS_AlertGeneral_Active");
            if( g_pAIS->IsAISSuppressed() ) iconName = _T("AIS_Suppressed_Active");
            if( !g_pAIS->ShowAIS() ) iconName = _T("AIS_Disabled");

            if( m_lastAISiconName != iconName ) {
                if( g_toolbar) {
                    g_toolbar->SetToolNormalBitmapEx( m_pAISTool, iconName );
                    g_toolbar->Refresh();
                    m_lastAISiconName = iconName;
                }
            }
        }
    }
}

void MyFrame::UpdateAISTool( void )
{
    if(!g_pAIS) return;

    bool b_need_refresh = false;
    ocpnStyle::Style* style = g_StyleManager->GetCurrentStyle();

    wxString iconName;

    if( m_pAISTool ) {
        bool b_update = false;

        iconName = _T("AIS");
        if( g_pAIS->IsAISSuppressed() )
            iconName = _T("AIS_Suppressed");
        if( g_pAIS->IsAISAlertGeneral() )
            iconName = _T("AIS_AlertGeneral");
        if( !g_pAIS->ShowAIS() )
            iconName = _T("AIS_Disabled");

        //  Manage timeout for AIS activity indicator
        if( g_nAIS_activity_timer ) {
            g_nAIS_activity_timer--;

            if( 0 == g_nAIS_activity_timer ) b_update = true;
            else {
                iconName = _T("AIS_Normal_Active");
                if( g_pAIS->IsAISSuppressed() )
                    iconName = _T("AIS_Suppressed_Active");
                if( g_pAIS->IsAISAlertGeneral() )
                    iconName = _T("AIS_AlertGeneral_Active");
                if( !g_pAIS->ShowAIS() )
                    iconName = _T("AIS_Disabled");
            }
        }

        if( ( m_lastAISiconName != iconName ) ) b_update = true;

        if( b_update && g_toolbar) {
            g_toolbar->SetToolNormalBitmapEx( m_pAISTool, iconName );
            g_toolbar->Refresh();
            m_lastAISiconName = iconName;
        }

    }
}

//    Cause refresh of active Tide/Current data, if displayed
void MyFrame::OnFrameTCTimer( wxTimerEvent& event )
{
    if( cc1 ) {
        cc1->SetbTCUpdate( true );
        cc1->Refresh( false );
    }
}

//    Keep and update the Viewport rotation angle according to average COG for COGUP mode
void MyFrame::OnFrameCOGTimer( wxTimerEvent& event )
{
    DoCOGSet();

    if(!g_bCourseUp)
        return;

    //    Restart the timer, max frequency is 10 hz.
    int period_ms = 100;
    if( g_COGAvgSec > 0 )
        period_ms = g_COGAvgSec * 1000;
    FrameCOGTimer.Start( period_ms, wxTIMER_CONTINUOUS );
}

void MyFrame::DoCOGSet( void )
{
    if( !g_bCourseUp )
        return;

    if(!cc1)
        return;

    double old_VPRotate = g_VPRotate;
    g_VPRotate = -g_COGAvg * PI / 180.;
    if(!g_bskew_comp)
        g_VPRotate -= cc1->GetVPSkew();

    cc1->SetVPRotation( g_VPRotate );
    bool bnew_chart = DoChartUpdate();

    if( ( bnew_chart ) || ( old_VPRotate != g_VPRotate ) )
        cc1->ReloadVP();
}

void RenderShadowText( wxDC *pdc, wxFont *pFont, wxString& str, int x, int y )
{
#ifdef DrawText
#undef DrawText
#define FIXIT
#endif

    wxFont oldfont = pdc->GetFont(); // save current font

    pdc->SetFont( *pFont );
    pdc->SetTextForeground( GetGlobalColor( _T("CHGRF") ) );
    pdc->SetBackgroundMode( wxTRANSPARENT );

    pdc->DrawText( str, x, y + 1 );
    pdc->DrawText( str, x, y - 1 );
    pdc->DrawText( str, x + 1, y );
    pdc->DrawText( str, x - 1, y );

    pdc->SetTextForeground( GetGlobalColor( _T("CHBLK") ) );

    pdc->DrawText( str, x, y );

    pdc->SetFont( oldfont );                  // restore last font

}

void MyFrame::UpdateRotationState( double rotation )
{
    //  If rotated manually, we switch to NORTHUP
    g_bCourseUp = false;
    
    if(fabs(rotation) > .001){
        SetMenubarItemState( ID_MENU_CHART_COGUP, false );
        SetMenubarItemState( ID_MENU_CHART_NORTHUP, true );
        if(m_pMenuBar){
            m_pMenuBar->SetLabel( ID_MENU_CHART_NORTHUP, _("Rotated Mode") );
        }
    }
    else{
        SetMenubarItemState( ID_MENU_CHART_COGUP, g_bCourseUp );
        SetMenubarItemState( ID_MENU_CHART_NORTHUP, !g_bCourseUp );
        if(m_pMenuBar){
            m_pMenuBar->SetLabel( ID_MENU_CHART_NORTHUP, _("North Up Mode") );
        }
    }
    
    UpdateGPSCompassStatusBox( true );
    DoChartUpdate();
    cc1->ReloadVP();
}


void MyFrame::UpdateGPSCompassStatusBox( bool b_force_new )
{
    if( !g_FloatingCompassDialog ) return;

    //    Look for change in overlap or positions
    bool b_update = false;
    wxRect tentative_rect;
    wxPoint tentative_pt_in_screen;
    int x_offset;
    int y_offset;
    int size_x, size_y;
    int cc1_edge_comp = 2;

    if( g_FloatingToolbarDialog ) {
        x_offset = g_FloatingCompassDialog->GetXOffset();
        y_offset = g_FloatingCompassDialog->GetYOffset();
        g_FloatingCompassDialog->GetSize(&size_x, &size_y);
        wxSize parent_size = g_FloatingCompassDialog->GetParent()->GetSize();

        // check to see if it would overlap if it was in its home position (upper right)
         tentative_pt_in_screen = g_FloatingCompassDialog->GetParent()->ClientToScreen(
                wxPoint( parent_size.x - size_x - x_offset - cc1_edge_comp, y_offset ) );

        tentative_rect = wxRect( tentative_pt_in_screen.x, tentative_pt_in_screen.y, size_x, size_y );

        //  If the toolbar location has changed, or the proposed compassDialog location has changed
        if( (g_FloatingToolbarDialog->GetScreenRect() != g_last_tb_rect) ||
            (tentative_rect != g_FloatingCompassDialog->GetScreenRect()) ) {

            wxRect tb_rect = g_FloatingToolbarDialog->GetScreenRect();

            //    if they would not intersect, go ahead and move it to the upper right
            //      Else it has to be on lower right
            if( !tb_rect.Intersects( tentative_rect ) ) {
                g_FloatingCompassDialog->Move( tentative_pt_in_screen );
            }
            else {
                wxPoint posn_in_canvas =
                    wxPoint( cc1->GetSize().x - size_x - x_offset - cc1_edge_comp,
                             cc1->GetSize().y - ( size_y + y_offset + cc1_edge_comp ) );
                g_FloatingCompassDialog->Move( cc1->ClientToScreen( posn_in_canvas ) );
            }

            b_update = true;

            g_last_tb_rect = tb_rect;

        }
    }

    if( g_FloatingCompassDialog && g_FloatingCompassDialog->IsShown()) {
        g_FloatingCompassDialog->UpdateStatus( b_force_new | b_update );
        g_FloatingCompassDialog->Update();
    }
}

int MyFrame::GetnChartStack( void )
{
    if(pCurrentStack)
        return pCurrentStack->nEntry;
    else
        return 0;
}

//    Application memory footprint management

int MyFrame::GetApplicationMemoryUse( void )
{
    int memsize = -1;
#ifdef __LINUX__

    //    Use a contrived ps command to get the virtual memory size associated with this process
    wxWindow *fWin = wxWindow::FindFocus();

    wxArrayString outputArray;
    wxString cmd(_T("ps --no-headers -o vsize "));
    unsigned long pid = wxGetProcessId();
    wxString cmd1;
    cmd1.Printf(_T("%ld"), pid);
    cmd += cmd1;
    wxExecute(cmd, outputArray);

    if(outputArray.GetCount())
    {
        wxString s = outputArray.Item(0);
        long vtmp;
        if(s.ToLong(&vtmp))
        memsize = vtmp;
    }

    if(fWin)
    fWin->SetFocus();

#endif

#ifdef __WXMSW__
    HANDLE hProcess;
    PROCESS_MEMORY_COUNTERS pmc;

    unsigned long processID = wxGetProcessId();

    hProcess = OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID );
    if( NULL == hProcess ) return 0;

    if( GetProcessMemoryInfo( hProcess, &pmc, sizeof( pmc ) ) ) {
        /*
         printf( "\tPageFaultCount: 0x%08X\n", pmc.PageFaultCount );
         printf( "\tPeakWorkingSetSize: 0x%08X\n",
         pmc.PeakWorkingSetSize );
         printf( "\tWorkingSetSize: 0x%08X\n", pmc.WorkingSetSize );
         printf( "\tQuotaPeakPagedPoolUsage: 0x%08X\n",
         pmc.QuotaPeakPagedPoolUsage );
         printf( "\tQuotaPagedPoolUsage: 0x%08X\n",
         pmc.QuotaPagedPoolUsage );
         printf( "\tQuotaPeakNonPagedPoolUsage: 0x%08X\n",
         pmc.QuotaPeakNonPagedPoolUsage );
         printf( "\tQuotaNonPagedPoolUsage: 0x%08X\n",
         pmc.QuotaNonPagedPoolUsage );
         printf( "\tPagefileUsage: 0x%08X\n", pmc.PagefileUsage );
         printf( "\tPeakPagefileUsage: 0x%08X\n",
         pmc.PeakPagefileUsage );
         */
        memsize = pmc.WorkingSetSize / 1024;
    }

    CloseHandle( hProcess );

#endif

    return memsize;
}

void MyFrame::HandlePianoClick( int selected_index, int selected_dbIndex )
{
    if( !pCurrentStack ) return;
    if( s_ProgDialog ) return;

    if( !cc1->GetQuiltMode() ) {
        if( m_bpersistent_quilt/* && g_bQuiltEnable*/ ) {
            if( cc1->IsChartQuiltableRef( selected_dbIndex ) ) {
                ToggleQuiltMode();
                SelectQuiltRefdbChart( selected_dbIndex );
                m_bpersistent_quilt = false;
            } else {
                SelectChartFromStack( selected_index );
            }
        } else {
            SelectChartFromStack( selected_index );
            g_sticky_chart = selected_dbIndex;
        }
    } else {
        if( cc1->IsChartQuiltableRef( selected_dbIndex ) ){
            if( ChartData ) ChartData->PurgeCache();


            //  If the chart is a vector chart, and of very large scale,
            //  then we had better set the new scale directly to avoid excessive underzoom
            //  on, eg, Inland ENCs
            bool set_scale = false;
            if(ChartData){
                if( CHART_TYPE_S57 == ChartData->GetDBChartType( selected_dbIndex ) ){
                    if( ChartData->GetDBChartScale(selected_dbIndex) < 5000){
                        set_scale = true;
                    }
                }
            }

            if(!set_scale){
                SelectQuiltRefdbChart( selected_dbIndex, true );  // autoscale
            }
            else {
                SelectQuiltRefdbChart( selected_dbIndex, false );  // no autoscale


            //  Adjust scale so that the selected chart is underzoomed/overzoomed by a controlled amount
                ChartBase *pc = ChartData->OpenChartFromDB( selected_dbIndex, FULL_INIT );
                if( pc ) {
                    double proposed_scale_onscreen = cc1->GetCanvasScaleFactor() / cc1->GetVPScale();

                    if(g_bPreserveScaleOnX){
                        proposed_scale_onscreen = wxMin(proposed_scale_onscreen,
                                                100 * pc->GetNormalScaleMax(cc1->GetCanvasScaleFactor(), cc1->GetCanvasWidth()));
                    }
                    else{
                        proposed_scale_onscreen = wxMin(proposed_scale_onscreen,
                                                        20 * pc->GetNormalScaleMax(cc1->GetCanvasScaleFactor(), cc1->GetCanvasWidth()));

                        proposed_scale_onscreen = wxMax(proposed_scale_onscreen,
                                                pc->GetNormalScaleMin(cc1->GetCanvasScaleFactor(), g_b_overzoom_x));
                    }

                    cc1->SetVPScale( cc1->GetCanvasScaleFactor() / proposed_scale_onscreen );
                }
            }
        }
        else {
            ToggleQuiltMode();
            SelectdbChart( selected_dbIndex );
            m_bpersistent_quilt = true;
        }
    }

    cc1->SetQuiltChartHiLiteIndex( -1 );
    UpdateGlobalMenuItems(); // update the state of the menu items (checkmarks etc)
    cc1->HideChartInfoWindow();
    DoChartUpdate();
    cc1->ReloadVP();                  // Pick up the new selections
}

void MyFrame::HandlePianoRClick( int x, int y, int selected_index, int selected_dbIndex )
{
    if( !pCurrentStack ) return;
    if( s_ProgDialog ) return;

    PianoPopupMenu( x, y, selected_index, selected_dbIndex );
    UpdateControlBar();

    cc1->SetQuiltChartHiLiteIndex( -1 );

}

void MyFrame::HandlePianoRollover( int selected_index, int selected_dbIndex )
{
    if( !cc1 ) return;
    if( !pCurrentStack ) return;
    if( s_ProgDialog ) return;

    if(ChartData && ChartData->IsBusy())
        return;

    int sx, sy;
    stats->GetPosition( &sx, &sy );
    wxPoint key_location = stats->pPiano->GetKeyOrigin( selected_index );
    wxPoint rolloverPos = stats->GetParent()->ScreenToClient( wxPoint( sx, sy ) );
    rolloverPos.y -= 3;
    rolloverPos.x += key_location.x;

    if( !cc1->GetQuiltMode() ) {
        SetChartThumbnail( selected_index );
        cc1->ShowChartInfoWindow( key_location.x, sy + key_location.y, selected_dbIndex );
    } else {
        ArrayOfInts piano_chart_index_array = cc1->GetQuiltExtendedStackdbIndexArray();

        if( ( pCurrentStack->nEntry > 1 ) || ( piano_chart_index_array.GetCount() > 1 ) ) {
            cc1->ShowChartInfoWindow( rolloverPos.x, rolloverPos.y, selected_dbIndex );
            cc1->SetQuiltChartHiLiteIndex( selected_dbIndex );

            cc1->ReloadVP( false );         // no VP adjustment allowed
        } else if( pCurrentStack->nEntry == 1 ) {
            const ChartTableEntry &cte = ChartData->GetChartTableEntry(
                    pCurrentStack->GetDBIndex( 0 ) );
            if( CHART_TYPE_CM93COMP != cte.GetChartType() ) {
                cc1->ShowChartInfoWindow( rolloverPos.x, rolloverPos.y, selected_dbIndex );
                cc1->ReloadVP( false );
            } else if( ( -1 == selected_index ) && ( -1 == selected_dbIndex ) ) {
                cc1->ShowChartInfoWindow( rolloverPos.x, rolloverPos.y, selected_dbIndex );
            }
        }
        SetChartThumbnail( -1 );        // hide all thumbs in quilt mode
    }
}

void MyFrame::HandlePianoRolloverIcon( int selected_index, int selected_dbIndex )
{
    if( !cc1 ) return;

    if( !cc1->GetQuiltMode() ) {
        SetChartThumbnail( selected_index );
    } else {
        cc1->SetQuiltChartHiLiteIndex( selected_dbIndex );
    }
}

double MyFrame::GetBestVPScale( ChartBase *pchart )
{
    if( pchart ) {
        double proposed_scale_onscreen = cc1->GetCanvasScaleFactor() / cc1->GetVPScale();

        if( ( g_bPreserveScaleOnX ) || ( CHART_TYPE_CM93COMP == pchart->GetChartType() ) ) {
            double new_scale_ppm = cc1->GetVPScale(); 
            proposed_scale_onscreen = cc1->GetCanvasScaleFactor() / new_scale_ppm;
        } else {
            //  This logic will bring the new chart onscreen at roughly twice the true paper scale equivalent.
            proposed_scale_onscreen = pchart->GetNativeScale() / 2;
            double equivalent_vp_scale = cc1->GetCanvasScaleFactor() / proposed_scale_onscreen;
            double new_scale_ppm = pchart->GetNearestPreferredScalePPM( equivalent_vp_scale );
            proposed_scale_onscreen = cc1->GetCanvasScaleFactor() / new_scale_ppm;
        }

        // Do not allow excessive underzoom, even if the g_bPreserveScaleOnX flag is set.
        // Otherwise, we get severe performance problems on all platforms

        double max_underzoom_multiplier = 2.0;
        proposed_scale_onscreen =
               wxMin(proposed_scale_onscreen,
                     pchart->GetNormalScaleMax(cc1->GetCanvasScaleFactor(), cc1->GetCanvasWidth()) *
                     max_underzoom_multiplier);
               
        //  And, do not allow excessive overzoom either       
        proposed_scale_onscreen =
                wxMax(proposed_scale_onscreen, pchart->GetNormalScaleMin(cc1->GetCanvasScaleFactor(), false));

        return cc1->GetCanvasScaleFactor() / proposed_scale_onscreen;
    } else
        return 1.0;
}

void MyFrame::SelectQuiltRefChart( int selected_index )
{
    ArrayOfInts piano_chart_index_array = cc1->GetQuiltExtendedStackdbIndexArray();
    int current_db_index = piano_chart_index_array.Item( selected_index );

    SelectQuiltRefdbChart( current_db_index );
}

void MyFrame::SelectQuiltRefdbChart( int db_index, bool b_autoscale )
{
    if( pCurrentStack ) pCurrentStack->SetCurrentEntryFromdbIndex( db_index );

    cc1->SetQuiltRefChart( db_index );

    ChartBase *pc = ChartData->OpenChartFromDB( db_index, FULL_INIT );
    if( pc ) {
        if(b_autoscale) {
            double best_scale = GetBestVPScale( pc );
            cc1->SetVPScale( best_scale );
        }
    }
    else
        cc1->SetQuiltRefChart( -1 );


}

void MyFrame::SelectChartFromStack( int index, bool bDir, ChartTypeEnum New_Type,
        ChartFamilyEnum New_Family )
{
    if( !pCurrentStack )
        return;

    if( index < pCurrentStack->nEntry ) {
//      Open the new chart
        ChartBase *pTentative_Chart;
        pTentative_Chart = ChartData->OpenStackChartConditional( pCurrentStack, index, bDir,
                New_Type, New_Family );

        if( pTentative_Chart ) {
            if( Current_Ch ) Current_Ch->Deactivate();

            Current_Ch = pTentative_Chart;
            Current_Ch->Activate();

            pCurrentStack->CurrentStackEntry = ChartData->GetStackEntry( pCurrentStack,
                    Current_Ch->GetFullPath() );
        } else
            SetChartThumbnail( -1 );   // need to reset thumbnail on failed chart open

//      Setup the view
        double zLat, zLon;
        if( cc1->m_bFollow ) {
            zLat = gLat;
            zLon = gLon;
        } else {
            zLat = vLat;
            zLon = vLon;
        }

        double best_scale = GetBestVPScale( Current_Ch );

        cc1->SetViewPoint( zLat, zLon, best_scale, Current_Ch->GetChartSkew() * PI / 180.,
                cc1->GetVPRotation() );

        SetChartUpdatePeriod( cc1->GetVP() );

        UpdateGPSCompassStatusBox();           // Pick up the rotation

    }

    //          Refresh the Piano Bar
    if( stats ) {
        ArrayOfInts piano_active_chart_index_array;
        piano_active_chart_index_array.Add( pCurrentStack->GetCurrentEntrydbIndex() );
        stats->pPiano->SetActiveKeyArray( piano_active_chart_index_array );

        stats->Refresh( true );
    }
}

void MyFrame::SelectdbChart( int dbindex )
{
    if( !pCurrentStack )
        return;

    if( dbindex >= 0 ) {
//      Open the new chart
        ChartBase *pTentative_Chart;
        pTentative_Chart = ChartData->OpenChartFromDB( dbindex, FULL_INIT );

        if( pTentative_Chart ) {
            if( Current_Ch ) Current_Ch->Deactivate();

            Current_Ch = pTentative_Chart;
            Current_Ch->Activate();

            pCurrentStack->CurrentStackEntry = ChartData->GetStackEntry( pCurrentStack,
                    Current_Ch->GetFullPath() );
        } else
            SetChartThumbnail( -1 );       // need to reset thumbnail on failed chart open

//      Setup the view
        double zLat, zLon;
        if( cc1->m_bFollow ) {
            zLat = gLat;
            zLon = gLon;
        } else {
            zLat = vLat;
            zLon = vLon;
        }

        double best_scale = GetBestVPScale( Current_Ch );

        if( Current_Ch )
            cc1->SetViewPoint( zLat, zLon, best_scale, Current_Ch->GetChartSkew() * PI / 180.,
                cc1->GetVPRotation() );

        SetChartUpdatePeriod( cc1->GetVP() );

        UpdateGPSCompassStatusBox();           // Pick up the rotation

    }

    //          Refresh the Piano Bar
    if( stats ) {
        ArrayOfInts piano_active_chart_index_array;
        piano_active_chart_index_array.Add( pCurrentStack->GetCurrentEntrydbIndex() );
        stats->pPiano->SetActiveKeyArray( piano_active_chart_index_array );

        stats->Refresh( true );
    }
}

void MyFrame::SetChartUpdatePeriod( ViewPort &vp )
{
    //    Set the chart update period based upon chart skew and skew compensator

    g_ChartUpdatePeriod = 1;            // General default

    if( !vp.b_quilt ) {
        if( g_bskew_comp && ( fabs( vp.skew ) ) > 0.01 ) g_ChartUpdatePeriod =
                g_SkewCompUpdatePeriod;
    }

    m_ChartUpdatePeriod = g_ChartUpdatePeriod;
}

void MyFrame::SetChartThumbnail( int index )
{
    if( bDBUpdateInProgress ) return;

    if( NULL == pCurrentStack ) return;

    if( NULL == pthumbwin ) return;

    if( NULL == cc1 ) return;

    if( index == -1 ) {
        wxRect thumb_rect_in_parent = pthumbwin->GetRect();

        pthumbwin->pThumbChart = NULL;
        pthumbwin->Show( false );
        cc1->RefreshRect( thumb_rect_in_parent, FALSE );
    }

    else
        if( index < pCurrentStack->nEntry ) {
            if( ( ChartData->GetCSChartType( pCurrentStack, index ) == CHART_TYPE_KAP )
                    || ( ChartData->GetCSChartType( pCurrentStack, index ) == CHART_TYPE_GEO )
                    || ( ChartData->GetCSChartType( pCurrentStack, index ) == CHART_TYPE_PLUGIN ) ) {
                ChartBase *new_pThumbChart = ChartData->OpenChartFromStack( pCurrentStack, index );
                if( new_pThumbChart )         // chart opened ok
                {

                    ThumbData *pTD = new_pThumbChart->GetThumbData( 150, 150, gLat, gLon );
                    if( pTD ) {
                        pthumbwin->pThumbChart = new_pThumbChart;

                        pthumbwin->Resize();
                        pthumbwin->Show( true );
                        pthumbwin->Refresh( FALSE );
                        pthumbwin->Move( wxPoint( 4, 4 ) );

                        // Simplistic overlap avoidance works only when toolbar is at top of screen.
                        if( g_FloatingToolbarDialog )
                            if( g_FloatingToolbarDialog->GetScreenRect().Intersects( pthumbwin->GetScreenRect() ) ) {
                                pthumbwin->Move( wxPoint( 4, g_FloatingToolbarDialog->GetSize().y + 4 ) );
                        }
                    }

                    else {
                        wxLogMessage(
                                _T("    chart1.cpp:SetChartThumbnail...Could not create thumbnail") );
                        pthumbwin->pThumbChart = NULL;
                        pthumbwin->Show( false );
                        cc1->Refresh( FALSE );
                    }

                } else                            // some problem opening chart
                {
                    wxString fp = ChartData->GetFullPath( pCurrentStack, index );
                    fp.Prepend( _T("    chart1.cpp:SetChartThumbnail...Could not open chart ") );
                    wxLogMessage( fp );
                    pthumbwin->pThumbChart = NULL;
                    pthumbwin->Show( false );
                    cc1->Refresh( FALSE );
                }

            } else {
                ChartBase *new_pThumbChart = ChartData->OpenChartFromStack( pCurrentStack, index,
                        THUMB_ONLY );

                pthumbwin->pThumbChart = new_pThumbChart;

                if( new_pThumbChart ) {
                    ThumbData *pTD = new_pThumbChart->GetThumbData( 200, 200, gLat, gLon );
                    if( pTD ) {
                        pthumbwin->Resize();
                        pthumbwin->Show( true );
                        pthumbwin->Refresh( true );
                    } else
                        pthumbwin->Show( false );

                    cc1->Refresh( FALSE );
                }
            }
        }

}

void MyFrame::UpdateControlBar( void )
{
    if( !cc1 ) return;

    if( !stats ) return;

    if( !pCurrentStack ) return;

    ArrayOfInts piano_chart_index_array;
    ArrayOfInts empty_piano_chart_index_array;

    wxString old_hash = stats->pPiano->GetStoredHash();
     
    if( cc1->GetQuiltMode() ) {
        piano_chart_index_array = cc1->GetQuiltExtendedStackdbIndexArray();
        stats->pPiano->SetKeyArray( piano_chart_index_array );

        ArrayOfInts piano_active_chart_index_array = cc1->GetQuiltCandidatedbIndexArray();
        stats->pPiano->SetActiveKeyArray( piano_active_chart_index_array );

        ArrayOfInts piano_eclipsed_chart_index_array = cc1->GetQuiltEclipsedStackdbIndexArray();
        stats->pPiano->SetSubliteIndexArray( piano_eclipsed_chart_index_array );

        stats->pPiano->SetNoshowIndexArray( g_quilt_noshow_index_array );

    } else {
        piano_chart_index_array = ChartData->GetCSArray( pCurrentStack );
        stats->pPiano->SetKeyArray( piano_chart_index_array );

        ArrayOfInts piano_active_chart_index_array;
        piano_active_chart_index_array.Add( pCurrentStack->GetCurrentEntrydbIndex() );
        stats->pPiano->SetActiveKeyArray( piano_active_chart_index_array );

    }

    //    Set up the TMerc and Skew arrays
    ArrayOfInts piano_skew_chart_index_array;
    ArrayOfInts piano_tmerc_chart_index_array;
    ArrayOfInts piano_poly_chart_index_array;

    for( unsigned int ino = 0; ino < piano_chart_index_array.GetCount(); ino++ ) {
        const ChartTableEntry &ctei = ChartData->GetChartTableEntry(
                piano_chart_index_array.Item( ino ) );
        double skew_norm = ctei.GetChartSkew();
        if( skew_norm > 180. ) skew_norm -= 360.;

        if( ctei.GetChartProjectionType() == PROJECTION_TRANSVERSE_MERCATOR ) piano_tmerc_chart_index_array.Add(
                piano_chart_index_array.Item( ino ) );

        //    Polyconic skewed charts should show as skewed
        else
            if( ctei.GetChartProjectionType() == PROJECTION_POLYCONIC ) {
                if( fabs( skew_norm ) > 1. ) piano_skew_chart_index_array.Add(
                        piano_chart_index_array.Item( ino ) );
                else
                    piano_poly_chart_index_array.Add( piano_chart_index_array.Item( ino ) );
            } else
                if( fabs( skew_norm ) > 1. ) piano_skew_chart_index_array.Add(
                        piano_chart_index_array.Item( ino ) );

    }
    stats->pPiano->SetSkewIndexArray( piano_skew_chart_index_array );
    stats->pPiano->SetTmercIndexArray( piano_tmerc_chart_index_array );
    stats->pPiano->SetPolyIndexArray( piano_poly_chart_index_array );

    stats->FormatStat();
    
    wxString new_hash = stats->pPiano->GenerateAndStoreNewHash();
    if(new_hash != old_hash)
        stats->Refresh( false );

}

//----------------------------------------------------------------------------------
//      DoChartUpdate
//      Create a chartstack based on current lat/lon.
//      Update Current_Ch, using either current chart, if still in stack, or
//      smallest scale new chart in stack if not.
//      Return true if a Refresh(false) was called within.
//----------------------------------------------------------------------------------
bool MyFrame::DoChartUpdate( void )
{

    double tLat, tLon;           // Chart Stack location
    double vpLat, vpLon;         // ViewPort location

    bool bNewChart = false;
    bool bNewView = false;

    bool bNewPiano = false;
    bool bOpenSpecified;
    ChartStack LastStack;
    ChartBase *pLast_Ch;

    ChartStack WorkStack;

    if( !cc1 ) return false;
    if( bDBUpdateInProgress ) return false;
    if( !ChartData ) return false;

    if(ChartData->IsBusy())
        return false;

    int last_nEntry = -1;
    if( pCurrentStack ) last_nEntry = pCurrentStack->nEntry;

    //    Startup case:
    //    Quilting is enabled, but the last chart seen was not quiltable
    //    In this case, drop to single chart mode, set persistence flag,
    //    And open the specified chart
    if( bFirstAuto && ( g_restore_dbindex >= 0 ) ) {
        if( cc1->GetQuiltMode() ) {
            if( !cc1->IsChartQuiltableRef( g_restore_dbindex ) ) {
                ToggleQuiltMode();
                m_bpersistent_quilt = true;
                Current_Ch = NULL;
            }
        }
    }

    //      If in auto-follow mode, use the current glat,glon to build chart stack.
    //      Otherwise, use vLat, vLon gotten from click on chart canvas, or other means

    if( cc1->m_bFollow == true ) {
        tLat = gLat;
        tLon = gLon;
        vpLat = gLat;
        vpLon = gLon;

        // on lookahead mode, adjust the vp center point
        if( cc1 && g_bLookAhead ) {
            double angle = g_COGAvg + ( cc1->GetVPRotation() * 180. / PI );

            double pixel_deltay = fabs( cos( angle * PI / 180. ) ) * cc1->GetCanvasHeight() / 4;
            double pixel_deltax = fabs( sin( angle * PI / 180. ) ) * cc1->GetCanvasWidth() / 4;

            double pixel_delta_tent = sqrt(
                    ( pixel_deltay * pixel_deltay ) + ( pixel_deltax * pixel_deltax ) );

            double pixel_delta = 0;

            //    The idea here is to cancel the effect of LookAhead for slow gSog, to avoid
            //    jumping of the vp center point during slow maneuvering, or at anchor....
            if( !wxIsNaN(gSog) ) {
                if( gSog < 1.0 ) pixel_delta = 0.;
                else
                    if( gSog >= 3.0 ) pixel_delta = pixel_delta_tent;
                    else
                        pixel_delta = pixel_delta_tent * ( gSog - 1.0 ) / 2.0;
            }

            double meters_to_shift = cos( gLat * PI / 180. ) * pixel_delta / cc1->GetVPScale();

            double dir_to_shift = g_COGAvg;

            ll_gc_ll( gLat, gLon, dir_to_shift, meters_to_shift / 1852., &vpLat, &vpLon );
        }

    } else {
        tLat = vLat;
        tLon = vLon;
        vpLat = vLat;
        vpLon = vLon;

    }

    if( cc1->GetQuiltMode() ) {
        int current_db_index = -1;
        if( pCurrentStack ) current_db_index = pCurrentStack->GetCurrentEntrydbIndex(); // capture the currently selected Ref chart dbIndex
        else
            pCurrentStack = new ChartStack;

        ChartData->BuildChartStack( pCurrentStack, tLat, tLon );
        pCurrentStack->SetCurrentEntryFromdbIndex( current_db_index );

        if( bFirstAuto ) {
            double proposed_scale_onscreen = cc1->GetCanvasScaleFactor() / cc1->GetVPScale(); // as set from config load

            int initial_db_index = g_restore_dbindex;
            if( initial_db_index < 0 ) {
                if( pCurrentStack->nEntry ) {
                    if( ( g_restore_stackindex < pCurrentStack->nEntry )
                            && ( g_restore_stackindex >= 0 ) ) initial_db_index =
                            pCurrentStack->GetDBIndex( g_restore_stackindex );
                    else
                        initial_db_index = pCurrentStack->GetDBIndex( pCurrentStack->nEntry - 1 );
                } else
                    initial_db_index = 0;
            }

            if( pCurrentStack->nEntry ) {

                int initial_type = ChartData->GetDBChartType( initial_db_index );

                //    Check to see if the target new chart is quiltable as a reference chart

                if( !cc1->IsChartQuiltableRef( initial_db_index ) ) {
                    // If it is not quiltable, then walk the stack up looking for a satisfactory chart
                    // i.e. one that is quiltable and of the same type
                    int stack_index = g_restore_stackindex;

                    while( ( stack_index < pCurrentStack->nEntry - 1 ) && ( stack_index >= 0 ) ) {
                        int test_db_index = pCurrentStack->GetDBIndex( stack_index );
                        if( cc1->IsChartQuiltableRef( test_db_index )
                                && ( initial_type == ChartData->GetDBChartType( initial_db_index ) ) ) {
                            initial_db_index = test_db_index;
                            break;
                        }
                        stack_index++;
                    }
                }

                if( ChartData ) {
                    ChartBase *pc = ChartData->OpenChartFromDB( initial_db_index, FULL_INIT );
                    if( pc ) {
                        cc1->SetQuiltRefChart( initial_db_index );
                        pCurrentStack->SetCurrentEntryFromdbIndex( initial_db_index );
                    }
                }

                // Try to bound the inital Viewport scale to something reasonable for the selected reference chart
                if( ChartData ) {
                    ChartBase *pc = ChartData->OpenChartFromDB( initial_db_index, FULL_INIT );
                    if( pc ) {
                        proposed_scale_onscreen =
                                wxMin(proposed_scale_onscreen, pc->GetNormalScaleMax(cc1->GetCanvasScaleFactor(), cc1->GetCanvasWidth()));
                        proposed_scale_onscreen =
                                wxMax(proposed_scale_onscreen, pc->GetNormalScaleMin(cc1->GetCanvasScaleFactor(), g_b_overzoom_x));
                    }
                }

            }

            bNewView |= cc1->SetViewPoint( vpLat, vpLon,
                    cc1->GetCanvasScaleFactor() / proposed_scale_onscreen, 0,
                    cc1->GetVPRotation() );

        }

        bNewView |= cc1->SetViewPoint( vpLat, vpLon, cc1->GetVPScale(), 0, cc1->GetVPRotation() );

        goto update_finish;

    }

    //  Single Chart Mode from here....
    pLast_Ch = Current_Ch;
    ChartTypeEnum new_open_type;
    ChartFamilyEnum new_open_family;
    if( pLast_Ch ) {
        new_open_type = pLast_Ch->GetChartType();
        new_open_family = pLast_Ch->GetChartFamily();
    } else {
        new_open_type = CHART_TYPE_KAP;
        new_open_family = CHART_FAMILY_RASTER;
    }

    bOpenSpecified = bFirstAuto;
    bAutoOpen = true;                             // debugging

    //  Make sure the target stack is valid
    if( NULL == pCurrentStack ) pCurrentStack = new ChartStack;

    // Build a chart stack based on tLat, tLon
    if( 0 == ChartData->BuildChartStack( &WorkStack, tLat, tLon, g_sticky_chart ) )       // Bogus Lat, Lon?
            {
        if( NULL == pDummyChart ) {
            pDummyChart = new ChartDummy;
            bNewChart = true;
        }

        if( Current_Ch ) if( Current_Ch->GetChartType() != CHART_TYPE_DUMMY ) bNewChart = true;

        Current_Ch = pDummyChart;

//    If the current viewpoint is invalid, set the default scale to something reasonable.
        double set_scale = cc1->GetVPScale();
        if( !cc1->GetVP().IsValid() ) set_scale = 1. / 20000.;

        bNewView |= cc1->SetViewPoint( tLat, tLon, set_scale, 0, cc1->GetVPRotation() );

        //      If the chart stack has just changed, there is new status
        if( !ChartData->EqualStacks( &WorkStack, pCurrentStack ) ) {
            bNewPiano = true;
            bNewChart = true;
        }

        //      Copy the new (by definition empty) stack into the target stack
        ChartData->CopyStack( pCurrentStack, &WorkStack );

        goto update_finish;
    }

    //              Check to see if Chart Stack has changed
    if( !ChartData->EqualStacks( &WorkStack, pCurrentStack ) ) {
        //      New chart stack, so...
        bNewPiano = true;

        //      Save a copy of the current stack
        ChartData->CopyStack( &LastStack, pCurrentStack );

        //      Copy the new stack into the target stack
        ChartData->CopyStack( pCurrentStack, &WorkStack );

        //  Is Current Chart in new stack?

        int tEntry = -1;
        if( NULL != Current_Ch )                                  // this handles startup case
            tEntry = ChartData->GetStackEntry( pCurrentStack, Current_Ch->GetFullPath() );

        if( tEntry != -1 ) {                // Current_Ch is in the new stack
            pCurrentStack->CurrentStackEntry = tEntry;
            bNewChart = false;
        }

        else                           // Current_Ch is NOT in new stack
        {                                       // So, need to open a new chart
                                                //      Find the largest scale raster chart that opens OK

            ChartBase *pProposed = NULL;

            if( bAutoOpen ) {
                bool search_direction = false;        // default is to search from lowest to highest
                int start_index = 0;

                //    A special case:  If panning at high scale, open largest scale chart first
                if( ( LastStack.CurrentStackEntry == LastStack.nEntry - 1 )
                        || ( LastStack.nEntry == 0 ) ) {
                    search_direction = true;
                    start_index = pCurrentStack->nEntry - 1;
                }

                //    Another special case, open specified index on program start
                if( bOpenSpecified ) {
                    search_direction = false;
                    start_index = g_restore_stackindex;
                    if( ( start_index < 0 ) | ( start_index >= pCurrentStack->nEntry ) ) start_index =
                            0;
                    new_open_type = CHART_TYPE_DONTCARE;
                }

                pProposed = ChartData->OpenStackChartConditional( pCurrentStack, start_index,
                        search_direction, new_open_type, new_open_family );

//    Try to open other types/families of chart in some priority
                if( NULL == pProposed ) pProposed = ChartData->OpenStackChartConditional(
                        pCurrentStack, start_index, search_direction, CHART_TYPE_CM93COMP,
                        CHART_FAMILY_VECTOR );

                if( NULL == pProposed ) pProposed = ChartData->OpenStackChartConditional(
                        pCurrentStack, start_index, search_direction, CHART_TYPE_CM93COMP,
                        CHART_FAMILY_RASTER );

                bNewChart = true;

            }     // bAutoOpen

            else
                pProposed = NULL;

//  If no go, then
//  Open a Dummy Chart
            if( NULL == pProposed ) {
                if( NULL == pDummyChart ) {
                    pDummyChart = new ChartDummy;
                    bNewChart = true;
                }

                if( pLast_Ch ) if( pLast_Ch->GetChartType() != CHART_TYPE_DUMMY ) bNewChart = true;

                pProposed = pDummyChart;
            }

// Arriving here, pProposed points to an opened chart, or NULL.
            if( Current_Ch ) Current_Ch->Deactivate();
            Current_Ch = pProposed;

            if( Current_Ch ) {
                Current_Ch->Activate();
                pCurrentStack->CurrentStackEntry = ChartData->GetStackEntry( pCurrentStack,
                        Current_Ch->GetFullPath() );
            }
        }   // need new chart

// Arriving here, Current_Ch is opened and OK, or NULL
        if( NULL != Current_Ch ) {

//      Setup the view using the current scale
            double set_scale = cc1->GetVPScale();

//    If the current viewpoint is invalid, set the default scale to something reasonable.
            if( !cc1->GetVP().IsValid() )
                set_scale = 1. / 20000.;
            else {                                    // otherwise, match scale if elected.
                double proposed_scale_onscreen;

                if( cc1->m_bFollow ) {          // autoset the scale only if in autofollow
                    double new_scale_ppm = Current_Ch->GetNearestPreferredScalePPM( cc1->GetVPScale() );
                    proposed_scale_onscreen = cc1->GetCanvasScaleFactor() / new_scale_ppm;
                }
                else
                    proposed_scale_onscreen = cc1->GetCanvasScaleFactor() / set_scale;


                //  This logic will bring a new chart onscreen at roughly twice the true paper scale equivalent.
                //  Note that first chart opened on application startup (bOpenSpecified = true) will open at the config saved scale
                if( bNewChart && !g_bPreserveScaleOnX && !bOpenSpecified ) {
                    proposed_scale_onscreen = Current_Ch->GetNativeScale() / 2;
                    double equivalent_vp_scale = cc1->GetCanvasScaleFactor()
                            / proposed_scale_onscreen;
                    double new_scale_ppm = Current_Ch->GetNearestPreferredScalePPM(
                            equivalent_vp_scale );
                    proposed_scale_onscreen = cc1->GetCanvasScaleFactor() / new_scale_ppm;
                }

                if( cc1->m_bFollow ) {     // bounds-check the scale only if in autofollow
                    proposed_scale_onscreen =
                        wxMin(proposed_scale_onscreen, Current_Ch->GetNormalScaleMax(cc1->GetCanvasScaleFactor(), cc1->GetCanvasWidth()));
                    proposed_scale_onscreen =
                        wxMax(proposed_scale_onscreen, Current_Ch->GetNormalScaleMin(cc1->GetCanvasScaleFactor(), g_b_overzoom_x));
                }

                set_scale = cc1->GetCanvasScaleFactor() / proposed_scale_onscreen;
            }

            bNewView |= cc1->SetViewPoint( vpLat, vpLon, set_scale,
                    Current_Ch->GetChartSkew() * PI / 180., cc1->GetVPRotation() );

        }
    }         // new stack

    else                                                                 // No change in Chart Stack
    {
        if( ( cc1->m_bFollow ) && Current_Ch ) bNewView |= cc1->SetViewPoint( vpLat, vpLon,
                cc1->GetVPScale(), Current_Ch->GetChartSkew() * PI / 180., cc1->GetVPRotation() );
    }

    update_finish:

    //    Ask for a new tool bar if the stack is going to or coming from only one entry.
    if( pCurrentStack
            && ( ( ( pCurrentStack->nEntry <= 1 ) && m_toolbar_scale_tools_shown )
                    || ( ( pCurrentStack->nEntry > 1 ) && !m_toolbar_scale_tools_shown ) ) ) if( !bFirstAuto ) RequestNewToolbar();

    if( bNewPiano ) UpdateControlBar();

    //  Update the ownship position on thumbnail chart, if shown
    if( pthumbwin && pthumbwin->IsShown() ) {
        if( pthumbwin->pThumbChart ){
            if( pthumbwin->pThumbChart->UpdateThumbData( gLat, gLon ) )
                pthumbwin->Refresh( TRUE );
        }
    }

    bFirstAuto = false;                           // Auto open on program start

    //  If we need a Refresh(), do it here...
    //  But don't duplicate a Refresh() done by SetViewPoint()
    if( bNewChart && !bNewView ) cc1->Refresh( false );

#ifdef ocpnUSE_GL
    // If a new chart, need to invalidate gl viewport for refresh
    // so the fbo gets flushed
    if(g_bopengl & bNewChart)
        cc1->GetglCanvas()->Invalidate();
#endif

    return bNewChart | bNewView;
}

void MyFrame::MouseEvent( wxMouseEvent& event )
{
    int x, y;
    event.GetPosition( &x, &y );

}

void MyFrame::RemoveChartFromQuilt( int dbIndex )
{
    //    Remove the item from the list (if it appears) to avoid multiple addition
    for( unsigned int i = 0; i < g_quilt_noshow_index_array.GetCount(); i++ ) {
        if( g_quilt_noshow_index_array.Item( i ) == dbIndex ) // chart is already in the noshow list
                {
            g_quilt_noshow_index_array.RemoveAt( i );
            break;
        }
    }

    g_quilt_noshow_index_array.Add( dbIndex );

}

//          Piano window Popup Menu Handlers and friends

static int menu_selected_dbIndex;
static int menu_selected_index;

void MyFrame::PianoPopupMenu( int x, int y, int selected_index, int selected_dbIndex )
{
    if( !pCurrentStack )
        return;

    //    No context menu if quilting is disabled
    if( !cc1->GetQuiltMode() ) return;

    menu_selected_dbIndex = selected_dbIndex;
    menu_selected_index = selected_index;

    piano_ctx_menu = new wxMenu();

    //    Search the no-show array
    bool b_is_in_noshow = false;
    for( unsigned int i = 0; i < g_quilt_noshow_index_array.GetCount(); i++ ) {
        if( g_quilt_noshow_index_array.Item( i ) == selected_dbIndex ) // chart is in the noshow list
                {
            b_is_in_noshow = true;
            break;
        }
    }

    if( b_is_in_noshow ) {
        piano_ctx_menu->Append( ID_PIANO_ENABLE_QUILT_CHART, _("Show This Chart") );
        Connect( ID_PIANO_ENABLE_QUILT_CHART, wxEVT_COMMAND_MENU_SELECTED,
                wxCommandEventHandler(MyFrame::OnPianoMenuEnableChart) );
    } else
        if( pCurrentStack->nEntry > 1 ) {
            piano_ctx_menu->Append( ID_PIANO_DISABLE_QUILT_CHART, _("Hide This Chart") );
            Connect( ID_PIANO_DISABLE_QUILT_CHART, wxEVT_COMMAND_MENU_SELECTED,
                    wxCommandEventHandler(MyFrame::OnPianoMenuDisableChart) );
        }

    int sx, sy;
    stats->GetPosition( &sx, &sy );
    wxPoint pos = stats->GetParent()->ScreenToClient( wxPoint( sx, sy ) );
    wxPoint key_location = stats->pPiano->GetKeyOrigin( selected_index );
    pos.x += key_location.x;
    pos.y -= 30;

//        Invoke the drop-down menu
    if( piano_ctx_menu->GetMenuItems().GetCount() ) PopupMenu( piano_ctx_menu, pos );

    delete piano_ctx_menu;
    piano_ctx_menu = NULL;
    
    cc1->HideChartInfoWindow();
    stats->pPiano->ResetRollover();

    cc1->SetQuiltChartHiLiteIndex( -1 );

    cc1->ReloadVP();

}

void MyFrame::OnPianoMenuEnableChart( wxCommandEvent& event )
{
    for( unsigned int i = 0; i < g_quilt_noshow_index_array.GetCount(); i++ ) {
        if( g_quilt_noshow_index_array.Item( i ) == menu_selected_dbIndex ) // chart is in the noshow list
                {
            g_quilt_noshow_index_array.RemoveAt( i );
            break;
        }
    }
}

void MyFrame::OnPianoMenuDisableChart( wxCommandEvent& event )
{
    if( !pCurrentStack )
        return;

    RemoveChartFromQuilt( menu_selected_dbIndex );

//      It could happen that the chart being disabled is the reference chart....
    if( menu_selected_dbIndex == cc1->GetQuiltRefChartdbIndex() ) {
        int type = ChartData->GetDBChartType( menu_selected_dbIndex );

        int i = menu_selected_index + 1;          // select next smaller scale chart
        bool b_success = false;
        while( i < pCurrentStack->nEntry - 1 ) {
            int dbIndex = pCurrentStack->GetDBIndex( i );
            if( type == ChartData->GetDBChartType( dbIndex ) ) {
                SelectQuiltRefChart( i );
                b_success = true;
                break;
            }
            i++;
        }

        //    If that did not work, try to select the next larger scale compatible chart
        if( !b_success ) {
            i = menu_selected_index - 1;
            while( i > 0 ) {
                int dbIndex = pCurrentStack->GetDBIndex( i );
                if( type == ChartData->GetDBChartType( dbIndex ) ) {
                    SelectQuiltRefChart( i );
                    b_success = true;
                    break;
                }
                i--;
            }
        }
    }
}

void MyFrame::DoPrint( void )
{
    if( NULL == g_printData ) {
        g_printData = new wxPrintData;
        g_printData->SetOrientation( wxLANDSCAPE );
        g_pageSetupData = new wxPageSetupDialogData;
    }

    wxPrintDialogData printDialogData( *g_printData );
    printDialogData.EnablePageNumbers( false );

    wxPrinter printer( &printDialogData );

    OCPN_Printout printout( cc1, _("Chart Print") );
    
    //  In OperGL mode, make the bitmap capture of the screen before the print method starts,
    //  so as to be sure the "Abort..." dialog does not appear on the image
    if(g_bopengl) 
        printout.GenerateGLbmp( );
                
    if( !printer.Print( this, &printout, true ) ) {
        if( wxPrinter::GetLastError() == wxPRINTER_ERROR ) OCPNMessageBox(NULL,
                _("There was a problem printing.\nPerhaps your current printer is not set correctly?"),
                _T("OpenCPN"), wxOK );
//        else
//            OCPNMessageBox(_T("Print Cancelled"), _T("OpenCPN"), wxOK);
    } else {
        ( *g_printData ) = printer.GetPrintDialogData().GetPrintData();
    }

// Pass two printout objects: for preview, and possible printing.
    /*
     wxPrintDialogData printDialogData(* g_printData);
     wxPrintPreview *preview = new wxPrintPreview(new MyPrintout, new MyPrintout, & printDialogData);
     if (!preview->Ok())
     {
     delete preview;
     OCPNMessageBox(_T("There was a problem previewing.\nPerhaps your current printer is not set correctly?"), _T("Previewing"), wxOK);
     return;
     }

     wxPreviewFrame *frame = new wxPreviewFrame(preview, this, _T("Demo Print Preview"), wxPoint(100, 100), wxSize(600, 650));
     frame->Centre(wxBOTH);
     frame->Initialize();
     frame->Show();
     */

}

void MyFrame::OnEvtPlugInMessage( OCPN_MsgEvent & event )
{
    wxString message_ID = event.GetID();
    wxString message_JSONText = event.GetJSONText();

    //  We are free to use or ignore any or all of the PlugIn messages flying thru this pipe tee.

    //  We can possibly use the estimated magnetic variation if WMM_pi is present and active
    //  and we have no other source of Variation
    if(!g_bVAR_Rx)
    {
        if(message_ID == _T("WMM_VARIATION_BOAT"))
        {

        // construct the JSON root object
            wxJSONValue  root;
        // construct a JSON parser
            wxJSONReader reader;

        // now read the JSON text and store it in the 'root' structure
        // check for errors before retreiving values...
            int numErrors = reader.Parse( message_JSONText, &root );
            if ( numErrors > 0 )  {
//              const wxArrayString& errors = reader.GetErrors();
                return;
            }

            // get the DECL value from the JSON message
            wxString decl = root[_T("Decl")].AsString();
            double decl_val;
            decl.ToDouble(&decl_val);

            gVar = decl_val;
        }
    }

    if(message_ID == _T("OCPN_TRACK_REQUEST"))
    {
        wxJSONValue  root;
        wxJSONReader reader;
        wxString trk_id = wxEmptyString;

        int numErrors = reader.Parse( message_JSONText, &root );
        if ( numErrors > 0 )
            return;

        if(root.HasMember(_T("Track_ID")))
            trk_id = root[_T("Track_ID")].AsString();

        for(RouteList::iterator it = pRouteList->begin(); it != pRouteList->end(); it++)
        {
            wxString name = wxEmptyString;
            if((*it)->IsTrack() && (*it)->m_GUID == trk_id)
            {
                name = (*it)->m_RouteNameString;
                if(name.IsEmpty())
                {
                    RoutePoint *rp = (*it)->GetPoint( 1 );
                    if( rp && rp->GetCreateTime().IsValid() )
                        name = rp->GetCreateTime().FormatISODate() + _T(" ") + rp->GetCreateTime().FormatISOTime();
                    else
                        name = _("(Unnamed Track)");
                }

/*                Tracks can be huge e.g merged tracks. On Compüters with small memory this can produce a crash by insufficient memory !!

                wxJSONValue v; unsigned long i = 0;
                for(RoutePointList::iterator itp = (*it)->pRoutePointList->begin(); itp != (*it)->pRoutePointList->end(); itp++)
                {
                    v[i][0] = (*itp)->m_lat;
                    v[i][1] = (*itp)->m_lon;
                    i++;
                }
                    wxString msg_id( _T("OCPN_TRACKPOINTS_COORDS") );
                    g_pi_manager->SendJSONMessageToAllPlugins( msg_id, v );
            }
*/
/*                To avoid memory problems send a single trackpoint. It's up to the plugin to collect the data. */
                int i = 1;     wxJSONValue v;
                for(RoutePointList::iterator itp = (*it)->pRoutePointList->begin(); itp != (*it)->pRoutePointList->end(); itp++)
                {
                    v[_T("lat")] = (*itp)->m_lat;
                    v[_T("lon")] = (*itp)->m_lon;
                    v[_T("TotalNodes")] = (*it)->pRoutePointList->GetCount();
                    v[_T("NodeNr")] = i;
                    v[_T("error")] = false;
                    i++;
                    wxString msg_id( _T("OCPN_TRACKPOINTS_COORDS") );
                    g_pi_manager->SendJSONMessageToAllPlugins( msg_id, v );
                }
            }
            else
            {
                wxJSONValue v;
                v[_T("error")] = true;

                wxString msg_id( _T("OCPN_TRACKPOINTS_COORDS") );
                g_pi_manager->SendJSONMessageToAllPlugins( msg_id, v );
            }
        }
    }
    else if(message_ID == _T("OCPN_ROUTE_REQUEST"))
    {
        wxJSONValue  root;
        wxJSONReader reader;
        wxString route_id = wxEmptyString;

        int numErrors = reader.Parse( message_JSONText, &root );
        if ( numErrors > 0 )  {
            return;
        }

        if(root.HasMember(_T("Route_ID")))
            route_id = root[_T("Route_ID")].AsString();

        for(RouteList::iterator it = pRouteList->begin(); it != pRouteList->end(); it++)
        {
            wxString name = wxEmptyString;
            wxJSONValue v;

            if(!(*it)->IsTrack() && (*it)->m_GUID == route_id)
            {
                name = (*it)->m_RouteNameString;
                if(name.IsEmpty())
                    name = _("(Unnamed Route)");

                v[_T("Name")] = name;

                wxJSONValue v; int i = 0;
                for(RoutePointList::iterator itp = (*it)->pRoutePointList->begin(); itp != (*it)->pRoutePointList->end(); itp++)
                {
                    v[i][_T("error")] = false;
                    v[i][_T("lat")] = (*itp)->m_lat;
                    v[i][_T("lon")] = (*itp)->m_lon;
                    v[i][_T("WPName")] = (*itp)->GetName();
                    v[i][_T("WPDescription")] = (*itp)->GetDescription();
                    wxHyperlinkListNode *node = (*itp)->m_HyperlinkList->GetFirst();
                    if(node)
                    {
                        int n = 1;
                        while(node)
                        {
                            Hyperlink *httpLink = node->GetData();
                            v[i][_T("WPLink")+wxString::Format(_T("%d"),n)] = httpLink->Link;
                            v[i][_T("WPLinkDesciption")+wxString::Format(_T("%d"),n++)] = httpLink->DescrText;
                            node = node->GetNext();
                        }
                    }
                    i++;
                }
                wxString msg_id( _T("OCPN_ROUTE_RESPONSE") );
                g_pi_manager->SendJSONMessageToAllPlugins( msg_id, v );
            }
            else
            {
                wxJSONValue v;
                v[0][_T("error")] = true;

                wxString msg_id( _T("OCPN_ROUTE_RESPONSE") );
                g_pi_manager->SendJSONMessageToAllPlugins( msg_id, v );
            }
        }
    }
    else if(message_ID == _T("OCPN_ROUTELIST_REQUEST"))
    {
        wxJSONValue  root;
        wxJSONReader reader;
        bool mode = true, error = false;

        int numErrors = reader.Parse( message_JSONText, &root );
        if ( numErrors > 0 )
            return;

        if(root.HasMember(_T("mode")))
        {
            wxString str = root[_T("mode")].AsString();
            if( str == _T("Track")) mode = false;

            wxJSONValue v; int i = 1;
            for(RouteList::iterator it = pRouteList->begin(); it != pRouteList->end(); it++)
            {
                if((*it)->IsTrack())
                    if(mode == true) continue;
                if(!(*it)->IsTrack())
                    if(mode == false) continue;
                v[0][_T("isTrack")] = !mode;

                wxString name = (*it)->m_RouteNameString;
                if(name.IsEmpty() && !mode)
                {
                    RoutePoint *rp = (*it)->GetPoint( 1 );
                    if( rp && rp->GetCreateTime().IsValid() ) name = rp->GetCreateTime().FormatISODate() + _T(" ")
                        + rp->GetCreateTime().FormatISOTime();
                    else
                        name = _("(Unnamed Track)");
                }
                else if(name.IsEmpty() && mode)
                    name = _("(Unnamed Route)");


                v[i][_T("error")] = false;
                v[i][_T("name")] = name;
                v[i][_T("GUID")] = (*it)->m_GUID;
                bool l = (*it)->IsTrack();
                if(g_pActiveTrack == (*it) && !mode)
                    v[i][_T("active")] = true;
                else
                    v[i][_T("active")] = (*it)->IsActive();
                i++;
            }
            wxString msg_id( _T("OCPN_ROUTELIST_RESPONSE") );
            g_pi_manager->SendJSONMessageToAllPlugins( msg_id, v );
        }
        else
        {
            wxJSONValue v;
            v[0][_T("error")] = true;
            wxString msg_id( _T("OCPN_ROUTELIST_RESPONSE") );
            g_pi_manager->SendJSONMessageToAllPlugins( msg_id, v );
        }
    }
}

void MyFrame::OnEvtTHREADMSG( OCPN_ThreadMessageEvent & event )
{
    wxLogMessage( wxString(event.GetSString().c_str(), wxConvUTF8 ));
}


bool MyFrame::EvalPriority(const wxString & message, DataStream *pDS )
{
    bool bret = true;
    wxString msg_type = message.Mid(1, 5);

    wxString stream_name;
    int stream_priority = 0;
    if( pDS ){
        stream_priority = pDS->GetPriority();
        stream_name = pDS->GetPort();
    }

    //  If the message type has never been seen before...
    if( NMEA_Msg_Hash.find( msg_type ) == NMEA_Msg_Hash.end() ) {
        NMEA_Msg_Container *pcontainer = new NMEA_Msg_Container;
        pcontainer-> current_priority = -1;     //  guarantee to execute the next clause
        pcontainer->stream_name = stream_name;
        pcontainer->receipt_time = wxDateTime::Now();

        NMEA_Msg_Hash[msg_type] = pcontainer;
    }

    NMEA_Msg_Container *pcontainer = NMEA_Msg_Hash[msg_type];
    wxString old_port = pcontainer->stream_name;

    int old_priority = pcontainer->current_priority;

    //  If the message has been seen before, and the priority is greater than or equal to current priority,
    //  then simply update the record
    if( stream_priority >= pcontainer->current_priority )
    {
        pcontainer->receipt_time = wxDateTime::Now();
        pcontainer-> current_priority = stream_priority;
        pcontainer->stream_name = stream_name;

        bret = true;
    }

    //  If the message has been seen before, and the priority is less than the current priority,
    //  then if the time since the last recorded message is greater than GPS_TIMEOUT_SECONDS
    //  then update the record with the new priority and stream.
    //  Otherwise, ignore the message as too low a priority
    else
    {
        if( (wxDateTime::Now().GetTicks() - pcontainer->receipt_time.GetTicks()) > GPS_TIMEOUT_SECONDS )
        {
            pcontainer->receipt_time = wxDateTime::Now();
            pcontainer-> current_priority = stream_priority;
            pcontainer->stream_name = stream_name;

            bret = true;
        }
        else
            bret = false;
    }

    wxString new_port = pcontainer->stream_name;

    //  If the data source or priority has changed for this message type, emit a log entry
    if (pcontainer->current_priority != old_priority || new_port != old_port )
    {
         wxString logmsg = wxString::Format(_T("Changing NMEA Datasource for %s to %s (Priority: %i)"),
                                            msg_type.c_str(),
                                            new_port.c_str(),
                                            pcontainer->current_priority);
         wxLogMessage(logmsg );

         if (NMEALogWindow::Get().Active())
         {
             wxDateTime now = wxDateTime::Now();
             wxString ss = now.FormatISOTime();
             ss.Append( _T(" ") );
             ss.Append( logmsg );
             ss.Prepend( _T("<RED>") );

             NMEALogWindow::Get().Add(ss);
             NMEALogWindow::Get().Refresh(false);
         }
    }
    return bret;
}

void MyFrame::OnEvtOCPN_NMEA( OCPN_DataStreamEvent & event )
{
    wxString sfixtime;
    bool pos_valid = false;
    bool bis_recognized_sentence = true;
    bool ll_valid = true;

    wxString str_buf = wxString(event.GetNMEAString().c_str(), wxConvUTF8);

    if( g_nNMEADebug && ( g_total_NMEAerror_messages < g_nNMEADebug ) )
    {
        g_total_NMEAerror_messages++;
        wxString msg( _T("MEH.NMEA Sentence received...") );
        msg.Append( str_buf );
        wxLogMessage( msg );
    }

    //  The message must be at least reasonably formed...
    if( (str_buf[0] != '$')  &&  (str_buf[0] != '!') )
        return;

    if( event.GetStream() )
    {
        if(!event.GetStream()->ChecksumOK(event.GetNMEAString()) )
        {
            if( g_nNMEADebug && ( g_total_NMEAerror_messages < g_nNMEADebug ) )
            {
                g_total_NMEAerror_messages++;
                wxString msg( _T(">>>>>>NMEA Sentence Checksum Bad...") );
                msg.Append( str_buf );
                wxLogMessage( msg );
            }
            return;
        }
    }

    bool b_accept = EvalPriority( str_buf, event.GetStream() );
    if( b_accept ) {
        m_NMEA0183 << str_buf;
        if( m_NMEA0183.PreParse() )
        {
            if( m_NMEA0183.LastSentenceIDReceived == _T("RMC") )
            {
                if( m_NMEA0183.Parse() )
                {
                    if( m_NMEA0183.Rmc.IsDataValid == NTrue )
                    {
                        if( !wxIsNaN(m_NMEA0183.Rmc.Position.Latitude.Latitude) )
                        {
                            double llt = m_NMEA0183.Rmc.Position.Latitude.Latitude;
                            int lat_deg_int = (int) ( llt / 100 );
                            double lat_deg = lat_deg_int;
                            double lat_min = llt - ( lat_deg * 100 );
                            gLat = lat_deg + ( lat_min / 60. );
                            if( m_NMEA0183.Rmc.Position.Latitude.Northing == South ) gLat = -gLat;
                        }
                        else
                            ll_valid = false;

                        if( !wxIsNaN(m_NMEA0183.Rmc.Position.Longitude.Longitude) )
                        {
                            double lln = m_NMEA0183.Rmc.Position.Longitude.Longitude;
                            int lon_deg_int = (int) ( lln / 100 );
                            double lon_deg = lon_deg_int;
                            double lon_min = lln - ( lon_deg * 100 );
                            gLon = lon_deg + ( lon_min / 60. );
                            if( m_NMEA0183.Rmc.Position.Longitude.Easting == West )
                                gLon = -gLon;
                        }
                        else
                            ll_valid = false;

                        gSog = m_NMEA0183.Rmc.SpeedOverGroundKnots;
                        gCog = m_NMEA0183.Rmc.TrackMadeGoodDegreesTrue;

                        if( !wxIsNaN(m_NMEA0183.Rmc.MagneticVariation) )
                        {
                            if( m_NMEA0183.Rmc.MagneticVariationDirection == East )
                                gVar = m_NMEA0183.Rmc.MagneticVariation;
                            else
                                if( m_NMEA0183.Rmc.MagneticVariationDirection == West )
                                    gVar = -m_NMEA0183.Rmc.MagneticVariation;

                            g_bVAR_Rx = true;
                            gVAR_Watchdog = gps_watchdog_timeout_ticks;
                        }

                        sfixtime = m_NMEA0183.Rmc.UTCTime;

                        if(ll_valid )
                        {
                            gGPS_Watchdog = gps_watchdog_timeout_ticks;
                            wxDateTime now = wxDateTime::Now();
                            m_fixtime = now.GetTicks();
                        }
                        pos_valid = ll_valid;
                    }
                }
                else
                    if( g_nNMEADebug )
                    {
                        wxString msg( _T("   ") );
                        msg.Append( m_NMEA0183.ErrorMessage );
                        msg.Append( _T(" : ") );
                        msg.Append( str_buf );
                        wxLogMessage( msg );
                    }
            }

            else
                if( m_NMEA0183.LastSentenceIDReceived == _T("HDT") )
                {
                    if( m_NMEA0183.Parse() )
                    {
                        gHdt = m_NMEA0183.Hdt.DegreesTrue;
                        if( !wxIsNaN(m_NMEA0183.Hdt.DegreesTrue) )
                        {
                            g_bHDT_Rx = true;
                            gHDT_Watchdog = gps_watchdog_timeout_ticks;
                        }
                    }
                    else
                        if( g_nNMEADebug )
                        {
                            wxString msg( _T("   ") );
                            msg.Append( m_NMEA0183.ErrorMessage );
                            msg.Append( _T(" : ") );
                            msg.Append( str_buf );
                            wxLogMessage( msg );
                        }
                }
                else
                    if( m_NMEA0183.LastSentenceIDReceived == _T("HDG") )
                    {
                        if( m_NMEA0183.Parse() )
                        {
                            gHdm = m_NMEA0183.Hdg.MagneticSensorHeadingDegrees;
                            if( !wxIsNaN(m_NMEA0183.Hdg.MagneticSensorHeadingDegrees) )
                                gHDx_Watchdog = gps_watchdog_timeout_ticks;

                            if( m_NMEA0183.Hdg.MagneticVariationDirection == East )
                                gVar = m_NMEA0183.Hdg.MagneticVariationDegrees;
                            else if( m_NMEA0183.Hdg.MagneticVariationDirection == West )
                                gVar = -m_NMEA0183.Hdg.MagneticVariationDegrees;

                            if( !wxIsNaN(m_NMEA0183.Hdg.MagneticVariationDegrees) )
                            {
                                g_bVAR_Rx = true;
                                gVAR_Watchdog = gps_watchdog_timeout_ticks;
                            }


                        } else
                            if( g_nNMEADebug )
                            {
                                wxString msg( _T("   ") );
                                msg.Append( m_NMEA0183.ErrorMessage );
                                msg.Append( _T(" : ") );
                                msg.Append( str_buf );
                                wxLogMessage( msg );
                            }

                    }
                    else
                        if( m_NMEA0183.LastSentenceIDReceived == _T("HDM") )
                        {
                            if( m_NMEA0183.Parse() )
                            {
                                gHdm = m_NMEA0183.Hdm.DegreesMagnetic;
                                if( !wxIsNaN(m_NMEA0183.Hdm.DegreesMagnetic) )
                                    gHDx_Watchdog = gps_watchdog_timeout_ticks;
                            }
                            else
                                if( g_nNMEADebug )
                                {
                                    wxString msg( _T("   ") );
                                    msg.Append( m_NMEA0183.ErrorMessage );
                                    msg.Append( _T(" : ") );
                                    msg.Append( str_buf );
                                    wxLogMessage( msg );
                                }
                        }
                        else
                            if( m_NMEA0183.LastSentenceIDReceived == _T("VTG") )
                            {
                                if( m_NMEA0183.Parse() )
                                {
                                    if( !wxIsNaN(m_NMEA0183.Vtg.SpeedKnots) )
                                        gSog = m_NMEA0183.Vtg.SpeedKnots;
                                    if( !wxIsNaN(m_NMEA0183.Vtg.TrackDegreesTrue) )
                                        gCog = m_NMEA0183.Vtg.TrackDegreesTrue;
                                    if( !wxIsNaN(m_NMEA0183.Vtg.SpeedKnots) && !wxIsNaN(m_NMEA0183.Vtg.TrackDegreesTrue) )
                                        gGPS_Watchdog = gps_watchdog_timeout_ticks;
                                }
                                else
                                    if( g_nNMEADebug )
                                    {
                                        wxString msg( _T("   ") );
                                        msg.Append( m_NMEA0183.ErrorMessage );
                                        msg.Append( _T(" : ") );
                                        msg.Append( str_buf );
                                        wxLogMessage( msg );
                                    }
                            }
                            else
                                if( m_NMEA0183.LastSentenceIDReceived == _T("GSV") )
                                {
                                    if( m_NMEA0183.Parse() )
                                    {
                                        g_SatsInView = m_NMEA0183.Gsv.SatsInView;
                                        gSAT_Watchdog = sat_watchdog_timeout_ticks;
                                        g_bSatValid = true;
                                    }
                                    else
                                        if( g_nNMEADebug )
                                        {
                                            wxString msg( _T("   ") );
                                            msg.Append( m_NMEA0183.ErrorMessage );
                                            msg.Append( _T(" : ") );
                                            msg.Append( str_buf );
                                            wxLogMessage( msg );
                                        }
                                }
                                else
                                    if( g_bUseGLL && m_NMEA0183.LastSentenceIDReceived == _T("GLL") )
                                    {
                                        if( m_NMEA0183.Parse() )
                                        {
                                            if( m_NMEA0183.Gll.IsDataValid == NTrue )
                                            {
                                                if( !wxIsNaN(m_NMEA0183.Gll.Position.Latitude.Latitude) )
                                                {
                                                    double llt = m_NMEA0183.Gll.Position.Latitude.Latitude;
                                                    int lat_deg_int = (int) ( llt / 100 );
                                                    double lat_deg = lat_deg_int;
                                                    double lat_min = llt - ( lat_deg * 100 );
                                                    gLat = lat_deg + ( lat_min / 60. );
                                                    if( m_NMEA0183.Gll.Position.Latitude.Northing
                                                            == South ) gLat = -gLat;
                                                }
                                                else
                                                    ll_valid = false;

                                                if( !wxIsNaN(m_NMEA0183.Gll.Position.Longitude.Longitude) )
                                                {
                                                    double lln = m_NMEA0183.Gll.Position.Longitude.Longitude;
                                                    int lon_deg_int = (int) ( lln / 100 );
                                                    double lon_deg = lon_deg_int;
                                                    double lon_min = lln - ( lon_deg * 100 );
                                                    gLon = lon_deg + ( lon_min / 60. );
                                                    if( m_NMEA0183.Gll.Position.Longitude.Easting == West )
                                                        gLon = -gLon;
                                                }
                                                else
                                                    ll_valid = false;

                                                sfixtime = m_NMEA0183.Gll.UTCTime;

                                                if(ll_valid)
                                                {
                                                    gGPS_Watchdog = gps_watchdog_timeout_ticks;
                                                    wxDateTime now = wxDateTime::Now();
                                                    m_fixtime = now.GetTicks();
                                                }
                                                pos_valid = ll_valid;
                                            }
                                        } else
                                            if( g_nNMEADebug )
                                            {
                                                wxString msg( _T("   ") );
                                                msg.Append( m_NMEA0183.ErrorMessage );
                                                msg.Append( _T(" : ") );
                                                msg.Append( str_buf );
                                                wxLogMessage( msg );
                                            }
                                    }
                                    else
                                        if( m_NMEA0183.LastSentenceIDReceived == _T("GGA") )
                                        {
                                            if( m_NMEA0183.Parse() )
                                            {
                                                if( m_NMEA0183.Gga.GPSQuality > 0 )
                                                {
                                                    if( !wxIsNaN(m_NMEA0183.Gga.Position.Latitude.Latitude) )
                                                    {
                                                        double llt = m_NMEA0183.Gga.Position.Latitude.Latitude;
                                                        int lat_deg_int = (int) ( llt / 100 );
                                                        double lat_deg = lat_deg_int;
                                                        double lat_min = llt - ( lat_deg * 100 );
                                                        gLat = lat_deg + ( lat_min / 60. );
                                                        if( m_NMEA0183.Gga.Position.Latitude.Northing == South )
                                                            gLat = -gLat;
                                                    }
                                                    else
                                                        ll_valid = false;

                                                    if( !wxIsNaN(m_NMEA0183.Gga.Position.Longitude.Longitude) )
                                                    {
                                                        double lln = m_NMEA0183.Gga.Position.Longitude.Longitude;
                                                        int lon_deg_int = (int) ( lln / 100 );
                                                        double lon_deg = lon_deg_int;
                                                        double lon_min = lln - ( lon_deg * 100 );
                                                        gLon = lon_deg + ( lon_min / 60. );
                                                        if( m_NMEA0183.Gga.Position.Longitude.Easting
                                                                == West ) gLon = -gLon;
                                                    }
                                                    else
                                                        ll_valid = false;

                                                    sfixtime = m_NMEA0183.Gga.UTCTime;

                                                    if(ll_valid)
                                                    {
                                                        gGPS_Watchdog = gps_watchdog_timeout_ticks;
                                                        wxDateTime now = wxDateTime::Now();
                                                        m_fixtime = now.GetTicks();
                                                    }
                                                    pos_valid = ll_valid;

                                                    g_SatsInView = m_NMEA0183.Gga.NumberOfSatellitesInUse;
                                                    gSAT_Watchdog = sat_watchdog_timeout_ticks;
                                                    g_bSatValid = true;

                                                }
                                            } else
                                                if( g_nNMEADebug )
                                                {
                                                    wxString msg( _T("   ") );
                                                    msg.Append( m_NMEA0183.ErrorMessage );
                                                    msg.Append( _T(" : ") );
                                                    msg.Append( str_buf );
                                                    wxLogMessage( msg );
                                                }
                                        }
        }
        //      Process ownship (AIVDO) messages from any source
        else if(str_buf.Mid( 1, 5 ).IsSameAs( _T("AIVDO") ) )
        {
            GenericPosDatEx gpd;
            AIS_Error nerr = AIS_GENERIC_ERROR;
            if(g_pAIS)
                nerr = g_pAIS->DecodeSingleVDO(str_buf, &gpd, &m_VDO_accumulator);

            if(nerr == AIS_NoError)
            {
                if( !wxIsNaN(gpd.kLat) )
                    gLat = gpd.kLat;
                if( !wxIsNaN(gpd.kLon) )
                    gLon = gpd.kLon;

                gCog = gpd.kCog;
                gSog = gpd.kSog;

                gHdt = gpd.kHdt;
                if( !wxIsNaN(gpd.kHdt) )
                {
                    g_bHDT_Rx = true;
                    gHDT_Watchdog = gps_watchdog_timeout_ticks;
                }

                if( !wxIsNaN(gpd.kLat) && !wxIsNaN(gpd.kLon) )
                {
                    gGPS_Watchdog = gps_watchdog_timeout_ticks;
                    wxDateTime now = wxDateTime::Now();
                    m_fixtime = now.GetTicks();

                    pos_valid = true;
                }
            }
            else
            {
                if( g_nNMEADebug && ( g_total_NMEAerror_messages < g_nNMEADebug ) )
                {
                    g_total_NMEAerror_messages++;
                    wxString msg( _T("   Invalid AIVDO Sentence...") );
                    msg.Append( str_buf );
                    wxLogMessage( msg );
                }
            }
        }
        else
        {
            bis_recognized_sentence = false;
            if( g_nNMEADebug && ( g_total_NMEAerror_messages < g_nNMEADebug ) )
            {
                g_total_NMEAerror_messages++;
                wxString msg( _T("   Unrecognized NMEA Sentence...") );
                msg.Append( str_buf );
                wxLogMessage( msg );
            }
        }

        if( bis_recognized_sentence ) PostProcessNNEA( pos_valid, sfixtime );
    }
}

void MyFrame::PostProcessNNEA( bool pos_valid, const wxString &sfixtime )
{
    FilterCogSog();

    //    If gSog is greater than some threshold, we determine that we are "cruising"
    if( gSog > 3.0 ) g_bCruising = true;

    //    Here is the one place we try to create gHdt from gHdm and gVar,
    //    but only if NMEA HDT sentence is not being received

    if( !g_bHDT_Rx ) {
        if( !wxIsNaN(gVar) && !wxIsNaN(gHdm)) {
            gHdt = gHdm + gVar;
            gHDT_Watchdog = gps_watchdog_timeout_ticks;
        }
    }

    if( pos_valid ) {
        if( g_nNMEADebug ) {
            wxString msg( _T("PostProcess NMEA with valid position") );
            wxLogMessage( msg );
        }

        //      Maintain the validity flags
        bool last_bGPSValid = bGPSValid;
        bGPSValid = true;
        if( !last_bGPSValid ) UpdateGPSCompassStatusBox();

        //      Show a little heartbeat tick in StatusWindow0 on NMEA events
        //      But no faster than 10 hz.
        unsigned long uiCurrentTickCount;
        m_MMEAeventTime.SetToCurrent();
        uiCurrentTickCount = m_MMEAeventTime.GetMillisecond() / 100;           // tenths of a second
        uiCurrentTickCount += m_MMEAeventTime.GetTicks() * 10;
        if( uiCurrentTickCount > m_ulLastNEMATicktime + 1 ) {
            m_ulLastNEMATicktime = uiCurrentTickCount;

            if( tick_idx++ > 6 ) tick_idx = 0;
        }
    }

//    Show gLat/gLon in StatusWindow0

    if( NULL != GetStatusBar() ) {
        char tick_buf[2];
        tick_buf[0] = nmea_tick_chars[tick_idx];
        tick_buf[1] = 0;

        wxString s1( tick_buf, wxConvUTF8 );
        s1 += _(" Ship ");
        s1 += toSDMM( 1, gLat );
        s1 += _T("   ");
        s1 += toSDMM( 2, gLon );
        SetStatusText( s1, STAT_FIELD_TICK );

        wxString sogcog;
        if( wxIsNaN(gSog) ) sogcog.Printf( _T("SOG --- ") + getUsrSpeedUnit() + _T("  ") );
        else
            sogcog.Printf( _T("SOG %2.2f ") + getUsrSpeedUnit() + _T("  "), toUsrSpeed( gSog ) );

        wxString cogs;
        if( wxIsNaN(gCog) )
            cogs.Printf( wxString( "COG ---\u00B0", wxConvUTF8 ) );
        else {
            if( g_bShowMag )
                cogs << wxString::Format( wxString("COG %03d°(M)  ", wxConvUTF8 ), (int)gFrame->GetTrueOrMag( gCog ) );
            else
                cogs << wxString::Format( wxString("COG %03d°  ", wxConvUTF8 ), (int)gFrame->GetTrueOrMag( gCog ) );
        }

        sogcog.Append( cogs );
        SetStatusText( sogcog, STAT_FIELD_SOGCOG );
    }

//    Maintain average COG for Course Up Mode

    if( !wxIsNaN(gCog) ) {
        if( g_COGAvgSec > 0 ) {
            //    Make a hole
            for( int i = g_COGAvgSec - 1; i > 0; i-- )
                COGTable[i] = COGTable[i - 1];
            COGTable[0] = gCog;

            double sum = 0.;
            for( int i = 0; i < g_COGAvgSec; i++ ) {
                double adder = COGTable[i];

                if( fabs( adder - g_COGAvg ) > 180. ) {
                    if( ( adder - g_COGAvg ) > 0. ) adder -= 360.;
                    else
                        adder += 360.;
                }

                sum += adder;
            }
            sum /= g_COGAvgSec;

            if( sum < 0. ) sum += 360.;
            else
                if( sum >= 360. ) sum -= 360.;

            g_COGAvg = sum;
        }
        else
            g_COGAvg = gCog;
    }

#ifdef ocpnUPDATE_SYSTEM_TIME
//      Use the fix time to update the local system clock, only once per session
    if( ( sfixtime.Len() ) && s_bSetSystemTime && ( m_bTimeIsSet == false ) ) {
        wxDateTime Fix_Time;

        if( 6 == sfixtime.Len() )                   // perfectly recognised format?
                {
            wxString a;
            long b;
            int hr = 0;
            int min = 0;
            int sec = 0;

            a = sfixtime.Mid( 0, 2 );
            if( a.ToLong( &b ) ) hr = b;
            a = sfixtime.Mid( 2, 2 );
            if( a.ToLong( &b ) ) min = b;
            a = sfixtime.Mid( 4, 2 );
            if( a.ToLong( &b ) ) sec = b;

            Fix_Time.Set( hr, min, sec );
        }
        wxString fix_time_format = Fix_Time.Format( _T("%Y-%m-%dT%H:%M:%S") ); // this should show as LOCAL

        //          Compare the server (fix) time to the current system time
        wxDateTime sdt;
        sdt.SetToCurrent();
        wxDateTime cwxft = Fix_Time;                  // take a copy
        wxTimeSpan ts;
        ts = cwxft.Subtract( sdt );

        int b = ( ts.GetSeconds() ).ToLong();

        //          Correct system time if necessary
        //      Only set the time if wrong by more than 1 minute, and less than 2 hours
        //      This should eliminate bogus times which may come from faulty GPS units

        if( ( abs( b ) > 60 ) && ( abs( b ) < ( 2 * 60 * 60 ) ) ) {

#ifdef __WXMSW__
            //      Fix up the fix_time to convert to GMT
            Fix_Time = Fix_Time.ToGMT();

            //    Code snippet following borrowed from wxDateCtrl, MSW

            const wxDateTime::Tm tm( Fix_Time.GetTm() );

            SYSTEMTIME stm;
            stm.wYear = (WXWORD) tm.year;
            stm.wMonth = (WXWORD) ( tm.mon - wxDateTime::Jan + 1 );
            stm.wDay = tm.mday;

            stm.wDayOfWeek = 0;
            stm.wHour = Fix_Time.GetHour();
            stm.wMinute = tm.min;
            stm.wSecond = tm.sec;
            stm.wMilliseconds = 0;

            ::SetSystemTime( &stm );            // in GMT

#else

            //      This contortion sets the system date/time on POSIX host
            //      Requires the following line in /etc/sudoers
            //          nav ALL=NOPASSWD:/bin/date -s *

            wxString msg;
            msg.Printf(_T("Setting system time, delta t is %d seconds"), b);
            wxLogMessage(msg);

            wxString sdate(Fix_Time.Format(_T("%D")));
            sdate.Prepend(_T("sudo /bin/date -s \""));

            wxString stime(Fix_Time.Format(_T("%T")));
            stime.Prepend(_T(" "));
            sdate.Append(stime);
            sdate.Append(_T("\""));

            msg.Printf(_T("Linux command is:"));
            msg += sdate;
            wxLogMessage(msg);
            wxExecute(sdate, wxEXEC_ASYNC);

#endif      //__WXMSW__
            m_bTimeIsSet = true;

        }           // if needs correction
    }               // if valid time

#endif            //ocpnUPDATE_SYSTEM_TIME
}

void MyFrame::FilterCogSog( void )
{
    if( g_bfilter_cogsog ) {
        //    If the data are undefined, leave the array intact
        if( !wxIsNaN(gCog) ) {
            //    Simple averaging filter for COG
            double cog_last = gCog;       // most recent reported value

            //    Make a hole in array
            for( int i = g_COGFilterSec - 1; i > 0; i-- )
                COGFilterTable[i] = COGFilterTable[i - 1];
            COGFilterTable[0] = cog_last;

            //
            double sum = 0.;
            for( int i = 0; i < g_COGFilterSec; i++ ) {
                double adder = COGFilterTable[i];

                if( fabs( adder - m_COGFilterLast ) > 180. ) {
                    if( ( adder - m_COGFilterLast ) > 0. ) adder -= 360.;
                    else
                        adder += 360.;
                }

                sum += adder;
            }
            sum /= g_COGFilterSec;

            if( sum < 0. ) sum += 360.;
            else
                if( sum >= 360. ) sum -= 360.;

            gCog = sum;
            m_COGFilterLast = sum;
        }

        //    If the data are undefined, leave the array intact
        if( !wxIsNaN(gSog) ) {
            //    Simple averaging filter for SOG
            double sog_last = gSog;       // most recent reported value

            //    Make a hole in array
            for( int i = g_SOGFilterSec - 1; i > 0; i-- )
                SOGFilterTable[i] = SOGFilterTable[i - 1];
            SOGFilterTable[0] = sog_last;

            double sum = 0.;
            for( int i = 0; i < g_SOGFilterSec; i++ ) {
                sum += SOGFilterTable[i];
            }
            sum /= g_SOGFilterSec;

            gSog = sum;
        }
    }
}

void MyFrame::StopSockets( void )
{
//TODO: Can be removed?
}

void MyFrame::ResumeSockets( void )
{
//TODO: Can be removed?
}

void MyFrame::LoadHarmonics()
{
    if(!ptcmgr) {
        ptcmgr = new TCMgr;
        ptcmgr->LoadDataSources(TideCurrentDataSet);
    }
    else {
        bool b_newdataset = false;

        //      Test both ways
        wxArrayString test = ptcmgr->GetDataSet();
        for(unsigned int i=0 ; i < test.GetCount() ; i++) {
            bool b_foundi = false;
            for(unsigned int j=0 ; j < TideCurrentDataSet.GetCount() ; j++) {
                if(TideCurrentDataSet.Item(j) == test.Item(i)) {
                    b_foundi = true;
                    break;              // j loop
                }
            }
            if(!b_foundi) {
                b_newdataset = true;
                break;                  //  i loop
            }
        }

        test = TideCurrentDataSet;
        for(unsigned int i=0 ; i < test.GetCount() ; i++) {
            bool b_foundi = false;
            for(unsigned int j=0 ; j < ptcmgr->GetDataSet().GetCount() ; j++) {
                if(ptcmgr->GetDataSet().Item(j) == test.Item(i)) {
                    b_foundi = true;
                    break;              // j loop
                }
            }
            if(!b_foundi) {
                b_newdataset = true;
                break;                  //  i loop
            }
        }

        if(b_newdataset)
            ptcmgr->LoadDataSources(TideCurrentDataSet);
    }
}

Route *pAISMOBRoute;

void MyFrame::ActivateAISMOBRoute( AIS_Target_Data *ptarget )
{
    if(!ptarget)
        return;

    //    The MOB point
    wxDateTime mob_time = wxDateTime::Now();
    wxString mob_label( _( "AIS MAN OVERBOARD" ) );
    mob_label += _T(" at ");
    mob_label += mob_time.FormatTime();

    RoutePoint *pWP_MOB = new RoutePoint( ptarget->Lat, ptarget->Lon, _T ( "mob" ), mob_label, GPX_EMPTY_STRING );
    pWP_MOB->m_bKeepXRoute = true;
    pWP_MOB->m_bIsolatedMark = true;
    pSelect->AddSelectableRoutePoint( ptarget->Lat, ptarget->Lon, pWP_MOB );
    pConfig->AddNewWayPoint( pWP_MOB, -1 );       // use auto next num


    if( bGPSValid && !wxIsNaN(gCog) && !wxIsNaN(gSog) ) {
        RoutePoint *pWP_src = new RoutePoint( gLat, gLon, g_default_wp_icon,
                                              wxString( _( "Ownship" ) ), GPX_EMPTY_STRING );
        pSelect->AddSelectableRoutePoint( gLat, gLon, pWP_src );

        pAISMOBRoute = new Route();
        pRouteList->Append( pAISMOBRoute );

        pAISMOBRoute->AddPoint( pWP_src );
        pAISMOBRoute->AddPoint( pWP_MOB );

        pSelect->AddSelectableRouteSegment(ptarget->Lat, ptarget->Lon, gLat, gLon, pWP_src, pWP_MOB, pAISMOBRoute );

        pAISMOBRoute->m_RouteNameString = _("Temporary AISMOB Route");
        pAISMOBRoute->m_RouteStartString = _("Present Ownship");
        pAISMOBRoute->m_RouteEndString = mob_label;

        pAISMOBRoute->m_bDeleteOnArrival = false;

        pAISMOBRoute->SetRouteArrivalRadius( -1.0 );                    // never arrives

        pAISMOBRoute->RebuildGUIDList();         // ensure the GUID list is intact and good

        if( g_pRouteMan->GetpActiveRoute() )
            g_pRouteMan->DeactivateRoute();
        //       g_pRouteMan->ActivateRoute( pAISMOBRoute, pWP_MOB );

        wxJSONValue v;
        v[_T("GUID")] = pAISMOBRoute->m_GUID;
        wxString msg_id( _T("OCPN_MAN_OVERBOARD") );
        g_pi_manager->SendJSONMessageToAllPlugins( msg_id, v );
    }

    if( pRouteManagerDialog && pRouteManagerDialog->IsShown() ) {
        pRouteManagerDialog->UpdateRouteListCtrl();
        pRouteManagerDialog->UpdateWptListCtrl();
    }

    cc1->Refresh( false );

    wxString mob_message( _( "AIS MAN OVERBOARD" ) );
    mob_message += _T(" Time: ");
    mob_message += mob_time.Format();
    mob_message += _T("  Ownship Position: ");
    mob_message += toSDMM( 1, gLat );
    mob_message += _T("   ");
    mob_message += toSDMM( 2, gLon );
    mob_message += _T("  MOB Position: ");
    mob_message += toSDMM( 1, ptarget->Lat );
    mob_message += _T("   ");
    mob_message += toSDMM( 2, ptarget->Lon );
    wxLogMessage( mob_message );

}

void MyFrame::UpdateAISMOBRoute( AIS_Target_Data *ptarget )
{
    if(pAISMOBRoute && ptarget)
    {
        //   Update Current Ownship point
        RoutePoint *OwnPoint = pAISMOBRoute->GetPoint( 1 );
        OwnPoint->m_lat = gLat;
        OwnPoint->m_lon = gLon;

        pSelect->DeleteSelectableRoutePoint( OwnPoint );
        pSelect->AddSelectableRoutePoint( gLat, gLon, OwnPoint );

        //   Update Current MOB point
        RoutePoint *MOB_Point = pAISMOBRoute->GetPoint( 2 );
        MOB_Point->m_lat = ptarget->Lat;
        MOB_Point->m_lon = ptarget->Lon;

        pSelect->DeleteSelectableRoutePoint( MOB_Point );
        pSelect->AddSelectableRoutePoint( ptarget->Lat, ptarget->Lon, MOB_Point );

        pSelect->UpdateSelectableRouteSegments( OwnPoint );
        pSelect->UpdateSelectableRouteSegments( MOB_Point );
    }

    cc1->Refresh( false );

    if( ptarget ){
        wxDateTime mob_time = wxDateTime::Now();

        wxString mob_message( _( "AIS MAN OVERBOARD UPDATE" ) );
        mob_message += _T(" Time: ");
        mob_message += mob_time.Format();
        mob_message += _T("  Ownship Position: ");
        mob_message += toSDMM( 1, gLat );
        mob_message += _T("   ");
        mob_message += toSDMM( 2, gLon );
        mob_message += _T("  MOB Position: ");
        mob_message += toSDMM( 1, ptarget->Lat );
        mob_message += _T("   ");
        mob_message += toSDMM( 2, ptarget->Lon );

        wxLogMessage( mob_message );
    }

}


#ifdef wxHAS_POWER_EVENTS
void MyFrame::OnSuspending(wxPowerEvent& event)
{
 //   wxDateTime now = wxDateTime::Now();
 //   printf("OnSuspending...%d\n", now.GetTicks());

    wxLogMessage(_T("System suspend starting..."));
}

void MyFrame::OnSuspended(wxPowerEvent& WXUNUSED(event))
{
//    wxDateTime now = wxDateTime::Now();
//    printf("OnSuspended...%d\n", now.GetTicks());
    wxLogMessage(_T("System is going to suspend."));
    
}

void MyFrame::OnSuspendCancel(wxPowerEvent& WXUNUSED(event))
{
//    wxDateTime now = wxDateTime::Now();
//    printf("OnSuspendCancel...%d\n", now.GetTicks());
    wxLogMessage(_T("System suspend was cancelled."));
}

int g_last_resume_ticks;
void MyFrame::OnResume(wxPowerEvent& WXUNUSED(event))
{
    wxDateTime now = wxDateTime::Now();
//    printf("OnResume...%d\n", now.GetTicks());
    wxLogMessage(_T("System resumed from suspend."));

    if((now.GetTicks() - g_last_resume_ticks) > 5){
        wxLogMessage(_T("Restarting streams."));
 //       printf("   Restarting streams\n");
        g_last_resume_ticks = now.GetTicks();
        if(g_pMUX){
            g_pMUX->ClearStreams();

            g_pMUX->StartAllStreams();
        }
    }

    //  If OpenGL is enabled, Windows Resume does not properly refresh the application GL context.
    //  We need to force a Resize event that actually does something.
    if(g_bopengl){
        if( IsMaximized() ){            // This is not real pretty on-screen, but works
            Maximize(false);
            wxYield();
            Maximize(true);
        }
        else {
            wxSize sz = GetSize();
            SetSize( wxSize(sz.x - 1, sz.y));
            wxYield();
            SetSize( sz );
        }
    }
    
}
#endif // wxHAS_POWER_EVENTS
