#include "driver.h"

struct gotya_sample
{
  int sound_command;
  int channel;
  int looping;
};


static struct gotya_sample gotya_samples[] =
{
  { 0x01, 0, 0 },
  { 0x02, 1, 0 },
  { 0x03, 2, 0 },
  { 0x05, 2, 0 },
  { 0x06, 3, 0 },
  { 0x07, 3, 0 },
  { 0x08, 0, 1 },
  { 0x0a, 0, 0 },
  { 0x0b, 0, 0 },

  /* all the speech can go to one channel? */

  { 0x10, 3, 0 },
  { 0x11, 3, 0 },
  { 0x12, 0, 0 },		/* this should stop the main tune */
  { 0x13, 3, 0 },
  { 0x14, 3, 0 },
  { 0x15, 3, 0 },
  { 0x16, 3, 0 },
  { 0x17, 3, 0 },
  { 0x18, 3, 0 },
  { 0x19, 3, 0 },
  { 0x1a, 3, 0 },
  {   -1, 0, 0 }		/* end of array */
};

WRITE_HANDLER( gotya_soundlatch_w )
{
  static int theme_playing;
  int sample_number;


  if (data == 0)
  {
    sample_stop(0);
    theme_playing = 0;
    return;
  }

  /* search for sample to play */

  for (sample_number = 0;
       gotya_samples[sample_number].sound_command != -1;
       sample_number++)
  {
    if (gotya_samples[sample_number].sound_command == data)
    {
      if (gotya_samples[sample_number].looping &&
          theme_playing)
      {
        /* don't restart main theme */
        return;
      }

      sample_start(gotya_samples[sample_number].channel,
                   sample_number,
                   gotya_samples[sample_number].looping);

      if (gotya_samples[sample_number].channel == 0)
      {
        theme_playing = gotya_samples[sample_number].looping;
      }
      return;
    }
  }
}
