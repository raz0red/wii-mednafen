#include "../sexyal.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>

#ifdef WII_NETTRACE
#include <network.h>
#include "net_print.h"  
#endif

#include <gccore.h>
#include <string.h>
#include <sdl.h>

#define SAMPLERATE 48000  

#define SOUNDBUFSIZE  4096 //2048

static u8 soundbuffer[2][SOUNDBUFSIZE] ATTRIBUTE_ALIGN(32);
static int mixhead = 0;
static int mixtail = 0;
static int whichab = 0;
static int IsPlaying = 0;

static int EPMaxValue;
static int BufferSize;
static int RealBufferSize;
static int RealBufferSizeW;
static int BufferSize_Raw;
static int RealBufferSize_Raw;
static int EPMaxVal;
static u8* mixbuffer;
static int64_t last_time;
static SexyAL_device* theDevice = NULL;

static int64_t Time64(void)
{
  int64_t ret;
  ret = (int64_t)SDL_GetTicks() * 1000;
  return(ret);
}

static int MixerCollect( u8 *outbuffer, int len )
{
  u32 *dst = (u32 *)outbuffer;
  u32 *src = (u32 *)mixbuffer;
  int done = 0;

  // Always clear output buffer
  memset(outbuffer, 0x0, len);
  while ( ( mixtail != mixhead ) && ( done < len ) )
  {
    *dst++ = src[mixtail++];
    if (mixtail == RealBufferSizeW) mixtail = 0;
    done += 4;
  }

  // Realign to 32 bytes for DMA
  mixtail -= ((done&0x1f) >> 2);
  if (mixtail < 0)
    mixtail += RealBufferSizeW;
  done &= ~0x1f;

  return done;
}

static void AudioSwitchBuffers()
{
  last_time = Time64();

  int len = MixerCollect( soundbuffer[whichab], SOUNDBUFSIZE );

  if (len == 0) 
    return;

  DCFlushRange(soundbuffer[whichab], len);
  AUDIO_InitDMA((u32)soundbuffer[whichab], len);
  AUDIO_StartDMA();
  whichab ^= 1;  
  IsPlaying = 1;   
}

void InitialiseAudio()
{
  AUDIO_Init(NULL); // Start audio subsystem
  AUDIO_SetDSPSampleRate(AI_SAMPLERATE_48KHZ);
  AUDIO_RegisterDMACallback( AudioSwitchBuffers );    
  memset(soundbuffer, 0, SOUNDBUFSIZE*2);
  memset(mixbuffer, 0, RealBufferSize_Raw);
}

void StopAudio()
{
  AUDIO_StopDMA();
  IsPlaying = 0;
}

void ResetAudio()
{
  memset(soundbuffer, 0, SOUNDBUFSIZE*2);
  memset(mixbuffer, 0, RealBufferSize_Raw);
  mixhead = mixtail = 0;
}

void PlaySound( u32 *Buffer, int count )
{
  u32 *dst = (u32 *)mixbuffer;
  int i;

  for( i = 0; i < count; i++ )
  {
    dst[mixhead++] = ((Buffer[i]<<16)&0xffff0000)|((Buffer[i]>>16)&0x0000ffff);
    if (mixhead == RealBufferSizeW)
      mixhead = 0;
  }

  // Restart Sound Processing if stopped
  if (IsPlaying == 0)
  {
    AudioSwitchBuffers ();
  }
}

static int Get_RCW(SexyAL_device *device, uint32_t *can_write, bool want_nega = false)
{
  int64_t curtime;
  int32_t cw;
  int32_t extra_precision;

  curtime = Time64();
  int available =     
    mixhead == mixtail ? 0 :
      ( ( mixhead > mixtail ? 
          ( mixhead - mixtail ) : 
          ( RealBufferSizeW - ( mixtail - mixhead ) ) ) << 2 );

  cw = BufferSize_Raw - available;

#if 0
#ifdef WII_NETTRACE
  net_print_string(NULL,0, "Raw:%d, Avail:%d, cw:%d\n", BufferSize_Raw, available, cw );
#endif
#endif

  extra_precision = ((curtime - last_time) / 1000 * device->format.rate / 1000);

  if(extra_precision < 0)
  {
    //printf("extra_precision < 0: %d\n", extra_precision);
    extra_precision = 0;
  }
  else if(extra_precision > EPMaxVal)
  {
    //printf("extra_precision > EPMaxVal: %d %d\n", extra_precision, sw->EPMaxVal);
    extra_precision = EPMaxVal;
  }

  cw += extra_precision * device->format.channels * (device->format.sampformat >> 4);

  if(cw < 0)
  {
    if(want_nega)
      *can_write = ~0U;
    else
      *can_write = 0;
  }
  else if(cw > BufferSize_Raw)
  {
    *can_write = BufferSize_Raw;
  }
  else 
    *can_write = cw;

  return(1);
}

static int RawCanWrite(SexyAL_device *device, uint32_t *can_write)
{
  return(Get_RCW(device, can_write, false));
}

static int RawWrite(SexyAL_device *device, const void *data, uint32_t len)
{
  PlaySound( (u32*)data, len>>2 );

  uint32_t cw_tmp;

  while(Get_RCW(device, &cw_tmp, true) && cw_tmp == ~0U )
  {
    SDL_Delay(1);
  }

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

SexyAL_device *SexyALI_WII_Open(const char *id, SexyAL_format *format, SexyAL_buffering *buffering)
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
  buffering->latency = (obtainedSamples + BufferSize); // (obtained) * 2
  buffering->period_size = 0;

  //printf("%d\n", buffering->latency);

  BufferSize_Raw = BufferSize * format->channels * (format->sampformat >> 4);
  RealBufferSize_Raw = RealBufferSize * format->channels * (format->sampformat >> 4);
  mixbuffer = (u8*)malloc(RealBufferSize_Raw);

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
