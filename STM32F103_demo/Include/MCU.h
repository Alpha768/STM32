#ifndef __MCU_H__
#define __MCU_H__

void DisableGlobalInterrupt(void);
void EnableGlobalInterrupt(void);
void RestoreGlobalInterrupt(void);
void MCUInit(void);

#endif
