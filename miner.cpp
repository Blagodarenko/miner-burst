#pragma warning( disable : 4715 4996)
#define _CRT_SECURE_NO_WARNINGS
#pragma warning( disable : 4290 )

//#pragma intrinsic( memset, memcpy, strlen, strcat, strcpy )

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <algorithm>
#include <lmerr.h>
#include <map>
#include <fstream>
#include <io.h>
#include <vector>
#include <thread>


#pragma comment(lib,"Ws2_32.lib")
#include <ws2tcpip.h>

#include "curses.h" 
#include "panel.h" 
#include "sph_shabal.h"
#include "mshabal.h"
#include "mshabal256.h"
#include "InstructionSet.h"

#include "rapidjson/document.h"		// rapidjson's DOM-style API
#include "rapidjson/prettywriter.h"	// for stringify JSON
#include "rapidjson/filestream.h"	// wrapper of C stream for prettywriter as output
using namespace rapidjson;



/*
#define GPU_ON_C 1
//#define GPU_ON_CPP

#ifdef GPU_ON_CPP
#define __CL_ENABLE_EXCEPTIONS 100
#include <CL\cl.hpp>

#include "OpenCL_Error.h"
#include "OpenCL_Platform.h"
#include "OpenCL_Device.h"
#endif

#ifdef GPU_ON_C
//#define __CL_ENABLE_EXCEPTIONS 100
#include <CL\cl.h>
#endif
*/

// Initialize static member data
const InstructionSet::InstructionSet_Internal InstructionSet::CPU_Rep;


HANDLE hConsole;
bool exit_flag = false;
#ifdef __AVX__
	char *version = "v1.160413_AVX";
#else
	char *version = "v1.160413";
#endif 

unsigned long long startnonce = 0;
unsigned long nonces = 0;
unsigned int scoop = 0;
 
unsigned long long deadline = 0;

//unsigned long long all_send_msg = 0;
//unsigned long long all_rcv_msg = 0;
//unsigned long long err_send_msg = 0;
//unsigned long long err_rcv_msg = 0;

//unsigned long long all_send_dl = 0;
//unsigned long long all_rcv_dl = 0;
//unsigned long long err_send_dl = 0;
//unsigned long long err_rcv_dl = 0;
int network_quality = 100;
 
char signature[33];
char str_signature[65];
char oldSignature[33];
 
char nodeaddr[100] = "localhost";	// адрес пула
size_t nodeport = 8125;				// порт пула

char updateraddr[100] = "localhost";// адрес пула
size_t updaterport = 8125;			// порт пула

char infoaddr[100] = "localhost";	// адрес пула
size_t infoport = 8125;				// порт пула

size_t proxyport = 8126;			// порт пула

char *p_minerPath;					// путь к папке майнера
size_t miner_mode = 0;				// режим майнера. 0=соло, 1=пул
size_t cache_size = 100000;			// размер кэша чтения плотов
std::vector<std::string> paths_dir; // пути
bool show_msg = false;				// Показать общение с сервером в отправщике
bool show_updates = false;			// Показать общение с сервером в апдейтере
FILE * fp_Log;						// указатель на лог-файл
FILE * fp_Stat;						// указатель на стат-файл
FILE * fp_Time;						// указатель на стат-файл
size_t send_interval = 100;			// время ожидания между отправками
size_t update_interval = 1000;		// время ожидания между апдейтами
short win_size_x = 80;
short win_size_y = 60;
bool use_fast_rcv = false;
bool use_debug = false;
bool enable_proxy = false;
bool send_best_only = true;
bool use_wakeup = false;
bool use_log = true;				// Вести лог
bool use_boost = false;				// Использовать повышенный приоритет для потоков
bool show_winner = true;			// показывать победителя



unsigned long long my_target_deadline = MAXDWORD;// 4294967295;
SYSTEMTIME cur_time;				// Текущее время
unsigned long long total_size = 0;	// Общий объем плотов

char *json = nullptr;

WINDOW * win_main;
//PANEL  *panel_main;

std::vector<size_t> worker_progress;
std::vector<std::thread> worker;
std::map <u_long, unsigned long long> satellite_size; // Структура с объемами плотов сателлитов

struct t_files{
	std::string Path;
	std::string Name;
	unsigned long long Size;// = 0;
	unsigned State;// = 0;
	//t_files(std::string p_Path, std::string p_Name, unsigned long long p_Size, unsigned p_State) : Path(std::move(p_Path)), Name(std::move(p_Name)), Size(p_Size), State(p_State){};
	//t_files(t_files &&fill) : Path(std::move(fill.Path)), Name(std::move(fill.Name)), Size(fill.Size), State(fill.State){};
	//t_files& operator=(const t_files& fill) = default;
};

struct t_shares{
	std::string file_name;
	unsigned long long account_id;// = 0;
	unsigned long long best;// = 0;
	unsigned long long nonce;// = 0;
	//t_shares(std::string p_file_name, unsigned long long p_account_id, unsigned long long p_best, unsigned long long p_nonce) : file_name(std::move(p_file_name)), account_id(p_account_id), best(p_best), nonce(p_nonce){};
}; 

std::vector<t_shares> shares;

struct t_best{
	unsigned long long account_id;// = 0;
	unsigned long long best;// = 0;
	unsigned long long nonce;// = 0;
	unsigned long long DL;// = 0;
	unsigned long long targetDeadline;// = 0;
	//t_best(unsigned long long p_account_id, unsigned long long p_best, unsigned long long p_nonce, unsigned long long p_DL, unsigned long long p_targetDeadline) : account_id(p_account_id), best(p_best), nonce(p_nonce), DL(p_DL), targetDeadline(p_targetDeadline){};
};

std::vector<t_best> bests;

struct t_session{
	SOCKET Socket;
	unsigned long long ID;
	unsigned long long deadline;
};

std::vector<t_session> sessions;

#ifdef GPU_ON_C
struct t_gpu{
	size_t max_WorkGroupSize = 1;
	size_t use_gpu_platform = 0;
	size_t use_gpu_device = 0;
	cl_device_id *devices = nullptr;
	cl_uint num_devices = 0;
	cl_uint max_ComputeUnits = 1;
};
t_gpu gpu_devices;
#endif

unsigned long long height = 0;
unsigned long long baseTarget = 0;
unsigned long long targetDeadlineInfo = 0; // Максимальный дедлайн пула
volatile int stopThreads = 0;
char *pass;						// пароль
 
CRITICAL_SECTION sessionsLock;	// обновление sessions
CRITICAL_SECTION bestsLock;		// обновление bests
CRITICAL_SECTION sharesLock;	// обновление shares


void ShowMemError(void);

void Log_init(void)
{
	if (use_log)
	{
		char * filename = (char*)calloc(MAX_PATH, sizeof(char));
		if (filename == nullptr)
		{
			ShowMemError();
			use_log = false;
			return;
		}
		if (CreateDirectory(L"Logs", nullptr) == ERROR_PATH_NOT_FOUND)
		{
			wattron(win_main, COLOR_PAIR(12));
			wprintw(win_main, "CreateDirectory failed (%d)\n", GetLastError(), 0);
			wattroff(win_main, COLOR_PAIR(12));
			use_log = false;
			free(filename);
			return;
		}
		GetLocalTime(&cur_time);
		sprintf_s(filename, _msize(filename), "Logs\\%02d-%02d-%02d_%02d_%02d_%02d.log", cur_time.wYear, cur_time.wMonth, cur_time.wDay, cur_time.wHour, cur_time.wMinute, cur_time.wSecond);
		fopen_s(&fp_Log, filename, "wt");
		if (fp_Log == nullptr)
		{
			wattron(win_main, COLOR_PAIR(12));
			wprintw(win_main, "LOG: file openinig error\n", 0);
			wattroff(win_main, COLOR_PAIR(12));
			use_log = false;
		}
		free(filename);
	}
}

void Log(char * strLog)
{
	if (use_log)
	{
		// если строка содержит интер, то добавить время  
		if (strLog[0] == '\n')
		{
			GetLocalTime(&cur_time);
			fprintf(fp_Log, "\n%02d:%02d:%02d %s", cur_time.wHour, cur_time.wMinute, cur_time.wSecond, strLog + 1);
		}
		else fprintf(fp_Log, "%s", strLog);
		fflush(fp_Log);
	}
}

void Log_server(char * strLog)
{
	size_t len_str = strlen(strLog);
	if ((len_str> 0) && use_log)
	{
		char * Msg_log = (char *)calloc(len_str * 2 + 1, sizeof(char));
		if (Msg_log == nullptr)
		{
			ShowMemError();
			exit(-1);
		}
		for (size_t i = 0, j = 0; i<len_str; i++, j++)
		{
			if(strLog[i] == '\r')
			{	
				Msg_log[j] = '\\'; 
				j++;
				Msg_log[j] = 'r';}
			else 
				if(strLog[i] == '\n') 
				{ 
					Msg_log[j] = '\\'; 
					j++;
					Msg_log[j] = 'n';
				}
				else
				if (strLog[i] == '%')
				{
					Msg_log[j] = '%';
					j++;
					Msg_log[j] = '%';
				}
				else Msg_log[j] = strLog[i];
		}
		
		fprintf(fp_Log, "%s", Msg_log);
		free(Msg_log);
		fflush(fp_Log);
	}
}

void Log_llu(unsigned long long llu_num)
{
	if (use_log)
	{
		fprintf(fp_Log, "%llu", llu_num);
		fflush(fp_Log);
	}
}

void Log_u(size_t u_num)
{
	if (use_log)
	{
		fprintf(fp_Log, "%u", (unsigned)u_num);
		fflush(fp_Log);
	}
}

void ShowMemError(void)
{
	Log("\n!!! Error allocating memory");
	wattron(win_main, COLOR_PAIR(12));
	wprintw(win_main, "Error allocating memory\n", 0);
	wattroff(win_main, COLOR_PAIR(12));
}


int load_config(char *filename)
{
	FILE * pFile;
	size_t len;

	fopen_s(&pFile, filename, "rt");

	if (pFile == nullptr)
	{
		wattron(win_main, COLOR_PAIR(12));
		wprintw(win_main, "miner.conf not found\n", 0);
		wattroff(win_main, COLOR_PAIR(12));
		return -1;
	}

	_fseeki64(pFile, 0, SEEK_END);
	__int64 size = _ftelli64(pFile);
	_fseeki64(pFile, 0, SEEK_SET);
	json = (char*)calloc(size + 1, sizeof(char));
	if (json == nullptr) {
		ShowMemError();
		exit(-1);
	}
	len = fread_s(json, size, 1, size - 1, pFile);
	fclose(pFile);

	Document document;	// Default template parameter uses UTF8 and MemoryPoolAllocator.
	if (document.Parse<0>(json).HasParseError()){
		wattron(win_main, COLOR_PAIR(12));
		wprintw(win_main, "JSON format error\n", 0);
		wattroff(win_main, COLOR_PAIR(12));
		exit(-1);
	}

	if (document.ParseInsitu<0>(json).HasParseError()){
		wattron(win_main, COLOR_PAIR(12));
		wprintw(win_main, "JSON format error (Insitu)\n", 0);
		wattroff(win_main, COLOR_PAIR(12));
		exit(-1);
	}
	

	if(document.IsObject())
	{	// Document is a JSON value represents the root of DOM. Root can be either an object or array.

		if (document.HasMember("UseLog") && (document["UseLog"].IsBool()))
			use_log = document["UseLog"].GetBool();

		Log_init();

		if(document.HasMember("Mode") && document["Mode"].IsString())
		{
			Log("\nMode: ");
			if(strcmp(document["Mode"].GetString(), "solo")==0) miner_mode = 0;
			else 
				if(strcmp(document["Mode"].GetString(), "pool")==0) miner_mode = 1;
				else
					if (strcmp(document["Mode"].GetString(), "poolV2") == 0) miner_mode = 2;
			Log_u(miner_mode);
		}

		Log("\nServer: "); 
		if(document.HasMember("Server") &&	document["Server"].IsString())
			strcpy(nodeaddr, document["Server"].GetString());
		Log(nodeaddr);

			Log("\nPort: "); 
		if(document.HasMember("Port") && (document["Port"].IsUint()))		// In this case, IsUint()/IsInt64()/IsUInt64() also return true.
			nodeport = (size_t)document["Port"].GetUint();
		Log_u(nodeport);

		if(document.HasMember("Paths") && document["Paths"].IsArray()){
			const Value& Paths = document["Paths"];	// Using a reference for consecutive access is handy and faster.
			for (SizeType i = 0; i < Paths.Size(); i++){	// rapidjson uses SizeType instead of size_t.
				paths_dir.push_back(Paths[i].GetString()); 
				Log("\nPath: "); Log((char*)paths_dir[i].c_str()); 
			}
		}
		
		if(document.HasMember("CacheSize") && (document["CacheSize"].IsUint64()))		// In this case, IsUint()/IsInt64()/IsUInt64() also return true.
			cache_size = document["CacheSize"].GetUint64();

		Log("\nCacheSize: "); Log_u(cache_size);
		
		if(document.HasMember("UseHDDWakeUp") && (document["UseHDDWakeUp"].IsBool()))		
			use_wakeup = document["UseHDDWakeUp"].GetBool();
		Log("\nUseHDDWakeUp: "); Log_u(use_wakeup);

		if(document.HasMember("ShowMsg") && (document["ShowMsg"].IsBool()))		
			show_msg = document["ShowMsg"].GetBool();
		Log("\nShowMsg: "); Log_u(show_msg);

		if(document.HasMember("ShowUpdates") && (document["ShowUpdates"].IsBool()))		
			show_updates = document["ShowUpdates"].GetBool();
		Log("\nShowUpdates: "); Log_u(show_updates);

		Log("\nSendInterval: "); 
		if(document.HasMember("SendInterval") && (document["SendInterval"].IsUint()))		// In this case, IsUint()/IsInt64()/IsUInt64() also return true.
			send_interval = (size_t)document["SendInterval"].GetUint();
		Log_u(send_interval);

		Log("\nUpdateInterval: "); 
		if(document.HasMember("UpdateInterval") && (document["UpdateInterval"].IsUint()))		// In this case, IsUint()/IsInt64()/IsUInt64() also return true.
			update_interval = (size_t)document["UpdateInterval"].GetUint();
		Log_u(update_interval);

		if(document.HasMember("UseFastRcv") && (document["UseFastRcv"].IsBool()))		
			use_fast_rcv = document["UseFastRcv"].GetBool();
		Log("\nUseFastRcv: "); Log_u(use_fast_rcv);

		if(document.HasMember("Debug") && (document["Debug"].IsBool()))		
			use_debug = document["Debug"].GetBool();
		Log("\nDebug: "); Log_u(use_debug);
				
		if (document.HasMember("UpdaterAddr") && document["UpdaterAddr"].IsString())
			strcpy(updateraddr, document["UpdaterAddr"].GetString());
		Log("\nUpdater address: "); Log(updateraddr);

		if (document.HasMember("UpdaterPort") && (document["UpdaterPort"].IsUint()))		
			updaterport = (size_t)document["UpdaterPort"].GetUint();
		Log("\nUpdater port: "); Log_u(updaterport);

		if (document.HasMember("InfoAddr") && document["InfoAddr"].IsString())
			strcpy(infoaddr, document["InfoAddr"].GetString());
		else strcpy(infoaddr, updateraddr);
		Log("\nInfo address: "); Log(infoaddr);

		if (document.HasMember("InfoPort") && (document["InfoPort"].IsUint()))
			infoport = (size_t)document["InfoPort"].GetUint();
		else infoport = updaterport;
		Log("\nInfo port: "); Log_u(infoport);


		if (document.HasMember("EnableProxy") && (document["EnableProxy"].IsBool()))
		enable_proxy = document["EnableProxy"].GetBool();
		Log("\nEnableProxy: "); Log_u(enable_proxy);

		Log("\nProxyPort: ");
		if (document.HasMember("ProxyPort") && (document["ProxyPort"].IsUint()))		// In this case, IsUint()/IsInt64()/IsUInt64() also return true.
			proxyport = (size_t)document["ProxyPort"].GetUint();
		Log_u(proxyport);

		if (document.HasMember("ShowWinner") && (document["ShowWinner"].IsBool()))
			show_winner = document["ShowWinner"].GetBool();
		Log("\nShowWinner: "); Log_u(show_winner);

		if (document.HasMember("SendBestOnly") && (document["SendBestOnly"].IsBool()))
			send_best_only = document["SendBestOnly"].GetBool();
		Log("\nSendBestOnly: "); Log_u(send_best_only);

		if (document.HasMember("TargetDeadline") && (document["TargetDeadline"].IsInt64()))
			my_target_deadline = document["TargetDeadline"].GetUint64();
		Log("\nTargetDeadline: "); Log_llu(my_target_deadline);

		if (document.HasMember("UseBoost") && (document["UseBoost"].IsBool()))
			use_boost = document["UseBoost"].GetBool();
		Log("\nUseBoost: "); Log_u(use_boost);

		
		if(document.HasMember("WinSizeX") && (document["WinSizeX"].IsUint()))		
			win_size_x = (short)document["WinSizeX"].GetUint();
		Log("\nWinSizeX: ");  Log_u(win_size_x);

		if(document.HasMember("WinSizeY") && (document["WinSizeY"].IsUint()))		
			win_size_y = (short)document["WinSizeY"].GetUint();
		Log("\nWinSizeY: ");  Log_u(win_size_y);

#ifdef GPU_ON_C
		if (document.HasMember("GPU_Platform") && (document["GPU_Platform"].IsInt()))
			gpu_devices.use_gpu_platform = (size_t)document["GPU_Platform"].GetUint();
		Log("\nGPU_Platform: "); Log_llu(gpu_devices.use_gpu_platform);
	
		if (document.HasMember("GPU_Device") && (document["GPU_Device"].IsInt()))
			gpu_devices.use_gpu_device = (size_t)document["GPU_Device"].GetUint();
		Log("\nGPU_Device: "); Log_llu(gpu_devices.use_gpu_device);
#endif	

	}
	 
	Log("\n=== Config loaded ===");
	free(json);
	return 1;
}

/*
LPSTR DisplayErrorText( DWORD dwLastError )
{ 
#pragma warning(suppress: 6102)
	HMODULE hModule = nullptr; // default to system source 
	LPSTR MessageBuffer = nullptr; 
	DWORD dwBufferLength; 
	DWORD dwFormatFlags = FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_SYSTEM ;

	if(dwLastError >= NERR_BASE && dwLastError <= MAX_NERR) 
	{ 
		hModule = LoadLibraryEx( TEXT("netmsg.dll"), nullptr, LOAD_LIBRARY_AS_DATAFILE );
		if(hModule != nullptr) dwFormatFlags |= FORMAT_MESSAGE_FROM_HMODULE;
	} 
	dwBufferLength = FormatMessageA(dwFormatFlags, hModule, dwLastError, MAKELANGID(LANG_SYSTEM_DEFAULT, SUBLANG_SYS_DEFAULT), (LPSTR)&MessageBuffer, 0, nullptr);
	if(hModule != nullptr) FreeLibrary(hModule); 

	return MessageBuffer;
}
*/

// Helper routines taken from http://stackoverflow.com/questions/1557400/hex-to-char-array-in-c
int xdigit( char digit ){
  int val;
       if( '0' <= digit && digit <= '9' ) val = digit -'0';
  else if( 'a' <= digit && digit <= 'f' ) val = digit -'a'+10;
  else if( 'A' <= digit && digit <= 'F' ) val = digit -'A'+10;
  else                                    val = -1;
  return val;
}
 
size_t xstr2strr(char *buf, size_t bufsize, const char *in) {
  if( !in ) return 0; // missing input string
 
  size_t inlen = (size_t)strlen(in);
  if( inlen%2 != 0 ) inlen--; // hex string must even sized
 
  size_t i,j;
  for(i=0; i<inlen; i++ )
    if( xdigit(in[i])<0 ) return 0; // bad character in hex string
 
  if( !buf || bufsize<inlen/2+1 ) return 0; // no buffer or too small
 
  for(i=0,j=0; i<inlen; i+=2,j++ )
    buf[j] = xdigit(in[i])*16 + xdigit(in[i+1]);
 
  buf[inlen/2] = '\0';
  return inlen/2+1;
}


std::wstring str2wstr(std::string &s)
{
	int slen = (int)s.length() + 1;
	size_t len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slen, 0, 0);
	wchar_t *buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slen, buf, (int)len);
	std::wstring r(buf);
	delete[] buf;
	return r;
}

 
void GetPass(char* p_strFolderPath)
{
  FILE * pFile;
  size_t lSize;
  unsigned char * buffer;
  size_t len_pass;
  char * filename = (char*)calloc(MAX_PATH, sizeof(char));
  if (filename == nullptr)
  {
	  ShowMemError();
	  exit(-1);
  }
  sprintf_s(filename, _msize(filename), "%s%s", p_strFolderPath, "passphrases.txt");
  
//  printf_s("\npass from: %s\n",filename);
  fopen_s(&pFile, filename, "rt");
  if (pFile==nullptr) 
  {
	  wattron(win_main, COLOR_PAIR(12));
	  wprintw(win_main, "passphrases.txt not found\n", 0);
	  wattroff(win_main, COLOR_PAIR(12));
	  exit (-1);
  }

  _fseeki64(pFile , 0 , SEEK_END);
  lSize = _ftelli64(pFile);
  _fseeki64(pFile, 0, SEEK_SET);

  buffer = (unsigned char*)calloc(lSize + 1, sizeof(char));
  if (buffer == nullptr) 
  {
	  ShowMemError();
	  exit(-1);
  }
  
  len_pass = fread(buffer, 1, lSize, pFile);

  fclose(pFile);
  free (filename);
  pass = (char*)calloc(lSize * 3, sizeof(char));
  if (pass == nullptr)
  {
	  ShowMemError();
	  exit(-1);
  }
  
	for(size_t i=0, j=0; i<len_pass; i++, j++) 
	{
		if ((buffer[i] == '\n') || (buffer[i] == '\r') || (buffer[i] == '\t')) j--; // Пропускаем символы, переделать buffer[i] < 20
		else
			if (buffer[i] == ' ') pass[j] = '+';
			else 
				if(isalnum(buffer[i])==0)
				{
					sprintf(pass + j, "%%%x", (unsigned char)buffer[i]);
					j = j+2;
				}
				else memcpy(&pass[j],&buffer[i],1);
  }
  //printf_s("\n%s\n",pass);
  free (buffer);
}



size_t GetFiles(const std::string &str, std::vector <t_files> *p_files)
{
    HANDLE hFile = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATAA   FindFileData;
	size_t i = 0;
	std::vector<std::string> path;
	size_t first = 0;
	size_t last = 0;
	
	do{
		last = str.find("+", first);
		if (last == -1) last = str.length();
		std::string str2(str.substr(first, last - first));
		if (str2.rfind("\\") < str2.length() - 1) str2 = str2 + "\\";
		path.push_back(str2);
		first = last + 1;
	} while (last != str.length());
	
	for (auto iter = path.begin(); iter != path.end(); ++iter)
	{
		hFile = FindFirstFileA(LPCSTR((*iter + "*").c_str()), &FindFileData);
		if (INVALID_HANDLE_VALUE != hFile)
		{
			do
			{
				if (FILE_ATTRIBUTE_DIRECTORY & FindFileData.dwFileAttributes) continue; //Skip directories
				char* ekey = strstr(FindFileData.cFileName, "_");
				if (ekey != nullptr)
				{
					char* estart = strstr(ekey + 1, "_");
					if (estart != nullptr)
					{
						char* enonces = strstr(estart + 1, "_");
						if (enonces != nullptr)
						{
							p_files->push_back({
								*iter,
								FindFileData.cFileName,
								(((static_cast<ULONGLONG>(FindFileData.nFileSizeHigh) << sizeof(FindFileData.nFileSizeLow) * 8) | FindFileData.nFileSizeLow)),
								0 });
							i++;
						}
					}
				}
			} while (FindNextFileA(hFile, &FindFileData));
			FindClose(hFile);
		}
	}
	return i;
}

/*
size_t Get_index_acc(unsigned long long key)
{
	EnterCriticalSection(&bestsLock);
	size_t cnt = bests.size();
	for (size_t a = 0; a < cnt; a++)
	{
		if (bests[a].account_id == key)
		{
			LeaveCriticalSection(&bestsLock);
			//Log("\nbests[a].targetDeadline = "); Log_llu(bests[a].targetDeadline);
			return a;
		}
	}
	bests.emplace_back(key, 0, 0, 0, targetDeadlineInfo);
	size_t acc_index = bests.size() - 1;
	LeaveCriticalSection(&bestsLock);
	return acc_index;
}
*/

size_t Get_index_acc(unsigned long long key)
{
	EnterCriticalSection(&bestsLock);
	size_t acc_index = 0;
	for (auto it = bests.begin(); it != bests.end(); ++it)
	{
		if (it->account_id == key)
		{
			LeaveCriticalSection(&bestsLock);
			return acc_index;
		}
		acc_index++;
	}
	bests.push_back({key, 0, 0, 0, targetDeadlineInfo});
	LeaveCriticalSection(&bestsLock);
	return bests.size() - 1;
}


/*
void gen_nonce(unsigned long long addr, unsigned long long n, unsigned long long size) {
	#define PLOT_SIZE	(4096 * 64)
	#define HASH_SIZE	32
	#define HASH_CAP	4096
	char * final = (char *)malloc(sizeof(char)*32);
	char * gendata = (char *)malloc(sizeof(char)*(16 + PLOT_SIZE));
	char * cache = (char *)malloc(sizeof(char)*(64*1000));
	char *xv = (char*)&addr;
	
	gendata[PLOT_SIZE] = xv[7]; gendata[PLOT_SIZE+1] = xv[6]; gendata[PLOT_SIZE+2] = xv[5]; gendata[PLOT_SIZE+3] = xv[4];
	gendata[PLOT_SIZE+4] = xv[3]; gendata[PLOT_SIZE+5] = xv[2]; gendata[PLOT_SIZE+6] = xv[1]; gendata[PLOT_SIZE+7] = xv[0];

	sph_shabal_context x;
	unsigned int i, len;
	unsigned long long z;
	for (z = n; (z < (n + size)) && (!stopThreads); z++)
	{
		xv = (char*)&z;
		gendata[PLOT_SIZE + 8] = xv[7]; gendata[PLOT_SIZE + 9] = xv[6]; gendata[PLOT_SIZE + 10] = xv[5]; gendata[PLOT_SIZE + 11] = xv[4];
		gendata[PLOT_SIZE + 12] = xv[3]; gendata[PLOT_SIZE + 13] = xv[2]; gendata[PLOT_SIZE + 14] = xv[1]; gendata[PLOT_SIZE + 15] = xv[0];

		for (i = PLOT_SIZE; i > 0; i -= HASH_SIZE)
		{
			shabal_init(&x, 256);
			len = PLOT_SIZE + 16 - i;
			if (len > HASH_CAP)	len = HASH_CAP;
			shabal_core(&x, (const unsigned char*)&gendata[i], len);
			shabal_close(&x, 0, 0, &gendata[i - HASH_SIZE], 8);
		}

		shabal_init(&x, 256);
		shabal_core(&x, (const unsigned char*)gendata, 16 + PLOT_SIZE);
		shabal_close(&x, 0, 0, final, 8);

		// XOR with final
		//for(i = 0; i < PLOT_SIZE; i ++)
		//	gendata[i] ^= (final[i % HASH_SIZE]);
		for (i = scoop * 64; i < ((scoop + 1) * 64); i++)
			gendata[i] ^= (final[i % HASH_SIZE]);
		memmove(&cache[(z-n)*64], &gendata[scoop * 64], 64);
	}
	free(final);
	free(gendata);
	


        char sig[32 + 64];
		unsigned acc = 0;
		char res[32];
        memmove(sig, signature, 32);
		
		for (z = 0; (z < size) && (!stopThreads); z++)
        {
			memmove(&sig[32], &cache[z*64], 64);
                //memmove(&sig[32], &gendata[scoop*64], 64);
				
                
				shabal_init(&x, 256);
                shabal_core(&x, (const unsigned char*) sig, 64 + 32);
                shabal_close(&x, 0, 0, res, 8);
				
                unsigned long long *wertung = (unsigned long long*)res;
				//printf("\r[%20llu][%llu]\tfound deadline %llu\n", bests[acc].account_id, nonce, *wertung / baseTarget);
              
				if (bests[acc].nonce == 0 || *wertung <= bests[acc].best)  
				{
                    bests[acc].best = *wertung;
					bests[acc].nonce = n;
					if ((bests[acc].best / baseTarget) <= bests[acc].targetDeadline) 
					{       
							Log("\nMEM: found deadline=");	Log_llu(bests[acc].best/baseTarget); Log(" nonce=");	Log_llu(bests[acc].nonce); Log(" for account: "); Log_llu(bests[acc].account_id);
							pthread_mutex_lock(&byteLock);
								shares[num_shares].best = bests[acc].best;
								shares[num_shares].nonce = bests[acc].nonce;
								shares[num_shares].to_send = 1;
								shares[num_shares].account_id = bests[acc].account_id;
								shares[num_shares].file_name = "Memory";
								//if(use_debug)
								{
									cls();
									SetConsoleTextAttribute(hConsole, 2);
									printf("\r[%llu]\tMEM: found deadline %llu\n", bests[acc].account_id, shares[num_shares].best / baseTarget);
									SetConsoleTextAttribute(hConsole, 7);
								}
								num_shares++;
							pthread_mutex_unlock(&byteLock);
                     }
					
				}
        }
		free(cache);
		
}

void *generator_i(void *x_void_ptr)
{
	unsigned long long nonce = 100000000;// * (local_num + 1);
	unsigned long long size = 100;
	clock_t start_work_time;
	do
	{
		start_work_time = clock();
		gen_nonce(bests[0].account_id, nonce, size);
		cls();
		printf_s("\r[%llu]  noces/min:  %llu \n", bests[0].account_id, ((clock() - start_work_time)*size / CLOCKS_PER_SEC) / 60);
		nonce = nonce + size;
	} while (!stopThreads);
	return 0;
}

*/

void proxy_i(void)
{
	int iResult;
	unsigned buffer_size = 1000;
	char* buffer = (char*)calloc(buffer_size, sizeof(char));
	if (buffer == nullptr) {
		ShowMemError();
		exit(-1);
	}
	char* tmp_buffer = (char*)calloc(buffer_size, sizeof(char));
	if (tmp_buffer == nullptr) {
		ShowMemError();
		exit(-1);
	}
	char tbuffer[9];
	SOCKET ServerSocket = INVALID_SOCKET;
	SOCKET ClientSocket = INVALID_SOCKET;
	struct addrinfo *result = nullptr;
	struct addrinfo hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET; //AF_UNSPEC;  // использовать IPv4 или IPv6, нам неважно
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	char pport[6];
	_itoa((int)proxyport, pport, 10);
	iResult = getaddrinfo(nullptr, pport, &hints, &result);
	if (iResult != 0) {
		wattron(win_main, COLOR_PAIR(12));
		wprintw(win_main, "PROXY: getaddrinfo failed with error: %d\n", iResult, 0);
		wattroff(win_main, COLOR_PAIR(12));
	}

	ServerSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ServerSocket == INVALID_SOCKET) {
		wattron(win_main, COLOR_PAIR(12));
		wprintw(win_main, "PROXY: socket failed with error: %ld\n", WSAGetLastError(), 0);
		wattroff(win_main, COLOR_PAIR(12));
		freeaddrinfo(result);
	}
	
	iResult = bind(ServerSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		wattron(win_main, COLOR_PAIR(12));
		wprintw(win_main, "PROXY: bind failed with error: %d\n", WSAGetLastError(), 0);
		wattroff(win_main, COLOR_PAIR(12));
		freeaddrinfo(result);
		closesocket(ServerSocket);
	}
	freeaddrinfo(result);
	BOOL l = TRUE;
	iResult = ioctlsocket(ServerSocket, FIONBIO, (unsigned long*)&l);
	if (iResult == SOCKET_ERROR)
	{
		Log("\nProxy: ! Error ioctlsocket's: "); Log_u(WSAGetLastError());
		wattron(win_main, COLOR_PAIR(12));
		wprintw(win_main, "PROXY: ioctlsocket failed: %ld\n", WSAGetLastError(), 0);
		wattroff(win_main, COLOR_PAIR(12));
	}

	iResult = listen(ServerSocket, 8);
	if (iResult == SOCKET_ERROR) {
		wattron(win_main, COLOR_PAIR(12));
		wprintw(win_main, "PROXY: listen failed with error: %d\n", WSAGetLastError(), 0);
		wattroff(win_main, COLOR_PAIR(12));
		closesocket(ServerSocket);
	}
	Log("\nProxy thread started");

	for (; !exit_flag;)
	{
		struct sockaddr_in client_socket_address;
		int iAddrSize = sizeof(struct sockaddr_in);
		ClientSocket = accept(ServerSocket, (struct sockaddr *)&client_socket_address, (socklen_t*)&iAddrSize);
		if (ClientSocket == INVALID_SOCKET)
		{
			if (WSAGetLastError() != WSAEWOULDBLOCK)
			{
				Log("\nProxy:! Error Proxy's accept: "); Log_u(WSAGetLastError());
				wattron(win_main, COLOR_PAIR(12));
				wprintw(win_main, "PROXY: can't accept. Error: %ld\n", WSAGetLastError(), 0);
				wattroff(win_main, COLOR_PAIR(12));
			}
		}
		else
		{
			memset(buffer, 0, _msize(buffer));
			do{
				memset(tmp_buffer, 0, _msize(buffer));
				iResult = recv(ClientSocket, tmp_buffer, buffer_size - 1, 0);
				strcat(buffer, tmp_buffer);
			} while ((iResult > 0) && !use_fast_rcv);

			Log("\nProxy get info: ");  Log_server(buffer);
			unsigned long long get_accountId = 0;
			unsigned long long get_nonce = 0;
			unsigned long long get_deadline = 0;
			unsigned long long get_totalsize = 0;
			// locate HTTP header
			char *find = strstr(buffer, "\r\n\r\n");
			if (find != nullptr)
			{
				if (strstr(buffer, "submitNonce") != nullptr)
				{

					char *startaccountId = strstr(buffer, "accountId=");
					if (startaccountId != nullptr)
					{
						startaccountId = strpbrk(startaccountId, "0123456789");
						char *endaccountId = strpbrk(startaccountId, "& }\"");

						char *startnonce = strstr(buffer, "nonce=");
						char *startdl = strstr(buffer, "deadline=");
						char *starttotalsize = strstr(buffer, "X-Capacity");
						if ((startnonce != nullptr) && (startdl != nullptr))
						{
							startnonce = strpbrk(startnonce, "0123456789");
							char *endnonce = strpbrk(startnonce, "& }\"");
							startdl = strpbrk(startdl, "0123456789");
							char *enddl = strpbrk(startdl, "& }\"");

							endaccountId[0] = 0;
							endnonce[0] = 0;
							enddl[0] = 0;

							get_accountId = _strtoui64(startaccountId, 0, 10);
							get_nonce = _strtoui64(startnonce, 0, 10);
							get_deadline = _strtoui64(startdl, 0, 10);

							if (starttotalsize != nullptr)
							{
								starttotalsize = strpbrk(starttotalsize, "0123456789");
								char *endtotalsize = strpbrk(starttotalsize, "& }\"");
								endtotalsize[0] = 0;
								get_totalsize = _strtoui64(starttotalsize, 0, 10);
								satellite_size.insert(std::pair <u_long, unsigned long long>(client_socket_address.sin_addr.S_un.S_addr, get_totalsize));
							}
							EnterCriticalSection(&sharesLock);
							shares.push_back({ inet_ntoa(client_socket_address.sin_addr), get_accountId, get_deadline, get_nonce });
							LeaveCriticalSection(&sharesLock);
							{
								_strtime(tbuffer);
								wattron(win_main, COLOR_PAIR(2));
								wprintw(win_main, "%s [%20llu]\treceived DL: %11llu {%s}\n", tbuffer, get_accountId, get_deadline / baseTarget, inet_ntoa(client_socket_address.sin_addr), 0);
								wattroff(win_main, COLOR_PAIR(2));
							}
							Log("Proxy: received DL "); Log_llu(get_deadline); Log(" from "); Log(inet_ntoa(client_socket_address.sin_addr));

							//Подтверждаем
							memset(buffer, 0, _msize(buffer));
							size_t acc = Get_index_acc(get_accountId);
							int bytes = sprintf_s(buffer, _msize(buffer), "HTTP/1.0 200 OK\r\nConnection: close\r\n\r\n{\"result\": \"proxy\",\"accountId\": %llu,\"deadline\": %llu,\"targetDeadline\": %llu}", get_accountId, get_deadline / baseTarget, bests[acc].targetDeadline);
							iResult = send(ClientSocket, buffer, bytes, 0);
							if (iResult == SOCKET_ERROR)
							{
								Log("\nProxy: ! Error sending to client: "); Log_u(WSAGetLastError());
								wattron(win_main, COLOR_PAIR(12));
								wprintw(win_main, "PROXY: failed sending to client: %ld\n", WSAGetLastError(), 0);
								wattroff(win_main, COLOR_PAIR(12));
							}
							else
							{
								if (use_debug)
								{
									_strtime(tbuffer);
									wattron(win_main, COLOR_PAIR(9));
									wprintw(win_main, "%s [%20llu]\tsent confirmation to %s\n", tbuffer, get_accountId, inet_ntoa(client_socket_address.sin_addr), 0);
									wattroff(win_main, COLOR_PAIR(9));
								}
								Log("\nProxy: sent confirmation to "); Log(inet_ntoa(client_socket_address.sin_addr));
							}
						}
					}
				}
				else
				{
					if (strstr(buffer, "getMiningInfo") != nullptr)
					{
						memset(buffer, 0, _msize(buffer));
						int bytes = sprintf_s(buffer, _msize(buffer), "HTTP/1.0 200 OK\r\nConnection: close\r\n\r\n{\"baseTarget\":\"%llu\",\"height\":\"%llu\",\"generationSignature\":\"%s\",\"targetDeadline\":%llu}", baseTarget, height, str_signature, targetDeadlineInfo);
						iResult = send(ClientSocket, buffer, bytes, 0);
						if (iResult == SOCKET_ERROR)
						{
							Log("\nProxy: ! Error sending to client: "); Log_u(WSAGetLastError());
							wattron(win_main, COLOR_PAIR(12));
							wprintw(win_main, "PROXY: failed sending to client: %ld\n", WSAGetLastError(), 0);
							wattroff(win_main, COLOR_PAIR(12));
						}
						else
						{
							Log("\nProxy: sent update to "); Log(inet_ntoa(client_socket_address.sin_addr));
						}
					}
					else
					{
						if ((strstr(buffer, "getBlocks") != nullptr) || (strstr(buffer, "getAccount") != nullptr) || (strstr(buffer, "getRewardRecipient") != nullptr))
						{
							; // ничего не делаем, не ошибка, пропускаем
						}
						else
						{
							find[0] = 0;
							wattron(win_main, COLOR_PAIR(15));
							wprintw(win_main, "PROXY: %s\n", buffer, 0);
							wattroff(win_main, COLOR_PAIR(15));
						}
					}
				}
			}
			iResult = closesocket(ClientSocket);
		}
		std::this_thread::yield();
		std::this_thread::sleep_for(std::chrono::milliseconds(200));
	}
	free(buffer);
	free(tmp_buffer);
}

void send_i(void)
{
	Log("\nSender: started thread");
	SOCKET ConnectSocket;

	int iResult = 0;
	char* buffer = (char*)calloc(1000, sizeof(char));
	if (buffer == nullptr) {
		ShowMemError();
		exit(-1);
	}

	char tbuffer[9];

	struct addrinfo *result = nullptr;
	struct addrinfo hints;
	
	for (;!exit_flag;)
	{
		//while (shares.empty() && sessions.empty() && !exit_flag)	 //пауза при бездействии   
		//{
		//	std::this_thread::yield();
		//	std::this_thread::sleep_for(std::chrono::milliseconds(100));
		//}
		
		//unsigned long long part = num_shares;
		//if ((part - i > 1) && (sent_num_shares != part))
		//{
		//	if (use_sorting) qsort(&shares[0], part, sizeof(t_shares), comp_min);
		//	else  qsort(&shares[0], part, sizeof(t_shares), comp_max);
		//	for (unsigned long long j = 0; j < part; j++)
		//	if (shares[j].to_send == 1)
		//	{
		//		i = j;
		//		break;
		//	}
		//}

		if (stopThreads == 1)
		{
			free(buffer);
			return;
		}

		for (auto iter = shares.begin(); iter != shares.end();)
		{

			if (send_best_only) //Гасим шару если она больше текущего targetDeadline, актуально для режима Proxy
			{
				if ((iter->best / baseTarget) > bests[Get_index_acc(iter->account_id)].targetDeadline)
				{
					if (use_debug)
					{
						_strtime(tbuffer);
						wattron(win_main, COLOR_PAIR(4));
						wprintw(win_main, "%s [%20llu]\t%llu > %llu  discarded\n", tbuffer, iter->account_id, iter->best / baseTarget, bests[Get_index_acc(iter->account_id)].targetDeadline, 0);
						wattroff(win_main, COLOR_PAIR(4));
					}
					EnterCriticalSection(&sharesLock);
					iter = shares.erase(iter);
					LeaveCriticalSection(&sharesLock);
					continue;
				}
			}

			ZeroMemory(&hints, sizeof(hints));
			hints.ai_family = AF_INET; //AF_UNSPEC;  // использовать IPv4 или IPv6, нам неважно
			hints.ai_socktype = SOCK_STREAM;
			hints.ai_protocol = IPPROTO_TCP;
			hints.ai_flags = AI_PASSIVE;
			char nport[6];
			_itoa((int)nodeport, nport, 10);
			iResult = getaddrinfo(nodeaddr, nport, &hints, &result);
			if (iResult != 0) {
				if (network_quality > 0) network_quality--;
				wattron(win_main, COLOR_PAIR(12));
				wprintw(win_main, "SENDER: getaddrinfo failed with error: %d\n", iResult, 0);
				wattroff(win_main, COLOR_PAIR(12));
				continue;
			}
			ConnectSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
			if (ConnectSocket == INVALID_SOCKET) {
				if (network_quality > 0) network_quality--;
				wattron(win_main, COLOR_PAIR(12));
				wprintw(win_main, "SENDER: socket failed with error: %ld\n", WSAGetLastError(), 0);
				wattroff(win_main, COLOR_PAIR(12));
				freeaddrinfo(result);
				continue;
			}
			const unsigned t = 1000;
			setsockopt(ConnectSocket, SOL_SOCKET, SO_RCVTIMEO, (char *)&t, sizeof(unsigned));
			iResult = connect(ConnectSocket, result->ai_addr, (int)result->ai_addrlen);
			if (iResult == SOCKET_ERROR)
			{
				if (network_quality > 0) network_quality--;
				Log("\nSender:! Error Sender's connect: "); Log_u(WSAGetLastError());
				wattron(win_main, COLOR_PAIR(12));
				_strtime(tbuffer);
				wprintw(win_main, "%s SENDER: can't connect. Error: %ld\n", tbuffer, WSAGetLastError(), 0);
				wattroff(win_main, COLOR_PAIR(12));
				freeaddrinfo(result);
				continue;
			}
			else
			{
				freeaddrinfo(result);

				int bytes = 0;
				memset(buffer, 0, _msize(buffer));
				if (miner_mode == 0)
				{
					bytes = sprintf_s(buffer, _msize(buffer), "POST /burst?requestType=submitNonce&secretPhrase=%s&nonce=%llu HTTP/1.0\r\nConnection: close\r\n\r\n", pass, iter->nonce);
				}
				if (miner_mode == 1)
				{
					unsigned long long total = total_size / 1024 / 1024 / 1024;
					for (auto It = satellite_size.begin(); It != satellite_size.end(); ++It) total = total + It->second;
					bytes = sprintf_s(buffer, _msize(buffer), "POST /burst?requestType=submitNonce&accountId=%llu&nonce=%llu&deadline=%llu HTTP/1.0\r\nX-Miner: Blago %s\r\nX-Capacity: %llu\r\nConnection: close\r\n\r\n", iter->account_id, iter->nonce, iter->best, version, total);
				}
				if (miner_mode == 2)
				{
					char *f1 = (char*)calloc(MAX_PATH, sizeof(char));
					char *str_len = (char*)calloc(MAX_PATH, sizeof(char));
					if ((f1 == nullptr) || (str_len == nullptr))
					{
						ShowMemError();
						exit(-1);
					}

					int len = sprintf_s(f1, _msize(f1), "%llu:%llu:%llu\n", iter->account_id, iter->nonce, height);
					_itoa_s(len, str_len, 10, 10);

					bytes = sprintf_s(buffer, _msize(buffer), "POST /pool/submitWork HTTP/1.0\r\nHost: %s:%llu\r\nContent-Type: text/plain;charset=UTF-8\r\nContent-Length: %i\r\n\r\n%s", nodeaddr, nodeport, len, f1);
					free(f1);
					free(str_len);
				}

				// Sending to server
				
				iResult = send(ConnectSocket, buffer, bytes, 0);
				if (iResult == SOCKET_ERROR)
				{
					if (network_quality > 0) network_quality--;
					Log("\nSender: ! Error deadline's sending: "); Log_u(WSAGetLastError());
					wattron(win_main, COLOR_PAIR(12));
					wprintw(win_main, "SENDER: send failed: %ld\n", WSAGetLastError(), 0);
					wattroff(win_main, COLOR_PAIR(12));
					continue;
				}
				else
				{
					unsigned long long dl = iter->best / baseTarget;
					_strtime(tbuffer);
					if (network_quality < 100) network_quality++;
					wattron(win_main, COLOR_PAIR(9));
					wprintw(win_main, "%s [%20llu] sent DL: %15llu %5llud %02llu:%02llu:%02llu\n", tbuffer, iter->account_id, dl, (dl) / (24 * 60 * 60), (dl % (24 * 60 * 60)) / (60 * 60), (dl % (60 * 60)) / 60, dl % 60, 0);
					wattroff(win_main, COLOR_PAIR(9));

					if (show_msg) wprintw(win_main, "send: %s\n", buffer, 0); // показываем послание
					Log("\nSender:   Sent to server: "); Log_server(buffer);

					EnterCriticalSection(&sessionsLock);
					sessions.push_back({ConnectSocket, iter->account_id, dl});
					LeaveCriticalSection(&sessionsLock);

					if (send_best_only) bests[Get_index_acc(iter->account_id)].targetDeadline = dl;
					EnterCriticalSection(&sharesLock);
					iter = shares.erase(iter);
					LeaveCriticalSection(&sharesLock);
				}
			}
		}
		
		if (!sessions.empty())
		{
			EnterCriticalSection(&sessionsLock);
			for (auto iter = sessions.begin(); iter != sessions.end() && !stopThreads;)
			{
				ConnectSocket = iter->Socket;

				BOOL l = TRUE;
				iResult = ioctlsocket(ConnectSocket, FIONBIO, (unsigned long*)&l);
				if (iResult == SOCKET_ERROR)
				{
					if (network_quality > 0) network_quality--;
					Log("\nSender: ! Error ioctlsocket's: "); Log_u(WSAGetLastError());
					wattron(win_main, COLOR_PAIR(12));
					wprintw(win_main, "SENDER: ioctlsocket failed: %ld\n", WSAGetLastError(), 0);
					wattroff(win_main, COLOR_PAIR(12));
					continue;
				}
				memset(buffer, 0, _msize(buffer));
				int  pos = 0;
				iResult = 0;
				do{
					iResult = recv(ConnectSocket, &buffer[pos], 1000 - pos - 1, 0);
					if (iResult > 0) pos += iResult;
				} while ((iResult > 0) && !use_fast_rcv);
				if (iResult == SOCKET_ERROR)
				{
					if (WSAGetLastError() != WSAEWOULDBLOCK)
					{
						if (network_quality > 0) network_quality--;
						Log("\nSender: ! Error getting deadline's confirmation: "); Log_u(WSAGetLastError());
						wattron(win_main, COLOR_PAIR(12));
						wprintw(win_main, "SENDER: receiving confirmation failed: %ld\n", WSAGetLastError(), 0);
						wattroff(win_main, COLOR_PAIR(12));
					}
				}
				else
				{
					if (show_msg) wprintw(win_main, "\nReceived: %s\n", buffer, 0);
					Log("\nSender:   Received from server: "); Log_server(buffer);
					if (network_quality < 100) network_quality++;

					char *find = strstr(buffer, "\r\n\r\n");
					if (find != nullptr)
					{
						char *rdeadline = strstr(find + 4, "\"deadline\"");
						if (rdeadline != nullptr)
						{
							rdeadline = strpbrk(rdeadline, "0123456789");
							char *enddeadline = strpbrk(rdeadline, ",}\"");

							char* rtargetDeadline = strstr(buffer, "\"targetDeadline\":");
							char* raccountId = strstr(buffer, "\"accountId\":");
							if (enddeadline != nullptr)
							{
								enddeadline[0] = 0;
								unsigned long long ndeadline = _strtoui64(rdeadline, 0, 10);
								unsigned long long naccountId = 0;
								unsigned long long ntargetDeadline = 0;
								if ((rtargetDeadline != nullptr) && (raccountId != nullptr))
								{
									rtargetDeadline = strpbrk(rtargetDeadline, "0123456789");
									char* endBaseTarget = strpbrk(rtargetDeadline, ",}\"");
									endBaseTarget[0] = 0;

									raccountId = strpbrk(raccountId, "0123456789");
									char* endaccountId = strpbrk(raccountId, ",}\"");
									endaccountId[0] = 0;
									naccountId = _strtoui64(raccountId, 0, 10);
									ntargetDeadline = _strtoui64(rtargetDeadline, 0, 10);
									bests[Get_index_acc(naccountId)].targetDeadline = ntargetDeadline;
								}
								Log("\nSender: confirmed deadline: "); Log_llu(ndeadline);
								//unsigned long long years = (ndeadline % (24*60*60))/(60*60)
								//unsigned month = (ndeadline % (365*24*60*60))/(24*60*60);
								unsigned long long days = (ndeadline) / (24 * 60 * 60);
								unsigned hours = (ndeadline % (24 * 60 * 60)) / (60 * 60);
								unsigned min = (ndeadline % (60 * 60)) / 60;
								unsigned sec = ndeadline % 60;
								_strtime(tbuffer);
								wattron(win_main, COLOR_PAIR(10));
								if ((naccountId != 0) && (ntargetDeadline != 0))
								{
									wprintw(win_main, "%s [%20llu] confirmed DL: %10llu %5llud %02u:%02u:%02u\n", tbuffer, naccountId, ndeadline, days, hours, min, sec, 0);
									if (use_debug) wprintw(win_main, "%s [%20llu] set targetDL: %10llu\n", tbuffer, naccountId, ntargetDeadline, 0);
								}
								else wprintw(win_main, "%s [%20llu] confirmed DL: %10llu %5llud %02u:%02u:%02u\n", tbuffer, iter->ID, ndeadline, days, hours, min, sec, 0);
								wattroff(win_main, COLOR_PAIR(10));
								if (ndeadline < deadline || deadline == 0)  deadline = ndeadline;

								if (ndeadline != iter->deadline)
								{
									wattron(win_main, COLOR_PAIR(6));
									wprintw(win_main, "----Fast block or corrupted file?----\nSent deadline:\t%llu\nServer's deadline:\t%llu \n----\n", iter->deadline, ndeadline, 0); //shares[i].file_name.c_str());
									wattroff(win_main, COLOR_PAIR(6));
								}
							}
						}
						else
						if (strstr(find + 4, "Received share") != nullptr)
						{
							_strtime(tbuffer);
							deadline = bests[Get_index_acc(iter->ID)].DL;
							wattron(win_main, COLOR_PAIR(10));
							wprintw(win_main, "%s [%20llu] confirmed DL   %9llu\n", tbuffer, iter->ID, iter->deadline, 0);
							wattroff(win_main, COLOR_PAIR(10));
						}
						else
						{
							wattron(win_main, COLOR_PAIR(15));
							wprintw(win_main, "%s\n", find + 4, 0);
							wattroff(win_main, COLOR_PAIR(15));
						}
						iResult = closesocket(ConnectSocket);
						Log("\nSender: Close socket. Code = "); Log_u(WSAGetLastError());
						iter = sessions.erase(iter);
					}
					else
					{
						wattron(win_main, COLOR_PAIR(6));
						wprintw(win_main, "%s [%20llu] NOT confirmed DL\n", tbuffer, iter->ID, 0);
						wattroff(win_main, COLOR_PAIR(6));
						Log("\nSender: wrong message");
						iResult = closesocket(ConnectSocket);
						Log("\nSender: Close socket. Code = "); Log_u(WSAGetLastError());
						iter = sessions.erase(iter);
					}
				}
				if (iter != sessions.end()) ++iter;
			}
			LeaveCriticalSection(&sessionsLock);
		}
		std::this_thread::yield();
		std::this_thread::sleep_for(std::chrono::milliseconds(send_interval));
	}
	free(buffer);
	//free(tmp_buffer);
	return;
}

void procscoop4(unsigned long long nonce, unsigned long long n, char *data, size_t acc, const std::string &file_name) {
	char *cache;
	char sig0[32 + 64];
	char sig1[32 + 64];
	char sig2[32 + 64];
	char sig3[32 + 64];
	cache = data;
	char tbuffer[9];
	unsigned long long v;

	memcpy(sig0, signature, 32);
	memcpy(sig1, signature, 32);
	memcpy(sig2, signature, 32);
	memcpy(sig3, signature, 32);

	char res0[32];
	char res1[32];
	char res2[32];
	char res3[32];
	unsigned posn;
	mshabal_context x;

	for (v = 0; v < n; v += 4)
	{
		memcpy(&sig0[32], &cache[(v + 0) * 64], 64);
		memcpy(&sig1[32], &cache[(v + 1) * 64], 64);
		memcpy(&sig2[32], &cache[(v + 2) * 64], 64);
		memcpy(&sig3[32], &cache[(v + 3) * 64], 64);

		
		avx1_mshabal_init(&x, 256);
		avx1_mshabal(&x, (const unsigned char*)sig0, (const unsigned char*)sig1, (const unsigned char*)sig2, (const unsigned char*)sig3, 64 + 32);
		avx1_mshabal_close(&x, 0, 0, 0, 0, 0, res0, res1, res2, res3);

		unsigned long long *wertung = (unsigned long long*)res0;
		unsigned long long *wertung1 = (unsigned long long*)res1;
		unsigned long long *wertung2 = (unsigned long long*)res2;
		unsigned long long *wertung3 = (unsigned long long*)res3;
		posn = 0;
		if (*wertung1 < *wertung)
		{
			*wertung = *wertung1;
			posn = 1;
		}
		else if (*wertung2 < *wertung)
		{
			*wertung = *wertung2;
			posn = 2;
		}
		else if (*wertung3 < *wertung)
		{
			*wertung = *wertung3;
			posn = 3;
		}


		if ((*wertung / baseTarget) <= bests[acc].targetDeadline)
		{
			if (send_best_only)
			{
				if (bests[acc].nonce == 0 || *wertung < bests[acc].best)
				{
					Log("\nfound deadline=");	Log_llu(*wertung / baseTarget); Log(" nonce=");	Log_llu(nonce + v + posn); Log(" for account: "); Log_llu(bests[acc].account_id); Log(" file: "); Log((char*)file_name.c_str());
					EnterCriticalSection(&bestsLock);
					bests[acc].best = *wertung;
					bests[acc].nonce = nonce + v + posn;
					bests[acc].DL = *wertung / baseTarget;
					LeaveCriticalSection(&bestsLock);
					EnterCriticalSection(&sharesLock);
					shares.push_back({ file_name, bests[acc].account_id, bests[acc].best, bests[acc].nonce });
					LeaveCriticalSection(&sharesLock);
					if (use_debug)
					{
						char tbuffer[9];
						_strtime(tbuffer);
						wattron(win_main, COLOR_PAIR(2));
						wprintw(win_main, "%s [%20llu] found DL:      %9llu\n", tbuffer, bests[acc].account_id, bests[acc].DL, 0);
						wattroff(win_main, COLOR_PAIR(2));
					}
				}
			}
			else
			{
				if (bests[acc].nonce == 0 || *wertung < bests[acc].best)
				{
					Log("\nfound deadline=");	Log_llu(*wertung / baseTarget); Log(" nonce=");	Log_llu(nonce + v); Log(" for account: "); Log_llu(bests[acc].account_id); Log(" file: "); Log((char*)file_name.c_str());
					EnterCriticalSection(&bestsLock);
					bests[acc].best = *wertung;
					bests[acc].nonce = nonce + v + posn;
					bests[acc].DL = *wertung / baseTarget;
					LeaveCriticalSection(&bestsLock);
				}
				EnterCriticalSection(&sharesLock);
				shares.push_back({ file_name, bests[acc].account_id, *wertung, nonce + v + posn });
				LeaveCriticalSection(&sharesLock);
				if (use_debug)
				{
					_strtime(tbuffer);
					wattron(win_main, COLOR_PAIR(2));
					wprintw(win_main, "%s [%20llu] found DL:      %9llu\n", tbuffer, bests[acc].account_id, *wertung / baseTarget, 0);
					wattroff(win_main, COLOR_PAIR(2));
				}
			}
		}

	}
}
//#pragma optimize("", on)


/*

unsigned long long procscoop8(unsigned long long nonce, unsigned long long n, char *data, int acc, const std::string &file_name) {
	char *cache;
	char sig0[32 + 64];
	char sig1[32 + 64];
	char sig2[32 + 64];
	char sig3[32 + 64];
	char sig4[32 + 64];
	char sig5[32 + 64];
	char sig6[32 + 64];
	char sig7[32 + 64];
	cache = data;
	unsigned long long v;
	char tbuffer[9];

	memmove(sig0, signature, 32);
	memmove(sig1, signature, 32);
	memmove(sig2, signature, 32);
	memmove(sig3, signature, 32);
	memmove(sig4, signature, 32);
	memmove(sig5, signature, 32);
	memmove(sig6, signature, 32);
	memmove(sig7, signature, 32);

	for (v = 0; v<n; v += 8) {
		memmove(&sig0[32], &cache[(v + 0) * 64], 64);
		memmove(&sig1[32], &cache[(v + 1) * 64], 64);
		memmove(&sig2[32], &cache[(v + 2) * 64], 64);
		memmove(&sig3[32], &cache[(v + 3) * 64], 64);
		memmove(&sig4[32], &cache[(v + 4) * 64], 64);
		memmove(&sig5[32], &cache[(v + 5) * 64], 64);
		memmove(&sig6[32], &cache[(v + 6) * 64], 64);
		memmove(&sig7[32], &cache[(v + 7) * 64], 64);
		char res0[32];
		char res1[32];
		char res2[32];
		char res3[32];
		char res4[32];
		char res5[32];
		char res6[32];
		char res7[32];

		mshabal256_context x;
		mshabal256_init(&x, 256);
		mshabal256(&x, (const unsigned char*)sig0, (const unsigned char*)sig1, (const unsigned char*)sig2, (const unsigned char*)sig3, (const unsigned char*)sig4, (const unsigned char*)sig5, (const unsigned char*)sig6, (const unsigned char*)sig7, 64 + 32);
		mshabal256_close(&x, 0, 0, 0, 0, 0, 0, 0, 0, 0, res0, res1, res2, res3, res4, res5, res6, res7);

		unsigned long long *wertung  = (unsigned long long*)res0;
		unsigned long long *wertung1 = (unsigned long long*)res1;
		unsigned long long *wertung2 = (unsigned long long*)res2;
		unsigned long long *wertung3 = (unsigned long long*)res3;
		unsigned long long *wertung4 = (unsigned long long*)res4;
		unsigned long long *wertung5 = (unsigned long long*)res5;
		unsigned long long *wertung6 = (unsigned long long*)res6;
		unsigned long long *wertung7 = (unsigned long long*)res7;
		unsigned posn = 0;
		if (*wertung1 < *wertung)
		{
			*wertung = *wertung1;
			posn = 1;
		}
		if (*wertung2 < *wertung)
		{
			*wertung = *wertung2;
			posn = 2;
		}
		if (*wertung3 < *wertung)
		{
			*wertung = *wertung3;
			posn = 3;
		}
		if (*wertung4 < *wertung)
		{
			*wertung = *wertung4;
			posn = 4;
		}
		if (*wertung5 < *wertung)
		{
			*wertung = *wertung5;
			posn = 5;
		}
		if (*wertung6 < *wertung)
		{
			*wertung = *wertung6;
			posn = 6;
		}
		if (*wertung7 < *wertung)
		{
			*wertung = *wertung7;
			posn = 7;
		}

		Log("\nЗашли");
		
		if (bests[acc].nonce == 0 || *wertung < bests[acc].best)
		{
			
			bests[acc].best = *wertung;
			bests[acc].nonce = nonce + v + posn;
			if ((bests[acc].best / baseTarget) <= bests[acc].targetDeadline) // Has to be this good before we inform the node
			{
				Log("\nfound deadline=");	Log_llu(bests[acc].best / baseTarget); Log(" nonce=");	Log_llu(bests[acc].nonce); Log(" for account: "); Log_llu(bests[acc].account_id);
				//printf("\rbestn_local: %llu  bestn_local: %llu  n:%llu\n", best_local, bestn_local, n);
				pthread_mutex_lock(&byteLock);
				shares[num_shares].best = bests[acc].best;
				shares[num_shares].nonce = bests[acc].nonce;
				//shares[num_shares].to_send = 1;
				shares[num_shares].account_id = bests[acc].account_id;
				shares[num_shares].file_name = file_name;
				
				if (use_debug)
				{
					cls();
					_strtime(tbuffer);
					SetConsoleTextAttribute(hConsole, 2);
					printf("\r%s [%20llu]\tfound deadline %llu\n", tbuffer, bests[acc].account_id, shares[num_shares].best / baseTarget);
					SetConsoleTextAttribute(hConsole, 7);
				}
				num_shares++;
				pthread_mutex_unlock(&byteLock);
			}
		}
		//nonce++;
		//cache += 64*8;
	}

	return v;
}

*/

void procscoop(const unsigned long long nonce, const unsigned long long n, char *data, const size_t acc, const std::string &file_name) {
	char *cache;
	char sig[32 + 64];
	cache = data;
	char res[32];
	memcpy_s(sig, sizeof(sig), signature, sizeof(char) * 32);
	sph_shabal_context x;
	for (unsigned long long v = 0; v < n; v++)
	{
		memcpy_s(&sig[32], sizeof(sig)-32, &cache[v * 64], sizeof(char)* 64);
		
		sph_shabal256_init(&x);
		sph_shabal256(&x, (const unsigned char*)sig, 64 + 32);
		sph_shabal256_close(&x, res);

		unsigned long long *wertung = (unsigned long long*)res;

		if ((*wertung / baseTarget) <= bests[acc].targetDeadline)
		{
			if (send_best_only)
			{
				if (bests[acc].nonce == 0 || *wertung < bests[acc].best)
				{
					Log("\nfound deadline=");	Log_llu(*wertung / baseTarget); Log(" nonce=");	Log_llu(nonce + v); Log(" for account: "); Log_llu(bests[acc].account_id); Log(" file: "); Log((char*)file_name.c_str());
					EnterCriticalSection(&bestsLock);
					bests[acc].best = *wertung;
					bests[acc].nonce = nonce + v;
					bests[acc].DL = *wertung / baseTarget;
					LeaveCriticalSection(&bestsLock);
					EnterCriticalSection(&sharesLock);
					shares.push_back({ file_name, bests[acc].account_id, bests[acc].best, bests[acc].nonce });
					LeaveCriticalSection(&sharesLock);
					if (use_debug)
					{
						char tbuffer[9];
						_strtime(tbuffer);
						wattron(win_main, COLOR_PAIR(2));
						wprintw(win_main, "%s [%20llu] found DL:      %9llu\n", tbuffer, bests[acc].account_id, bests[acc].DL, 0);
						wattroff(win_main, COLOR_PAIR(2));
					}
				}
			}
			else
			{
				if (bests[acc].nonce == 0 || *wertung < bests[acc].best)
				{
					Log("\nfound deadline=");	Log_llu(*wertung / baseTarget); Log(" nonce=");	Log_llu(nonce + v); Log(" for account: "); Log_llu(bests[acc].account_id); Log(" file: "); Log((char*)file_name.c_str());
					EnterCriticalSection(&bestsLock);
					bests[acc].best = *wertung;
					bests[acc].nonce = nonce + v;
					bests[acc].DL = *wertung / baseTarget;
					LeaveCriticalSection(&bestsLock);
				}
				EnterCriticalSection(&sharesLock);
				shares.push_back({ file_name, bests[acc].account_id, *wertung, nonce + v });
				LeaveCriticalSection(&sharesLock);
				if (use_debug)
				{
					char tbuffer[9];
					_strtime(tbuffer);
					wattron(win_main, COLOR_PAIR(2));
					wprintw(win_main, "%s [%20llu] found DL:      %9llu\n", tbuffer, bests[acc].account_id, *wertung / baseTarget, 0);
					wattroff(win_main, COLOR_PAIR(2));
				}
			}
		}
	}
}


void work_i(const size_t local_num) {
	
	__int64 start_work_time, end_work_time;
	__int64 start_time_read, end_time_read;
	__int64 start_time_proc;
	double sum_time_proc = 0;
	LARGE_INTEGER li;
	QueryPerformanceFrequency(&li);
	double pcFreq = double(li.QuadPart);
	QueryPerformanceCounter((LARGE_INTEGER*)&start_work_time);
		
	if (use_boost)
	{
		//SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
		//SetThreadAffinityMask(GetCurrentThread(), 1 << (int)(working_threads));
		SetThreadIdealProcessor(GetCurrentThread(), (DWORD)(local_num % std::thread::hardware_concurrency()) );
	}
	
	std::string path_loc_str = paths_dir[local_num];
	unsigned long long files_size_per_thread = 0;
		
	Log("\nStart thread: ["); Log_llu(local_num); Log("]  ");	Log((char*)path_loc_str.c_str());

	std::vector<t_files> files;
	GetFiles(path_loc_str, &files);
	
	size_t cache_size_local;
	DWORD sectorsPerCluster;
	DWORD bytesPerSector;
	DWORD numberOfFreeClusters;
	DWORD totalNumberOfClusters;

	for (auto iter = files.begin(); iter != files.end(); ++iter)
	{
		unsigned long long key, nonce, nonces, stagger;
		QueryPerformanceCounter((LARGE_INTEGER*)&start_time_read);
		sscanf_s(iter->Name.c_str(), "%llu_%llu_%llu_%llu", &key, &nonce, &nonces, &stagger);
		//if ((double)(nonces % stagger) > DBL_EPSILON)
		if ((double)(nonces % stagger) != 0)  
		{
			wattron(win_main, COLOR_PAIR(12));
			wprintw(win_main, "File %s wrong stagger?\n", iter->Name.c_str(), 0);
			wattroff(win_main, COLOR_PAIR(12));
		}

		if (nonces != (iter->Size) / (4096 * 64)) // Проверка на повреждения плота
		{
			wattron(win_main, COLOR_PAIR(12));
			wprintw(win_main, "file \"%s\" name/size mismatch\n", iter->Name.c_str(), 0);
			wattroff(win_main, COLOR_PAIR(12));
			if (nonces != stagger)
				nonces = (((iter->Size) / (4096 * 64)) / stagger) * stagger; //обрезаем плот по размеру и стаггеру
			else
			if (scoop > (iter->Size) / (stagger * 64)) //если номер скупа попадает в поврежденный смерженный плот, то все нормально
			{
				wattron(win_main, COLOR_PAIR(12));
				wprintw(win_main, "skipped\n", 0);
				wattroff(win_main, COLOR_PAIR(12));
				continue;
			}
		}

		if (!GetDiskFreeSpace(str2wstr(iter->Path).c_str(), &sectorsPerCluster, &bytesPerSector, &numberOfFreeClusters, &totalNumberOfClusters))
		{
			wattron(win_main, COLOR_PAIR(12));
			wprintw(win_main, "GetDiskFreeSpace failed\n", 0);
			wattroff(win_main, COLOR_PAIR(12));
			continue;
		}

		// Если стаггер в плоте меньше чем размер сектора - пропускаем
		if ((stagger * 64) < bytesPerSector)
		{
			wattron(win_main, COLOR_PAIR(12));
			wprintw(win_main, "stagger (%llu) must be >= %llu\n", stagger, bytesPerSector/64, 0);
			wattroff(win_main, COLOR_PAIR(12));
			continue;
		}

		// Если нонсов в плоте меньше чем размер сектора - пропускаем
		if ((nonces * 64) < bytesPerSector)
		{
			wattron(win_main, COLOR_PAIR(12));
			wprintw(win_main, "nonces (%llu) must be >= %llu\n", nonces, bytesPerSector/64, 0);
			wattroff(win_main, COLOR_PAIR(12));
			continue;
		}

		if ((stagger == nonces) && (cache_size < stagger)) cache_size_local = cache_size;  // смерженный плот
		else cache_size_local = stagger; // обычный плот

		// Выравниваем cache_size_local по размеру сектора
		//cache_size_local = round((double)cache_size_local / (double)(bytesPerSector / 64)) * (bytesPerSector / 64);
		cache_size_local = (cache_size_local / (size_t)(bytesPerSector / 64)) * (size_t)(bytesPerSector / 64);
		//wprintw(win_main, "round: %llu\n", cache_size_local);

		// Если стаггер не выровнен по сектору - можем читать сдвигая посследний стагер назад (доделать)
		if ((stagger % (bytesPerSector/64)) != 0)
		{
			wattron(win_main, COLOR_PAIR(12));
			wprintw(win_main, "stagger (%llu) must be a multiple of %llu\n", stagger, bytesPerSector / 64, 0);
			stagger = (stagger / (bytesPerSector / 64)) * (bytesPerSector / 64);
			nonces = (nonces/stagger) * stagger;
			//Нужно добавить остаток от предидущего значения стаггера для компенсации сдвига по нонсам
			wprintw(win_main, "stagger setup to %llu\n", stagger, 0);
			wprintw(win_main, "nonces setup to %llu\n", nonces, 0);
			wattroff(win_main, COLOR_PAIR(12));
			//continue;
		}

		char *cache = (char *)VirtualAlloc(nullptr, cache_size_local * 64, MEM_COMMIT, PAGE_READWRITE);
		if (cache == nullptr) {
			ShowMemError();
			exit(-1);
		}

		Log("\nRead file : ");	Log((char*)iter->Name.c_str());
		
		//wprintw(win_main, "%S \n", str2wstr(iter->Path + iter->Name).c_str());
		HANDLE ifile = CreateFile(str2wstr(iter->Path + iter->Name).c_str(), GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_FLAG_NO_BUFFERING, nullptr);
		if (ifile == INVALID_HANDLE_VALUE)
		{
			wattron(win_main, COLOR_PAIR(12));
			wprintw(win_main, "File \"%s\" error opening\n", iter->Name.c_str(), 0);
			wattroff(win_main, COLOR_PAIR(12));
			VirtualFree(cache, 0, MEM_RELEASE);
			continue;
		}
		files_size_per_thread += iter->Size;
		
		unsigned long long start, bytes;
		DWORD b = 0;
		LARGE_INTEGER liDistanceToMove;

		size_t acc = Get_index_acc(key);
		for (unsigned long long n = 0; n < nonces; n += stagger)
		{
			start = n * 4096 * 64 + scoop * stagger * 64;
			for (unsigned long long i = 0; i < stagger; i += cache_size_local)
			{
				if (i + cache_size_local > stagger)
				{
					cache_size_local = stagger - i;
					//wprintw(win_main, "%llu\n", cache_size_local);
				}
				bytes = 0;
				b = 0;
				liDistanceToMove.QuadPart = start + i*64;
				if (!SetFilePointerEx(ifile, liDistanceToMove, nullptr, FILE_BEGIN)) wprintw(win_main, "error SetFilePointerEx\n", 0);


				do {
					if (!ReadFile(ifile, &cache[bytes], (DWORD)(cache_size_local * 64), &b, 0)) wprintw(win_main, "error ReadFile\n", 0);
					bytes += b;
					//wprintw(win_main, "%llu   %llu\n", bytes, readsize);
				} while (bytes < cache_size_local * 64);

				if (bytes == cache_size_local * 64)
				{
					QueryPerformanceCounter((LARGE_INTEGER*)&start_time_proc);
					#ifdef __AVX__
						procscoop4(n + nonce + i, cache_size_local, cache, acc, iter->Name);// Process block
					#else
						procscoop(n + nonce + i, cache_size_local, cache, acc, iter->Name);// Process block
					#endif
					QueryPerformanceCounter(&li);
					sum_time_proc += (double)(li.QuadPart - start_time_proc);
					worker_progress[local_num] += bytes;
				}
				else
				{
					wattron(win_main, COLOR_PAIR(12));
					wprintw(win_main, "Unexpected end of file %s\n", iter->Name.c_str(), 0);
					wattroff(win_main, COLOR_PAIR(12));
				}

				if (stopThreads) // New block while processing: Stop.
				{
					Log("\nReading file: ");	Log((char*)iter->Name.c_str()); Log(" interrupted");
					CloseHandle(ifile);
					files.clear();
					VirtualFree(cache, 0, MEM_RELEASE);
					//if (use_boost) SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);
					return;
				}
			}
		}
		QueryPerformanceCounter((LARGE_INTEGER*)&end_time_read);
		Log("\nClose file: ");	Log((char*)iter->Name.c_str()); Log(" [@ "); Log_llu((long long unsigned)((double)(end_time_read - start_time_read) * 1000 / pcFreq)); Log(" ms]");
		CloseHandle(ifile);
		VirtualFree(cache, 0, MEM_RELEASE);
	}
	QueryPerformanceCounter((LARGE_INTEGER*)&end_work_time);
	
	//if (use_boost) SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);

	double thread_time = (double)(end_work_time - start_work_time) / pcFreq;
	if (use_debug)
	{
		char tbuffer[9];
		_strtime(tbuffer);
		wattron(win_main, COLOR_PAIR(7));
		wprintw(win_main, "%s Thread \"%s\" @ %.1f sec (%.1f MB/s) CPU %.2f%%\n", tbuffer, path_loc_str.c_str(), thread_time, (double)(files_size_per_thread) / thread_time / 1024 / 1024 / 4096, sum_time_proc / pcFreq * 100 / thread_time, 0);
		wattroff(win_main, COLOR_PAIR(7));
	}
	return;
}

void HDD_wakeup_i(void) {
	for (; !exit_flag;)
	{
		std::vector<t_files> tmp_files;
		for (auto i = 0; i < paths_dir.size(); i++)		GetFiles(paths_dir[i], &tmp_files);
		if (use_debug)
		{
			char tbuffer[9];
			_strtime(tbuffer);
			wattron(win_main, COLOR_PAIR(7));
			wprintw(win_main, "%s HDD, WAKE UP !\n", tbuffer, 0);
			wattroff(win_main, COLOR_PAIR(7));
		}
		std::this_thread::yield();
		std::this_thread::sleep_for(std::chrono::milliseconds(360000));
	}
}


void GetJSON(char* req) {
	const unsigned BUF_SIZE = 1024;

	char *buffer = (char*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, BUF_SIZE);
	if (buffer == nullptr) {
		ShowMemError();
		exit(-1);
	}

	char *find = nullptr;
	unsigned long long msg_len = 0;
	int iResult = 0;
	struct addrinfo *result = nullptr;
	struct addrinfo hints;
	SOCKET WalletSocket = INVALID_SOCKET;

	json = nullptr;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET; //AF_UNSPEC;  // использовать IPv4 или IPv6, нам неважно
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;
	char iport[6];
	_itoa((int)infoport, iport, 10);
	iResult = getaddrinfo(infoaddr, iport, &hints, &result);
	if (iResult != 0) {
		wattron(win_main, COLOR_PAIR(12));
		wprintw(win_main, "WINNER: Getaddrinfo failed with error: %d\n", iResult, 0);
		wattroff(win_main, COLOR_PAIR(12));
		Log("\nWinner: getaddrinfo error");
	}
	else
	{
		WalletSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
		if (WalletSocket == INVALID_SOCKET)
		{
			wattron(win_main, COLOR_PAIR(12));
			wprintw(win_main, "WINNER: Socket function failed with error: %ld\n", WSAGetLastError(), 0);
			wattroff(win_main, COLOR_PAIR(12));
			Log("\nWinner: Socket error: "); Log_u(WSAGetLastError());
		}
		else
		{
			unsigned t = 3000;
			setsockopt(WalletSocket, SOL_SOCKET, SO_RCVTIMEO, (char *)&t, sizeof(unsigned));
			//Log("\n-Connecting to server: "); Log(updaterip); Log(":"); Log_u(updaterport);
			iResult = connect(WalletSocket, result->ai_addr, (int)result->ai_addrlen);
			if (iResult == SOCKET_ERROR) {
				wattron(win_main, COLOR_PAIR(12));
				wprintw(win_main, "WINNER: Connect function failed with error: %ld\n", WSAGetLastError(), 0);
				wattroff(win_main, COLOR_PAIR(12));
				Log("\nWinner: Connect server error "); Log_u(WSAGetLastError());
			}
			else
			{
				iResult = send(WalletSocket, req, (int)strlen(req), 0);
				if (iResult == SOCKET_ERROR)
				{
					wattron(win_main, COLOR_PAIR(12));
					wprintw(win_main, "WINNER: Send request failed: %ld\n", WSAGetLastError(), 0);
					wattroff(win_main, COLOR_PAIR(12));
					Log("\nWinner: Error sending request: "); Log_u(WSAGetLastError());
				}
				else
				{
					char *tmp_buffer;
					unsigned long long msg_len = 0;
					int iReceived_size = 0;
					while ((iReceived_size = recv(WalletSocket, buffer + msg_len, BUF_SIZE - 1, 0)) > 0)
					{
						msg_len = msg_len + iReceived_size;
						Log("\nrealloc: ");
						tmp_buffer = (char*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, msg_len + BUF_SIZE);
						if (tmp_buffer == nullptr) {
							ShowMemError();
							exit(-1);
						}
						memcpy(tmp_buffer, buffer, msg_len);
						HeapFree(GetProcessHeap(), 0, buffer);
						buffer = tmp_buffer;
						buffer[msg_len + 1] = 0;
						Log_llu(msg_len);
					}

					if (iReceived_size < 0)
					{
						wattron(win_main, COLOR_PAIR(12));
						wprintw(win_main, "WINNER: Get info failed: %ld\n", WSAGetLastError(), 0);
						wattroff(win_main, COLOR_PAIR(12));
						Log("\nWinner: Error response: "); Log_u(WSAGetLastError());
					}
					else
					{
						find = strstr(buffer, "\r\n\r\n");
						if (find != nullptr)
						{
							json = (char*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, msg_len);
							if (json == nullptr) {
								ShowMemError();
								exit(-1);
							}
							sprintf_s(json, _msize(json), "%s", find + 4 * sizeof(char));
						}
					} // recv() != SOCKET_ERROR
				} //send() != SOCKET_ERROR
			} // Connect() != SOCKET_ERROR
		} // socket() != INVALID_SOCKET
		iResult = closesocket(WalletSocket);
	} // getaddrinfo() == 0
	HeapFree(GetProcessHeap(), 0, buffer);
	freeaddrinfo(result);
}

void GetBlockInfo(unsigned num_block) 
{
	char* generator = nullptr;
	char* generatorRS = nullptr;
	unsigned long long last_block_height = 0;
	char* name = nullptr;
	char* rewardRecipient = nullptr;
	char* pool_accountRS = nullptr;
	char* pool_name = nullptr;
	unsigned long long timestamp0 = 0;
	unsigned long long timestamp1 = 0;
	const unsigned req_size = 255;
	char tbuffer[9];
	// Запрос двух последних блоков из блокчейна

	char* str_req = (char*)calloc(req_size, sizeof(char));
	if (str_req == nullptr) {
		ShowMemError();
		exit(-1);
	}
	sprintf_s(str_req, _msize(str_req), "POST /burst?requestType=getBlocks&firstIndex=%u&lastIndex=%u HTTP/1.0\r\nConnection: close\r\n\r\n", num_block, num_block + 1);
	GetJSON(str_req);
	//Log("\n getBlocks: ");

	if (json == nullptr)	Log("\n-! error in message from pool (getBlocks)\n");
	else
	{
		rapidjson::Document doc_block;
		if (doc_block.Parse<0>(json).HasParseError() == false)
		{
			const Value& blocks = doc_block["blocks"];
			if (blocks.IsArray())
			{
				const Value& bl_0 = blocks[SizeType(0)];
				const Value& bl_1 = blocks[SizeType(1)];
				generatorRS = (char*)calloc(strlen(bl_0["generatorRS"].GetString()) + 1, sizeof(char));
				if (generatorRS == nullptr) {
					ShowMemError();
					exit(-1);
				}
				strcpy(generatorRS, bl_0["generatorRS"].GetString());
				generator = (char*)calloc(strlen(bl_0["generator"].GetString()) + 1, sizeof(char));
				if (generator == nullptr) {
					ShowMemError();
					exit(-1);
				}
				strcpy(generator, bl_0["generator"].GetString());
				last_block_height = bl_0["height"].GetUint();
				timestamp0 = bl_0["timestamp"].GetUint64();
				timestamp1 = bl_1["timestamp"].GetUint64();
			}
		}
		else Log("\n- error parsing JSON getBlocks");
	}
	free(str_req);
	if (json != nullptr) HeapFree(GetProcessHeap(), 0, json);
	if ((generator != nullptr) && (generatorRS != nullptr) && (timestamp0 != 0) && (timestamp1 != 0))
	if (last_block_height == height - 1)
	{
		// Запрос данных аккаунта
		str_req = (char*)calloc(req_size, sizeof(char));
		if (str_req == nullptr) {
			ShowMemError();
			exit(-1);
		}
		sprintf_s(str_req, _msize(str_req), "POST /burst?requestType=getAccount&account=%s HTTP/1.0\r\nConnection: close\r\n\r\n", generator);
		GetJSON(str_req);
		//Log("\n getAccount: ");

		if (json == nullptr)	Log("\n- error in message from pool (getAccount)\n");
		else
		{
			rapidjson::Document doc_acc;
			if (doc_acc.Parse<0>(json).HasParseError() == false)
			{
				if (doc_acc.HasMember("name"))
				{
					name = (char*)calloc(strlen(doc_acc["name"].GetString()) + 1, sizeof(char));
					if (name == nullptr) {
						ShowMemError();
						exit(-1);
					}
					strcpy(name, doc_acc["name"].GetString());
				}
			}
			else Log("\n- error parsing JSON getAccount");
		}
		free(str_req);
		if (json != nullptr) HeapFree(GetProcessHeap(), 0, json);

		// Запрос RewardAssighnment по данному аккаунту
		str_req = (char*)calloc(req_size, sizeof(char));
		if (str_req == nullptr) {
			ShowMemError();
			exit(-1);
		}
		sprintf_s(str_req, _msize(str_req), "POST /burst?requestType=getRewardRecipient&account=%s HTTP/1.0\r\nConnection: close\r\n\r\n", generator);
		GetJSON(str_req);
		free(str_req);
		//Log("\n getRewardRecipient: ");
		
		if (json == nullptr)	Log("\n- error in message from pool (getRewardRecipient)\n");
		else
		{
			rapidjson::Document doc_reward;
			if (doc_reward.Parse<0>(json).HasParseError() == false)
			{
				if (doc_reward.HasMember("rewardRecipient"))
				{
					rewardRecipient = (char*)calloc(strlen(doc_reward["rewardRecipient"].GetString()) + 1, sizeof(char));
					if (rewardRecipient == nullptr) {
						ShowMemError();
						exit(-1);
					}
					strcpy(rewardRecipient, doc_reward["rewardRecipient"].GetString());
				}
			}
			else Log("\n-! error parsing JSON getRewardRecipient");
		}
		
		if (json != nullptr) HeapFree(GetProcessHeap(), 0, json);
		
		if (rewardRecipient != nullptr)
		{
			// Если rewardRecipient != generator, то майнит на пул, узнаём имя пула...
			if (strcmp(generator, rewardRecipient) != 0)
			{
				// Запрос данных аккаунта пула
				str_req = (char*)calloc(req_size, sizeof(char));
				if (str_req == nullptr) {
					ShowMemError();
					exit(-1);
				}
				sprintf_s(str_req, _msize(str_req), "POST /burst?requestType=getAccount&account=%s HTTP/1.0\r\nConnection: close\r\n\r\n", rewardRecipient);
				GetJSON(str_req);
				//Log("\n getAccount: ");

				if (json == nullptr)
				{
					Log("\n- error in message from pool (pool getAccount)\n");
				}
				else
				{
					rapidjson::Document doc_pool;
					if (doc_pool.Parse<0>(json).HasParseError() == false)
					{
						pool_accountRS = (char*)calloc(strlen(doc_pool["accountRS"].GetString()) + 1, sizeof(char));
						if (pool_accountRS == nullptr) {
							ShowMemError();
							exit(-1);
						}
						strcpy(pool_accountRS, doc_pool["accountRS"].GetString());
						if (doc_pool.HasMember("name"))
						{
							pool_name = (char*)calloc(strlen(doc_pool["name"].GetString()) + 1, sizeof(char));
							if (pool_name == nullptr) {
								ShowMemError();
								exit(-1);
							}
							strcpy(pool_name, doc_pool["name"].GetString());
						}
					}
					else Log("\n- error parsing JSON pool getAccount");
				}
				free(str_req);
				HeapFree(GetProcessHeap(), 0, json);
			}
		}

		wattron(win_main, COLOR_PAIR(11));
		_strtime(tbuffer);
		if (name != nullptr) wprintw(win_main, "%s Winner: %llus by %s (%s)\n", tbuffer, timestamp0 - timestamp1, generatorRS + 6, name, 0);
		else wprintw(win_main, "%s Winner: %llus by %s\n", tbuffer, timestamp0 - timestamp1, generatorRS + 6, 0);
		if (pool_accountRS != nullptr)
		{
			if (pool_name != nullptr) wprintw(win_main, "%s Winner's pool: %s (%s)\n", tbuffer, pool_accountRS + 6, pool_name, 0);
			else wprintw(win_main, "%s Winner's pool: %s\n", tbuffer, pool_accountRS + 6, 0);
		}
		wattroff(win_main, COLOR_PAIR(11));
	}
	else
	{
		_strtime(tbuffer);
		wattron(win_main, COLOR_PAIR(11));
		wprintw(win_main, "%s Winner: no info yet\n", tbuffer, 0);
		wattroff(win_main, COLOR_PAIR(11));
	}
	free(generatorRS);
	free(generator);
	free(name);
	free(rewardRecipient);
	free(pool_name);
	free(pool_accountRS);
}


void pollLocal(void) {
	char *buffer = (char*)calloc(1000, sizeof(char));
	if (buffer == nullptr) {
		ShowMemError();
		exit(-1);
	}

	int iResult;
	struct addrinfo *result = nullptr;
	struct addrinfo hints;
	SOCKET UpdaterSocket = INVALID_SOCKET;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET; //AF_UNSPEC;  // использовать IPv4 или IPv6, нам неважно
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;
	char uport[6];
	_itoa((int)updaterport, uport, 10);
	iResult = getaddrinfo(updateraddr, uport, &hints, &result);
	if (iResult != 0) {
		if (network_quality > 0) network_quality--;
		Log("\n*! GMI: getaddrinfo failed with error: "); Log_u(WSAGetLastError());
	}
	else {
		UpdaterSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
		if (UpdaterSocket == INVALID_SOCKET)
		{
			if (network_quality > 0) network_quality--;
			Log("\n*! GMI: socket function failed with error: "); Log_u(WSAGetLastError());
		}
		else {
			const unsigned t = 1000;
			setsockopt(UpdaterSocket, SOL_SOCKET, SO_RCVTIMEO, (char *)&t, sizeof(unsigned));
			Log("\n*Connecting to server: "); Log(updateraddr); Log(":"); Log_u(updaterport);
			iResult = connect(UpdaterSocket, result->ai_addr, (int)result->ai_addrlen);
			if (iResult == SOCKET_ERROR) {
				if (network_quality > 0) network_quality--;
				Log("\n*! GMI: connect function failed with error: "); Log_u(WSAGetLastError());
			}
			else {
				int bytes;
				if (miner_mode == 2) bytes = sprintf_s(buffer, _msize(buffer), "GET /pool/getMiningInfo HTTP/1.0\r\nHost: %s:%llu\r\nContent-Type: text/plain;charset=UTF-8\r\n\r\n", updateraddr, updaterport);
				else bytes = sprintf_s(buffer, _msize(buffer), "POST /burst?requestType=getMiningInfo HTTP/1.0\r\nConnection: close\r\n\r\n");

				iResult = send(UpdaterSocket, buffer, bytes, 0);
				if (iResult == SOCKET_ERROR)
				{
					if (network_quality > 0) network_quality--;
					Log("\n*! GMI: send request failed: "); Log_u(WSAGetLastError());
				}
				else{
					if (show_updates) wprintw(win_main, "Sent: \n%s\n", buffer, 0);
					Log("\n*Sent to server: "); Log_server(buffer);


					memset(buffer, 0, _msize(buffer));
					int  pos = 0;
					iResult = 0;
					do{
						iResult = recv(UpdaterSocket, &buffer[pos], 1000 - pos - 1, 0);
						if (iResult > 0) pos += iResult;
					} while ((iResult > 0) && !use_fast_rcv);
					if (iResult == SOCKET_ERROR)
					{
						if (network_quality > 0) network_quality--;
						Log("\n*! GMI: get mining info failed:: "); Log_u(WSAGetLastError());
					}
					else {
						if (network_quality < 100) network_quality++;
						Log("\n* GMI: Received from server: "); Log_server(buffer);
						if (show_updates)  wprintw(win_main, "Received: %s\n", buffer, 0);

						// locate HTTP header
						char *find = strstr(buffer, "\r\n\r\n");
						if (find == nullptr)	Log("\n*! GMI: error in message from pool");
						else {
							char *rbaseTarget = strstr(buffer, "\"baseTarget\":");
							char *rheight = strstr(buffer, "\"height\":");
							char *generationSignature = strstr(buffer, "\"generationSignature\":");
							if (generationSignature != nullptr) generationSignature = strstr(generationSignature + strlen("\"generationSignature\":"), "\"") + 1; //+1 убираем начальные ковычки
							char* rtargetDeadline = strstr(buffer, "\"targetDeadline\"");

							if (rbaseTarget == nullptr || rheight == nullptr || generationSignature == nullptr)
							{
								Log("\n*! GMI: error parsing (1) message from server");
							}
							else {
								rbaseTarget = strpbrk(rbaseTarget, "0123456789");
								char *endBaseTarget = strpbrk(rbaseTarget, "\"");
								rheight = strpbrk(rheight, "0123456789");
								char *endHeight = strpbrk(rheight, "\"");
								char *endGenerationSignature = strstr(generationSignature, "\"");

								if (endBaseTarget == nullptr || endHeight == nullptr || endGenerationSignature == nullptr){
									Log("\n*! GMI: error parsing (2) message from server");
								}
								else {
									// Set endpoints
									endBaseTarget[0] = 0;
									endHeight[0] = 0;
									endGenerationSignature[0] = 0;

									if (rtargetDeadline != nullptr)
									{
										rtargetDeadline = strpbrk(rtargetDeadline, "0123456789");
										char* endBaseTarget = strpbrk(rtargetDeadline, "\"");
										if (endBaseTarget == nullptr) endBaseTarget = strpbrk(rtargetDeadline, "}");
										endBaseTarget[0] = 0;

										targetDeadlineInfo = _strtoui64(rtargetDeadline, 0, 10);
										Log("\ntargetDeadlineInfo: "), Log_llu(targetDeadlineInfo);
									}

									unsigned long long  heightInfo = _strtoui64(rheight, 0, 10);
									height = heightInfo;
									baseTarget = _strtoui64(rbaseTarget, 0, 10);
									strcpy(str_signature, generationSignature);
									if (xstr2strr(signature, 33, generationSignature) == 0)	Log("\n*! GMI: Node response: Error decoding generationsignature\n");
								}
							}
						}
					}
				}
			}
			iResult = closesocket(UpdaterSocket);
		}
		freeaddrinfo(result);
	}
	free(buffer);
	return;
}


void updater_i(void) {
	if ((updaterport <= 0) && (strlen(updateraddr) <= 3))
	{
		Log("\nGMI: ERROR in UpdaterAddr or UpdaterPort");
		exit(2);
	}
	for (; !exit_flag;)
	{
		pollLocal();
		std::this_thread::yield();
		std::this_thread::sleep_for(std::chrono::milliseconds(update_interval));
	}
}
 

void hostname_to_ip(char * in, char* out)
{
	struct in_addr addr;
    size_t i = 0;
	
	struct hostent *remoteHost = gethostbyname(in);
	if (remoteHost != nullptr)
	{
		if (remoteHost->h_addrtype == AF_INET)
		{
			while (remoteHost->h_addr_list[i] != 0) {
				addr.s_addr = *(u_long *)remoteHost->h_addr_list[i++];
				strcpy(out, inet_ntoa(addr));
				Log("\nAddress: "); Log(in); Log(" defined as: "); Log(out);
			}
		}
		else if (remoteHost->h_addrtype == AF_NETBIOS) printf_s("NETBIOS address was returned\n");
	}
	else strcpy(out, "Error");
}


void GetCPUInfo(void)
{
		ULONGLONG  TotalMemoryInKilobytes = 0;
		
		wprintw(win_main, "CPU support: ");
		if (InstructionSet::AES())    wprintw(win_main, " AES ", 0);
		if (InstructionSet::SSE())   wprintw(win_main, " SSE ", 0);
		if (InstructionSet::SSE2())   wprintw(win_main, " SSE2 ", 0);
		if (InstructionSet::SSE3())   wprintw(win_main, " SSE3 ", 0);
		if (InstructionSet::SSE42())   wprintw(win_main, " SSE4.2 ", 0);
		if (InstructionSet::AVX())   wprintw(win_main, " AVX ", 0);
		if (InstructionSet::AVX2())  	wprintw(win_main, " AVX2 ", 0);
		
#ifndef __AVX__
		// Checking for AVX requires 3 things:
		// 1) CPUID indicates that the OS uses XSAVE and XRSTORE instructions (allowing saving YMM registers on context switch)
		// 2) CPUID indicates support for AVX
		// 3) XGETBV indicates the AVX registers will be saved and restored on context switch
		bool avxSupported = false;
		int cpuInfo[4];
		__cpuid(cpuInfo, 1);

		bool osUsesXSAVE_XRSTORE = cpuInfo[2] & (1 << 27) || false;
		bool cpuAVXSuport = cpuInfo[2] & (1 << 28) || false;

		if (osUsesXSAVE_XRSTORE && cpuAVXSuport)
		{
			// Check if the OS will save the YMM registers
			unsigned long long xcrFeatureMask = _xgetbv(_XCR_XFEATURE_ENABLED_MASK);
			avxSupported = (xcrFeatureMask & 0x6) == 0x6;
		}
		if (avxSupported)	wprintw(win_main, "     [recomend use AVX]", 0);;
#endif
		SYSTEM_INFO sysinfo;
		GetSystemInfo(&sysinfo);
		wprintw(win_main, "\n%s", InstructionSet::Vendor().c_str(), 0);
		wprintw(win_main, " %s  [%u cores]", InstructionSet::Brand().c_str(), sysinfo.dwNumberOfProcessors);

		if (GetPhysicallyInstalledSystemMemory(&TotalMemoryInKilobytes))
			wprintw(win_main, "\nRAM: %llu Mb", (unsigned long long)TotalMemoryInKilobytes / 1024, 0);
		
		wprintw(win_main, "\n", 0);
}


/*
#ifdef GPU_ON_CPP
std::vector<std::shared_ptr<OpenclPlatform>> GPU_PlatformInfo(void)
{
	//инициализация платформы

	try {
		std::vector<std::shared_ptr<OpenclPlatform>> platforms(OpenclPlatform::list());
		printf("\nPlatforms number: %u\n", platforms.size());

		std::size_t i = 0;
		for (const std::shared_ptr<OpenclPlatform>& platform : platforms) {
			printf("\n----");
			printf("\nId:       %u", i++);
			printf("\nName:     %s", (platform->getName()).c_str());
			printf("\nVendor:   %s", (platform->getVendor()).c_str());
			printf("\nVersion:  %s\n", (platform->getVersion()).c_str());
		}
		return platforms;
	}
	catch (const OpenclError& ex) {
		printf("\n[ERROR][%u][%s] %s", ex.getCode(), ex.getCodeString().c_str(), ex.what());
		//return -1;
	}
	catch (const std::exception& ex) {
		printf("\n[ERROR] %s", ex.what());
		//return -1;
	}
}

int GPU_DeviceInfo(unsigned platform)
{
	//инициализация девайсов
	try {
		std::size_t platformId = platform; //std::atol(p_args[0].c_str());

		std::vector<std::shared_ptr<OpenclPlatform>> platforms(OpenclPlatform::list());
		if (platformId >= platforms.size()) {
			throw std::runtime_error("No platform found with the provided id");
		}

		//std::vector<unsigned long long> sizeUnits{ 1024, 1024, 1024 };
		//std::vector<std::string> sizeLabels{ "KB", "MB", "GB", "TB" };

		std::vector<std::shared_ptr<OpenclDevice>> devices(OpenclDevice::list(platforms[platformId]));
		printf("\nDevices number : %u", devices.size());

		std::size_t i = 0;
		for (const std::shared_ptr<OpenclDevice>& device : devices) {
			printf("\n----");
			printf("\nId:                          %u", i++);
			printf("\nType:                        %s", device->getType().c_str());
			printf("\nName:                        %s", device->getName().c_str());
			printf("\nVendor:                      %s", device->getVendor().c_str());
			printf("\nVersion:                     %s", device->getVersion().c_str());
			printf("\nDriver version:              %s", device->getDriverVersion().c_str());
			printf("\nMax clock frequency:         %u Mhz", device->getMaxClockFrequency());
			printf("\nMax compute units:           %u", device->getMaxComputeUnits());
			printf("\nGlobal memory size:          %llu Mb", device->getGlobalMemorySize()>>20); //cryo::util::formatValue(device->getGlobalMemorySize() >> 10, sizeUnits, sizeLabels));
			printf("\nMax memory allocation size:  %llu Mb", device->getMaxMemoryAllocationSize()>>20); // cryo::util::formatValue(device->getMaxMemoryAllocationSize() >> 10, sizeUnits, sizeLabels));
			printf("\nMax work group size:         %u", device->getMaxWorkGroupSize());
			printf("\nLocal memory size:           %llu Kb", device->getLocalMemorySize()>>10);// cryo::util::formatValue(device->getLocalMemorySize() >> 10, sizeUnits, sizeLabels));

			std::vector<std::size_t> maxWorkItemSizes(device->getMaxWorkItemSizes());
			//printf("\nMax work-item sizes:         (" << cryo::util::join(maxWorkItemSizes.begin(), maxWorkItemSizes.end(), ", ") << ")" << std::endl;
			printf("\n----");
		}
	}
	catch (const OpenclError& ex) {
		printf("\n[ERROR][%u][%s] %s", ex.getCode(), ex.getCodeString().c_str(), ex.what());
		return -1;
	}
	catch (const std::exception& ex) {
		printf("\n[ERROR] %s", ex.what());
		return -1;
	}
	
	return 0;
}


int GPU(void)
{
//std::vector<std::shared_ptr<OpenclPlatform>> plat = GPU_PlatformInfo();
//GPU_DeviceInfo(0);

//Loading platforms
//std::vector<std::shared_ptr<OpenclPlatform>> platforms(OpenclPlatform::list());
//Loading devices
//std::vector<std::vector<std::shared_ptr<OpenclDevice>>> devices;
//for (const std::shared_ptr<OpenclPlatform>& platform : platforms) {
//	devices.push_back(OpenclDevice::list(platform));
//}
//Initializing generation contexts
//std::shared_ptr<GenerationContext> generationContext;
//generationContext = std::shared_ptr<GenerationContext>(new GenerationContextBuffer(config, plotsFile));


cl_int err = CL_SUCCESS;
//OpenclError& ex;
//	try
{

std::vector<cl::Platform> platforms;
cl::Platform::get(&platforms);


cl_context_properties properties[] = { CL_CONTEXT_PLATFORM, (cl_context_properties)(platforms[0])(), 0 };
cl::Context context(CL_DEVICE_TYPE_GPU, properties);

std::vector<cl::Device> devices = context.getInfo<CL_CONTEXT_DEVICES>();



//Load OpenCL source code
std::ifstream sourceFile("kernel\\nonce.cl");
std::string sourceCode(std::istreambuf_iterator<char>(sourceFile), (std::istreambuf_iterator<char>()));
cl::Program::Sources source(1, std::make_pair(sourceCode.c_str(), sourceCode.length() + 1));
printf("\nmake program");
cl::Program program_ = cl::Program(context, source, &err);
//program_.build(contextDevices);
//cl::Kernel kernel(program_, "TestKernel");

//cl::Program::Sources source(1, std::make_pair(helloStr, strlen(helloStr)));
//cl::Program program_ = cl::Program(context, source);
printf("\nbuild program");
const char options[] = "-Werror";
program_.build(devices, options);
char* programLog;
err = program_.getBuildInfo(devices[0], CL_PROGRAM_BUILD_LOG, &programLog);
// check build log
//clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, nullptr, &logSize);
//programLog = (char*)calloc(logSize + 1, sizeof(char));
//clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, logSize + 1, programLog, nullptr);
printf("Build failed: error=%d, programLog:\n%s", err, programLog);
//free(programLog);


printf("\nkernel");
cl::Kernel kernel(program_, "nonce_step3");




//cl::Event event;
//cl::CommandQueue queue(context, devices[0], 0, &err);
//queue.enqueueNDRangeKernel(
//	kernel,
//	cl::nullptrRange,
//	cl::NDRange(4, 4),
//	cl::nullptrRange,
//	nullptr,
//	&event);

//event.wait();
printf("\n\nDONE\n");
}
catch (const OpenclError& err) {
printf("\n[ERROR][%u][%s] %s\n", err.getCode(), err.getCodeString().c_str(), err.what());
return -1;
}
catch (const std::exception& err) {
printf("\n[ERROR] %s\n", err.what());


return -1;
}




return 0;
}


#endif

#ifdef GPU_ON_C

int GPU_init(void)
{
	cl_int ret_err = 0;
	//cl_uint num_devices = 0;
	cl_uint num_platforms = 0;

	//gpu_devices;

	wprintw(win_main, "\nInit GPU platform and devices:");
	wrefresh(win_main);
	ret_err = clGetPlatformIDs(1, nullptr, &num_platforms);
	if (CL_SUCCESS == ret_err)
	{
		wprintw(win_main, "\n Detected OpenCL platforms: %d", num_platforms);
		if (num_platforms < (cl_uint)gpu_devices.use_gpu_platform+1)
		{
			wattron(win_main, COLOR_PAIR(12));
			wprintw(win_main, "\nError. Set correct gpu_platform");
			wattroff(win_main, COLOR_PAIR(12));
			wrefresh(win_main);
			return -1;
		}
	}
	else
	{
		wattron(win_main, COLOR_PAIR(12)); 
		wprintw(win_main, "\nError calling clGetPlatformIDs. Error code: %d", ret_err);
		wattroff(win_main, COLOR_PAIR(12));
		wrefresh(win_main);
		return -1;
	}

	cl_platform_id *platforms = (cl_platform_id*)calloc((size_t)num_platforms, sizeof(cl_platform_id));
	if (platforms == nullptr) {
		ShowMemError();
		wrefresh(win_main);
		return -1;
	}

	ret_err = clGetPlatformIDs(num_platforms, platforms, &num_platforms);
	if (ret_err != CL_SUCCESS)
	{
		wattron(win_main, COLOR_PAIR(12));
		wprintw(win_main, "\n clGetPlatformIDs (error %i)", ret_err);
		wattroff(win_main, COLOR_PAIR(12));
		wrefresh(win_main);
		free(platforms);
		return -1;
	}


	ret_err = clGetDeviceIDs(platforms[gpu_devices.use_gpu_platform], CL_DEVICE_TYPE_GPU, 0, nullptr, &gpu_devices.num_devices);
	if (CL_SUCCESS == ret_err)
	{
		char *str_info_platform = (char*)calloc(255, sizeof(char));
		ret_err = clGetPlatformInfo(platforms[gpu_devices.use_gpu_platform], CL_PLATFORM_VERSION, 254, str_info_platform, nullptr);
		wprintw(win_main, "\n Platform #%d: %s\n  Detected GPU devices: %llu", (cl_uint)gpu_devices.use_gpu_platform, str_info_platform, gpu_devices.num_devices, 0);
		free(str_info_platform);
		if (gpu_devices.num_devices < (cl_uint)gpu_devices.use_gpu_device + 1)
		{
			wattron(win_main, COLOR_PAIR(12));
			wprintw(win_main, "\nError. Set correct gpu_device");
			wattroff(win_main, COLOR_PAIR(12));
			wrefresh(win_main);
			free(platforms);
			return -2;
		}
	}
	else
	{
		wattron(win_main, COLOR_PAIR(12));
		wprintw(win_main, "\nError calling clGetDeviceIDs. Error code: %d", ret_err);
		wattroff(win_main, COLOR_PAIR(12));
		wrefresh(win_main);
		free(platforms);
		return -2;
	}

	gpu_devices.devices = (cl_device_id*)calloc((size_t)gpu_devices.num_devices, sizeof(cl_device_id));
	if (gpu_devices.devices == nullptr) 
	{
		ShowMemError();
		free(platforms);
		return -2;
	}

	ret_err	= clGetDeviceIDs(platforms[gpu_devices.use_gpu_platform], CL_DEVICE_TYPE_GPU, gpu_devices.num_devices, gpu_devices.devices, &gpu_devices.num_devices);
	if (ret_err != CL_SUCCESS)
	{
		wattron(win_main, COLOR_PAIR(12));
		wprintw(win_main, "\n clGetDeviceIDs (error %i)", gpu_devices.num_devices, ret_err);
		wattroff(win_main, COLOR_PAIR(12));
		wrefresh(win_main);
		free(platforms);
		return -2;
	}

	ret_err = clGetDeviceInfo(gpu_devices.devices[gpu_devices.use_gpu_device], CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(size_t), &gpu_devices.max_WorkGroupSize, nullptr);

	ret_err = clGetDeviceInfo(gpu_devices.devices[gpu_devices.use_gpu_device], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(size_t), &gpu_devices.max_ComputeUnits, nullptr);

	char *str_info_1 = (char*)calloc(255, sizeof(char));
	ret_err = clGetDeviceInfo(gpu_devices.devices[gpu_devices.use_gpu_device], CL_DEVICE_NAME, 254, str_info_1, nullptr);

	char *str_info_2 = (char*)calloc(255, sizeof(char));
	ret_err = clGetDeviceInfo(gpu_devices.devices[gpu_devices.use_gpu_device], CL_DEVICE_VERSION, 254, str_info_2, nullptr);
	wprintw(win_main, "\n  Device: %s, %s, %u", str_info_1, str_info_2, gpu_devices.max_ComputeUnits);
	wrefresh(win_main);
	free(str_info_1);
	free(str_info_2);

	free(platforms);
	return 0;
}

int GPU1(void)
{
	const unsigned int HASH_SIZE = 32;
	const unsigned int HASHES_PER_SCOOP = 2;
	const unsigned int SCOOP_SIZE = HASHES_PER_SCOOP * HASH_SIZE;
	const unsigned int SCOOPS_PER_PLOT = 4096;
	const unsigned int PLOT_SIZE = SCOOPS_PER_PLOT * SCOOP_SIZE;
	const unsigned int HASH_CAP = 4096;
	const unsigned int GEN_SIZE = PLOT_SIZE + 16;
	unsigned int WorkSize = 64;
	
	cl_context context;
	cl_command_queue commandQueue;
	cl_mem bufferDevice;
		
	FILE* programHandle;
	size_t programSize; 
	char *programBuffer;
	cl_program program;
	
	cl_int ret_err = 0;


	context = clCreateContext(nullptr, gpu_devices.num_devices, gpu_devices.devices, nullptr, nullptr, &ret_err);
	wprintw(win_main, "\nclCreateContext (error %i)", ret_err);
	wrefresh(win_main);

	commandQueue = clCreateCommandQueue(context, gpu_devices.devices[gpu_devices.use_gpu_device], 0, &ret_err);
	wprintw(win_main, "\nclCreateCommandQueue (error %i)", ret_err);
	wrefresh(win_main);

	bufferDevice = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(unsigned char)* WorkSize * GEN_SIZE, 0, &ret_err);
	wprintw(win_main, "\nclCreateBuffer (error %i)", ret_err);
	wrefresh(win_main);

	wprintw(win_main, "\nget size of kernel source: ");
	// get size of kernel source
	programHandle = fopen("calcdeadlines.cl", "rb");
	_fseeki64(programHandle, 0, SEEK_END);
	programSize = _ftelli64(programHandle);
	rewind(programHandle);
	wprintw(win_main, "%Iu", programSize);
	wrefresh(win_main);

	wprintw(win_main, "\nread kernel source into buffer");
	// read kernel source into buffer
	programBuffer = (char*)calloc(programSize+1, sizeof(char));
	programBuffer[programSize] = '\0';
	fread(programBuffer, sizeof(char), programSize, programHandle);
	fclose(programHandle);
	wrefresh(win_main);

	wprintw(win_main, "\ncreate program from buffer");
	// create program from buffer
	program = clCreateProgramWithSource(context, 1,	(const char**)&programBuffer, &programSize, &ret_err);
	free(programBuffer);
	wprintw(win_main, " (error %i)", ret_err);
	wrefresh(win_main);

	wprintw(win_main, "\nbuild program");
	// build program
	
	char *opt = (char*)calloc(strlen(p_minerPath)+12, sizeof(char));
	opt = strcat(opt, "-I ");
	opt = strcat(opt, p_minerPath);
	opt = strcat(opt, " -Werror");
	wprintw(win_main, "\n%s", opt);
	wrefresh(win_main);
	
	ret_err = clBuildProgram(program, 1, gpu_devices.devices, opt, nullptr, nullptr);
	wprintw(win_main, "\nclBuildProgram %d", ret_err);
	wrefresh(win_main);
	free(opt);
	// build failed
	if (ret_err != CL_SUCCESS) {
		cl_build_status status;
		size_t logSize;

		wprintw(win_main, "\nbuild ERROR");
		wrefresh(win_main);
		// check build error and build status first
		clGetProgramBuildInfo(program, gpu_devices.devices[gpu_devices.use_gpu_device], CL_PROGRAM_BUILD_STATUS, sizeof(cl_build_status), &status, nullptr);
		wprintw(win_main, "\nCL_PROGRAM_BUILD_STATUS",0);
		wrefresh(win_main);
		// check build log
		clGetProgramBuildInfo(program, gpu_devices.devices[gpu_devices.use_gpu_device], CL_PROGRAM_BUILD_LOG, 0, nullptr, &logSize);
		wprintw(win_main, "\nCL_PROGRAM_BUILD_LOG %Iu", logSize);
		wrefresh(win_main);
		char *programLog = (char*)calloc(logSize + 1, sizeof(char));
		clGetProgramBuildInfo(program, gpu_devices.devices[gpu_devices.use_gpu_device], CL_PROGRAM_BUILD_LOG, logSize, programLog, &logSize);
		wprintw(win_main, "\nCL_PROGRAM_BUILD_LOG");
		programLog[475] = 0;
		wprintw(win_main, "\nBuild failed: error=%d, status=%d, programLog:\n%s", ret_err, status, programLog);
		wrefresh(win_main);
		free(programLog);
		
	}
	
	//cl_kernel kernel[3];
	cl_kernel *kernels = (cl_kernel*)calloc(3, sizeof(cl_kernel));
	kernels[0] = clCreateKernel(program, "calculate_deadlines", &ret_err);
	wprintw(win_main, "\nclCreateKernel_1 %d", ret_err);
	wrefresh(win_main);

	//size_t lwg = 0;
	//ret_err = clGetKernelWorkGroupInfo(kernels[0], gpu_devices.devices[gpu_devices.use_gpu_device], CL_KERNEL_WORK_GROUP_SIZE, sizeof(lwg), (void*)&lwg, nullptr);
	//wprintw(win_main, "\nlocal WGS %llu", lwg);
	//wrefresh(win_main);

	ret_err = clSetKernelArg(kernels[0], 0, sizeof(cl_mem), (void*)&bufferDevice);
	wprintw(win_main, "\nclSetKernelArg_1 %d", ret_err);
	wrefresh(win_main);

	kernels[1] = clCreateKernel(program, "reduce_best", &ret_err);
	wprintw(win_main, "\nclCreateKernel_2 %d", ret_err);
	wrefresh(win_main);

	ret_err = clSetKernelArg(kernels[1], 0, sizeof(cl_mem), (void*)&bufferDevice);
	wprintw(win_main, "\nclSetKernelArg_2 %d", ret_err);
	wrefresh(win_main);

	kernels[2] = clCreateKernel(program, "reduce_target", &ret_err);
	wprintw(win_main, "\nclCreateKernel_3 %d", ret_err);
	wrefresh(win_main);

	ret_err = clSetKernelArg(kernels[2], 0, sizeof(cl_mem), (void*)&bufferDevice);
	wprintw(win_main, "\nclSetKernelArg_3 %d", ret_err);
	wrefresh(win_main);
	



	free(gpu_devices.devices);
	if (kernels[2]) { clReleaseKernel(kernels[2]); }
	if (kernels[1]) { clReleaseKernel(kernels[1]); }
	if (kernels[0]) { clReleaseKernel(kernels[0]); }
	
	if (program) { clReleaseProgram(program); }
	if (bufferDevice) { clReleaseMemObject(bufferDevice); }
	if (commandQueue) { clReleaseCommandQueue(commandQueue); }
	if (context) { clReleaseContext(context); }
	wprintw(win_main, "\n--\n");
	wrefresh(win_main);
	return 0;
}

#endif
*/

int main(int argc, char **argv) {
	//low fragmentation heap. При переключении кучи в данный режим повышается расход памяти за счёт выделения чаще всего существенно большего фрагмента, чем был запрошен, но повышается общее быстродействие кучи
	ULONG HeapInformation = 2;
	HeapSetInformation(GetProcessHeap(), HeapCompatibilityInformation, &HeapInformation, sizeof(HeapInformation));


	std::thread proxy;
	std::thread HDD_wakeup;
	
	InitializeCriticalSection(&sessionsLock);
	InitializeCriticalSection(&bestsLock);
	InitializeCriticalSection(&sharesLock);

	size_t i = 0;
	char tbuffer[9];
	unsigned long long bytesRead = 0;

	shares.reserve(20);
	bests.reserve(4);
	sessions.reserve(20);

	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	_COORD coord;
	coord.X = win_size_x;
	coord.Y = win_size_y;

	_SMALL_RECT Rect;
	Rect.Top = 0;
	Rect.Left = 0;
	Rect.Bottom = coord.Y - 1;
	Rect.Right = coord.X - 1;
	
	SetConsoleScreenBufferSize(hConsole, coord);
	SetConsoleWindowInfo(hConsole, TRUE, &Rect);
	
	initscr();
	raw();
	cbreak();		// не использовать буфер для getch()
	noecho();		// не отображать нажатия клавиш
	curs_set(0);	// убрать курсор
	start_color();	// будет всё цветное 			
	
	init_pair(2, COLOR_GREEN, COLOR_BLACK);
	init_pair(4, COLOR_RED, COLOR_BLACK);
	init_pair(6, COLOR_CYAN, COLOR_BLACK);
	init_pair(7, COLOR_WHITE, COLOR_BLACK);
	init_pair(9, 9, COLOR_BLACK);
	init_pair(10, 10, COLOR_BLACK);
	init_pair(11, 11, COLOR_BLACK);
	init_pair(12, 12, COLOR_BLACK);
	init_pair(14, 14, COLOR_BLACK);
	init_pair(15, 15, COLOR_BLACK);

	init_pair(25, 15, COLOR_BLUE);

	win_main = newwin(LINES-2, COLS, 0, 0);

	//box(win_main, 0, 0);
	scrollok(win_main, true);
	//leaveok(win_main, true);
	keypad(win_main, true);
	nodelay(win_main, true);
	//wsetscrreg(win_main, 0, 25);
	//wrefresh(win_main);

	//panel_main = new_panel(win_main);

	WINDOW * win_progress = newwin(3, COLS, LINES-3, 0);
	leaveok(win_progress, true);
//	box(win_progress, 0, 0);
//	wrefresh(win_progress);
	
	wattron(win_main, COLOR_PAIR(12));
	wprintw(win_main, "\nBURST miner, %s", version, 0);
	wattroff(win_main, COLOR_PAIR(12));
	wattron(win_main, COLOR_PAIR(4));
	wprintw(win_main, "\nProgramming: dcct (Linux) & Blago (Windows)\n", 0);
	wattroff(win_main, COLOR_PAIR(4));


	size_t cwdsz = GetCurrentDirectoryA(0, 0);
	p_minerPath = (char*)calloc(cwdsz + 2, sizeof(char));
	if (p_minerPath == nullptr) {
		ShowMemError();
		exit(-1);
	}
	GetCurrentDirectoryA(DWORD(cwdsz), LPSTR(p_minerPath));
	strcat(p_minerPath, "\\");

	char* conf_filename = (char*)calloc(MAX_PATH, sizeof(char));
	if (conf_filename == nullptr) {
		ShowMemError();
		exit(-1);
	}
	if ((argc >= 2) && (strcmp(argv[1], "-config") == 0)){
		if (strstr(argv[2], ":\\")) sprintf_s(conf_filename, _msize(conf_filename), "%s", argv[2]);
		else sprintf_s(conf_filename, _msize(conf_filename), "%s%s", p_minerPath, argv[2]);
	}
	else sprintf_s(conf_filename, _msize(conf_filename), "%s%s", p_minerPath, "miner.conf");

	load_config(conf_filename);
	free(conf_filename);
	Log("\nMiner path: "); Log(p_minerPath);

	GetCPUInfo();

	wrefresh(win_main);
	wrefresh(win_progress);
	//GPU_init();
	//GPU1();
	wrefresh(win_main);
	wrefresh(win_progress);

	if (miner_mode == 0) GetPass(p_minerPath);

	// адрес и порт сервера
	Log("\nSearching servers...");
	WSADATA wsaData;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		wprintw(win_main, "WSAStartup failed\n", 0);
		exit(-1);
	}

	char* updaterip = (char*)calloc(50, sizeof(char));
	if (updaterip == nullptr) {
		ShowMemError();
		exit(-1);
	}
	char* nodeip = (char*)calloc(50, sizeof(char));
	if (nodeip == nullptr) {
		ShowMemError();
		exit(-1);
	}
	wattron(win_main, COLOR_PAIR(11));

	hostname_to_ip(nodeaddr, nodeip);
	wprintw(win_main, "Pool address    %s (ip %s:%Iu)\n", nodeaddr, nodeip, nodeport, 0); 

	if (strlen(updateraddr) > 3) hostname_to_ip(updateraddr, updaterip);
	wprintw(win_main, "Updater address %s (ip %s:%Iu)\n", updateraddr, updaterip, updaterport, 0); 

	wattroff(win_main, COLOR_PAIR(11));
	free(updaterip);
	free(nodeip);


	// обнуляем сигнатуру
	memset(oldSignature, 0, 33);
	memset(signature, 0, 33);

	// Инфа по файлам
	wattron(win_main, COLOR_PAIR(15));
	wprintw(win_main, "Using plots:\n", 0);
	wattroff(win_main, COLOR_PAIR(15));

	total_size = 0;
	for (auto iter = paths_dir.begin(); iter != paths_dir.end(); ++iter)
	{
		std::vector<t_files> files;
		GetFiles(*iter, &files);
		unsigned long long tot_size = 0;
		for (auto it = files.begin(); it != files.end(); ++it) 	tot_size += it->Size;
		wprintw(win_main, "%s\tfiles: %2Iu\t size: %4llu Gb\n", (char*)iter->c_str(), files.size(), tot_size / 1024 / 1024 / 1024, 0);
		total_size += tot_size;
	}
	
	wattron(win_main, COLOR_PAIR(15));
	wprintw(win_main, "TOTAL: %llu Gb\n", total_size / 1024 / 1024 / 1024, 0);
	wattroff(win_main, COLOR_PAIR(15));

	// Run Proxy
	if (enable_proxy)
	{
		proxy = std::thread(proxy_i);
		wattron(win_main, COLOR_PAIR(25));
		wprintw(win_main, "Proxy thread started\n", 0);
		wattroff(win_main, COLOR_PAIR(25));
	}

	// Run updater;
	std::thread updater(updater_i);
	Log("\nUpdater thread started");

	if (use_wakeup)
	{
		HDD_wakeup = std::thread(HDD_wakeup_i);
		Log("\nHDD_wakeup thread started");
	}

	Log("\nUpdate mining info");
	while (height == 0) 
	{
		std::this_thread::yield();
		std::this_thread::sleep_for(std::chrono::milliseconds(2));
	};

	
	// Main loop
	for (; !exit_flag;) 
	{
		worker.clear();
		worker_progress.clear();
		stopThreads = 0;
		

		char scoopgen[40];
		memmove(scoopgen, signature, 32);
		const char *mov = (char*)&height;
		scoopgen[32] = mov[7]; scoopgen[33] = mov[6]; scoopgen[34] = mov[5]; scoopgen[35] = mov[4]; scoopgen[36] = mov[3]; scoopgen[37] = mov[2]; scoopgen[38] = mov[1]; scoopgen[39] = mov[0];

		sph_shabal_context x;
		sph_shabal256_init(&x);
		sph_shabal256(&x, (const unsigned char*)(const unsigned char*)scoopgen, 40);
		char xcache[32]; 
		sph_shabal256_close(&x, xcache);

		scoop = (((unsigned char)xcache[31]) + 256 * (unsigned char)xcache[30]) % 4096;

		deadline = 0;

		

		Log("\n------------------------    New block: "); Log_llu(height);

		_strtime(tbuffer);
		wattron(win_main, COLOR_PAIR(25));
		wprintw(win_main, "\n%s New block %llu, baseTarget %llu, netDiff %llu Tb          \n", tbuffer, height, baseTarget, 4398046511104 / 240 / baseTarget, 0);
		wattron(win_main, COLOR_PAIR(25));
		if (miner_mode == 0)
		{
			unsigned long long sat_total_size = 0;
			for (auto It = satellite_size.begin(); It != satellite_size.end(); ++It) sat_total_size += It->second;
			wprintw(win_main, "*** Chance to find a block: %.5f%%  (%llu Gb)\n", ((double)((sat_total_size * 1024 + total_size / 1024 / 1024) * 100 * 60)*(double)baseTarget) / 1152921504606846976, sat_total_size + total_size / 1024 / 1024 / 1024, 0);
		}

		EnterCriticalSection(&sessionsLock);
		for (auto it = sessions.begin(); it != sessions.end(); ++it) closesocket(it->Socket);
		sessions.clear();
		LeaveCriticalSection(&sessionsLock);

		EnterCriticalSection(&sharesLock);
		shares.clear();
		LeaveCriticalSection(&sharesLock);

		EnterCriticalSection(&bestsLock);
		bests.clear();
		LeaveCriticalSection(&bestsLock);
		
		if ((targetDeadlineInfo > 0) && (targetDeadlineInfo < my_target_deadline)){
			Log("\nUpdate targetDeadline: "); Log_llu(targetDeadlineInfo);
		}
		else {
			targetDeadlineInfo = my_target_deadline;
			//Log("\ntargetDeadline not found, targetDeadline: "); Log_llu(targetDeadlineInfo);
		}

		// Run Sender
		std::thread sender(send_i);

		// Run Threads
		for (i = 0; i < paths_dir.size(); i++)
		{
			worker.push_back(std::thread(work_i, i));
			worker_progress.push_back(0);
		}


		memmove(oldSignature, signature, 32);
		unsigned long long old_baseTarget = baseTarget;
		unsigned long long old_height = height;
		wclear(win_progress);

		// Wait until signature changed
		while ((memcmp(signature, oldSignature, 32) == 0))
		{
			switch (wgetch(win_main))
			{
			case 'q':
				exit_flag = true;
				memset(signature, 0, 33); //для выхода из цикла
				break;
			}
			box(win_progress, 0, 0);
			bytesRead = 0;
			for (auto it = worker_progress.begin(); it != worker_progress.end(); ++it) bytesRead += *it;

			wmove(win_progress, 1, 1); 
			wattron(win_progress, COLOR_PAIR(14));
			if (deadline == 0)
				wprintw(win_progress, "%3llu%% %6llu GB. no deadline                   Network quality: %3u%%", (bytesRead * 4096 * 100 / total_size), (bytesRead / (256 * 1024)), network_quality, 0);
			else
				wprintw(win_progress, "%3llu%% %6llu GB. Deadline =%10llu          Network quality: %3u%%", (bytesRead * 4096 * 100 / total_size), (bytesRead / (256 * 1024)), deadline, network_quality, 0);
			wattroff(win_progress, COLOR_PAIR(14));
			
			wrefresh(win_main);
			wrefresh(win_progress);
			
			std::this_thread::yield();
			std::this_thread::sleep_for(std::chrono::milliseconds(39));
		}

		stopThreads = 1;   // Tell all threads to stop

		if (show_winner)	GetBlockInfo(0);

		for (auto it = worker.begin(); it != worker.end(); ++it)
		{
			Log("\nInterrupt thread. ");	
			if (it->joinable()) it->join();
		}

		Log("\nInterrupt Sender. ");
		if (sender.joinable()) sender.join();

		//Log("\nWriting info to stat-log.csv ");
		//fopen_s(&fp_Stat, "stat-log.csv", "at+");
		//if (fp_Stat != nullptr)
		//{
		//	for (size_t a = 0; a < bests.size(); a++) if ((bests[a].account_id != 0) && (bests[a].DL != 0)) fprintf_s(fp_Stat, "%llu,%llu,%llu,%llu\n", bests[a].account_id, old_height, old_baseTarget, bests[a].DL);
		//	fclose(fp_Stat);
		//}

	}
	
	if (updater.joinable()) updater.join();
	Log("\nUpdater stopped");
	if (use_wakeup)  HDD_wakeup.~thread();
	if (enable_proxy) proxy.join();
	worker.~vector();
	worker_progress.~vector();
	paths_dir.~vector();
	bests.~vector();
	shares.~vector();
	sessions.~vector();
	DeleteCriticalSection(&sessionsLock);
	DeleteCriticalSection(&sharesLock);
	DeleteCriticalSection(&bestsLock);
	free(p_minerPath);

	WSACleanup();
	return 0;
}

