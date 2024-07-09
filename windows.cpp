#include <iostream>
#include <windows.h>
#include <stdint.h>
#include <dsound.h>
#include <math.h>
// #include <GL/gl.h>

static bool Running;
struct win32_offscreen_buffer
{
  BITMAPINFO Info;
  void *Memory;
  int Width;
  int Height;
  int Pitch;
  int BytesPerPixel;
};

static win32_offscreen_buffer GlobalBackBuffer;
static LPDIRECTSOUNDBUFFER GlobalSecondaryBuffer;

#define PI32 3.14159265359f
#define DIRECT_SOUND_CREATE(name) HRESULT WINAPI name(LPCGUID pcGuidDevice, LPDIRECTSOUND *ppDS, LPUNKNOWN pUnkOuter)
typedef DIRECT_SOUND_CREATE(direct_sound_create);

void Win32InitDSound(HWND Window, int32_t SamplesPerSecond, int32_t BufferSize)
{
  HMODULE DSoundLibrary = LoadLibraryA("dsound.dll");
  if (DSoundLibrary)
  {
    direct_sound_create *DirectSoundCreate = (direct_sound_create *)GetProcAddress(DSoundLibrary, "DirectSoundCreate");

    LPDIRECTSOUND DirectSound;
    if (DirectSoundCreate && SUCCEEDED(DirectSoundCreate(0, &DirectSound, 0)))
    {
      WAVEFORMATEX WaveFormat = {};
      WaveFormat.wFormatTag = WAVE_FORMAT_PCM;
      WaveFormat.nChannels = 2;
      WaveFormat.wBitsPerSample = 16;
      WaveFormat.nSamplesPerSec = SamplesPerSecond;
      WaveFormat.nBlockAlign = (WaveFormat.nChannels * WaveFormat.wBitsPerSample) / 8;
      WaveFormat.nAvgBytesPerSec = WaveFormat.nSamplesPerSec * WaveFormat.nBlockAlign;
      WaveFormat.cbSize = 0;

      LPDIRECTSOUNDBUFFER PrimaryBuffer;
      if (SUCCEEDED(DirectSound->SetCooperativeLevel(Window, DSSCL_PRIORITY)))
      {
        DSBUFFERDESC PrimBufferDescription = {};
        PrimBufferDescription.dwSize = sizeof(PrimBufferDescription);
        PrimBufferDescription.dwFlags = DSBCAPS_PRIMARYBUFFER;

        if (SUCCEEDED(DirectSound->CreateSoundBuffer(&PrimBufferDescription, &PrimaryBuffer, 0)))
        {
          if (SUCCEEDED(PrimaryBuffer->SetFormat(&WaveFormat)))
          {
            OutputDebugString("Primary buffer format was set.\n");
          }
          else
          {
            // Log error
          }
        }
        else
        {
          // Log error
        }
      }

      DSBUFFERDESC SecBufferDescription = {};
      SecBufferDescription.dwSize = sizeof(SecBufferDescription);
      SecBufferDescription.dwFlags = 0;
      SecBufferDescription.dwBufferBytes = BufferSize;
      SecBufferDescription.lpwfxFormat = &WaveFormat;

      if (SUCCEEDED(DirectSound->CreateSoundBuffer(&SecBufferDescription, &GlobalSecondaryBuffer, 0)))
      {
        OutputDebugString("Secondary buffer created.\n");
      }
      else
      {
        // Log error
      }
    }
    else
    {
      // TODO: Log error
    }
  }
  else
  {
    // TODO: Log error
  }
}

void RenderPixelAnimation(win32_offscreen_buffer Buffer, int XOffset, int YOffset)
{
  uint8_t *Row = (uint8_t *)Buffer.Memory;

  for (int Y = 0; Y < Buffer.Height; Y++)
  {
    uint32_t *Pixel = (uint32_t *)Row;

    for (int X = 0; X < Buffer.Width; X++)
    {
      // Shorthand method
      uint8_t Blue(X + XOffset);
      uint8_t Green(Y - YOffset);
      *Pixel++ = ((Green << 8) | Blue);

      // Individual colour channels
      // *Pixel++ = ((uint8_t)X + XOffset); // Blue // this seg faults
      // *Pixel++ = ((uint8_t)Y + YOffset); // Green
      // *Pixel++ = (uint8_t)(100); // Red
      // *Pixel++ = 0; // Alpha
    }

    Row += Buffer.Pitch;
  }
}

void Win32ResizeDIBSection(win32_offscreen_buffer *Buffer, int Width, int Height)
{
  if (Buffer->Memory)
  {
    VirtualFree(Buffer->Memory, 0, MEM_RELEASE);
  }

  Buffer->Width = Width;
  Buffer->Height = Height;
  Buffer->BytesPerPixel = 4;

  Buffer->Info.bmiHeader.biSize = sizeof(Buffer->Info.bmiHeader);
  Buffer->Info.bmiHeader.biWidth = Buffer->Width;
  Buffer->Info.bmiHeader.biHeight = Buffer->Height;
  Buffer->Info.bmiHeader.biPlanes = 1;
  Buffer->Info.bmiHeader.biBitCount = 32;
  Buffer->Info.bmiHeader.biCompression = BI_RGB;

  int BitmapMemorySize = Buffer->Width * Buffer->Height * Buffer->BytesPerPixel;
  Buffer->Memory = VirtualAlloc(0, BitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);
  Buffer->Pitch = Width * Buffer->BytesPerPixel;
}

void Win32UpdateWindow(HDC DeviceContext, RECT WindowRect, win32_offscreen_buffer Buffer, int X, int Y, int Width, int Height)
{
  int WindowWidth = WindowRect.right - WindowRect.left;
  int WindowHeight = WindowRect.bottom - WindowRect.top;

  StretchDIBits(DeviceContext, 0, 0, WindowWidth, WindowHeight, 0, 0, Buffer.Width, Buffer.Height, Buffer.Memory, &Buffer.Info, DIB_RGB_COLORS, SRCCOPY);
  return;
}

LRESULT CALLBACK MainWindowCallback(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam)
{

  LRESULT Result = 0;
  switch (Message)
  {
  case WM_SIZE:
  {
    RECT ClientRect;
    GetClientRect(Window, &ClientRect);
    int Width = ClientRect.right - ClientRect.left;
    int Height = ClientRect.bottom - ClientRect.top;
    Win32ResizeDIBSection(&GlobalBackBuffer, Width, Height);

    OutputDebugString("WM_SIZE\n");
  }
  break;

  case WM_DESTROY:
  {
    Running = false;
    OutputDebugString("WM_DESTROY\n");
  }
  break;

  case WM_SYSKEYDOWN:
  case WM_SYSKEYUP:
  case WM_KEYDOWN:
  case WM_KEYUP:
  {
    uint32_t VKCode = WParam;
    bool WasDown = ((LParam & (1 << 30)) != 0);
    bool IsDown = ((LParam & (1 << 31)) == 0);

    // if (WasDown != IsDown)
    // {
    // OutputDebugString("WM_KEYDOWN\n")
    // std::cout << "WasDown: " << WasDown << "\n";
    // std::cout << "IsDown: " << IsDown << "\n";

    if (WasDown != IsDown) // Key up/down
    {
      if (VKCode == 'W')
      {
        std::cout << "W\n";
        OutputDebugString("W\n");
      }
      else if (VKCode == 'A')
      {
        OutputDebugString("A\n");
      }
      else if (VKCode == 'S')
      {
        OutputDebugString("S\n");
      }
      else if (VKCode == 'D')
      {
        OutputDebugString("D\n");
      }
      else if (VKCode == 'Q')
      {
        OutputDebugString("Q\n");
      }
      else if (VKCode == 'E')
      {
        OutputDebugString("E\n");
      }
      else if (VKCode == VK_UP)
      {
        OutputDebugString("UP\n");
      }
      else if (VKCode == VK_DOWN)
      {
        OutputDebugString("DOWN\n");
      }
      else if (VKCode == VK_LEFT)
      {
        OutputDebugString("LEFT\n");
      }
      else if (VKCode == VK_RIGHT)
      {
        OutputDebugString("RIGHT\n");
      }
      else if (VKCode == VK_ESCAPE)
      {
        OutputDebugString("ESCAPE\n");
      }
      else if (VKCode == VK_SPACE)
      {
        OutputDebugString("SPACE\n");
      }
    }

    bool AltKeyWasDown = (LParam & (1 << 29));
    if ((VKCode == VK_F4) && AltKeyWasDown)
    {
      Running = false;
    }
  }
  break;

  case WM_CLOSE:
  {
    Running = false;
    OutputDebugString("WM_CLOSE\n");
  }
  break;

  case WM_ACTIVATEAPP:
  {
    OutputDebugString("WM_ACTIVATEAPP\n");
  }
  break;

  case WM_PAINT:
  {
    PAINTSTRUCT Paint;
    HDC DeviceContext = BeginPaint(Window, &Paint);
    int X = Paint.rcPaint.left;
    int Y = Paint.rcPaint.top;
    int Width = Paint.rcPaint.right - Paint.rcPaint.left;
    int Height = Paint.rcPaint.bottom - Paint.rcPaint.top;

    RECT ClientRect;
    GetClientRect(Window, &ClientRect);

    Win32UpdateWindow(DeviceContext, ClientRect, GlobalBackBuffer, X, Y, Width, Height);
    EndPaint(Window, &Paint);
  }
  break;
  default:
  {
    return DefWindowProc(Window, Message, WParam, LParam);
  }
  break;
  }

  return Result;
};

int WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR commandLine, int showCommand)
{
  WNDCLASS WindowClass = {};
  WindowClass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
  WindowClass.lpfnWndProc = MainWindowCallback;
  WindowClass.hInstance = instance;
  WindowClass.lpszClassName = "WindowClass";
  // WindowClass.hIcon = ;

  if (RegisterClass(&WindowClass))
  {
    OutputDebugString("Window Class Registered\n");
    HWND Window = CreateWindowEx(
        0,
        WindowClass.lpszClassName,
        "skibidi rizzler",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        0,
        0,
        instance,
        0);

    if (Window)
    {
      MSG Message;
      Running = true;

      /* ---- Experimental Variables ---- */
      int XOffset = 0;
      int YOffset = 0;

      int RunningSampleIndex = 0;
      int SquareWaveCounter = 0;
      int16_t ToneVolume = 3000;
      int SamplesPerSec = 48000;
      int WaveFrequency = 256;
      int WavePeriod = SamplesPerSec / WaveFrequency;
      int BytesPerSample = sizeof(int16_t) * 2;
      int SecBufferSize = SamplesPerSec * BytesPerSample;

      short up = 1;
      u_int upThreshold = 600;
      u_int downThreshold = 200;

      /* --------------------------------- */

      Win32InitDSound(Window, SamplesPerSec, SecBufferSize);
      GlobalSecondaryBuffer->Play(0, 0, DSBPLAY_LOOPING);
      HDC DeviceContext = GetDC(Window);

      while (Running)
      {
        while (PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
        {
          if (Message.message == WM_QUIT)
            Running = false;

          TranslateMessage(&Message);
          DispatchMessage(&Message);
        }

        RECT ClientRect;
        GetClientRect(Message.hwnd, &ClientRect);
        int WindowWidth = ClientRect.right - ClientRect.left;
        int WindowHeight = ClientRect.bottom - ClientRect.top;

        RenderPixelAnimation(GlobalBackBuffer, XOffset, YOffset);

        DWORD PlayCursor;
        DWORD WriteCursor;
        if (SUCCEEDED(GlobalSecondaryBuffer->GetCurrentPosition(&PlayCursor, &WriteCursor)))
        {
          DWORD ByteToLock = (RunningSampleIndex * BytesPerSample) % SecBufferSize;
          DWORD BytesToWrite = 0;
          if (ByteToLock > PlayCursor)
          {
            BytesToWrite = SecBufferSize - ByteToLock;
            BytesToWrite += PlayCursor;
          }
          else
          {
            BytesToWrite = PlayCursor - ByteToLock;
          }

          VOID *Region1;
          DWORD Region1Size;
          VOID *Region2;
          DWORD Region2Size;

          WavePeriod = SamplesPerSec / WaveFrequency;

          if (SUCCEEDED(GlobalSecondaryBuffer->Lock(ByteToLock, BytesToWrite, &Region1, &Region1Size, &Region2, &Region2Size, 0)))
          {
            int16_t *SampleOut = (int16_t *)Region1;
            DWORD Region1SampleCount = Region1Size / BytesPerSample;

            for (DWORD SampleIndex = 0; SampleIndex < Region1SampleCount; ++SampleIndex)
            {
              float t = 2.0f * PI32 * (float)RunningSampleIndex++ / (float)WavePeriod;
              float SineValue = sinf(t);
              int16_t SampleValue = (int16_t)(SineValue * ToneVolume);
              *SampleOut++ = SampleValue;
              *SampleOut++ = SampleValue;
            }

            DWORD Region2SampleCount = Region2Size / BytesPerSample;
            SampleOut = (int16_t *)Region2;
            for (DWORD SampleIndex = 0; SampleIndex < Region2SampleCount; ++SampleIndex)
            {
              float t = 2.0f * PI32 * (float)RunningSampleIndex++ / (float)WavePeriod;
              float SineValue = sinf(t);
              int16_t SampleValue = (int16_t)(SineValue * ToneVolume);
              *SampleOut++ = SampleValue;
              *SampleOut++ = SampleValue;
            }

            GlobalSecondaryBuffer->Unlock(Region1, Region1Size, Region2, Region2Size);

            // if (up == 1) ++WaveFrequency;
            // else if (up == 0) --WaveFrequency;

            // if (WaveFrequency >= upThreshold) up = 0;
            // else if (WaveFrequency <= downThreshold) up = 1;
          }
        }

        Win32UpdateWindow(DeviceContext, ClientRect, GlobalBackBuffer, 0, 0, WindowWidth, WindowHeight);

        ++XOffset;
        YOffset = YOffset + 3;

        // int duration = begin + 1;
        // int current = time(0);

        // if (current > duration) {
        //   begin = time(0);
        //   count = 0;
        // }

        // count++;
      }

      ReleaseDC(Window, DeviceContext);
    }
  }

  return 0;
}
