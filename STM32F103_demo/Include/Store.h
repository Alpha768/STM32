#ifndef __EEPROM_H__
#define __EEPROM_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifndef FIELD_OFFSET
#define FIELD_OFFSET(type, field)    ((U16)((U32)(&(((type *)0)->field))))
#endif

#define USER_DATA_ADDR 0
#define SPEC_DATA_ADDR 1024

extern void LoadSetting(void);
extern void SaveSetting(void);

typedef struct tagProductId
{
  U8 id0;//38
  U8 id1;//28
  U8 id2;//EA
  U8 id3;//00
  U8 id4;//xx
  U8 id5;//xx
  U8 id6;//year
  U8 id7;//week
  U8 id8;//month
  U8 id9;//date
}__attribute__((packed)) PRODUCT_ID;

typedef struct tagUniqueId
{
  U16 id0;
  U16 id1;
  U32 id2;
  U32 id3;
  U32 Reserved;
}__attribute__((packed)) UniqueId;

typedef struct  
{
	  U32 crc32;
    UniqueId uid;
	  PRODUCT_ID ProductId;
} __attribute__((packed)) SpecialSettingType;

typedef struct 
{
  U32 crc32;
	U8 current;
	U8 max_current;
	U8 min_current;
} __attribute__((packed)) UserSettingType;

typedef struct tag_Target_setting {
    SpecialSettingType SpecialSetting;
    UserSettingType  UserSetting;
} __attribute__((packed)) Target_setting;

extern Target_setting Target;

#ifdef __cplusplus
};
#endif

#endif

