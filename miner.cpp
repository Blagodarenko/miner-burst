#pragma warning( disable : 4715 4996)
#define _CRT_SECURE_NO_WARNINGS
#pragma warning( disable : 4290 )

//#include <vld.h> 
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>
//#include <curses.h> 

#pragma comment(lib,"Ws2_32.lib")
#include <winsock2.h>
#include <ws2tcpip.h>

#include <windows.h>
#include <io.h>
#include "pthread.h"
#include <algorithm>
#include <lmerr.h>
#include <map>

//#define GPU_ON
#ifdef GPU_ON
#define __CL_ENABLE_EXCEPTIONS 100
#include <CL/cl.hpp>

#include "OpenCL_Error.h"
#include "OpenCL_Platform.h"
#include "OpenCL_Device.h"
#endif

#include <fstream>
//#define __cplusplus
#include "sph_shabal.h"
#include "mshabal.h"
#include "mshabal256.h"

#include "InstructionSet.h"
// Initialize static member data
const InstructionSet::InstructionSet_Internal InstructionSet::CPU_Rep;

#include "rapidjson/document.h"		// rapidjson's DOM-style API
#include "rapidjson/prettywriter.h"	// for stringify JSON
#include "rapidjson/filestream.h"	// wrapper of C stream for prettywriter as output
using namespace rapidjson;

#include "PurgeStandbyList.cpp"

HANDLE hConsole;

char *version = "v1.150401";

unsigned long long startnonce = 0;
unsigned long nonces = 0;
unsigned int scoop = 0;
 
unsigned long long deadline = 0;

unsigned long long all_send_msg = 0;
unsigned long long all_rcv_msg = 0;
unsigned long long err_send_msg = 0;
unsigned long long err_rcv_msg = 0;

unsigned long long all_send_dl = 0;
unsigned long long all_rcv_dl = 0;
unsigned long long err_send_dl = 0;
unsigned long long err_rcv_dl = 0;
 
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
size_t paths_num = 0;				// количество параметров пути к файлам
std::vector<std::string> paths_dir;
std::vector<double> thread_times;
bool use_sorting = false;			// Использовать сортировку в отправщике
bool show_msg = false;				// Показать общение с сервером в отправщике
bool show_updates = false;			// Показать общение с сервером в апдейтере
FILE * fp_Log;						// указатель на лог-файл
FILE * fp_Stat;						// указатель на стат-файл
FILE * fp_Time;						// указатель на стат-файл
size_t can_connect = 1;				// уже установлено соединение?
size_t send_interval = 300;			// время ожидания между отправками
size_t update_interval = 1000;		// время ожидания между апдейтами
bool use_fast_rcv = false;
bool use_debug = false;
bool enable_proxy = false;
bool use_generator = false;
bool send_best_only = true;
bool use_log = true;				// Вести лог
bool use_boost = false;			// Использовать повышенный приоритет для потоков
bool use_clean_mem = true;			// Очищать память
bool show_winner = true;			// показывать победителя

unsigned long long my_target_deadline = 4294967295;
SYSTEMTIME cur_time;				// Текущее время
size_t working_threads = 0;			// Поток закончил работу
unsigned long long total_size = 0;	// Общий объем плотов

char *json = NULL;

std::map <char*, unsigned long long> satellite_size; // Структура с объемами плотов сателлитов

struct t_files{
	std::string Path;
	std::string Name;
	unsigned long long Size = 0;
	unsigned State = 0;
	t_files(std::string p_Path, std::string p_Name, unsigned long long p_Size, unsigned p_State) : Path(std::move(p_Path)), Name(std::move(p_Name)), Size(p_Size), State(p_State){};
	//t_files(t_files &&fill) : Path(std::move(fill.Path)), Name(std::move(fill.Name)), Size(fill.Size), State(fill.State){};
	//t_files& operator=(const t_files& fill) = default;
};

struct t_shares{
	std::string file_name;
	unsigned long long account_id = 0;
	unsigned long long best = 0;
	unsigned long long nonce = 0;
	t_shares(std::string p_file_name, unsigned long long p_account_id, unsigned long long p_best, unsigned long long p_nonce) : file_name(std::move(p_file_name)), account_id(p_account_id), best(p_best), nonce(p_nonce){};
}; 

std::vector<t_shares> shares;

struct t_best{
	unsigned long long account_id = 0;
	unsigned long long best = 0;
	unsigned long long nonce = 0;
	unsigned long long DL = 0;
	unsigned long long targetDeadline = 0;
	t_best(unsigned long long p_account_id, unsigned long long p_best, unsigned long long p_nonce, unsigned long long p_DL, unsigned long long p_targetDeadline) : account_id(p_account_id), best(p_best), nonce(p_nonce), DL(p_DL), targetDeadline(p_targetDeadline){};
};

std::vector<t_best> bests;

struct t_session{
	SOCKET Socket;
	unsigned long long ID;
	unsigned long long deadline;
};

std::vector<t_session> sessions;


unsigned long long bytesRead = 0;
unsigned long long height = 0;
unsigned long long baseTarget = 0;
unsigned long long targetDeadlineInfo = 0; // Максимальный дедлайн пула
int stopThreads = 0;
char *pass;		// пароль
 
pthread_mutex_t byteLock = PTHREAD_MUTEX_INITIALIZER;	// прочитанные байты
pthread_mutex_t connectLock = PTHREAD_MUTEX_INITIALIZER;// можно ли коннектиться
pthread_mutex_t bestsLock = PTHREAD_MUTEX_INITIALIZER;	// обновление bests


void cls(void)
{
	SetConsoleTextAttribute(hConsole, 7);
	printf_s("\r\t\t\t\t\t\t\t\t\t       \r");
}

void Log_init(void)
{
	if (use_log)
	{
		char * filename = (char*)calloc(MAX_PATH, 1);
		if (CreateDirectory(L"Logs", NULL) == ERROR_PATH_NOT_FOUND)		printf_s("CreateDirectory failed (%d)\n", GetLastError());
		if (filename == NULL)
		{
			SetConsoleTextAttribute(hConsole, 12);
			printf_s("\nLOG: Memory allocation error\n");
			SetConsoleTextAttribute(hConsole, 7);
			use_log = false;
			return;
		}
		GetLocalTime(&cur_time);
		sprintf(filename, "Logs\\%02d-%02d-%02d_%02d_%02d_%02d.log", cur_time.wYear, cur_time.wMonth, cur_time.wDay, cur_time.wHour, cur_time.wMinute, cur_time.wSecond);
		fopen_s(&fp_Log, filename, "wt");
		if (fp_Log == NULL) {
			printf_s("\nLOG: file openinig error\n");
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
		char * Msg_log = (char *)calloc(len_str * 2 + 1, 1);
		if (Msg_log == NULL)
		{
			cls();
			SetConsoleTextAttribute(hConsole, 12);
			printf_s("\nError allocating memory\n");
			SetConsoleTextAttribute(hConsole, 7);
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

int load_config(char *filename)
{
	FILE * pFile;
	size_t len;

	fopen_s(&pFile, filename, "rt");

	if (pFile == NULL)
	{
		SetConsoleTextAttribute(hConsole, 12);
		printf_s("\nminer.conf not found");
		SetConsoleTextAttribute(hConsole, 7);
		return -1;
	}

	_fseeki64(pFile, 0, SEEK_END);
	__int64 size = _ftelli64(pFile);
	_fseeki64(pFile, 0, SEEK_SET);
	json = (char*)calloc(size+1, 1);
	if (json == NULL) {
		cls();
		SetConsoleTextAttribute(hConsole, 12);
		printf_s("\nError allocating memory\n");
		SetConsoleTextAttribute(hConsole, 7);
		exit(-1);
	}
	len = fread_s(json, size, 1, size - 1, pFile);
	fclose(pFile);

	Document document;	// Default template parameter uses UTF8 and MemoryPoolAllocator.
	if (document.Parse<0>(json).HasParseError()){
		SetConsoleTextAttribute(hConsole, 12);
		printf("\nJSON format error\n");
		SetConsoleTextAttribute(hConsole, 7);
		return 0;
	}

	if (document.ParseInsitu<0>(json).HasParseError()){
		SetConsoleTextAttribute(hConsole, 12);
		printf("\nJSON format error (Insitu)\n");
		SetConsoleTextAttribute(hConsole, 7);
		return 0;
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
			nodeport = document["Port"].GetUint();
		Log_u(nodeport);

		if(document.HasMember("Paths") && document["Paths"].IsArray()){
			const Value& Paths = document["Paths"];	// Using a reference for consecutive access is handy and faster.
			paths_num = Paths.Size();
			Log("\nPaths: "); Log_u(paths_num);
			for (SizeType i = 0; i < Paths.Size(); i++){	// rapidjson uses SizeType instead of size_t.
				paths_dir.push_back(Paths[i].GetString());
				Log("\nPath: "); Log((char*)paths_dir[i].c_str());
			}
		}
		
		if(document.HasMember("CacheSize") && (document["CacheSize"].IsUint64()))		// In this case, IsUint()/IsInt64()/IsUInt64() also return true.
			cache_size = document["CacheSize"].GetUint64();

		Log("\nCacheSize: "); Log_u(cache_size);
		
		if(document.HasMember("UseSorting") && (document["UseSorting"].IsBool()))		
			use_sorting = document["UseSorting"].GetBool();
		Log("\nUseSorting: "); Log_u(use_sorting);

		if(document.HasMember("ShowMsg") && (document["ShowMsg"].IsBool()))		
			show_msg = document["ShowMsg"].GetBool();
		Log("\nShowMsg: "); Log_u(show_msg);

		if(document.HasMember("ShowUpdates") && (document["ShowUpdates"].IsBool()))		
			show_updates = document["ShowUpdates"].GetBool();
		Log("\nShowUpdates: "); Log_u(show_updates);

		Log("\nSendInterval: "); 
		if(document.HasMember("SendInterval") && (document["SendInterval"].IsUint()))		// In this case, IsUint()/IsInt64()/IsUInt64() also return true.
			send_interval = document["SendInterval"].GetUint();
		Log_u(send_interval);

		Log("\nUpdateInterval: "); 
		if(document.HasMember("UpdateInterval") && (document["UpdateInterval"].IsUint()))		// In this case, IsUint()/IsInt64()/IsUInt64() also return true.
			update_interval = document["UpdateInterval"].GetUint();
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
			updaterport = document["UpdaterPort"].GetUint();
		Log("\nUpdater port: "); Log_u(updaterport);

		if (document.HasMember("InfoAddr") && document["InfoAddr"].IsString())
			strcpy(infoaddr, document["InfoAddr"].GetString());
		else strcpy(infoaddr, updateraddr);
		Log("\nInfo address: "); Log(infoaddr);

		if (document.HasMember("InfoPort") && (document["InfoPort"].IsUint()))
			infoport = document["InfoPort"].GetUint();
		else infoport = updaterport;
		Log("\nInfo port: "); Log_u(infoport);


		if (document.HasMember("EnableProxy") && (document["EnableProxy"].IsBool()))
		enable_proxy = document["EnableProxy"].GetBool();
		Log("\nEnableProxy: "); Log_u(enable_proxy);

		Log("\nProxyPort: ");
		if (document.HasMember("ProxyPort") && (document["ProxyPort"].IsUint()))		// In this case, IsUint()/IsInt64()/IsUInt64() also return true.
			proxyport = document["ProxyPort"].GetUint();
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

		if (document.HasMember("UseCleanMem") && (document["UseCleanMem"].IsBool()))
			use_clean_mem = document["UseCleanMem"].GetBool();
		Log("\nUseCleanMem: "); Log_u(use_clean_mem);
		
		if (document.HasMember("UseBoost") && (document["UseBoost"].IsBool()))
			use_boost = document["UseBoost"].GetBool();
		Log("\nUseBoost: "); Log_u(use_boost);

	}
	// параметры по-умолчанию
	Log("\nConfig loaded");
	if(json != NULL) free(json);
	return 1;
}


LPSTR DisplayErrorText( DWORD dwLastError )
{ 
	HMODULE hModule = NULL; // default to system source 
	LPSTR MessageBuffer = NULL; 
	DWORD dwBufferLength; 
	DWORD dwFormatFlags = FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_SYSTEM ;

	if(dwLastError >= NERR_BASE && dwLastError <= MAX_NERR) 
	{ 
		hModule = LoadLibraryEx( TEXT("netmsg.dll"), NULL, LOAD_LIBRARY_AS_DATAFILE );
		if(hModule != NULL) dwFormatFlags |= FORMAT_MESSAGE_FROM_HMODULE;
	} 
	dwBufferLength = FormatMessageA(dwFormatFlags, hModule, dwLastError, MAKELANGID(LANG_SYSTEM_DEFAULT, SUBLANG_SYS_DEFAULT), (LPSTR)&MessageBuffer, 0, NULL);
	if(hModule != NULL) FreeLibrary(hModule); 
	return MessageBuffer;
}


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
// End helper routines
 
void GetPass(char* p_strFolderPath)
{
  FILE * pFile;
  size_t lSize;
  unsigned char * buffer;
  size_t len_pass;
  char * filename = (char*)calloc(MAX_PATH, 1);
  if (filename == NULL)
  {
	  cls();
	  SetConsoleTextAttribute(hConsole, 12);
	  printf_s("\nError allocating memory\n");
	  SetConsoleTextAttribute(hConsole, 7);
	  exit(-1);
  }
  sprintf(filename,"%s%s", p_strFolderPath, "passphrases.txt");
  
//  printf_s("\npass from: %s\n",filename);
  fopen_s(&pFile, filename, "rt");
  if (pFile==NULL) 
  {
	  SetConsoleTextAttribute(hConsole, 12);
	  printf_s("passphrases.txt not found"); 
	  SetConsoleTextAttribute(hConsole, 7);
	  exit (1);
  }

  _fseeki64(pFile , 0 , SEEK_END);
  lSize = _ftelli64(pFile);
  rewind (pFile);

  buffer = (unsigned char*)calloc(lSize+1, 1);
  if (buffer == NULL) 
  {
	  cls();
	  SetConsoleTextAttribute(hConsole, 12);
	  printf_s("\nError allocating memory\n");
	  SetConsoleTextAttribute(hConsole, 7);
	  exit(-1);
  }
  
  len_pass = fread(buffer, 1, lSize, pFile);

  fclose(pFile);
  free (filename);
  char *pass = (char*)calloc(lSize * 3, 1);
  if (pass == NULL)
  {
	  cls();
	  SetConsoleTextAttribute(hConsole, 12);
	  printf_s("\nError allocating memory\n");
	  SetConsoleTextAttribute(hConsole, 7);
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
  printf_s("\n%s\n",pass);
  free (buffer);
 // return str;

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
	
	for (size_t iter = 0; iter < path.size(); iter++)
	{
		hFile = FindFirstFileA(LPCSTR((path.at(iter)+"*").c_str()), &FindFileData);
		if (INVALID_HANDLE_VALUE != hFile)
		{
			do
			{
				//Skip directories
				if (FILE_ATTRIBUTE_DIRECTORY & FindFileData.dwFileAttributes) continue;

				char *test = (char*)calloc(MAX_PATH, 1);
				if (test == NULL)
				{
					cls();
					SetConsoleTextAttribute(hConsole, 12);
					printf_s("\nError allocating memory\n");
					SetConsoleTextAttribute(hConsole, 7);
					exit(-1);
				}
				strcpy(test, FindFileData.cFileName);
				char* ekey = strstr(test, "_");
				if (ekey != NULL){
					char* estart = strstr(ekey + 1, "_");
					if (estart != NULL){
						char* enonces = strstr(estart + 1, "_");
						if (enonces != NULL){
							p_files->emplace_back(
								path.at(iter),
								FindFileData.cFileName,
								(((static_cast<ULONGLONG>(FindFileData.nFileSizeHigh) << sizeof(FindFileData.nFileSizeLow) * 8) | FindFileData.nFileSizeLow)),
								0);
							//p_files->push_back(t_files()); 
							//p_files->at(i).Size = (((static_cast<ULONGLONG>(FindFileData.nFileSizeHigh) << sizeof(FindFileData.nFileSizeLow) * 8) | FindFileData.nFileSizeLow));
							//p_files->at(i).Name = FindFileData.cFileName;
							//p_files->at(i).Path = path.at(iter);
							//p_files->at(i).State = 0;
							i++;
						}
					}
				}
				free(test);
			} while (FindNextFileA(hFile, &FindFileData));
			FindClose(hFile);
		}
	}
	path.clear();
	return i;
}

size_t Get_index_acc(unsigned long long key)
{
	pthread_mutex_lock(&bestsLock);
	size_t cnt = bests.size();
	for (size_t a = 0; a < cnt; a++)
	{
		if (bests[a].account_id == key)
		{
			pthread_mutex_unlock(&bestsLock);
			//Log("\nbests[a].targetDeadline = "); Log_llu(bests[a].targetDeadline);
			return a;
		}
	}
	bests.emplace_back(key, 0, 0, 0, targetDeadlineInfo);
	size_t acc_index = bests.size() - 1;
	pthread_mutex_unlock(&bestsLock);
	return acc_index;
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
				//printf("\r[%llu][%llu]\tfound deadline %llu\n", bests[acc].account_id, nonce, *wertung / baseTarget);
              
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
int _cdecl comp_min(const void *a, const void *b)
{
	struct t_shares *ia = (struct t_shares *) a;
	struct t_shares *ib = (struct t_shares *) b;
	if(ib->best > ia->best) return -1;
	else return 1;
}

int _cdecl comp_max(const void *a, const void *b)
{
	struct t_shares *ia = (struct t_shares *) a;
	struct t_shares *ib = (struct t_shares *) b;
	if (ib->best > ia->best) return 1;
	else return -1;
}

void *proxy_i(void *x_void_ptr)
{
	
	int iResult;
	char buffer[1000];
	char tmp_buffer[1000];
	char tbuffer[9];
	SOCKET ServerSocket = INVALID_SOCKET;
	SOCKET ClientSocket = INVALID_SOCKET;
	struct addrinfo *result = NULL;
	struct addrinfo hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET; //AF_UNSPEC;  // использовать IPv4 или IPv6, нам неважно
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	char pport[6];
	_itoa((int)proxyport, pport, 10);
	iResult = getaddrinfo(NULL, pport, &hints, &result);
	if (iResult != 0) {
		cls();
		printf("\rgetaddrinfo failed with error: %d\n", iResult);
	}

	ServerSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ServerSocket == INVALID_SOCKET) {
		printf("\rsocket failed with error: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
	}
	BOOL l = TRUE;
	iResult = bind(ServerSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("\rbind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ServerSocket);
	}
	freeaddrinfo(result);
	iResult = ioctlsocket(ServerSocket, FIONBIO, (unsigned long*)&l);
	if (iResult == SOCKET_ERROR)
	{
		Log("\nProxy: ! Error ioctlsocket's: "); Log(DisplayErrorText(WSAGetLastError()));
		cls();
		SetConsoleTextAttribute(hConsole, 12);
		printf_s("\rioctlsocket failed: %ld\n", WSAGetLastError());
		SetConsoleTextAttribute(hConsole, 7);
	}

	iResult = listen(ServerSocket, 8);
	if (iResult == SOCKET_ERROR) {
		printf("\rlisten failed with error: %d\n", WSAGetLastError());
		closesocket(ServerSocket);
	}
	Log("\nProxy thread started");

	do{
		if (can_connect == 1 )
		{
			pthread_mutex_lock(&connectLock);
			can_connect = 0;
			pthread_mutex_unlock(&connectLock);

			struct sockaddr_in client_socket_address;
			int iAddrSize = sizeof(client_socket_address);
			ClientSocket = accept(ServerSocket, (struct sockaddr *)&client_socket_address, &iAddrSize);
			if (ClientSocket == INVALID_SOCKET)
			{
				if (WSAGetLastError() != WSAEWOULDBLOCK)
				{
					Log("\nProxy:! Error Proxy's accept: "); Log(DisplayErrorText(WSAGetLastError()));
					cls();
					SetConsoleTextAttribute(hConsole, 12);
					printf_s("\rProxy can't accept. Error: %ld\n", WSAGetLastError());
					SetConsoleTextAttribute(hConsole, 7);
				}
			}
			else
			{
				memset(buffer, 0, 1000);
				do{
					memset(tmp_buffer, 0, 1000);
					iResult = recv(ClientSocket, tmp_buffer, 999, 0);
					strcat(buffer, tmp_buffer);
				}while ((iResult > 0) && !use_fast_rcv);

				
				Log("\nProxy get info: ");  Log_server(buffer);
				unsigned long long get_accountId = 0;
				unsigned long long get_nonce = 0;
				unsigned long long get_deadline = 0;
				unsigned long long get_totalsize = 0;
				// locate HTTP header
				char *find = strstr(buffer, "\r\n\r\n");
				if (find != NULL)
				{
					if (strstr(buffer, "submitNonce") != NULL)
					{

						char *startaccountId = strstr(buffer, "accountId=");
						if (startaccountId != NULL)
						{
							startaccountId = strpbrk(startaccountId, "0123456789");
							char *endaccountId = strpbrk(startaccountId, "& }\"");

							char *startnonce = strstr(buffer, "nonce=");
							char *startdl = strstr(buffer, "deadline=");
							char *starttotalsize = strstr(buffer, "TotalSize");
							if ((startnonce != NULL) && (startdl != NULL))
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

								if (starttotalsize != NULL)
								{
									starttotalsize = strpbrk(starttotalsize, "0123456789");
									char *endtotalsize = strpbrk(starttotalsize, "& }\"");
									endtotalsize[0] = 0;
									get_totalsize = _strtoui64(starttotalsize, 0, 10);
									satellite_size.insert(std::pair <char*, unsigned long long>(inet_ntoa(client_socket_address.sin_addr), get_totalsize));
								}
																
								shares.emplace_back(inet_ntoa(client_socket_address.sin_addr), get_accountId, get_deadline, get_nonce);
								
								{
									cls();
									SetConsoleTextAttribute(hConsole, 2);
									_strtime(tbuffer);
									printf_s("\r%s [%20llu]\treceived DL: %11llu {%s}\n", tbuffer, get_accountId, get_deadline / baseTarget, inet_ntoa(client_socket_address.sin_addr));
									SetConsoleTextAttribute(hConsole, 7);

								}
								Log("Proxy: received DL "); Log_llu(get_deadline); Log(" from "); Log(inet_ntoa(client_socket_address.sin_addr));

								//Подтверждаем
								memset(buffer, 0, 1000);
								size_t acc = Get_index_acc(get_accountId);
								int bytes = sprintf_s(buffer, "HTTP/1.0 200 OK\r\nConnection: close\r\n\r\n{\"result\": \"proxy\",\"accountId\": %llu,\"deadline\": %llu,\"targetDeadline\": %llu}", get_accountId, get_deadline / baseTarget, bests[acc].targetDeadline);
								iResult = send(ClientSocket, buffer, bytes, 0);
								if (iResult == SOCKET_ERROR)
								{
									err_send_dl++;
									Log("\nProxy: ! Error sending to client: "); Log(DisplayErrorText(WSAGetLastError()));
									cls();
									SetConsoleTextAttribute(hConsole, 12);
									printf_s("\rfailed sending to client: %ld\n", WSAGetLastError());
									SetConsoleTextAttribute(hConsole, 7);
								}
								else
								{
									if (use_debug)
									{
										cls();
										SetConsoleTextAttribute(hConsole, 9);
										_strtime(tbuffer);
										printf_s("\r%s [%llu]\tsent confirmation to %s\n", tbuffer, get_accountId, inet_ntoa(client_socket_address.sin_addr));
										SetConsoleTextAttribute(hConsole, 7);
									}
									Log("\nProxy: sent confirmation to "); Log(inet_ntoa(client_socket_address.sin_addr));
								}
							}
						}
					}
					else
					{
						if (strstr(buffer, "getMiningInfo") != NULL)
						{
							memset(buffer, 0, 1000);
							int bytes = sprintf_s(buffer, "HTTP/1.0 200 OK\r\nConnection: close\r\n\r\n{\"baseTarget\":\"%llu\",\"height\":\"%llu\",\"generationSignature\":\"%s\",\"targetDeadline\":%llu}", baseTarget, height, str_signature, targetDeadlineInfo);
							iResult = send(ClientSocket, buffer, bytes, 0);
							if (iResult == SOCKET_ERROR)
							{
								Log("\nProxy: ! Error sending to client: "); Log(DisplayErrorText(WSAGetLastError()));
								cls();
								SetConsoleTextAttribute(hConsole, 12);
								printf_s("\rfailed sending to client: %ld\n", WSAGetLastError());
								SetConsoleTextAttribute(hConsole, 7);
							}
							else
							{
								Log("\nProxy: sent update to "); Log(inet_ntoa(client_socket_address.sin_addr));
							}
						}
						else
						{
							if ((strstr(buffer, "getBlocks") != NULL) || (strstr(buffer, "getAccount") != NULL) || (strstr(buffer, "getRewardRecipient") != NULL))
							{
								; // ничего не делаем
							}
							else
							{
								find[0] = 0;
								SetConsoleTextAttribute(hConsole, 15);
								printf_s("\rProxy: %s\n", buffer);
								SetConsoleTextAttribute(hConsole, 7);
							}
						}
					}
				}
				iResult = closesocket(ClientSocket);
			}
			pthread_mutex_lock(&connectLock);
			can_connect = 1;
			pthread_mutex_unlock(&connectLock);
		}
		Sleep(200);
	}while (true);

}

void *send_i(void *x_void_ptr)
{
	SOCKET ConnectSocket;

	int iResult = 0;
	unsigned buffer_size = 1000;
	char* buffer = (char*)calloc(buffer_size, 1);
	if (buffer == NULL) {
		cls();
		SetConsoleTextAttribute(hConsole, 12);
		printf_s("\nError allocating memory\n");
		SetConsoleTextAttribute(hConsole, 7);
		exit(-1);
	}
	char* tmp_buffer = (char*)calloc(buffer_size, 1);
	if (tmp_buffer == NULL) {
		cls();
		SetConsoleTextAttribute(hConsole, 12);
		printf_s("\nError allocating memory\n");
		SetConsoleTextAttribute(hConsole, 7);
		exit(-1);
	}
	char tbuffer[9];

	struct addrinfo *result = NULL;
	struct addrinfo hints;
	size_t iter;
	size_t acc;
	for (;;)
	{
		while (shares.empty() && sessions.empty())	{Sleep(10);} //поуза при бездействии

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
		if (send_best_only) //Гасим все шары, которые больше текущего targetDeadline
		{
			for (iter = 0; iter < shares.size(); iter++)
			{
				acc = Get_index_acc(shares[iter].account_id);
				//printf("\r\nshares id: %llu\n", shares[iter].account_id);
				if ((shares[iter].best / baseTarget) > bests[acc].targetDeadline)
				{
					if (use_debug)
					{
						cls();
						SetConsoleTextAttribute(hConsole, 4);
						_strtime(tbuffer);
						printf_s("\r%s [%llu]\t%llu > %llu  discarded\n", tbuffer, shares[iter].account_id, shares[iter].best / baseTarget, bests[acc].targetDeadline);
						SetConsoleTextAttribute(hConsole, 7);
					}
					shares.erase(shares.begin() + iter);
					iter--;
				}
			}
		}

		for (size_t iter = 0; iter < shares.size(); iter++)
		{
			if (can_connect == 1)
			{
				pthread_mutex_lock(&connectLock);
				can_connect = 0;
				pthread_mutex_unlock(&connectLock);

				ZeroMemory(&hints, sizeof(hints));
				hints.ai_family = AF_INET; //AF_UNSPEC;  // использовать IPv4 или IPv6, нам неважно
				hints.ai_socktype = SOCK_STREAM;
				hints.ai_protocol = IPPROTO_TCP;
				hints.ai_flags = AI_PASSIVE;
				char nport[6];
				_itoa((int)nodeport, nport, 10);
				iResult = getaddrinfo(nodeaddr, nport, &hints, &result);
				if (iResult != 0) {
					cls();
					printf("\rgetaddrinfo failed with error: %d\n", iResult);
				}
				ConnectSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
				if (ConnectSocket == INVALID_SOCKET) {
					printf("\rsocket failed with error: %ld\n", WSAGetLastError());
					freeaddrinfo(result);
				}

				iResult = connect(ConnectSocket, result->ai_addr, (int)result->ai_addrlen);
				if (iResult == SOCKET_ERROR)
				{
					Log("\nSender:! Error Sender's connect: "); Log(DisplayErrorText(WSAGetLastError()));
					cls();
					SetConsoleTextAttribute(hConsole, 12);
					printf_s("\rSender can't connect. Error: %ld\n", WSAGetLastError());
					SetConsoleTextAttribute(hConsole, 7);
				}
				else
				{
					freeaddrinfo(result);

					int bytes = 0;
					memset(buffer, 0, buffer_size);
					if (miner_mode == 0)
					{
						bytes = sprintf_s(buffer, _msize(buffer), "POST /burst?requestType=submitNonce&secretPhrase=%s&nonce=%llu HTTP/1.0\r\nConnection: close\r\n\r\n", pass, shares[iter].nonce);
					}
					if (miner_mode == 1)
					{
						unsigned long long total = total_size / 1024 / 1024 / 1024;
						for (std::map <char*, unsigned long long>::iterator It = satellite_size.begin(); It != satellite_size.end(); It) total = total + It->second;
						bytes = sprintf_s(buffer, _msize(buffer), "POST /burst?requestType=submitNonce&accountId=%llu&nonce=%llu&deadline=%llu HTTP/1.0\r\nX-Miner: Blago %s\r\nX-Capacity: %llu\r\nConnection: close\r\n\r\n", shares[iter].account_id, shares[iter].nonce, shares[iter].best, version, total);
					}
					if (miner_mode == 2)
					{
						char *f1 = (char*)calloc(MAX_PATH, 1);
						char *str_len = (char*)calloc(MAX_PATH, 1);
						if ((f1 == NULL) || (str_len == NULL))
						{
							cls();
							SetConsoleTextAttribute(hConsole, 12);
							printf_s("\nError allocating memory\n");
							SetConsoleTextAttribute(hConsole, 7);
							exit(-1);
						}

						int len = sprintf(f1, "%llu:%llu:%llu\n", shares[iter].account_id, shares[iter].nonce, height);
						_itoa_s(len, str_len, 10, 10);

						bytes = sprintf_s(buffer, _msize(buffer), "POST /pool/submitWork HTTP/1.0\r\nHost: %s:%llu\r\nContent-Type: text/plain;charset=UTF-8\r\nContent-Length: %i\r\n\r\n%s", nodeaddr, nodeport, len, f1);
						free(f1);
						free(str_len);
					}

					// Sending to server
					all_send_dl++;
					iResult = send(ConnectSocket, buffer, bytes, 0);
					if (iResult == SOCKET_ERROR)
					{
						err_send_dl++;
						Log("\nSender: ! Error deadline's sending: "); Log(DisplayErrorText(WSAGetLastError()));
						cls();
						SetConsoleTextAttribute(hConsole, 12);
						printf_s("\rsend failed: %ld\n", WSAGetLastError());
						SetConsoleTextAttribute(hConsole, 7);
					}
					else
					{
						cls();
						SetConsoleTextAttribute(hConsole, 9);
						unsigned long long dl = shares[iter].best / baseTarget;
						_strtime(tbuffer);
						printf_s("\r%s [%20llu] sent DL: %15llu %5llud %02llu:%02llu:%02llu\n", tbuffer, shares[iter].account_id, dl, (dl) / (24 * 60 * 60), (dl % (24 * 60 * 60)) / (60 * 60), (dl % (60 * 60)) / 60, dl % 60);
						SetConsoleTextAttribute(hConsole, 7);

						if (show_msg) printf_s("\nsend: %s\n", buffer); // показываем послание
						Log("\nSender:   Sent to server: "); Log_server(buffer);

						t_session *to = (t_session*)calloc(1, sizeof(t_session));
						if (to == NULL) {
							cls();
							SetConsoleTextAttribute(hConsole, 12);
							printf_s("\nError allocating memory\n");
							SetConsoleTextAttribute(hConsole, 7);
							exit(-1);
						}
						to->Socket = ConnectSocket;
						to->ID = shares[iter].account_id;
						to->deadline = dl;
						sessions.push_back(*to);
						free(to);
						if (send_best_only) bests[Get_index_acc(shares[iter].account_id)].targetDeadline = dl;
						shares.erase(shares.begin() + iter);
						iter--;
					}
				}
				pthread_mutex_lock(&connectLock);
				can_connect = 1;
				pthread_mutex_unlock(&connectLock);
			}
		}

		if ((can_connect == 1) && !sessions.empty())
		{
			pthread_mutex_lock(&connectLock);
			can_connect = 0;
			pthread_mutex_unlock(&connectLock);


			for (size_t iter = 0; iter < sessions.size(); iter++)
			{
				ConnectSocket = sessions.at(iter).Socket;

				BOOL l = TRUE;
				iResult = ioctlsocket(ConnectSocket, FIONBIO, (unsigned long*)&l);
				if (iResult == SOCKET_ERROR)
				{
					Log("\nSender: ! Error ioctlsocket's: "); Log(DisplayErrorText(WSAGetLastError()));
					cls();
					SetConsoleTextAttribute(hConsole, 12);
					printf_s("\rioctlsocket failed: %ld\n", WSAGetLastError());
					SetConsoleTextAttribute(hConsole, 7);
				}

				memset(buffer, 0, buffer_size);
				size_t resp = 0;
				do{
					memset(tmp_buffer, 0, buffer_size);
					iResult = recv(ConnectSocket, tmp_buffer, buffer_size - 1, 0);
					strcat(buffer, tmp_buffer);
					resp++;
				} while ((iResult > 0) && !use_fast_rcv);

				if (iResult == SOCKET_ERROR)
				{
					if (WSAGetLastError() != WSAEWOULDBLOCK)
					{
						err_rcv_dl++;
						Log("\nSender: ! Error getting deadline's confirmation: "); Log(DisplayErrorText(WSAGetLastError()));
						cls();
						SetConsoleTextAttribute(hConsole, 12);
						printf_s("\rreceiving confirmation failed: %ld\n", WSAGetLastError());
						SetConsoleTextAttribute(hConsole, 7);
					}
				}
				else
				{
					if (show_msg) printf_s("\nReceived: %s\n", buffer);
					Log("\nSender:   Received from server: "); Log_server(buffer); Log("\nCount responses: "); Log_u(resp);

					char *find = strstr(buffer, "\r\n\r\n");
					if (find != NULL)
					{
						char *rdeadline = strstr(find + 4, "\"deadline\"");
						if (rdeadline != NULL)
						{
							rdeadline = strpbrk(rdeadline, "0123456789");
							char *enddeadline = strpbrk(rdeadline, ",}\"");

							char* rtargetDeadline = strstr(buffer, "\"targetDeadline\":");
							char* raccountId = strstr(buffer, "\"accountId\":");
							if (enddeadline != NULL)
							{
								enddeadline[0] = 0;
								unsigned long long ndeadline = _strtoui64(rdeadline, 0, 10);
								unsigned long long naccountId = 0;
								unsigned long long ntargetDeadline = 0;
								if ((rtargetDeadline != NULL) && (raccountId != NULL))
								{
									rtargetDeadline = strpbrk(rtargetDeadline, "0123456789");
									char* endBaseTarget = strpbrk(rtargetDeadline, ",}\"");
									endBaseTarget[0] = 0;

									raccountId = strpbrk(raccountId, "0123456789");
									char* endaccountId = strpbrk(raccountId, ",}\"");
									endaccountId[0] = 0;
									naccountId = _strtoui64(raccountId, 0, 10);
									size_t acc = Get_index_acc(naccountId);
									ntargetDeadline = _strtoui64(rtargetDeadline, 0, 10);
									bests[acc].targetDeadline = ntargetDeadline;
								}
								all_rcv_dl++;
								Log("\nSender: confirmed deadline: "); Log_llu(ndeadline);
								cls();
								//unsigned long long years = (ndeadline % (24*60*60))/(60*60)
								//unsigned month = (ndeadline % (365*24*60*60))/(24*60*60);
								unsigned long long days = (ndeadline) / (24 * 60 * 60);
								unsigned hours = (ndeadline % (24 * 60 * 60)) / (60 * 60);
								unsigned min = (ndeadline % (60 * 60)) / 60;
								unsigned sec = ndeadline % 60;
								cls();
								_strtime(tbuffer);
								SetConsoleTextAttribute(hConsole, 10);
								if ((naccountId != 0) && (ntargetDeadline != 0))
								{
									printf_s("\r%s [%llu] confirmed DL: %9llu\t%llud %2u:%2u:%2u\n", tbuffer, naccountId, ndeadline, days, hours, min, sec);
									if (use_debug) printf_s("\r%s [%llu] Set targetDL\t%llu\n", tbuffer, naccountId, ntargetDeadline);
								}
								else printf_s("\r%s [%20llu] confirmed DL: %10llu %5llud %02u:%02u:%02u\n", tbuffer, sessions.at(iter).ID, ndeadline, days, hours, min, sec);
								SetConsoleTextAttribute(hConsole, 7);
								if (ndeadline < deadline || deadline == 0)  deadline = ndeadline;

								if (ndeadline != sessions.at(iter).deadline)
								{
									SetConsoleTextAttribute(hConsole, 6);
									printf_s("----Fast block or corrupted file?----\nSent deadline:\t%llu\nServer's deadline:\t%llu \n---- file: %s\n", sessions.at(iter).deadline, ndeadline, " "); //shares[i].file_name.c_str());
									SetConsoleTextAttribute(hConsole, 7);
								}
							}
						}
						else
						if (strstr(find + 4, "Received share") != NULL)
						{
							cls();
							_strtime(tbuffer);
							SetConsoleTextAttribute(hConsole, 10);
							deadline = bests[Get_index_acc(sessions.at(iter).ID)].DL;
							all_rcv_dl++;
							printf_s("\r%s [%20llu] confirmed DL\n", tbuffer, sessions.at(iter).ID);
							SetConsoleTextAttribute(hConsole, 7);
						}
						else
						{
							SetConsoleTextAttribute(hConsole, 15);
							printf_s("\r%s\n", find + 4);
							SetConsoleTextAttribute(hConsole, 7);
						}
						iResult = closesocket(ConnectSocket);
						Log("\nSender:Close socket. Code = "); Log_u(WSAGetLastError());
						//if (sessions.size() >= iter)
						//{
							sessions.erase(sessions.begin() + iter);
							iter--;
						//}
					}
				}
			}
			pthread_mutex_lock(&connectLock);
			can_connect = 1;
			pthread_mutex_unlock(&connectLock);
		}

		Sleep((DWORD)send_interval);
	}
	free(buffer);
	free(tmp_buffer);
	return 0;
}

/*
unsigned long long procscoop4(unsigned long long nonce, unsigned long long n, char *data, int acc, const std::string &file_name) {
        char *cache;
        char sig0[32 + 64];
		char sig1[32 + 64];
		char sig2[32 + 64];
		char sig3[32 + 64];
        cache = data;
        unsigned long long v;
 
        memmove(sig0, signature, 32);
		memmove(sig1, signature, 32);
		memmove(sig2, signature, 32);
		memmove(sig3, signature, 32);
 
        for( v=0; v<n; v+=4) 
		{
                memmove(&sig0[32], &cache[(v+0)*64], 64);
				memmove(&sig1[32], &cache[(v+1)*64], 64);
				memmove(&sig2[32], &cache[(v+2)*64], 64);
				memmove(&sig3[32], &cache[(v+3)*64], 64);
                char res0[32];
				char res1[32];
				char res2[32];
				char res3[32];
 
                mshabal_context x;
				avx1_mshabal_init(&x, 256);
                avx1_mshabal(&x, (const unsigned char*) sig0, (const unsigned char*) sig1, (const unsigned char*) sig2, (const unsigned char*) sig3, 64 + 32);
                avx1_mshabal_close(&x, 0, 0, 0, 0, 0, res0, res1, res2, res3);

                unsigned long long *wertung = (unsigned long long*)res0;
				unsigned long long *wertung1 = (unsigned long long*)res1;
				unsigned long long *wertung2 = (unsigned long long*)res2;
				unsigned long long *wertung3 = (unsigned long long*)res3;
				unsigned posn = 0;
				if(*wertung1 < *wertung) 
				{
					*wertung = *wertung1;
					posn=1;
				}
				if(*wertung2 < *wertung) 
				{
					*wertung = *wertung2;
					posn=2;
				}
				if(*wertung3 < *wertung) 
				{
					*wertung = *wertung3;
					posn=3;
				}
				
 
                if(bests[acc].nonce == 0 || *wertung < bests[acc].best) 
				{
                    bests[acc].best = *wertung;
					bests[acc].nonce = nonce + v + posn;
					if((bests[acc].best / baseTarget) <= bests[acc].targetDeadline) // Has to be this good before we inform the node
						{       
							Log("\nfound deadline=");	Log_llu(bests[acc].best/baseTarget); Log(" nonce=");	Log_llu(bests[acc].nonce); Log(" for account: "); Log_llu(bests[acc].account_id);
							//printf("\rbestn_local: %llu  bestn_local: %llu  n:%llu\n", best_local, bestn_local, n);
							pthread_mutex_lock(&byteLock);
								shares[num_shares].best = bests[acc].best;
								shares[num_shares].nonce = bests[acc].nonce;
								//shares[num_shares].to_send = 1;
								shares[num_shares].account_id = bests[acc].account_id;
								shares[num_shares].file_name = file_name;
								
								if(use_debug)
								{
									cls();
									SetConsoleTextAttribute(hConsole, 2);
									printf("\r[%llu]\tfound deadline %llu\n", bests[acc].account_id, shares[num_shares].best / baseTarget);
									SetConsoleTextAttribute(hConsole, 7);
								}
								num_shares++;
							pthread_mutex_unlock(&byteLock);
                        }
				}
                //nonce++;
                //cache += 64*4;
        }
		
		return v;
}



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
					printf("\r%s [%llu]\tfound deadline %llu\n", tbuffer, bests[acc].account_id, shares[num_shares].best / baseTarget);
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

void procscoop(unsigned long long nonce, unsigned long long n, char *data, size_t acc, std::string file_name) {
	char *cache;
	char sig[32 + 64];
	cache = data;
	unsigned long long v;
	char tbuffer[9];
	memmove(sig, signature, 32);

	for (v = 0; v < n; v++)
	{
		memmove(&sig[32], cache, 64);
		char res[32];

		sph_shabal_context x;
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
					pthread_mutex_lock(&bestsLock);
					bests[acc].best = *wertung;
					bests[acc].nonce = nonce + v;
					bests[acc].DL = *wertung / baseTarget;
					pthread_mutex_unlock(&bestsLock);
					shares.emplace_back(file_name, bests[acc].account_id, bests[acc].best, bests[acc].nonce);
					if (use_debug)
					{
						cls();
						_strtime(tbuffer);
						SetConsoleTextAttribute(hConsole, 2);
						printf_s("\r%s [%llu] found DL:      %9llu\n", tbuffer, bests[acc].account_id, bests[acc].DL);
						SetConsoleTextAttribute(hConsole, 7);
					}
				}
			}
			else
			{
				pthread_mutex_lock(&bestsLock);
				if (bests[acc].nonce == 0 || *wertung < bests[acc].best)
				{
					bests[acc].best = *wertung;
					bests[acc].nonce = nonce + v;
					bests[acc].DL = *wertung / baseTarget;
				}
				pthread_mutex_unlock(&bestsLock);
				shares.emplace_back(file_name, bests[acc].account_id, *wertung, nonce + v);
				if (use_debug)
				{
					cls();
					_strtime(tbuffer);
					SetConsoleTextAttribute(hConsole, 2);
					printf_s("\r%s [%llu] found DL:      %9llu\n", tbuffer, bests[acc].account_id, *wertung / baseTarget);
					SetConsoleTextAttribute(hConsole, 7);
				}
			}
		}
		cache += 64;
	}
}

void stick_thread_to_core(size_t core_id)
{
	//DWORD_PTR processAffinityMask;
	//processAffinityMask = 1 << 15;//(2*i);
	//SetProcessAffinityMask(GetCurrentProcess(), processAffinityMask);
	
	//DWORD_PTR threadAffinityMask = 1 << core_id;// 1 << (2 * (int)core_id);
	//return SetThreadAffinityMask(GetCurrentThread(), threadAffinityMask);

	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
	SetThreadIdealProcessor(GetCurrentThread(),(DWORD)core_id);
}


void *work_i(void* path_str) {
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	working_threads++;
	//pthread_self();
	if (use_boost) stick_thread_to_core(working_threads - 1);
	
	std::string path_loc_str((char*)path_str);
	
	unsigned long long files_size_per_thread = 0;
	size_t cache_size_lacal;
	clock_t start_work_time, end_work_time;
	clock_t start_time_read, end_time_read;			// Текущее время
	start_work_time = clock();
	Log("\nStart thread: ");	Log((char*)path_loc_str.c_str()); //Log(paths_dir[local_num]);

		double avg_time_proc = 0;
		clock_t start_time_proc;			

		FILE * pFile;
		std::vector<t_files> files;
		size_t f_count = GetFiles(path_loc_str, &files);
		size_t files_count = 0;
		size_t acc_index;
		unsigned long long  bytes_per_read;
	
		for(; files_count < f_count; files_count++)
		{
			unsigned long long key, nonce, nonces, stagger;
			start_time_read = clock();
			sscanf_s(files[files_count].Name.c_str(), "%llu_%llu_%llu_%llu", &key , &nonce, &nonces, &stagger);
			if ((float)(nonces % stagger) != 0)
			{
				cls();
				SetConsoleTextAttribute(hConsole, 12);
				printf("\rFile %s wrong stagger?\n", files[files_count].Name.c_str());
				SetConsoleTextAttribute(hConsole, 7);
			}
			
			if (nonces != stagger)
			{

				cache_size_lacal = stagger;
				bytes_per_read = 64 * stagger;
			}
			else
			{
				cache_size_lacal = cache_size;
				bytes_per_read = 64;
			}
			char *cache = (char*)calloc(cache_size_lacal * 64, 1);
			if (cache == NULL) {
				cls();
				SetConsoleTextAttribute(hConsole, 12);
				printf_s("\nError allocating memory\n");
				SetConsoleTextAttribute(hConsole, 7);
				exit(-1);
			}
						
			if (nonces != (files[files_count].Size) / (4096 * 64)) // Проверка на повреждения плота
			{
				SetConsoleTextAttribute(hConsole, 12);
				cls();
				printf_s("\rfile \"%s\" name/size mismatch\n", files[files_count].Name.c_str());
				SetConsoleTextAttribute(hConsole, 7);
				if (nonces != stagger)
					nonces = (((files[files_count].Size) / (4096 * 64)) / stagger) * stagger; //обрезаем плот по размеру и стаггеру
				else
				if (scoop > (files[files_count].Size) / (stagger * 64)) //если номер скупа попадает в поврежденный смерженный плот, то все нормально
				{
					SetConsoleTextAttribute(hConsole, 12);
					cls();
					printf_s("\rskipped\n");
					SetConsoleTextAttribute(hConsole, 7);
					continue;
				}
			}
			
			Log("\nRead file : ");	Log((char*)files[files_count].Name.c_str());
						
			_set_fmode(_O_BINARY);
			fopen_s(&pFile, (files[files_count].Path + files[files_count].Name).c_str(), "rb");
			if (pFile==NULL)
			{
				SetConsoleTextAttribute(hConsole, 12);
				cls();
				printf_s("\rfile \"%s\" error opening\n", files[files_count].Name.c_str());
				SetConsoleTextAttribute(hConsole, 7);
				continue;
			}
			files_size_per_thread += files[files_count].Size;
			acc_index = Get_index_acc(key);
//			Log("\nGet_index_acc = "); Log_u(acc_index);
			unsigned long long start, noffset, i, readsize, bytes;
			
			for (unsigned long long n = 0; n<nonces; n += stagger)
			{
				start = n * 4096 * 64 + scoop * stagger*64;
				noffset = 0;
                                
				for (i = start; i < start + stagger * 64; i += cache_size_lacal * 64)
				{
					
					readsize = cache_size_lacal * 64;
					if (readsize > start + stagger*64 - i)	readsize = start + stagger*64 - i;
					bytes = 0;

					_fseeki64_nolock(pFile , i , SEEK_SET);

					do {
						//b = _fread_nolock_s(&cache[bytes], cache_size * 64, sizeof(char), readsize - bytes, pFile);
						bytes += bytes_per_read * _fread_nolock_s(&cache[bytes], cache_size_lacal * 64, bytes_per_read, readsize / bytes_per_read, pFile);
					} while ((bytes < readsize) && !feof(pFile));
					
					if (bytes == readsize)
					{
						//procscoop4(n + nonce + noffset, readsize / 64, cache, acc_index, files[files_count].Name);// Process block
						start_time_proc = clock();
						procscoop(n + nonce + noffset, readsize / 64, cache, acc_index, files[files_count].Name);// Process block
						avg_time_proc += (double)(clock() - start_time_proc);
						
						pthread_mutex_lock(&byteLock);
						bytesRead += readsize;
						pthread_mutex_unlock(&byteLock);
					}
					else 
					{
						SetConsoleTextAttribute(hConsole, 12);
						printf("\nUnexpected end of file %s\n", files[files_count].Name.c_str());
						SetConsoleTextAttribute(hConsole, 7);
					}
					noffset += cache_size_lacal;

					if (stopThreads) // New block while processing: Stop.
					{
						working_threads--;
						Log("\nReading file: ");	Log((char*)files[files_count].Name.c_str()); Log(" interrupted");
						fclose(pFile);
						files.clear();
						free(cache);
						if (use_boost) SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);
						Sleep(0);
						return 0;
					}
				}
			}
			end_time_read = clock();
			Log("\nClose file: ");	Log((char*)files[files_count].Name.c_str()); Log(" [@ "); Log_llu(end_time_read-start_time_read); Log(" ms]");
			fclose(pFile);		
			free(cache);
		}
		end_work_time = clock();

		if (use_boost) SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);

		double thread_time = (double)(end_work_time - start_work_time) / CLOCKS_PER_SEC;
		if (use_debug)
		{
			cls();
			char tbuffer[9];
			_strtime(tbuffer);
			SetConsoleTextAttribute(hConsole, 7);
			printf_s("\r%s Thread \"%s\" in %.1f sec (%.1f MB/s) CPU %.2f%%\n", tbuffer, path_loc_str.c_str(), thread_time, (double)(files_size_per_thread) / thread_time / 1024 / 1024 / 4096, avg_time_proc * 100 / CLOCKS_PER_SEC / thread_time);
		}
		working_threads--;
		thread_times.push_back(thread_time);
		files.clear();
		Sleep(0);
}


/*
void *work_i(void* path_str) {
pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
//pthread_self();
DWORD_PTR ret = stick_thread_to_core(working_threads);
std::string path_loc_str((char*)path_str);

unsigned long long files_size_per_thread = 0;
size_t cache_size_lacal;
clock_t start_work_time, end_work_time;
clock_t start_time_read, end_time_read;			// Текущее время
start_work_time = clock();
Log("\nStart thread: ");	Log((char*)path_loc_str.c_str()); //Log(paths_dir[local_num]);

double avg_time_proc = 0;
clock_t start_time_proc;

FILE * pFile;
std::vector<t_files> files;
size_t f_count = GetFiles(path_loc_str, &files);
size_t files_count = 0;
size_t acc_index;
unsigned long long  bytes_per_read;
for(; files_count < f_count; files_count++)
{
unsigned long long key, nonce, nonces, stagger;
start_time_read = clock();
sscanf_s(files[files_count].Name.c_str(), "%llu_%llu_%llu_%llu", &key , &nonce, &nonces, &stagger);
if ((float)(nonces % stagger) != 0)
{
cls();
SetConsoleTextAttribute(hConsole, 12);
printf("\rFile %s wrong stagger?\n", files[files_count].Name.c_str());
SetConsoleTextAttribute(hConsole, 7);
}

if (nonces != stagger)
{
cache_size_lacal = stagger;
bytes_per_read = 64 * stagger;
}
else
{
cache_size_lacal = cache_size;
bytes_per_read = 64;
}
char *cache = (char*)calloc(cache_size_lacal * 64, 1);
if (cache == NULL) {
cls();
SetConsoleTextAttribute(hConsole, 12);
printf_s("\nError allocating memory\n");
SetConsoleTextAttribute(hConsole, 7);
exit(-1);
}

if (nonces != (files[files_count].Size) / (4096 * 64)) // Проверка на повреждения плота
{
SetConsoleTextAttribute(hConsole, 12);
cls();
printf_s("\rfile \"%s\" name/size mismatch\n", files[files_count].Name.c_str());
SetConsoleTextAttribute(hConsole, 7);
if (nonces != stagger)
nonces = (((files[files_count].Size) / (4096 * 64)) / stagger) * stagger; //обрезаем плот по размеру и стаггеру
else
if (scoop > (files[files_count].Size) / (stagger * 64)) //если номер скупа попадает в поврежденный смерженный плот, то все нормально
{
SetConsoleTextAttribute(hConsole, 12);
cls();
printf_s("\rskipped\n");
SetConsoleTextAttribute(hConsole, 7);
continue;
}
}

Log("\nRead file: ");	Log((char*)files[files_count].Name.c_str());

_set_fmode(_O_BINARY);
fopen_s(&pFile, (files[files_count].Path + files[files_count].Name).c_str(), "rb");
if (pFile==NULL)
{
SetConsoleTextAttribute(hConsole, 12);
cls();
printf_s("\rfile \"%s\" error opening\n", files[files_count].Name.c_str());
SetConsoleTextAttribute(hConsole, 7);
continue;
}
files_size_per_thread += files[files_count].Size;
acc_index = Get_index_acc(key);
//			Log("\nGet_index_acc = "); Log_u(acc_index);
unsigned long long start, noffset, i, readsize, bytes;

for (unsigned long long n = 0; n<nonces; n += stagger)
{
start = n * 4096 * 64 + scoop * stagger*64;
noffset = 0;

for (i = start; i < start + stagger * 64; i += cache_size_lacal * 64)
{

readsize = cache_size_lacal * 64;
if (readsize > start + stagger*64 - i)	readsize = start + stagger*64 - i;
bytes = 0;

_fseeki64_nolock(pFile , i , SEEK_SET);

do {
//b = _fread_nolock_s(&cache[bytes], cache_size * 64, sizeof(char), readsize - bytes, pFile);
bytes += bytes_per_read * _fread_nolock_s(&cache[bytes], cache_size_lacal * 64, bytes_per_read, readsize / bytes_per_read, pFile);
} while ((bytes < readsize) && !feof(pFile));

if (bytes == readsize)
{
//procscoop4(n + nonce + noffset, readsize / 64, cache, acc_index, files[files_count].Name);// Process block
start_time_proc = clock();
procscoop(n + nonce + noffset, readsize / 64, cache, acc_index, files[files_count].Name);// Process block
avg_time_proc += (double)(clock() - start_time_proc);

pthread_mutex_lock(&byteLock);
bytesRead += readsize;
pthread_mutex_unlock(&byteLock);
}
else
{
SetConsoleTextAttribute(hConsole, 12);
printf("\nUnexpected end of file %s\n", files[files_count].Name.c_str());
SetConsoleTextAttribute(hConsole, 7);
}
noffset += cache_size_lacal;

if (stopThreads) // New block while processing: Stop.
{
working_threads--;
Log("\nReading file: ");	Log((char*)files[files_count].Name.c_str()); Log(" interrupted");
fclose(pFile);
files.clear();
free(cache);
return 0;
}
}
}
end_time_read = clock();
Log("\nClose file: ");	Log((char*)files[files_count].Name.c_str()); Log(" [@ "); Log_llu(end_time_read-start_time_read); Log(" ms]");
fclose(pFile);
free(cache);
}
end_work_time = clock();
double thread_time = (double)(end_work_time - start_work_time) / CLOCKS_PER_SEC;
if (use_debug)
{
cls();
char tbuffer[9];
_strtime(tbuffer);
SetConsoleTextAttribute(hConsole, 7);
printf_s("\r%s Thread \"%s\" in %.1f sec (%.1f MB/s) CPU %.2f%%  %llu\n", tbuffer, path_loc_str.c_str(), thread_time, (double)(files_size_per_thread) / thread_time / 1024 / 1024 / 4096, avg_time_proc * 100 / CLOCKS_PER_SEC / thread_time, ret);
}
working_threads--;
thread_times.push_back(thread_time);
files.clear();
}

*/

void GetJSON(char* req) {
	unsigned BUF_SIZE = 1024;
	char *buffer = (char*)calloc(BUF_SIZE, 1);
	if (buffer == NULL) {
		cls();
		SetConsoleTextAttribute(hConsole, 12);
		printf_s("\nError allocating memory\n");
		SetConsoleTextAttribute(hConsole, 7);
		exit(-1);
	}
	
	char *find = NULL;
	unsigned long long msg_len = 0;
	int iResult;
	struct addrinfo *result = NULL;
	struct addrinfo hints;
	SOCKET WalletSocket = INVALID_SOCKET;

	json = NULL;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET; //AF_UNSPEC;  // использовать IPv4 или IPv6, нам неважно
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;
	char iport[6];
	_itoa((int)infoport, iport, 10);
	iResult = getaddrinfo(infoaddr, iport, &hints, &result);
	if (iResult != 0) {
		cls();
		SetConsoleTextAttribute(hConsole, 12);
		printf_s("\rWinner. Getaddrinfo failed with error: %d\n", iResult);
		SetConsoleTextAttribute(hConsole, 7);
		Log("\nWinner: getaddrinfo error: ");
	}
	else 
	{
		WalletSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
		if (WalletSocket == INVALID_SOCKET)
		{
			cls();
			SetConsoleTextAttribute(hConsole, 12);
			printf_s("\rWinner. Socket function failed with error: %ld\n", WSAGetLastError());
			SetConsoleTextAttribute(hConsole, 7);
			Log("\nWinner: Socket error: "); Log(DisplayErrorText(WSAGetLastError()));
		}
		else 
		{
			unsigned t = 3000;
			setsockopt(WalletSocket, SOL_SOCKET, SO_RCVTIMEO, (char *)&t, sizeof(unsigned));
			//Log("\n-Connecting to server: "); Log(updaterip); Log(":"); Log_u(updaterport);
			iResult = connect(WalletSocket, result->ai_addr, (int)result->ai_addrlen);
			if (iResult == SOCKET_ERROR) {
				cls();
				SetConsoleTextAttribute(hConsole, 12);
				printf_s("\rWinner. Connect function failed with error: %ld\n", WSAGetLastError());
				SetConsoleTextAttribute(hConsole, 7);
				Log("\nWinner: Connect server error "); Log(DisplayErrorText(WSAGetLastError()));
			}
			else 
			{
				iResult = send(WalletSocket, req, (int)strlen(req), 0);
				if (iResult == SOCKET_ERROR)
				{
					cls();
					SetConsoleTextAttribute(hConsole, 12);
					printf_s("\rWinner. Send request failed: %ld\n", WSAGetLastError());
					SetConsoleTextAttribute(hConsole, 7);
					Log("\nWinner: Error sending request: "); Log(DisplayErrorText(WSAGetLastError())); Log("Errors: "); Log_llu(err_send_msg);
				}
				else
				{
					char *tmp_buffer = (char*)calloc(BUF_SIZE, 1);
					if (tmp_buffer == NULL) {
						cls();
						SetConsoleTextAttribute(hConsole, 12);
						printf_s("\nError allocating memory\n");
						SetConsoleTextAttribute(hConsole, 7);
						exit(-1);
					}
					
						do{
							memset(tmp_buffer, 0, BUF_SIZE);
							iResult = recv(WalletSocket, tmp_buffer, BUF_SIZE - 1, 0);
							if (iResult > 0)
							{
								msg_len = msg_len + iResult;
								if (msg_len >= BUF_SIZE - 1)
								{
									Log("\nrealloc: ");
									char* tmp_buffer = buffer;
									if ((buffer = (char*)realloc(buffer, msg_len)) == NULL)
									{
										Log(" ERROR!");
										buffer = tmp_buffer;
										break;
									}
									else
									{
										buffer[msg_len - iResult] = 0;
										Log_llu(msg_len);
									}
								}
								strcat(buffer, tmp_buffer);
							}
						} while (iResult > 0);
						free(tmp_buffer);

						if (iResult == SOCKET_ERROR)
						{
							cls();
							SetConsoleTextAttribute(hConsole, 12);
							printf_s("\rWinner. Get info failed: %ld\n", WSAGetLastError());
							SetConsoleTextAttribute(hConsole, 7);
							Log("\nWinner: Error response: "); Log(DisplayErrorText(WSAGetLastError()));
						}
						else
						{
							find = strstr(buffer, "\r\n\r\n");
							if (find != NULL)
							{
								json = (char*)calloc((msg_len), sizeof(char));
								if (json == NULL) {
									cls();
									SetConsoleTextAttribute(hConsole, 12);
									printf_s("\nError allocating memory\n");
									SetConsoleTextAttribute(hConsole, 7);
									exit(-1);
								}
								sprintf(json, "%s", find + 4);
							}

						} // recv() != SOCKET_ERROR
					
				} //send() != SOCKET_ERROR
			} // Connect() != SOCKET_ERROR
		} // socket() != INVALID_SOCKET
		iResult = closesocket(WalletSocket);
	} // getaddrinfo() == 0
	freeaddrinfo(result);
}

void GetBlockInfo(unsigned num_block) {
	pthread_mutex_lock(&connectLock);
	can_connect = 0;
	pthread_mutex_unlock(&connectLock);

	char* str_req = NULL;
	char* generator = NULL;
	char* generatorRS = NULL;
	unsigned long long last_block_height = 0;
	char* name = NULL;
	char* rewardRecipient = NULL;
	char* pool_accountRS = NULL;
	char* pool_name = NULL;
	unsigned long long timestamp0 = 0;
	unsigned long long timestamp1 = 0;
	unsigned req_size = 255;
	// Запрос двух последних блоков из блокчейна

	str_req = (char*)calloc(req_size, 1);
	if (str_req == NULL) {
		cls();
		SetConsoleTextAttribute(hConsole, 12);
		printf_s("\nError allocating memory\n");
		SetConsoleTextAttribute(hConsole, 7);
		exit(-1);
	}
	sprintf(str_req, "POST /burst?requestType=getBlocks&firstIndex=%u&lastIndex=%u HTTP/1.0\r\nConnection: close\r\n\r\n", num_block, num_block + 1);
	GetJSON(str_req);
	Log("\n getBlocks: ");
	//Log(json);
	if (json == NULL)
	{
		Log("\n-! error in message from pool (getBlocks)\n");
	}
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
				generatorRS = (char*)calloc(strlen(bl_0["generatorRS"].GetString()) + 1, 1);
				if (generatorRS == NULL) {
					cls();
					SetConsoleTextAttribute(hConsole, 12);
					printf_s("\nError allocating memory\n");
					SetConsoleTextAttribute(hConsole, 7);
					exit(-1);
				}
				strcpy(generatorRS, bl_0["generatorRS"].GetString());
				generator = (char*)calloc(strlen(bl_0["generator"].GetString()) + 1, 1);
				if (generator == NULL) {
					cls();
					SetConsoleTextAttribute(hConsole, 12);
					printf_s("\nError allocating memory\n");
					SetConsoleTextAttribute(hConsole, 7);
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
	if (json != NULL) free(json);
	if ((generator != NULL) && (generatorRS != NULL) && (timestamp0 != 0) && (timestamp1 != 0))
	if (last_block_height == height - 1)
	{
		// Запрос данных аккаунта

		str_req = (char*)calloc(req_size, 1);
		if (str_req == NULL) {
			cls();
			SetConsoleTextAttribute(hConsole, 12);
			printf_s("\nError allocating memory\n");
			SetConsoleTextAttribute(hConsole, 7);
			exit(-1);
		}
		sprintf(str_req, "POST /burst?requestType=getAccount&account=%s HTTP/1.0\r\nConnection: close\r\n\r\n", generator);
		GetJSON(str_req);
		Log("\n getAccount: ");
		//Log(json);
		if (json == NULL)
		{
			Log("\n- error in message from pool (getAccount)\n");
		}
		else
		{
			rapidjson::Document doc_acc;
			if (doc_acc.Parse<0>(json).HasParseError() == false)
			{
				if (doc_acc.HasMember("name"))
				{
					name = (char*)calloc(strlen(doc_acc["name"].GetString()) + 1, 1);
					if (name == NULL) {
						cls();
						SetConsoleTextAttribute(hConsole, 12);
						printf_s("\nError allocating memory\n");
						SetConsoleTextAttribute(hConsole, 7);
						exit(-1);
					}
					strcpy(name, doc_acc["name"].GetString());
				}
			}
			else Log("\n- error parsing JSON getAccount");
		}
		free(str_req);
		if (json != NULL) free(json);

		// Запрос RewardAssighnment по данному аккаунту

		str_req = (char*)calloc(req_size, 1);
		if (str_req == NULL) {
			cls();
			SetConsoleTextAttribute(hConsole, 12);
			printf_s("\nError allocating memory\n");
			SetConsoleTextAttribute(hConsole, 7);
			exit(-1);
		}
		sprintf(str_req, "POST /burst?requestType=getRewardRecipient&account=%s HTTP/1.0\r\nConnection: close\r\n\r\n", generator);
		GetJSON(str_req);

		Log("\n getRewardRecipient: ");
		//Log(json);
		if (json == NULL)
		{
			Log("\n- error in message from pool (getRewardRecipient)\n");
		}
		else
		{
			rapidjson::Document doc_reward;
			if (doc_reward.Parse<0>(json).HasParseError() == false)
			{
				if (doc_reward.HasMember("rewardRecipient"))
				{
					rewardRecipient = (char*)calloc(strlen(doc_reward["rewardRecipient"].GetString()) + 1, 1);
					if (rewardRecipient == NULL) {
						cls();
						SetConsoleTextAttribute(hConsole, 12);
						printf_s("\nError allocating memory\n");
						SetConsoleTextAttribute(hConsole, 7);
						exit(-1);
					}
					strcpy(rewardRecipient, doc_reward["rewardRecipient"].GetString());
				}
			}
			else Log("\n-! error parsing JSON getRewardRecipient");
		}
		free(str_req);
		if (json != NULL) free(json);
		//Log(rewardRecipient);

		if (rewardRecipient != NULL)
		{
			// Если rewardRecipient != generator, то майнит на пул, узнаём имя пула...
			if (strcmp(generator, rewardRecipient) != 0)
			{
				// Запрос данных аккаунта пула
				str_req = (char*)calloc(req_size, 1);
				if (str_req == NULL) {
					cls();
					SetConsoleTextAttribute(hConsole, 12);
					printf_s("\nError allocating memory\n");
					SetConsoleTextAttribute(hConsole, 7);
					exit(-1);
				}
				sprintf(str_req, "POST /burst?requestType=getAccount&account=%s HTTP/1.0\r\nConnection: close\r\n\r\n", rewardRecipient);
				GetJSON(str_req);
				Log("\n getAccount: ");
				//Log(json);
				if (json == NULL)
				{
					Log("\n- error in message from pool (pool getAccount)\n");
				}
				else
				{
					rapidjson::Document doc_pool;
					if (doc_pool.Parse<0>(json).HasParseError() == false)
					{
						pool_accountRS = (char*)calloc(strlen(doc_pool["accountRS"].GetString()) + 1, 1);
						if (pool_accountRS == NULL) {
							cls();
							SetConsoleTextAttribute(hConsole, 12);
							printf_s("\nError allocating memory\n");
							SetConsoleTextAttribute(hConsole, 7);
							exit(-1);
						}
						strcpy(pool_accountRS, doc_pool["accountRS"].GetString());
						if (doc_pool.HasMember("name"))
						{
							pool_name = (char*)calloc(strlen(doc_pool["name"].GetString()) + 1, 1);
							if (pool_name == NULL) {
								cls();
								SetConsoleTextAttribute(hConsole, 12);
								printf_s("\nError allocating memory\n");
								SetConsoleTextAttribute(hConsole, 7);
								exit(-1);
							}
							strcpy(pool_name, doc_pool["name"].GetString());
						}
					}
					else Log("\n- error parsing JSON pool getAccount");
				}
				free(str_req);
				if (json != NULL) free(json);
			}
		}


		SetConsoleTextAttribute(hConsole, 11);
		if (name != NULL) printf_s("\r\nWinner: %llus by %s (%s)", timestamp0 - timestamp1, generatorRS + 6, name);
		else printf_s("\r\nWinner: %llus by %s", timestamp0 - timestamp1, generatorRS + 6);
		if (pool_accountRS != NULL)
		{
			if (pool_name != NULL) printf_s("\r\nWinner's pool: %s (%s)", pool_accountRS + 6, pool_name);
			else printf_s("\r\nWinner's pool: %s", pool_accountRS + 6);
		}
		SetConsoleTextAttribute(hConsole, 7);
	}
	else
	{
		SetConsoleTextAttribute(hConsole, 11);
		printf_s("\r\nWinner: no info yet");
		SetConsoleTextAttribute(hConsole, 7);
	}
	pthread_mutex_lock(&connectLock);
	can_connect = 1;
	pthread_mutex_unlock(&connectLock);
	free(generatorRS);
	free(generator);
	free(name);
	free(rewardRecipient);
	free(pool_name);
	free(pool_accountRS);
}


void pollLocal(void) {
	char *buffer = (char*)calloc(1000, 1);
	if (buffer == NULL) {
		cls();
		SetConsoleTextAttribute(hConsole, 12);
		printf_s("\nError allocating memory\n");
		SetConsoleTextAttribute(hConsole, 7);
		exit(-1);
	}
	char *tmp_buffer = (char*)calloc(1000, 1);
	if (tmp_buffer == NULL) {
		cls();
		SetConsoleTextAttribute(hConsole, 12);
		printf_s("\nError allocating memory\n");
		SetConsoleTextAttribute(hConsole, 7);
		exit(-1);
	}
	int iResult;
	struct addrinfo *result = NULL;
	struct addrinfo hints;
	SOCKET UpdaterSocket = INVALID_SOCKET;

	pthread_mutex_lock(&connectLock);
	can_connect = 0;
	pthread_mutex_unlock(&connectLock);

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET; //AF_UNSPEC;  // использовать IPv4 или IPv6, нам неважно
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;
	char uport[6];
	_itoa((int)updaterport, uport, 10);
	iResult = getaddrinfo(updateraddr, uport, &hints, &result);
	if (iResult != 0) {
		cls();
		printf("\rgetaddrinfo failed with error: %d\n", iResult);
	}
	else {
		// Get Mininginfo from node:
		UpdaterSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
		if (UpdaterSocket == INVALID_SOCKET)
		{
			cls();
			printf_s("\rsocket function failed with error: %ld\n", WSAGetLastError());
			Log("\n*! Socket error: "); Log(DisplayErrorText(WSAGetLastError()));
		}
		else {
				unsigned t = 60000;
				setsockopt(UpdaterSocket, SOL_SOCKET, SO_RCVTIMEO, (char *)&t, sizeof(unsigned));
				Log("\n*Connecting to server: "); Log(updateraddr); Log(":"); Log_u(updaterport);
			iResult = connect(UpdaterSocket, result->ai_addr, (int)result->ai_addrlen);
			if (iResult == SOCKET_ERROR) {
				cls();
				printf_s("\rconnect function failed with error: %ld\n", WSAGetLastError());
				Log("\n*! Connect server error "); Log(DisplayErrorText(WSAGetLastError()));
			}
			else {
				// wrire some bytes
				all_send_msg++;
				if (all_send_msg >= 1000) all_send_msg = 0;
				int bytes;
				if (miner_mode == 2) bytes = sprintf_s(buffer, _msize(buffer), "GET /pool/getMiningInfo HTTP/1.0\r\nHost: %s:%llu\r\nContent-Type: text/plain;charset=UTF-8\r\n\r\n", nodeaddr, nodeport);
				else bytes = sprintf_s(buffer, _msize(buffer), "POST /burst?requestType=getMiningInfo HTTP/1.0\r\nConnection: close\r\n\r\n");

				iResult = send(UpdaterSocket, buffer, bytes, 0);
				if (iResult == SOCKET_ERROR)
				{
					err_send_msg++;
					Log("\n*! Error sending request: "); Log(DisplayErrorText(WSAGetLastError())); Log("Errors: "); Log_llu(err_send_msg);
					cls();
					printf_s("\rsend request failed: %ld\n", WSAGetLastError());
				}
				else{
					if (show_updates) printf_s("\nSent: \n%s\n", buffer);
					Log("\n*Sent to server: "); Log_server(buffer);

					// read some bytes
					all_rcv_msg++;
					if (all_rcv_msg >= 1000) all_rcv_msg = 0;

					memset(buffer, 0, 1000);
					size_t resp = 0;
					do{
						memset(tmp_buffer, 0, 1000);
						iResult = recv(UpdaterSocket, tmp_buffer, 999, 0);
						strcat(buffer, tmp_buffer);
						resp++;
					} while ((iResult > 0) && !use_fast_rcv);

					if (iResult == SOCKET_ERROR)
					{
						err_rcv_msg++;
						Log("\n*! Error response: "); Log(DisplayErrorText(WSAGetLastError())); Log("Errors: "); Log_llu(err_rcv_msg);
						cls();
						printf_s("\rget mining info failed: %ld\n", WSAGetLastError());
					}
					else {
						Log("\n*Received from server: "); Log_server(buffer); Log("\n*Count packets of response: "); Log_u(resp);
						if (show_updates)  printf_s("\nReceived: \n%s\n", buffer);

						// locate HTTP header
						char *find = strstr(buffer, "\r\n\r\n");
						if (find == NULL)
						{
							Log("\n*! error in message from pool\n");
						}
						else {
							char *rbaseTarget = strstr(buffer, "\"baseTarget\":");
							char *rheight = strstr(buffer, "\"height\":");
							char *generationSignature = strstr(buffer, "\"generationSignature\":");
							if (generationSignature != NULL) generationSignature = strstr(generationSignature + strlen("\"generationSignature\":"), "\"") + 1; //+1 убираем начальные ковычки
							char* rtargetDeadline = strstr(buffer, "\"targetDeadline\"");

							if (rbaseTarget == NULL || rheight == NULL || generationSignature == NULL)
							{
								Log("\n*! error parsing (1) message from server");
							}
							else {
								rbaseTarget = strpbrk(rbaseTarget, "0123456789");
								char *endBaseTarget = strpbrk(rbaseTarget, "\"");
								rheight = strpbrk(rheight, "0123456789");
								char *endHeight = strpbrk(rheight, "\"");
								char *endGenerationSignature = strstr(generationSignature, "\"");

								if (endBaseTarget == NULL || endHeight == NULL || endGenerationSignature == NULL){
									Log("\n*! error parsing (2) message from server");
								}
								else {
									// Set endpoints
									endBaseTarget[0] = 0;
									endHeight[0] = 0;
									endGenerationSignature[0] = 0;

									if (rtargetDeadline != NULL) 
									{
										rtargetDeadline = strpbrk(rtargetDeadline, "0123456789");
										char* endBaseTarget = strpbrk(rtargetDeadline, "\"");
										if (endBaseTarget == NULL) endBaseTarget = strpbrk(rtargetDeadline, "}");
										endBaseTarget[0] = 0;

										targetDeadlineInfo = _strtoui64(rtargetDeadline, 0, 10);
										Log("\ntargetDeadlineInfo: "), Log_llu(targetDeadlineInfo);
									}

									unsigned long long  heightInfo = _strtoui64(rheight, 0, 10);
									//if (heightInfo > height)
									{
										height = heightInfo;
										baseTarget = _strtoui64(rbaseTarget, 0, 10);
										// Parse
										strcpy(str_signature, generationSignature);
										//str_signature[65] = 0;
										if (xstr2strr(signature, 33, generationSignature) == 0) 
											Log("\n*! Node response: Error decoding generationsignature\n");
									}
								}
							}
						}
					}
				}
			}
			//printf("\rblock: %llu  BT: %llu   %s\n", height, baseTarget, signature);
			iResult = closesocket(UpdaterSocket);
		}
		freeaddrinfo(result);
	}
	pthread_mutex_lock(&connectLock);
	can_connect = 1;
	pthread_mutex_unlock(&connectLock);
	
	free(buffer);
	free(tmp_buffer);
}


void *updater_i(void * path) {
	if ((updaterport > 0) && (strlen(updateraddr) > 3))
	{
		do{
			if (can_connect == 1) pollLocal();
			Sleep((DWORD)update_interval);
		} while (true);
	}
	Log("\nERROR in UpdaterAddr or UpdaterPort");
	exit(2);
}
 

void hostname_to_ip(char * in, char* out)
{
    struct hostent *remoteHost = NULL;
    struct in_addr addr;
    size_t i = 0;
	

	if ((remoteHost = gethostbyname(in)) != NULL)
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
		bool bIsSSE4Available = InstructionSet::SSE42();
		bool bIsAVX1Available = InstructionSet::AVX();
		bool bIsAVX2Available = InstructionSet::AVX2();
		bool bIsAESAvailable = InstructionSet::AES();
		ULONGLONG  TotalMemoryInKilobytes = 0;
		SetConsoleTextAttribute(hConsole, 15);
		printf_s("\rCPU support: ");
		if (bIsAESAvailable)    SetConsoleTextAttribute(hConsole, 0xA0);
		else SetConsoleTextAttribute(hConsole, 0xC0);
		printf_s(" AES ");
		if (bIsSSE4Available)    SetConsoleTextAttribute(hConsole, 0xA0);
		else SetConsoleTextAttribute(hConsole, 0xC0);
		printf_s(" SSE4 ");
		if (bIsAVX1Available)    SetConsoleTextAttribute(hConsole, 0xA0);
		else SetConsoleTextAttribute(hConsole, 0xC0);
		printf_s(" AVX ");
		if (bIsAVX2Available)    SetConsoleTextAttribute(hConsole, 0xA0);
		else SetConsoleTextAttribute(hConsole, 0xC0);
		printf_s(" AVX2 ");
		SetConsoleTextAttribute(hConsole, 7);
		
		SYSTEM_INFO sysinfo;
		GetSystemInfo(&sysinfo);
		printf("\n%s", InstructionSet::Vendor().c_str());
		printf(" %s  [%u cores]", InstructionSet::Brand().c_str(), sysinfo.dwNumberOfProcessors);

		if (GetPhysicallyInstalledSystemMemory(&TotalMemoryInKilobytes))
			printf_s("\nRAM: %llu Mb", (unsigned long long)TotalMemoryInKilobytes / 1024);
		
		printf_s("\n");
}


NTSTATUS ClearMem(void)
{
	
		HANDLE hProcess = GetCurrentProcess();
		HANDLE hToken;
		if (!OpenProcessToken(hProcess, TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES, &hToken))
		{
			fprintf(stderr, "\nCan't open current process token\n");
			return 0;
		}

		if (!GetPrivilege(hToken, "SeProfileSingleProcessPrivilege", 1))
		{
			fprintf(stderr, "\nCan't get SeProfileSingleProcessPrivilege\n");
			return 0;
		}

		CloseHandle(hToken);

		HMODULE ntdll = LoadLibrary(L"ntdll.dll");
		if (!ntdll)
		{
			fprintf(stderr, "\nCan't load ntdll.dll, wrong Windows version?\n");
			return 0;
		}

		typedef DWORD NTSTATUS; // ?
		NTSTATUS(WINAPI *NtSetSystemInformation)(INT, PVOID, ULONG) = (NTSTATUS(WINAPI *)(INT, PVOID, ULONG))GetProcAddress(ntdll, "NtSetSystemInformation");
		NTSTATUS(WINAPI *NtQuerySystemInformation)(INT, PVOID, ULONG, PULONG) = (NTSTATUS(WINAPI *)(INT, PVOID, ULONG, PULONG))GetProcAddress(ntdll, "NtQuerySystemInformation");
		if (!NtSetSystemInformation || !NtQuerySystemInformation)
		{
			printf_s("\nCan't get function addresses, wrong Windows version?\n");
			return 0;
		}

		SYSTEM_MEMORY_LIST_COMMAND command = MemoryPurgeStandbyList;
		NTSTATUS status = NtSetSystemInformation(
			SystemMemoryListInformation,
			&command,
			sizeof(SYSTEM_MEMORY_LIST_COMMAND)
			);
		if (status == STATUS_PRIVILEGE_NOT_HELD)
		{
			printf_s("\nInsufficient privileges to execute the memory list command\n");
		}
		else if (status > 0)
		{
			printf_s("\nUnable to execute the memory list command %x\n", status);
		}
		return status;
	
}

#ifdef GPU_ON
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

int GPU1(void)
{
	cl_platform_id platform;
	cl_device_id device;
	cl_context context;
	cl_program program;
	cl_int error;
	cl_build_status status;

	FILE* programHandle;
	char *programBuffer; char *programLog;
	size_t programSize; size_t logSize;

	// get first available platform and gpu and create context
	clGetPlatformIDs(1, &platform, NULL);
	clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
	context = clCreateContext(NULL, 1, &device, NULL, NULL, NULL);

	// get size of kernel source
	programHandle = fopen("kernel\\nonce.cl", "r");
	fseek(programHandle, 0, SEEK_END);
	programSize = ftell(programHandle);
	rewind(programHandle);

	// read kernel source into buffer
	programBuffer = (char*)malloc(programSize + 1);
	programBuffer[programSize] = '\0';
	fread(programBuffer, sizeof(char), programSize, programHandle);
	fclose(programHandle);

	// create program from buffer
	
	program = clCreateProgramWithSource(context, 1,	(const char**)&programBuffer, &programSize, NULL);
	free(programBuffer);

	// build program
	const char options[] = "-Werror";
	error = clBuildProgram(program, 1, &device, options, NULL, NULL);

	// build failed
	if (error != CL_SUCCESS) {

		// check build error and build status first
		clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_STATUS,	sizeof(cl_build_status), &status, NULL);

		// check build log
		clGetProgramBuildInfo(program, device,	CL_PROGRAM_BUILD_LOG, 0, NULL, &logSize);
		programLog = (char*)calloc(logSize + 1, sizeof(char));
		clGetProgramBuildInfo(program, device,	CL_PROGRAM_BUILD_LOG, logSize + 1, programLog, NULL);
		printf("Build failed: error=%d, status=%d, programLog:\n%s", error, status, programLog);
		free(programLog);
	}

	clReleaseContext(context);
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
		//clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &logSize);
		//programLog = (char*)calloc(logSize + 1, sizeof(char));
		//clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, logSize + 1, programLog, NULL);
		printf("Build failed: error=%d, programLog:\n%s", err, programLog);
		//free(programLog);


		printf("\nkernel");
		cl::Kernel kernel(program_, "nonce_step3");
		



		//cl::Event event;
		//cl::CommandQueue queue(context, devices[0], 0, &err);
		//queue.enqueueNDRangeKernel(
		//	kernel,
		//	cl::NullRange,
		//	cl::NDRange(4, 4),
		//	cl::NullRange,
		//	NULL,
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
int main(int argc, char **argv) {

	std::vector<pthread_t> worker;
	pthread_t sender;
	pthread_t proxy;
	pthread_t updater;
	size_t i = 0;
	char tbuffer[9];
	bool cleared = false;

	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	//SetProcessAffinityMask(GetCurrentProcess(), 1);
	shares.reserve(20);
	bests.reserve(2);

	SetConsoleTextAttribute(hConsole, 12);
	printf_s("\nBURST miner, %s", version);
	SetConsoleTextAttribute(hConsole, 4);
	printf_s("\nProgramming: dcct (Linux) & Blago (Windows)\n");
	SetConsoleTextAttribute(hConsole, 7);

	//		GPU();


	size_t cwdsz = GetCurrentDirectoryA(0, 0);
	p_minerPath = (char*)calloc(cwdsz + 2, 1);
	if (p_minerPath == NULL) {
		cls();
		SetConsoleTextAttribute(hConsole, 12);
		printf_s("\nError allocating memory\n");
		SetConsoleTextAttribute(hConsole, 7);
		exit(-1);
	}
	GetCurrentDirectoryA(DWORD(cwdsz), LPSTR(p_minerPath));
	strcat(p_minerPath, "\\");

	char* conf_filename = (char*)calloc(MAX_PATH, 1);
	if (conf_filename == NULL) {
		cls();
		SetConsoleTextAttribute(hConsole, 12);
		printf_s("\nError allocating memory\n");
		SetConsoleTextAttribute(hConsole, 7);
		exit(-1);
	}
	if ((argc >= 2) && (strcmp(argv[1], "-config") == 0)){
		if (strstr(argv[2], ":\\")) sprintf(conf_filename, "%s", argv[2]);
		else sprintf(conf_filename, "%s%s", p_minerPath, argv[2]);
	}
	else sprintf(conf_filename, "%s%s", p_minerPath, "miner.conf");

	load_config(conf_filename);
	free(conf_filename);
	Log("\nMiner path: "); Log(p_minerPath);

	GetCPUInfo();

	if (miner_mode == 0) GetPass(p_minerPath);

	// адрес и порт сервера
	Log("\nSearching servers...");
	WSADATA wsaData;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		printf_s("WSAStartup failed\n");
		exit(-1);
	}

	char* updaterip = (char*)calloc(50, 1);
	if (updaterip == NULL) {
		cls();
		SetConsoleTextAttribute(hConsole, 12);
		printf_s("\nError allocating memory\n");
		SetConsoleTextAttribute(hConsole, 7);
		exit(-1);
	}
	char* nodeip = (char*)calloc(50, 1);
	if (nodeip == NULL) {
		cls();
		SetConsoleTextAttribute(hConsole, 12);
		printf_s("\nError allocating memory\n");
		SetConsoleTextAttribute(hConsole, 7);
		exit(-1);
	}
	SetConsoleTextAttribute(hConsole, 11);
	hostname_to_ip(nodeaddr, nodeip);
	printf_s("Pool address    %s (ip %s:%Iu)\n", nodeaddr, nodeip, nodeport);

	if (strlen(updateraddr) > 3) hostname_to_ip(updateraddr, updaterip);
	printf_s("Updater address %s (ip %s:%Iu)\n", updateraddr, updaterip, updaterport);

	SetConsoleTextAttribute(hConsole, 7);
	free(updaterip);
	free(nodeip);


	// обнуляем сигнатуру
	memset(oldSignature, 0, 33);
	memset(signature, 0, 33);

	// Инфа по файлам
	SetConsoleTextAttribute(hConsole, 15);
	printf_s("Using plots:\n");
	SetConsoleTextAttribute(hConsole, 7);
	total_size = 0;
	for (i = 0; i < paths_num; i++)
	{
		std::vector<t_files> files;
		size_t count = GetFiles(paths_dir[i], &files);
		unsigned long long tot_size = 0;
		for (size_t j = 0; j < count; j++) 	tot_size += files[j].Size;
		SetConsoleTextAttribute(hConsole, 7);
		printf_s("%s\tfiles: %u\t size: %llu Gb\n", (char*)paths_dir[i].c_str(), count, tot_size / 1024 / 1024 / 1024);
		total_size += tot_size;
		files.clear();
	}
	
	

	SetConsoleTextAttribute(hConsole, 15);
	printf_s("TOTAL: %llu Gb", total_size / 1024 / 1024 / 1024);
	SetConsoleTextAttribute(hConsole, 7);

	// Run Proxy
	if (enable_proxy)
	{
		if (pthread_create(&proxy, NULL, proxy_i, p_minerPath))
		{
			SetConsoleTextAttribute(hConsole, 12);
			printf_s("\nError creating thread. Out of memory?\n");
			SetConsoleTextAttribute(hConsole, 7);
			exit(-1);
		}
		SetConsoleTextAttribute(hConsole, 0x9F);
		printf_s("\nProxy thread started\n");
		SetConsoleTextAttribute(hConsole, 7);
	}

	Log("\nUpdate mining info");

	// Run updater;
	if (pthread_create(&updater, NULL, updater_i, p_minerPath))
	{
		SetConsoleTextAttribute(hConsole, 12);
		printf_s("\nError creating thread. Out of memory?\n");
		SetConsoleTextAttribute(hConsole, 7);
		exit(-1);
	}
	Log("\nUpdater thread started");

	do{ ; } while (height == 0);

	// Run Sender
	if (pthread_create(&sender, NULL, send_i, p_minerPath))
	{
		SetConsoleTextAttribute(hConsole, 12);
		printf_s("\nError creating thread. Out of memory?\n");
		SetConsoleTextAttribute(hConsole, 7);
		exit(-1);
	}

	// Main loop
	for (;;) {

		working_threads = 0;

		char scoopgen[40];
		memmove(scoopgen, signature, 32);

		char *mov = (char*)&height;

		scoopgen[32] = mov[7]; scoopgen[33] = mov[6]; scoopgen[34] = mov[5]; scoopgen[35] = mov[4]; scoopgen[36] = mov[3]; scoopgen[37] = mov[2]; scoopgen[38] = mov[1]; scoopgen[39] = mov[0];

		sph_shabal_context x;
		sph_shabal256_init(&x);
		sph_shabal256(&x, (const unsigned char*)(const unsigned char*)scoopgen, 40);
		char xcache[32];
		sph_shabal256_close(&x, xcache);

		scoop = (((unsigned char)xcache[31]) + 256 * (unsigned char)xcache[30]) % 4096;

		deadline = 0;
		bytesRead = 0;

		Log("\n------------------------    New block: "); Log_llu(height);

		_strtime(tbuffer);
		printf_s("\n");
		SetConsoleTextAttribute(hConsole, 31);
		printf_s("\r\n%s New block %llu, baseTarget %llu, netDiff %llu Tb          \r\n", tbuffer, height, baseTarget, 4398046511104 / 240 / baseTarget);
		SetConsoleTextAttribute(hConsole, 7);
		if (miner_mode == 0) printf_s("*** Chance to find a block: %.5f%%\n", ((double)(total_size / 1024 / 1024 * 100 * 60)*(double)baseTarget) / 1152921504606846976);

		for (i = 0; i < sessions.size(); i++) closesocket(sessions.at(i).Socket);
		sessions.clear();

		shares.clear();
		bests.clear();
		
		if ((targetDeadlineInfo > 0) && (targetDeadlineInfo < my_target_deadline)){
			Log("\nUpdate targetDeadline: "); Log_llu(targetDeadlineInfo);
		}
		else {
			targetDeadlineInfo = my_target_deadline;
			//Log("\ntargetDeadline not found, targetDeadline: "); Log_llu(targetDeadlineInfo);
		}

		for (i = 0; i < paths_num; i++)
		{
			worker.push_back(pthread_t());
			if (pthread_create(&worker[i], NULL, work_i, (char*)paths_dir[i].c_str()))
			{
				printf_s("\nError creating thread. Out of memory? Try lower stagger size\n");
				Log("\n! Error thread starting: ");	Log_u(i);
				exit(-1);
			}
		}


		memmove(oldSignature, signature, 32);
		unsigned long long old_baseTarget = baseTarget;
		unsigned long long old_height = height;

		// Wait until signature changed
		while (memcmp(signature, oldSignature, 32) == 0)
		{
			cls();
			SetConsoleTextAttribute(hConsole, 14);
			if (deadline == 0) printf_s("\r%llu%% %llu GB. no deadline\tsdl:%llu(%llu) cdl:%llu(%llu) ss:%llu(%llu) rs:%llu(%llu)", (bytesRead * 4096 * 100 / total_size), (bytesRead / (256 * 1024)), all_send_dl, err_send_dl, all_rcv_dl, err_rcv_dl, all_send_msg, err_send_msg, all_rcv_msg, err_rcv_msg);
			else              printf_s("\r%llu%% %llu GB. DL=%llu\tsdl:%llu(%llu) cdl:%llu(%llu) ss:%llu(%llu) rs:%llu(%llu)", (bytesRead * 4096 * 100 / total_size), (bytesRead / (256 * 1024)), deadline, all_send_dl, err_send_dl, all_rcv_dl, err_rcv_dl, all_send_msg, err_send_msg, all_rcv_msg, err_rcv_msg);
			SetConsoleTextAttribute(hConsole, 7);
			Sleep(18);
			if ((working_threads == 0) && (cleared == false) && (use_clean_mem == true))
			{
				//Sleep(100);
				ClearMem();
				cleared = true;
				Log("\nCleanup memory ");
			}
		}

		// Tell all threads to stop:
		stopThreads = 1;

		if (show_winner)
		{
			size_t counter = 0;
			while ((can_connect == 0) && (counter < 100))
			{
				Sleep(10);
				counter++;
			}
			if (counter < 100) GetBlockInfo(0);
		}

		for (i = 0; i < paths_num; i++)
		{
			Log("\nInterrupt thread: ");	Log_u(i);
			if (pthread_join(worker[i], NULL) > 0)
			{
				cls();
				SetConsoleTextAttribute(hConsole, 12);
				printf_s("\r Error stoping thread #%Iu of %Iu", i, paths_num);
				SetConsoleTextAttribute(hConsole, 7);
				Log("\n! Error thread stoping: ");	Log_u(i);
			}
		}
		stopThreads = 0;
		// clearmem for fast blocks
		if ((cleared == false) && (use_clean_mem == true))
		{
			ClearMem();
			Log("\nCleanup memory after fast block");
		}
		cleared = false;

		Log("\nWriting info to stat-log.csv ");
		fopen_s(&fp_Stat, "stat-log.csv", "at+");
		if (fp_Stat != NULL)
		{
			for (size_t a = 0; a < bests.size(); a++) if ((bests[a].account_id != 0) && (bests[a].DL != 0)) fprintf_s(fp_Stat, "%llu,%llu,%llu,%llu\n", bests[a].account_id, old_height, old_baseTarget, bests[a].DL);
			fclose(fp_Stat);
		}
		
//		fopen_s(&fp_Time, "debug-log.csv", "at+");
//		if (fp_Time != NULL)
//		{
//			for (size_t a = 0; a < thread_times.size(); a++)  fprintf_s(fp_Time, "%.2f;", thread_times[a]);
//			fprintf_s(fp_Time, "\n");
//			thread_times.clear();
//			fclose(fp_Time);
//		}
	}
	//pthread_join(sender, NULL); Добавить в сендер условие прекращения работы
	worker.clear();
	worker.~vector();
	paths_dir.clear();
	paths_dir.~vector();
	bests.clear();
	bests.~vector();
	shares.clear();
	shares.~vector();
	free(p_minerPath);

	WSACleanup();
}

