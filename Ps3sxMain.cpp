#include "cell.h"
#include <vector>
#include "rom_list.h"
#include <math.h>
#include <sys/paths.h>
#include "kammy.h"
#include "ps3video.h"
#include "ps3audio.h"
#include "cellinput.h"
#include "ini.h"
#include <unistd.h> 
#include <pthread.h>

SYS_PROCESS_PARAM(1001, 0x10000);

PS3Graphics* Graphics;
CellInputFacade* PS3input  = 0;
FileIniConfig Iniconfig;

bool is_running = 0;
int boot_cdrom;
int RomType;
int CpuConfig = 0;

#define SAMPLERATE_44KHZ 44100
#define SB_SIZE 3840

static bool runbios = 0;
char rom_path[256];

void InitPS3()
{
	Graphics = new PS3Graphics();
	Graphics->Init();

	// FIXME: Is this necessary?
	if (Graphics->InitCg() != CELL_OK)
	{
		printf("Failed to InitCg: %d\n", __LINE__);
		exit(0);
	}

	PS3input = new CellInputFacade();
	Graphics->InitDbgFont();	
}

extern "C"
{
#include "psxcommon.h"
#include "Sio.h"
#include "PlugCD.h"
#include "plugins.h"
#include "misc.h"
#include "R3000a.h"

int NeedReset = 0;
int Running =0;
long LoadCdBios = 0;

//Sound Function
unsigned long SoundGetBytesBuffered(void)
{
		return cellAudioPortWriteAvail();
}

void SoundFeedStreamData(unsigned char *pSound, long lBytes)
{		
	cellAudioPortWrite((const audio_input_t*)pSound,lBytes / 2);
}

void SetupSound(void)
{	
	cellAudioPortInit(SAMPLERATE_44KHZ,SB_SIZE);
}

void RemoveSound(void)
{
	cellAudioPortExit();
	sys_ppu_thread_exit(0);

}

//end Sound 

//Video Output Function
void ps3sxSwapBuffer(unsigned char *pixels,int w,int h)
{
	Graphics->Draw(w,h,pixels);
	Graphics->Swap();
}
//end Video

//Start PAD
long PAD__readPort1(PadDataS* pad)
{
		static unsigned short pad_status = 0xffff;

		PS3input->UpdateDevice(0);
  
		if (PS3input->IsButtonPressed(0,CTRL_CIRCLE))
		{
			pad_status &= ~(1<<13);
		}else{
			pad_status |=  (1<<13);
		}

		if (PS3input->IsButtonPressed(0,CTRL_CROSS))
		{
			pad_status &= ~(1<<14);
		}else{
			pad_status |=  (1<<14);
		}

		if (PS3input->IsButtonPressed(0,CTRL_SELECT))
		{
			pad_status &= ~(1<<2); 
		}else{
			pad_status |=  (1<<2);
		}

		if (PS3input->IsButtonPressed(0,CTRL_START))
		{
			pad_status &= ~(1<<3);
		}else{
			pad_status |=  (1<<3);
		}

		if (PS3input->IsButtonPressed(0,CTRL_DOWN))
		{
			pad_status &= ~(1<<6);
		}else{
			pad_status |=  (1<<6);
		}

		if (PS3input->IsButtonPressed(0,CTRL_UP))
		{
			pad_status &= ~(1<<4);
		}else{
			pad_status |=  (1<<4);
		}

		if (PS3input->IsButtonPressed(0,CTRL_RIGHT))
		{
			pad_status &= ~(1<<5);
		}else{
			pad_status |=  (1<<5);
		}

		if (PS3input->IsButtonPressed(0,CTRL_LEFT))
		{
			pad_status &= ~(1<<7);
		}else{
			pad_status |=  (1<<7);
		}

		if (PS3input->IsButtonPressed(0,CTRL_R1))
		{
			pad_status &= ~(1<<11);
		}else{
			pad_status |=  (1<<11);
		}

		if (PS3input->IsButtonPressed(0,CTRL_L1))
		{
			pad_status &= ~(1<<10);
		}else{
			pad_status |=  (1<<10);
		}

		if (PS3input->IsButtonPressed(0,CTRL_R2))
		{
			pad_status &= ~(1<<8);
		}else{
			pad_status |=  (1<<8);
		}

		if (PS3input->IsButtonPressed(0,CTRL_L2))
		{
			pad_status &= ~(1<<9);
		}else{
			pad_status |=  (1<<9);
		}

		if (PS3input->IsButtonPressed(0,CTRL_TRIANGLE))
		{
			pad_status &= ~(1<<12);
		}else{
			pad_status |=  (1<<12);
		}

		if (PS3input->IsButtonPressed(0,CTRL_SQUARE))
		{
			pad_status &= ~(1<<15);
		}else{
			pad_status |=  (1<<15);
		}


	pad->buttonStatus = pad_status;

	if(Settings.PAD)
		pad->controllerType = PSE_PAD_TYPE_ANALOGPAD; 
	else
		pad->controllerType = PSE_PAD_TYPE_STANDARD;

	return PSE_PAD_ERR_SUCCESS;
}

long PAD__readPort2(PadDataS* pad)
{	
		static unsigned short pad_status = 0xffff;

		PS3input->UpdateDevice(1);
  
		if (PS3input->IsButtonPressed(1,CTRL_CIRCLE))
		{
			pad_status &= ~(1<<13);
		}else{
			pad_status |=  (1<<13);
		}

		if (PS3input->IsButtonPressed(1,CTRL_CROSS))
		{
			pad_status &= ~(1<<14);
		}else{
			pad_status |=  (1<<14);
		}

		if (PS3input->IsButtonPressed(1,CTRL_SELECT))
		{
			pad_status &= ~(1<<2); 
		}else{
			pad_status |=  (1<<2);
		}

		if (PS3input->IsButtonPressed(1,CTRL_START))
		{
			pad_status &= ~(1<<3);
		}else{
			pad_status |=  (1<<3);
		}

		if (PS3input->IsButtonPressed(1,CTRL_DOWN))
		{
			pad_status &= ~(1<<6);
		}else{
			pad_status |=  (1<<6);
		}

		if (PS3input->IsButtonPressed(1,CTRL_UP))
		{
			pad_status &= ~(1<<4);
		}else{
			pad_status |=  (1<<4);
		}

		if (PS3input->IsButtonPressed(1,CTRL_RIGHT))
		{
			pad_status &= ~(1<<5);
		}else{
			pad_status |=  (1<<5);
		}

		if (PS3input->IsButtonPressed(1,CTRL_LEFT))
		{
			pad_status &= ~(1<<7);
		}else{
			pad_status |=  (1<<7);
		}

		if (PS3input->IsButtonPressed(1,CTRL_R1))
		{
			pad_status &= ~(1<<11);
		}else{
			pad_status |=  (1<<11);
		}

		if (PS3input->IsButtonPressed(1,CTRL_L1))
		{
			pad_status &= ~(1<<10);
		}else{
			pad_status |=  (1<<10);
		}

		if (PS3input->IsButtonPressed(1,CTRL_R2))
		{
			pad_status &= ~(1<<8);
		}else{
			pad_status |=  (1<<8);
		}

		if (PS3input->IsButtonPressed(1,CTRL_L2))
		{
			pad_status &= ~(1<<9);
		}else{
			pad_status |=  (1<<9);
		}

		if (PS3input->IsButtonPressed(1,CTRL_TRIANGLE))
		{
			pad_status &= ~(1<<12);
		}else{
			pad_status |=  (1<<12);
		}

		if (PS3input->IsButtonPressed(1,CTRL_SQUARE))
		{
			pad_status &= ~(1<<15);
		}else{
			pad_status |=  (1<<15);
		}


	pad->buttonStatus = pad_status;

	if(Settings.PAD)
		pad->controllerType = PSE_PAD_TYPE_ANALOGPAD; 
	else
		pad->controllerType = PSE_PAD_TYPE_STANDARD;

	return PSE_PAD_ERR_SUCCESS;
}

//end Pad

void InitConfig()
{
	memset(&Config, 0, sizeof(PcsxConfig));

	Config.PsxAuto = 1; //Autodetect
	Config.HLE	   = Settings.HLE; //Use HLE
	Config.Xa      = 0; //disable xa decoding (audio)
	Config.Sio     = 0; //disable sio interrupt ?
	Config.Mdec    = 0; //movie decode
	Config.Cdda    = 0; //diable cdda playback
	
	Config.Cpu	   = Settings.CPU;// interpreter 1 :  dynarec 0

	Config.SpuIrq  = 0;
	Config.RCntFix = 0;//Parasite Eve 2, Vandal Hearts 1/2 Fix
	Config.VSyncWA = 0; // interlaced /non ? something with the display timer
	Config.PsxOut =  0; // on screen debug 
	Config.UseNet = 0;

	strcpy(Config.Net, "Disabled"); 
	strcpy(Config.Net, _("Disabled"));
	strcpy(Config.BiosDir,Iniconfig.biospath);

	sprintf(Config.Mcd1, "%s/Mcd001.mcr",Iniconfig.savpath);
	sprintf(Config.Mcd2, "%s/Mcd002.mcr",Iniconfig.savpath);
}

static int sysInited = 0;

int SysInit()
{
	if (!sysInited) {
		emuLog = fopen("/dev_usb000/emuLog.txt","wb");
		
		if(!emuLog)
			Config.PsxOut = 0;
		
		sysInited = 1;
	}

    SysPrintf("start SysInit()\r\n");

    SysPrintf("psxInit()\r\n");
	psxInit();

    SysPrintf("LoadPlugins()\r\n");
	LoadPlugins();
    SysPrintf("LoadMcds()\r\n");
	LoadMcds(Config.Mcd1, Config.Mcd2);
	SysPrintf("end SysInit()\r\n");
	return 0;
}

void SysReset() {
    SysPrintf("start SysReset()\r\n");
	psxReset();
	SysPrintf("end SysReset()\r\n");
}

void SysPrintf(char *fmt, ...) {
    va_list list;
    char msg[512];

    va_start(list, fmt);
    vsprintf(msg, fmt, list);
    va_end(list);

	if (Config.PsxOut)
	{
	dprintf_console(msg);
    fprintf(emuLog, "%s", msg);
	}
}

void SysMessage(char *fmt, ...) {
	va_list list;
    char msg[512];

    va_start(list, fmt);
    vsprintf(msg, fmt, list);
    va_end(list);

    fprintf(emuLog, "%s", msg);
	fclose(emuLog);
}

void *SysLoadLibrary(char *lib) {
		return lib;
}

void *SysLoadSym(void *lib, char *sym) {
	return lib; //smhzc
}

const char *SysLibError() {
}

void SysCloseLibrary(void *lib) {
}

// Called periodically from the emu thread
void SysUpdate() {

}

// Returns to the Gui
void SysRunGui()
{

}

// Close mem and plugins
void SysClose() {
	psxShutdown();
	ReleasePlugins();
}

void OnFile_Exit() {

}

void RunCD(){ // run the cd, no bios
	LoadCdBios = 0;
	SysPrintf("RunCD\n");
	newCD(rom_path); 
	SysReset();
	CheckCdrom();
	if (LoadCdrom() == -1) {
		ClosePlugins();

		exit(0);//epic fail
	}
	psxCpu->Execute();
}

void RunCDBIOS(){ // run the bios on the cd?
	SysPrintf("RunCDBIOS\n");
	LoadCdBios = 1;
	newCD(rom_path); 
	CheckCdrom();
	SysReset();
	psxCpu->Execute();
}

void RunEXE(){
	SysPrintf("RunEXE\n");
	SysReset();
	Load(rom_path);
	psxCpu->Execute();
}

void RunBios(){
	SysPrintf("RunBios\n");
	SysReset();
	SysMessage("Bios done!!!\n");
	psxCpu->Execute();
}
//end extern C
}

void sysutil_callback (uint64_t status, uint64_t param, void *userdata) {
	(void) param;
	(void) userdata;

	switch (status) {
		case CELL_SYSUTIL_REQUEST_EXITGAME:
			printf("exit from game\n");
			is_running = 0;
			break;
		case CELL_SYSUTIL_DRAWING_BEGIN:
		case CELL_SYSUTIL_DRAWING_END:
			break;
	}
}

//we parse our ini files
static int handler(void* user, const char* section, const char* name,const char* value)
{
    FileIniConfig* pconfig = (FileIniConfig*)user;

    #define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
    if (MATCH("PS3SX", "version")) {
        pconfig->version = strdup(value);
    } else if (MATCH("psxrom", "rompath")) {
        pconfig->rompath = strdup(value);
    } else if (MATCH("psxsav", "savpath")) {
        pconfig->savpath = strdup(value);
    }else if (MATCH("psxsram", "srampath")) {
        pconfig->sram_path = strdup(value);
    }else if (MATCH("psxbios", "biospath")) {
        pconfig->biospath = strdup(value);
    }
}

void CreatFolder(char* folders)
{
	if(mkdir(folders,0777))
	{
	gl_dprintf(0.09f,0.05f,FontSize(),"Error folder cannot be created %s !!\nplease check your GenesisConf.ini\n",folders);
	sys_timer_sleep(5);
	sys_process_exit(0);
	}
}

void RomBrowser()
{
	printf("aspec ration  0x%X \n",(int)Graphics->GetDeviceAspectRatio());
	//detection 16/9 or 4/3 Anonymous
	if((int)Graphics->GetDeviceAspectRatio() == 0x1) //0x1 == 16:9 
		Graphics->SetAspectRatio(0); // 16:9
	else
		Graphics->SetAspectRatio(1); // 4:3

	//browser with roms folder
	MenuMainLoop(Iniconfig.rompath);

	InitConfig();

	if (Config.HLE){
		strcpy(Config.Bios, "HLE");
	}else{
		strcpy(Config.Bios, "scph1001.bin");  	
	}

	SysInit();

	OpenPlugins();


	//clear screen to black
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

}

int main()
{
    int i;
	struct stat st;
	sys_spu_initialize(6, 1); 
	cellSysutilRegisterCallback(0, (CellSysutilCallback)sysutil_callback, NULL); 

	cellSysmoduleLoadModule(CELL_SYSMODULE_FS);
	cellSysmoduleLoadModule(CELL_SYSMODULE_IO);

	is_running = 1;

	InitPS3();
	PS3input->Init();

	//read the ini now 
	if (ini_parse("/dev_hdd0/game/PCSX00001/Ps3sxConf.ini", handler, &Iniconfig) < 0)
	{
		gl_dprintf(0.09f,0.05f,FontSize(),"Can't load /dev_hdd0/game/PCSX00001/Ps3sxConf.ini\n");
		sys_timer_sleep(5);
		gl_dprintf(0.09f,0.05f,FontSize(),"Wtf where is the ini!!!!!!!!bye bye try again\n");
		sys_timer_sleep(5);
		sys_process_exit(0);
	}

	printf(" version  %s \n",Iniconfig.version);
	printf(" rompath  %s \n",Iniconfig.rompath);
	printf(" savpath  %s \n",Iniconfig.savpath);
	printf(" srampath %s \n",Iniconfig.sram_path);
	printf(" biospath %s \n",Iniconfig.biospath);

	//main path Check if not present creat all folder and exit
	if(stat(Iniconfig.rompath,&st) != 0)
	{
	gl_dprintf(0.09f,0.05f,FontSize(),"Creat generic folder Tree on PS3SX\n");
	sys_timer_sleep(5);
	CreatFolder(Iniconfig.rompath);
	CreatFolder(Iniconfig.savpath);
	CreatFolder(Iniconfig.sram_path);
	CreatFolder(Iniconfig.biospath);
	gl_dprintf(0.09f,0.05f,FontSize(),"generic folder Tree done!! reboot\nplease put all your roms inside %s\n",Iniconfig.rompath);
	sys_timer_sleep(5);
	sys_process_exit(0);
	}

	//Set Bios
	sprintf(Iniconfig.biospath,"%s/scph1001.bin",Iniconfig.biospath);

	printf("Run the emulator\n");

	RomBrowser();

	//clear screen to black
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);


	switch(RomType){
	 case 1: 
		 RunEXE();
	 case 2: 
		 RunCD();
	 default:
		 RunBios();
	}

	printf("done \n");
	
	cellSysmoduleUnloadModule(CELL_SYSMODULE_IO);    
	cellSysmoduleUnloadModule(CELL_SYSMODULE_FS);             
	cellSysutilUnregisterCallback(0);  
    
	 return(-1);
}
