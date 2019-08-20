#include <math.h>
#include <windows.h>
#include "curve.h"

DRAWING_DATA gDrawData; // global data
void drawCubicCurve();

void reDraw(HWND hwnd)
{
  InvalidateRect(hwnd, NULL, 1);
}

void drawPoint(int x, int y)
{
    Ellipse(gDrawData.hdcMem,x,y,x,y);
}

void setupMenus(HWND hwnd)
{
  HMENU hMenu = GetMenu(hwnd);

  switch (gDrawData.drawMode)
  {
    case READY_MODE :
      // enable cubic curve menu
      EnableMenuItem(hMenu, ID_CUBIC_CURVE, MF_BYCOMMAND|MF_ENABLED);
      break;
    case DRAW_MODE :
    case DRAWN_MODE :
      // disable cubic curve menu once it is drawn on the screen 
      EnableMenuItem(hMenu, ID_CUBIC_CURVE, MF_BYCOMMAND|MF_GRAYED);
      break;		  
  }
}

void setDrawMode(MODE mode, HWND hwnd)
{
  gDrawData.drawMode = mode;
  setupMenus(hwnd);
}

void createMemoryBitmap(HDC hdc)
{
  gDrawData.hdcMem = CreateCompatibleDC(hdc);
  gDrawData.hbmp = CreateCompatibleBitmap(hdc, 
    gDrawData.maxBoundary.cx, gDrawData.maxBoundary.cy);
  SelectObject(gDrawData.hdcMem, gDrawData.hbmp);
  PatBlt(gDrawData.hdcMem, 0, 0, gDrawData.maxBoundary.cx, 
         gDrawData.maxBoundary.cy, PATCOPY);
}

void initialize(HWND hwnd, HDC hdc)
{
  gDrawData.hDrawPen=CreatePen(PS_SOLID, 1, RGB(0, 0, 0));

  gDrawData.maxBoundary.cx = GetSystemMetrics(SM_CXSCREEN);
  gDrawData.maxBoundary.cy = GetSystemMetrics(SM_CYSCREEN);

  createMemoryBitmap(hdc);
  setDrawMode(READY_MODE, hwnd);
}

void cleanup()
{
  DeleteDC(gDrawData.hdcMem);
}

LRESULT CALLBACK DlgAxis(HWND hdlg,UINT mess,WPARAM more,LPARAM pos)
{
  char str[20];
  switch(mess)
  {
    case WM_COMMAND:
      switch(more)
      {
        case ID_OK:
          GetDlgItemText(hdlg,ID_EB1,str,20);
          gDrawData.scale=(atof(str));
          if(gDrawData.scale < 1.0 || 
             gDrawData.scale > 1.5 )
          {
            MessageBox(hdlg,
              "Scale should be between 1 and 1.5", 
              "Warning!",MB_ICONERROR);
               gDrawData.scale = 0.0;
          } 
          gDrawData.axis = (BST_CHECKED == IsDlgButtonChecked(hdlg,ID_LABEL2));
          EndDialog(hdlg,TRUE);
          return 1;
          break;

        case ID_CANCEL:
          EndDialog(hdlg,TRUE);
          break;  
      }
      break;
    default:
      break;
  }
  return 0;
}

void reset(HWND hwnd)
{
  gDrawData.origin.x = gDrawData.origin.y = 0;

  gDrawData.drawMode = READY_MODE;

  PatBlt(gDrawData.hdcMem, 0, 0, gDrawData.maxBoundary.cx, 
         gDrawData.maxBoundary.cy, PATCOPY);

  reDraw(hwnd);
  setupMenus(hwnd);
}

void plot_sympoint(int ex, int ey, COLORREF clr)
{
  int cx = gDrawData.origin.x;
  int cy = gDrawData.origin.y;
	
  SetPixel(gDrawData.hdcMem, cx-ex,cy+ey, clr);
  SetPixel(gDrawData.hdcMem, cx+ex,cy-ey, clr);
}

void addPoint(HWND hwnd, int x, int y)
{
  switch (gDrawData.drawMode)
  {
    case DRAW_MODE:
      /* the coordinates of the centre is stored 
         and the cubic curvr is drawn */
     if(gDrawData.scale > 0.0){
      SelectObject(gDrawData.hdcMem, gDrawData.hDrawPen);
      drawPoint(x,y);
      gDrawData.origin.x = x;
      gDrawData.origin.y = y;
      drawCubicCurve();
      setDrawMode(DRAWN_MODE, hwnd);
      reDraw(hwnd);
  	 } else {
	 	setDrawMode(DRAWN_MODE, hwnd);
	 	MessageBox(hwnd,
       "Input parameters were not valid clear the area to start afresh", 
          "Warning",MB_OK);
	 }
      break;
    case DRAWN_MODE:
      MessageBox(hwnd,
       "Curve is already drawn, now you can clear the area", 
          "Warning",MB_OK);
      break;
    default:
      break;
  }
}

void drawImage(HDC hdc)
{
  BitBlt(hdc, 0, 0, gDrawData.maxBoundary.cx, 
    gDrawData.maxBoundary.cy, gDrawData.hdcMem, 
    0, 0, SRCCOPY);
}

void processLeftButtonDown(HWND hwnd, int x, int y)
{
  addPoint(hwnd,x,y);
}

void processCommand(int cmd, HWND hwnd)
{
  int response;
  switch(cmd)
  {
    case ID_CLEAR:
      reset(hwnd);
      setDrawMode(READY_MODE, hwnd);
      break;
    case ID_CUBIC_CURVE:
     setDrawMode(DRAW_MODE, hwnd);
      DialogBox(NULL,"MyDB",hwnd,(DLGPROC)DlgAxis);	
      break;
    case ID_EXIT:
        response=MessageBox(hwnd,
          "Quit the program?", "EXIT", 
          MB_YESNO);
        if(response==IDYES) 
            PostQuitMessage(0);
        break;
    default:
      break;
  }
}

void drawCubicCurve()
{
	int x,y;
	x=0;
	y=0;	
	int p = -5; // calculated from initial x and y value.
	while(x <= 2*gDrawData.scale){
		plot_sympoint(x,y, RGB(0,0,0));
		if(p > 0 ){
			// decision parameter is scaled properly to avoid floating point calculations
			p = p + 3*x*x + 9*x - 5;
			y++;
		} else {
			p = p + 3*x*x + 9*x + 7;
		}		
		x++;
	}
	// multiplied by a factor 96 to avoid floating point calculations.
	p = 96*(y+1) - (2*x+1)*(2*x+1)*(2*x+1);
	while(x <= 10*gDrawData.scale ){
		plot_sympoint(x,y, RGB(0,0,0));
		if(p > 0){
		 p = p + 96 - 24*x*x- 48*x-26;
		 x++;
		} else{
			p = p + 96;
		}
		y++;
	}
	if(gDrawData.axis){
		for(int i=0; i<=60*gDrawData.scale; i++){
			plot_sympoint(i,0, RGB(0,0,255));
		}
		for(int i=0;i<=100*pow(gDrawData.scale, 3.0);i++) 	plot_sympoint(0,i, RGB(0,0,255));
	}
}

LRESULT CALLBACK WindowF(HWND hwnd,UINT message,WPARAM wParam,
                         LPARAM lParam) 
{
  HDC hdc;
  PAINTSTRUCT ps;
  int x,y;

  switch(message)
  {
    case WM_CREATE:
      hdc = GetDC(hwnd);
      initialize(hwnd, hdc);
      ReleaseDC(hwnd, hdc);
      break;
    
    case WM_COMMAND:
      processCommand(LOWORD(wParam), hwnd);
      break;

    case WM_LBUTTONDOWN:
      x = LOWORD(lParam);
      y = HIWORD(lParam);
      processLeftButtonDown(hwnd, x,y);
      break;

    case WM_PAINT:
      hdc = BeginPaint(hwnd, &ps);
      drawImage(hdc);
      EndPaint(hwnd, &ps);
      break;

    case WM_DESTROY:
      cleanup();
      PostQuitMessage(0);
      break;
  }
  // Call the default window handler
  return DefWindowProc(hwnd, message, wParam, lParam);
}
