 
#include "Include.h"

//----------------------------------------------------
Target_setting Target;

Target_setting DefaultSetting=
{
	//SpecialSettingType SpecialSetting;
	{
		0xffffffff,//U32 crc32;
		{0x04C1,0x1DB7,0xEDB88320,0x80051021},//UniqueId uid;
		//PRODUCT_ID ProductId;
		{
		  //U16 id0;//3828
			0x38,
			0x28,
			//U16 id1;//EA00
			0xEA,
			0x00,
			//U16 id2;//xxxx
			0x00,
			0x00,
			//U16 id3;//year/week
			0x16,
			0x01,
			//U16 id4;//month/date
			0x12,
			0x00,
		}
	},
	//UserSettingType  UserSetting;
	{
		0xffffffff,//crc32
		50,//U8 current;
    100,//U8 max_current;
		0//U8 min_current;
	}
};

static U32 UserOldCRC32=0;
static U32 SpecOldCRC32=0;

#ifdef USE_AES
extern U8  AesKey[32];
#endif

void LoadSetting(void)
{
	  //U32 crc;
    U8  Buffer[1024];
	  UniqueId uid,temp;
	  SpecialSettingType *pSpecialSetting=NULL;
	  UserSettingType *pUserSetting=NULL;
    U32 *pCRC=NULL;
	  DeviceControl(STORECTL_READ,USER_DATA_ADDR,sizeof(UserSettingType)+4,Buffer);
	  pUserSetting=(UserSettingType *)Buffer;
    pCRC=(U32*)&Buffer[sizeof(UserSettingType)];
    //crc = *pCRC;
	  if((*pCRC!=CRC32(Buffer,sizeof(UserSettingType))) || (pUserSetting->crc32 != CRC32((U8*)&DefaultSetting.UserSetting,sizeof(UserSettingType))))
		{
			  Target.UserSetting=DefaultSetting.UserSetting;
        Target.UserSetting.crc32=CRC32((U8*)&DefaultSetting.UserSetting,sizeof(UserSettingType));
			  *pUserSetting=Target.UserSetting;
        *pCRC=CRC32((U8*)&Target.UserSetting,sizeof(UserSettingType));
        DeviceControl(STORECTL_WRITE,USER_DATA_ADDR,sizeof(UserSettingType)+4,Buffer);
		}
		else
			 Target.UserSetting=*pUserSetting;
    //UserOldCRC32=CRC32((U8*)&Target.UserSetting,sizeof(UserSettingType));
		UserOldCRC32=*pCRC;
		//-----------------------------------------------------------------------
		//TODO Other Setting
		//-----------------------------------------------------------------------
	  DeviceControl(STORECTL_READ,SPEC_DATA_ADDR,sizeof(SpecialSettingType)+4,Buffer);
	  pSpecialSetting=(SpecialSettingType *)Buffer;
    pCRC=(U32*)&Buffer[sizeof(SpecialSettingType)];
	  if((pSpecialSetting->crc32 != CRC32((U8*)&DefaultSetting.SpecialSetting,sizeof(SpecialSettingType))))
		{
			  Target.SpecialSetting=DefaultSetting.SpecialSetting;
        Target.SpecialSetting.crc32=CRC32((U8*)&DefaultSetting.SpecialSetting,sizeof(SpecialSettingType));
			  uid.id0=UniqueID->id0;
			  uid.id1=UniqueID->id1;
			  uid.id2=UniqueID->id2;
			  uid.id3=UniqueID->id3;
			  uid.Reserved=0;
			  #ifdef USE_AES
					AES_ENCODE(AesKey,(U8*)&uid,16);
				#endif
			  Target.SpecialSetting.uid=uid;
			  *pSpecialSetting=Target.SpecialSetting;
        *pCRC=CRC32((U8*)&Target.SpecialSetting,sizeof(SpecialSettingType));
        DeviceControl(STORECTL_WRITE,SPEC_DATA_ADDR,sizeof(SpecialSettingType)+4,Buffer);
			  SpecOldCRC32 = *pCRC;
			  return ;
		}
		if(*pCRC == CRC32(Buffer,sizeof(SpecialSettingType)))
		{
				uid=pSpecialSetting->uid;
				#ifdef USE_AES
					AES_DECODE(AesKey,(U8*)&uid,16);
				#endif
			  temp.id0=UniqueID->id0;
			  temp.id1=UniqueID->id1;
			  temp.id2=UniqueID->id2;
			  temp.id3=UniqueID->id3;
				if((uid.id0==temp.id0) && (uid.id1==temp.id1) && (uid.id2==temp.id2) && (uid.id3==temp.id3))
				{
					Target.SpecialSetting=*pSpecialSetting;
					SpecOldCRC32 = *pCRC;
					return ;
				}
		}
		while(1);
		//Target.SpecialSetting=*pSpecialSetting;
		//SpecOldCRC32 = *pCRC;
}

//----------------------------------------------------

void SaveSetting(void)
{
	  U32 crc32;
	  U8  Buffer[1024];
	  SpecialSettingType *pSpecialSetting=NULL;
	  UserSettingType *pUserSetting=NULL;
    U32 *pCRC=NULL;
		crc32=CRC32((U8*)&Target.UserSetting,sizeof(UserSettingType));
		if(crc32!=UserOldCRC32)
		{
			pUserSetting=(UserSettingType *)Buffer;
      pCRC=(U32*)&Buffer[sizeof(UserSettingType)];
			*pUserSetting=Target.UserSetting;
			*pCRC=crc32;
			DisableGlobalInterrupt();
			DeviceControl(STORECTL_WRITE,USER_DATA_ADDR,sizeof(UserSettingType)+4,Buffer);
			RestoreGlobalInterrupt();
			UserOldCRC32=crc32;
		}
		crc32=CRC32((U8*)&Target.SpecialSetting,sizeof(SpecialSettingType));
		if(crc32!=SpecOldCRC32)
		{
			pSpecialSetting=(SpecialSettingType *)Buffer;
      pCRC=(U32*)&Buffer[sizeof(SpecialSettingType)];
			*pSpecialSetting=Target.SpecialSetting;
			*pCRC=crc32;
			DisableGlobalInterrupt();
			DeviceControl(STORECTL_WRITE,SPEC_DATA_ADDR,sizeof(SpecialSettingType)+4,Buffer);
			RestoreGlobalInterrupt();
			SpecOldCRC32=crc32;
		}
}
