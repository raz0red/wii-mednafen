#include "../sexyal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

#include <emscripten.h>

#define SAMPLERATE 48000

#define SOUNDBUFSIZE 4096  // 2048

typedef unsigned char u8;
typedef unsigned int u32;
typedef short s16;

static s16* sbuff;

static int EPMaxValue;
static int BufferSize;
static int RealBufferSize;
static int RealBufferSizeW;
static int BufferSize_Raw;
static int RealBufferSize_Raw;
static int EPMaxVal;
static SexyAL_device* theDevice = NULL;

void InitialiseAudio() {
    sbuff = (s16*)malloc(sizeof(short) * SOUNDBUFSIZE * 2);
}

void StopAudio() {
}

void ResetAudio() {
}

void PlaySound(u32* Buffer, int count) {
    int j = 0;
    for (int i = 0; i < count << 1; i++) {
        sbuff[j++] = (short)((Buffer[i] >> 16) & 0xFFFF);
        sbuff[j++] = (short)(Buffer[i] & 0xFFFF);
    }
    EM_ASM({ window.audioCallback($0, $1); }, sbuff, count << 1);
}

static int RawCanWrite(SexyAL_device *device, uint32_t *can_write)
{
  *can_write = BufferSize_Raw;
  return (1);
}

static int RawWrite(SexyAL_device *device, const void *data, uint32_t len)
{
  PlaySound( (u32*)data, len>>2 );
  return(1);
}

static int Pause(SexyAL_device *device, int state)
{
  StopAudio();
  return state?1:0;
}

static int Clear(SexyAL_device *device)
{
  ResetAudio();
  return(1);
}

static int RawClose(SexyAL_device *device)
{
  if(device)
  {
    StopAudio();
    return(1);
  }
  return(0);
}

SexyAL_device *SexyALI_EM_Open(const char *id, SexyAL_format *format, SexyAL_buffering *buffering)
{  
  SexyAL_device *device;

  int obtainedSamples = SOUNDBUFSIZE>>1;

  if(!buffering->ms) 
    buffering->ms = 100;
  else if(buffering->ms > 1000)
    buffering->ms = 1000;

  format->channels = 2;
  format->rate = SAMPLERATE;

  if( theDevice ) 
  {
    ResetAudio();
    buffering->buffer_size = BufferSize;
    buffering->latency = (obtainedSamples + BufferSize); // (obtained) * 2
    buffering->period_size = 0;
    return theDevice;
  }

  device = (SexyAL_device *)malloc(sizeof(SexyAL_device));
  memset(device, 0, sizeof(SexyAL_device));

  EPMaxVal = obtainedSamples; //8192;
  BufferSize = (format->rate * buffering->ms / 1000) - obtainedSamples; // (obtained) * 2
  if(BufferSize < obtainedSamples)
    BufferSize = obtainedSamples;

  //printf("%d\n", sw->BufferSize);
  // *2 for safety room, FIXME?
  RealBufferSize = SexyAL_rupow2(BufferSize + (EPMaxVal + 2048) * 2);
  RealBufferSizeW = RealBufferSize>>1;
  buffering->buffer_size = BufferSize;
  //buffering->latency = (obtainedSamples + BufferSize); // (obtained) * 2
  buffering->period_size = 0;

  BufferSize_Raw = BufferSize * format->channels * (format->sampformat >> 4);
  RealBufferSize_Raw = RealBufferSize * format->channels * (format->sampformat >> 4);

  memcpy(&device->format, format, sizeof(SexyAL_format));
  memcpy(&device->buffering, buffering, sizeof(SexyAL_buffering));

  device->RawCanWrite = RawCanWrite;
  device->RawWrite = RawWrite;
  device->RawClose = RawClose;
  device->Clear = Clear;
  device->Pause = Pause;

  InitialiseAudio();

  theDevice = device;

  return(device);
}
