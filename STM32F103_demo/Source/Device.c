#include "Include.h"

#define	MAX_DEV_NUM		5
static BOOLEAN IsInitialization=FALSE;

static  DEVICE  *DevicePtr[MAX_CLASS][ MAX_DEV_NUM ];


typedef void ( *__PatchDriverMode__) ( U8 flag) ;

U32 DeviceControl(U32 cmd,...)
{
	U16  i,j,ClassID,DeviceID;
	U32 dwRet=TRUE;
	U32 Command;
	U16 Index=0;
	U16 PatchIndex=0;
	BOOLEAN IsVaList=FALSE;
	LPATCH_TABLE PatchTable=NULL;
	CTRL_TAB * ControlTab=NULL;
	va_list arg_ptr;
	DEVICE *Device=NULL;
	__PatchDriverMode__ PatchDriverMode=NULL;
	Command=GetCmdId(cmd);
//++++++++++++++++++++++++++++++++++++++++++
	if((cmd & (REGISTER_FLAG | UNREGISTER_FLAG)) == (REGISTER_FLAG | UNREGISTER_FLAG))
		return FALSE;
	if(IsInitialization==FALSE)
	{
		for( i = 0 ; i < MAX_CLASS ; i ++ )
		{
			for( j = 0 ; j < MAX_DEV_NUM ; j++)
				DevicePtr[i][j]=NULL;
		}
		IsInitialization=TRUE;
	}

	if(GetDeviceClass(cmd)!=0x3f)
	{
		if(GetDeviceClass(cmd)>=MAX_CLASS  || GetDeviceIndex(cmd)>=MAX_DEV_NUM)
			return FALSE;
	}

	if(cmd&VA_ARG_PTR)
		IsVaList=TRUE;

	//------------------------
	if(cmd & REGISTER_FLAG)
	{
		va_start(arg_ptr, cmd); 
		if(IsVaList)
			arg_ptr=va_arg(arg_ptr,va_list);
		Device=va_arg(arg_ptr, DEVICE *);
		PatchTable=va_arg(arg_ptr, LPATCH_TABLE);
		if(Device==NULL)
			return FALSE;
		if(Device->DeviceClass>=MAX_CLASS)
			return FALSE;
		if(PatchTable!=NULL)
			Device->lpPatchTable=PatchTable;
		ControlTab=Device->lpControlTab;
		if(ControlTab==NULL)
			return FALSE;
		PatchDriverMode=Device->PatchDriverMode;
		for( i = 0 ; i < MAX_DEV_NUM ; i ++ )
		{
			if( DevicePtr[Device->DeviceClass][i] == Device )
				return TRUE ;
		}
		va_end(arg_ptr);
	}
    
	if(cmd & UNREGISTER_FLAG)
	{
		va_start(arg_ptr, cmd); 
		if(IsVaList)
			arg_ptr=va_arg(arg_ptr,va_list);
		Device=va_arg(arg_ptr, DEVICE *);
		if(Device==NULL)
			return FALSE;
		if(Device->DeviceClass>=MAX_CLASS)
			return FALSE;
		ControlTab=Device->lpControlTab;
		if(ControlTab==NULL)
			return FALSE;
		PatchDriverMode=Device->PatchDriverMode;
		PatchTable=Device->lpPatchTable;
		va_end(arg_ptr);
	}
	//------------------------

    for(i=0;i<MAX_CLASS;i++)
	{
		for(j=0;j<MAX_DEV_NUM;j++)
		{
			if((cmd & (REGISTER_FLAG | UNREGISTER_FLAG)) == FALSE)
			{
				if(GetDeviceClass(cmd)==0x3f)
				{
					ClassID=i;
					DeviceID=j;
				}
				else
				{
					ClassID=GetDeviceClass(cmd);
					DeviceID=GetDeviceIndex(cmd);
				}
				
				if(DevicePtr[ClassID][DeviceID]!=NULL)
				{
					PatchTable=DevicePtr[ClassID][DeviceID]->lpPatchTable;
					ControlTab=DevicePtr[ClassID][DeviceID]->lpControlTab;
					PatchDriverMode=DevicePtr[ClassID][DeviceID]->PatchDriverMode;
				}
				else
				{
					PatchTable=NULL;
					ControlTab=NULL;
					PatchDriverMode=NULL;
				}
					
				
				if(ControlTab==NULL && GetDeviceClass(cmd)!=0x3f)
					return FALSE;
				if(ControlTab==NULL && GetDeviceClass(cmd)==0x3f)
				continue;
			}

//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
			if(PatchDriverMode!=NULL)
               PatchDriverMode(1);
			
			if(PatchTable != NULL)
			{
				while(1)
				{
					if(PatchTable[Index].CmdId==INVALID_CMDID)
					{
						break;
					}
					if(PatchTable[Index].CmdId==Command)
					{
						break;
					}
					Index++;
				}
				PatchIndex=Index;
				//-------------------------------------------------------------------
				//Full Patch
				va_start(arg_ptr, cmd); 
				if(IsVaList)
					arg_ptr=va_arg(arg_ptr,va_list);
				
				if(cmd & REGISTER_FLAG )
				{
					Device=va_arg(arg_ptr, DEVICE *);
					PatchTable=va_arg(arg_ptr, LPATCH_TABLE);
				}
				
				if(cmd & UNREGISTER_FLAG )
				{
					Device=va_arg(arg_ptr, DEVICE *);
				}

				if(PatchTable[PatchIndex].CmdId==Command && PatchTable[PatchIndex].FullPatch!=NULL)
				{
					dwRet=PatchTable[PatchIndex].FullPatch(cmd,arg_ptr);
					goto Lable;
				}
				
				va_end(arg_ptr); 
				//-------------------------------------------------------------------
				// Before Patch
				va_start(arg_ptr, cmd); 
				if(IsVaList)
					arg_ptr=va_arg(arg_ptr,va_list);
				
				if(cmd & REGISTER_FLAG )
				{
					Device=va_arg(arg_ptr, DEVICE *);
					PatchTable=va_arg(arg_ptr, LPATCH_TABLE);
				}
				
				if(cmd & UNREGISTER_FLAG )
				{
					Device=va_arg(arg_ptr, DEVICE *);
				}
				
				if(PatchTable[PatchIndex].CmdId==Command && PatchTable[PatchIndex].BeforePatch!=NULL)
				{
					dwRet=PatchTable[PatchIndex].BeforePatch(cmd,arg_ptr);
				}
				
				va_end(arg_ptr); 
			}
			//##########################################################################
			Index=0;
			va_start(arg_ptr, cmd); 
			if(IsVaList)
				arg_ptr=va_arg(arg_ptr,va_list);

			if(cmd & REGISTER_FLAG )
			{
				Device=va_arg(arg_ptr, DEVICE *);
				PatchTable=va_arg(arg_ptr, LPATCH_TABLE);
			}
			
			if(cmd & UNREGISTER_FLAG )
			{
				Device=va_arg(arg_ptr, DEVICE *);
			}

			while(1)
			{
				if(ControlTab[Index].CmdId==INVALID_CMDID)
				{
					if(ControlTab[Index].DoCommand!=NULL)
					{
						dwRet=ControlTab[Index].DoCommand(cmd,arg_ptr);
						break;
					}
					
					dwRet=FALSE;
					break;
				}
				if(ControlTab[Index].CmdId==Command && ControlTab[Index].DoCommand!=NULL)
				{
					dwRet=ControlTab[Index].DoCommand(cmd,arg_ptr);
					break;
				}
				Index++;
			}
			
			va_end(arg_ptr); 
			//#############################################################################
			if(PatchTable != NULL)
			{
				// After Patch
				va_start(arg_ptr, cmd); 
				if(IsVaList)
					arg_ptr=va_arg(arg_ptr,va_list);

				if(cmd & REGISTER_FLAG )
				{
					Device=va_arg(arg_ptr, DEVICE *);
					PatchTable=va_arg(arg_ptr, LPATCH_TABLE);
				}
				
				if(cmd & UNREGISTER_FLAG )
				{
					Device=va_arg(arg_ptr, DEVICE *);
				}

				if(PatchTable[PatchIndex].CmdId==Command && PatchTable[PatchIndex].AfterPatch!=NULL)
				{
					dwRet=PatchTable[PatchIndex].AfterPatch(cmd,arg_ptr);
				}
				
				va_end(arg_ptr); 
			}
Lable:
			if(PatchDriverMode!=NULL)
				PatchDriverMode(0);
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
			if(cmd & REGISTER_FLAG )
			{
				if(dwRet)
				{	
					Device->status=STATUS_ONLINE;
					for( i = 0 ; i < MAX_DEV_NUM ; i ++ )
					{
						if( DevicePtr[Device->DeviceClass][i] == Device )
							break ;
						if( DevicePtr[Device->DeviceClass][i] == NULL )
						{
							DevicePtr[Device->DeviceClass][i] = Device ;
							if(Device->lpBusDevice!=NULL)
								Device->lpBusDevice->Attach(NULL);
							break ;
						}
					}
				}
				else
					Device->status=STATUS_OFFLINE;
				return dwRet;
			}
			if(cmd & UNREGISTER_FLAG)
			{
				for( i = 0 ; i < MAX_DEV_NUM ; i ++ )
				{
					if( DevicePtr[Device->DeviceClass][i] == Device )
					{
						DevicePtr[Device->DeviceClass][i] =NULL;
						break ;
					}
				}
				return dwRet;
			}
			if(GetDeviceClass(cmd)!=0x3f)
				return dwRet;
		}
	}

	return dwRet;
//++++++++++++++++++++++++++++++++++++++++++
}

