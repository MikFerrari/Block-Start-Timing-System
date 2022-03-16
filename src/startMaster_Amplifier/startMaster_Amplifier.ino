#include <PCM.h>
#include "Sound_MARKS.h"
#include "Sound_SET.h"
#include "Sound_GUN.h"

void setup()
{
  
}

void loop()
{
  startPlayback(sounddata_data_MARKS, sizeof(sounddata_data_MARKS));
  delay(2000);
  startPlayback(sounddata_data_SET, sizeof(sounddata_data_SET));
  delay(2000);
  startPlayback(sounddata_data_GUN, sizeof(sounddata_data_GUN));
  delay(2000);
}
