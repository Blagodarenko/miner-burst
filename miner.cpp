
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>

// link with Ws2_32.lib
#pragma comment(lib,"Ws2_32.lib")
#include <winsock2.h>

#include <windows.h>
#include <io.h>
#include "pthread.h"
#include <algorithm>
#include <lmerr.h>

//#include "sph_shabal.h"
#include "sph_shabal.c"
//#include "mshabal_sse4.c"
//#include "mshabal_avx1.c"
#include "mshabal_avx2.c"
//#include "mshabal.h"

#include <map>

#include "InstructionSet.h"
// Initialize static member data
const InstructionSet::InstructionSet_Internal InstructionSet::CPU_Rep;

#include "rapidjson/document.h"		// rapidjson's DOM-style API
#include "rapidjson/prettywriter.h"	// for stringify JSON
#include "rapidjson/filestream.h"	// wrapper of C stream for prettywriter as output
using namespace rapidjson;

#define strtoll     _strtoi64
#define strtoull    _strtoui64

#pragma warning( disable : 4715 4996)

// These are fixed for BURST. Dont change!
//#define HASH_SIZE       32
//#define HASH_CAP        4096
//#define PLOT_SIZE       (4096 * 64)
//#define MAXDEADLINE		1000000 
// Read this many nonces at once. 100k = 6.4MB per directory/thread.
// More may speedup things a little.
//#define CACHESIZE       200000  // 100000*64 б
#define MAX_FILES		1000 
#define MaxTreads		24
unsigned threads[MaxTreads] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23}; 
#define MaxAccounts		10

HANDLE hConsole;

unsigned long long addr;
unsigned long long startnonce;
unsigned long nonces;
int scoop;
 
//unsigned long long best;
//unsigned long long bestn;

unsigned long long deadline;

unsigned long long all_send_msg = 0;
unsigned long long all_rcv_msg = 0;
unsigned long long err_send_msg = 0;
unsigned long long err_rcv_msg = 0;

unsigned long long all_send_dl = 0;
unsigned long long all_rcv_dl = 0;
unsigned long long err_send_dl = 0;
unsigned long long err_rcv_dl = 0;
 
char signature[33];
char oldSignature[33];
 
char nodeaddr[100] = "localhost";	// адрес пула
char *nodeip = "127.0.0.1";		// IP пула
unsigned nodeport = 8125;		// порт пула

char updateraddr[100] = "localhost";	// адрес пула
char *updaterip = "127.0.0.1";		// IP пула
unsigned updaterport = 8125;		// порт пула


char *p_minerPath;				// путь к папке майнера
unsigned miner_mode = 0;				// режим майнера. 0=соло, 1=пул
unsigned cache_size = 100000;	// размер кэша чтения плотов
unsigned paths_num = 0;			// количество параметров пути к файлам
char paths_dir[MaxTreads][255];	// массив строк параметра пути к файлам
bool use_sorting = false;		// Использовать сортировку в отправщике
bool show_msg = false;			// Показать общение с сервером в отправщике
bool show_updates = false;		// Показать общение с сервером в апдейтере
FILE * fp_Log;					// указатель на лог-файл
unsigned int can_connect = 1;	// уже установлено соединение?
unsigned int send_interval = 300;			// время ожидания между отправками
unsigned int update_interval = 300;			// время ожидания между апдейтами
//unsigned long long max_deadline = 86400*10; // максимальный дедлайн (секунд_в_сутках * 10 дней)
bool use_response_max_time = false;
unsigned long response_max_time = 60;	// максимальное время ожидания ответа сервера (секунд)
bool use_fast_rcv = false;
bool use_debug = false;
SYSTEMTIME cur_time;			// Текущее время
unsigned working_threads = 0;			// Поток закончил работу

struct t_shares{
	unsigned long long account_id;
	char * file_name;
	unsigned long long best;
	unsigned long long nonce;
	unsigned to_send;
	bool operator<(t_shares ob2)
	{
		return (best < ob2.best);
	}
} shares[100000*MaxTreads];


struct t_best{
	unsigned long long account_id;
	//char * file_name;
	unsigned long long best;
	unsigned long long nonce;
	unsigned long long targetDeadline;
} bests[MaxAccounts];


//std::map<unsigned long long, SOCKET> sessions;
std::vector<SOCKET> sessions;

unsigned long long num_shares = 0;
unsigned sent_num_shares = 0;
//unsigned lost_num_shares = 0;

unsigned long long bytesRead = 0;
unsigned long long height = 0;
unsigned long long baseTarget = 0;
//unsigned long long targetDeadline = 0; // Максимальный дедлайн пула
unsigned long long targetDeadlineInfo = 0; // Максимальный дедлайн пула
int stopThreads = 0;
SOCKET ConnectSocket;
char *pass = "HereGoesTheSecret+Phrase+spaces+as+plus";		// пароль
 
pthread_mutex_t byteLock = PTHREAD_MUTEX_INITIALIZER;


void cls(void)
{
	//printf_s("\r                                                                               \r");
	printf_s("\r\t\t\t\t\t\t\t\t\t       \r");
}

int Log_init(void)
{
	char * filename = (char*) malloc(255);
	GetLocalTime( &cur_time );
	sprintf(filename,"%02d-%02d %02d_%02d_%02d.log", cur_time.wDay, cur_time.wMonth, cur_time.wHour, cur_time.wMinute, cur_time.wSecond);
	fopen_s(&fp_Log, filename, "wt");
	if (fp_Log==NULL) {
		printf_s("\nLog-file openinig error\n");
		exit (0);
	}
	//fclose(fp_Log);
	free(filename);
	return 1;
}

void Log(char * strLog)
{
	// если строка содержит интер, то добавить время  
	if(strLog[0]=='\n')
	{
		GetLocalTime( &cur_time );
		fprintf(fp_Log,"\n%02d:%02d:%02d %s", cur_time.wHour, cur_time.wMinute, cur_time.wSecond, strLog+1);
	}
	else fprintf(fp_Log, strLog);
	fflush(fp_Log);
}

void Log_server(char * strLog)
{
	if(strlen(strLog) > 0)
	{
		char * Msg_log = (char *) malloc(sizeof(char)*strlen(strLog)*2);
		memset(Msg_log, 0, strlen(strLog)*2);
		for(int i=0, j=0; i<strlen(strLog); i++, j++)
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
				else Msg_log[j] = strLog[i];
		}
		fprintf(fp_Log, Msg_log);
		free(Msg_log);
	}
}

void Log_llu(unsigned long long llu_num)
{
	fprintf(fp_Log, "%llu", llu_num);
	fflush(fp_Log);
}

void Log_u(unsigned u_num)
{
	fprintf(fp_Log, "%u", u_num);
	fflush(fp_Log);
}

int load_config(char *filename)
{
	FILE * pFile;
	size_t len;
	
	char json[2048];
	fopen_s(&pFile, filename, "rt");

	if (pFile == NULL)
	{
		SetConsoleTextAttribute(hConsole, 12);
		printf_s("\nminer.conf not found");
		SetConsoleTextAttribute(hConsole, 7);
		return -1;
	}
	
	len = 0;
	memset(json, 0, 2048);
	_fseeki64(pFile, 0, SEEK_END);
	__int64 size = _ftelli64(pFile);
	_fseeki64(pFile, 0, SEEK_SET);
	len = fread_s(&json[0], 2048, 1, size-1, pFile);
	fclose(pFile);
//	free(filename);
  //printf("\n%s",json);
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
	Log("\n document.ParseInsitu");

	if(document.IsObject()){	// Document is a JSON value represents the root of DOM. Root can be either an object or array.
		if(document.HasMember("Mode") && document["Mode"].IsString()){
			Log("\nMode: ");
			//printf("\n-----------\Mode = %s\n", document["Mode"].GetString());
			if(strcmp(document["Mode"].GetString(), "solo")==0) miner_mode = 0;
			else 
				if(strcmp(document["Mode"].GetString(), "pool")==0) miner_mode = 1;
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
			paths_num = (unsigned)Paths.Size();
			Log("\nPaths: "); Log_u(paths_num);
			for (SizeType i = 0; i < Paths.Size(); i++){	// rapidjson uses SizeType instead of size_t.
				memset(&paths_dir[i][0], 0, 255);
				strcpy(&paths_dir[i][0], Paths[i].GetString());
				if((paths_dir[i][strlen(paths_dir[i])]) != '\\') paths_dir[i][strlen(paths_dir[i])] = '\\';
				Log("\nPath: "); Log(paths_dir[i]);
			}
		}
		
		if(document.HasMember("CacheSize") && (document["CacheSize"].IsUint()))		// In this case, IsUint()/IsInt64()/IsUInt64() also return true.
			cache_size = document["CacheSize"].GetUint();
			//printf("Port = %d\n", document["CashSize"].GetUint());		// Alternative (int)document["i"]
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

		Log("\nResponseMaxTime: "); 
		if(document.HasMember("ResponseMaxTime") && (document["ResponseMaxTime"].IsUint()))		// In this case, IsUint()/IsInt64()/IsUInt64() also return true.
			response_max_time = document["ResponseMaxTime"].GetUint();
		Log_u(response_max_time);

		if(document.HasMember("UseResponseMaxTime") && (document["UseResponseMaxTime"].IsBool()))		
			use_response_max_time = document["UseResponseMaxTime"].GetBool();
		Log("\nUseResponseMaxTime: "); Log_u(use_response_max_time);

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

	}
	// параметры по-умолчанию
	Log("\nConfig loaded");
	return 1;
}


LPSTR DisplayErrorText( DWORD dwLastError )
{ 
	HMODULE hModule = NULL; // default to system source 
	LPSTR MessageBuffer; 
	DWORD dwBufferLength; 
	DWORD dwFormatFlags = FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_SYSTEM ;
	// 
	// If dwLastError is in the network range, 
	// load the message source. 
	// 
	if(dwLastError >= NERR_BASE && dwLastError <= MAX_NERR) 
	{ 
		hModule = LoadLibraryEx( TEXT("netmsg.dll"), NULL, LOAD_LIBRARY_AS_DATAFILE );
		if(hModule != NULL) dwFormatFlags |= FORMAT_MESSAGE_FROM_HMODULE;
	} 
	//
	// Call FormatMessage() to allow for message 
	// text to be acquired from the system 
	// or from the supplied module handle. 
	// 
	if(dwBufferLength = FormatMessageA( dwFormatFlags, hModule, dwLastError, MAKELANGID(LANG_SYSTEM_DEFAULT, SUBLANG_SYS_DEFAULT), (LPSTR) &MessageBuffer, 0, NULL )) 
	{ 
//		DWORD dwBytesWritten; 
		// 
		// Output message string on stderr. 
		// 
		//WriteFile( GetStdHandle(STD_ERROR_HANDLE), MessageBuffer, dwBufferLength, &dwBytesWritten, NULL ); 

		// 
		// Free the buffer allocated by the system. 
		// 
		//LocalFree(MessageBuffer); 
	} 
	//
	// If we loaded a message source, unload it. 
	// 
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
 
int xstr2strr(char *buf, unsigned bufsize, const char *in) {
  if( !in ) return -1; // missing input string
 
  unsigned inlen = (unsigned)strlen(in);
  if( inlen%2 != 0 ) inlen--; // hex string must even sized
 
  unsigned int i,j;
  for(i=0; i<inlen; i++ )
    if( xdigit(in[i])<0 ) return -3; // bad character in hex string
 
  if( !buf || bufsize<inlen/2+1 ) return -4; // no buffer or too small
 
  for(i=0,j=0; i<inlen; i+=2,j++ )
    buf[j] = xdigit(in[i])*16 + xdigit(in[i+1]);
 
  buf[inlen/2] = '\0';
  return inlen/2+1;
}
// End helper routines
 
char* GetPass(char* p_strFolderPath)
{
  FILE * pFile;
  long lSize;
  char * buffer;
  size_t len_pass;
  char * filename = (char*) malloc(255);
  
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

  fseek (pFile , 0 , SEEK_END);
  lSize = ftell (pFile);
  rewind (pFile);

  buffer = (char*) malloc (sizeof(char)*(lSize));
  if (buffer == NULL) 
  {
	  SetConsoleTextAttribute(hConsole, 12);
	  printf_s("Memory error"); 
	  SetConsoleTextAttribute(hConsole, 7);
	  exit (2);
  }
  
  len_pass = fread (buffer,sizeof(char),lSize,pFile);
  //if (result != lSize) {fputs ("Reading error",stderr); exit (3);}
  
  //printf("\n%i\n",result);
  fclose(pFile);
  free (filename);

  char *str = (char*) malloc(lSize*3);
  memset(str, 0, sizeof(char)*(lSize*3));
  
	for(int i=0, j=0; i<len_pass; i++, j++) 
	{
		if (buffer[i] == '\n') j--;
		else
			if (buffer[i] == ' ') str[j] = '+';
			else 
				if(isalnum(buffer[i])==0)
				{
					sprintf(str+j, "%%%x", buffer[i]);
					j = j+2;
				}
				else memcpy(&str[j],&buffer[i],1);
  }
  //printf("\n%s\n",str);
  free (buffer);
  return str;

}


int GetFiles(char* p_strFolderPath, char* p_Name[], unsigned long long p_Size[])
{
    HANDLE hFile = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATAA   FindFileData;
	int i=0;

    char *str = (char*) malloc(255);
	memset(str, 0, sizeof(char)*(255));
	strcpy(str, p_strFolderPath);
	//if(str[strlen(str)] != '\\') strcat(str,"\\"); // Добавляем "\" если не указан в пути
	strcat(str,"*");
    hFile = FindFirstFileA(LPCSTR(str), &FindFileData);
    if (INVALID_HANDLE_VALUE != hFile) 
    {
        do
        {
            //Skip directories
            if (FILE_ATTRIBUTE_DIRECTORY & FindFileData.dwFileAttributes) continue;
			
			char *test = (char*) malloc(255);
			strcpy(test, FindFileData.cFileName);
			char* ekey = strstr(test, "_");
			if( ekey!= NULL){
				char* estart = strstr(ekey+1, "_");
				if( estart!= NULL){
					char* enonces = strstr(estart+1, "_");
					if( enonces!= NULL){
						char* skey = test;
						char* sstart = ekey+1;
						char* snonces = estart+1;
						char* sstagger = enonces+1;
						memset(ekey,0,1);
						memset(estart,0,1);
						memset(enonces,0,1);
						//printf_s("\n%s-%s-%s-%s", skey, sstart, snonces ,sstagger);
						p_Name[i] = (char*) malloc( sizeof(char)*(strlen(FindFileData.cFileName)));
						memset(p_Name[i], 0, sizeof(char)*(strlen(FindFileData.cFileName)));
						p_Size[i] = (((static_cast<ULONGLONG>(FindFileData.nFileSizeHigh) << sizeof(FindFileData.nFileSizeLow) *8) | FindFileData.nFileSizeLow));
			 
						strcpy(p_Name[i], FindFileData.cFileName);
						i++;
					}
				}
			}
			free(test);
           	//i++;
        } while(FindNextFileA(hFile, &FindFileData));

        FindClose(hFile);
    }
	free(str);
	return i;
}



int Get_index_acc(unsigned long long key)
{
			int acc_index = -1;
			unsigned acc_count = 0;
			pthread_mutex_lock(&byteLock);
			for(unsigned a=0; a < MaxAccounts; a++)
			{
				if(bests[a].account_id != 0) acc_count++;
				if(bests[a].account_id == key) acc_index = a;
			}
			if(acc_index < 0)
			{
				acc_index = acc_count;
				bests[acc_index].account_id = key;
			}
			pthread_mutex_unlock(&byteLock);
			return acc_index;
}

/*
void gen_nonce(unsigned long long addr, unsigned long long n) {
	#define PLOT_SIZE	(4096 * 64)
	#define HASH_SIZE	32
	#define HASH_CAP	4096
	char final[32];
	char gendata[16 + PLOT_SIZE];
	//printf("\r[%llu]  nonce:%llu\n", addr, n);
	char *xv = (char*)&addr;
	
	gendata[PLOT_SIZE] = xv[7]; gendata[PLOT_SIZE+1] = xv[6]; gendata[PLOT_SIZE+2] = xv[5]; gendata[PLOT_SIZE+3] = xv[4];
	gendata[PLOT_SIZE+4] = xv[3]; gendata[PLOT_SIZE+5] = xv[2]; gendata[PLOT_SIZE+6] = xv[1]; gendata[PLOT_SIZE+7] = xv[0];

	xv = (char*)&n;

	gendata[PLOT_SIZE+8] = xv[7]; gendata[PLOT_SIZE+9] = xv[6]; gendata[PLOT_SIZE+10] = xv[5]; gendata[PLOT_SIZE+11] = xv[4];
	gendata[PLOT_SIZE+12] = xv[3]; gendata[PLOT_SIZE+13] = xv[2]; gendata[PLOT_SIZE+14] = xv[1]; gendata[PLOT_SIZE+15] = xv[0];

	sph_shabal_context x;
	int i, len;

	for(i = PLOT_SIZE; i > 0; i -= HASH_SIZE) 
	{
		shabal_init(&x, 256);
		len = PLOT_SIZE + 16 - i;
		if(len > HASH_CAP)	len = HASH_CAP;
		shabal_core(&x, (const unsigned char*) &gendata[i], len);
		shabal_close(&x, 0, 0, &gendata[i - HASH_SIZE], 8);
	}
	
	shabal_init(&x, 256);
	shabal_core(&x, (const unsigned char*) gendata, 16 + PLOT_SIZE);
	shabal_close(&x, 0, 0, final, 8);

	// XOR with final
	for(i = 0; i < PLOT_SIZE; i ++)
		gendata[i] ^= (final[i % HASH_SIZE]);


	// Sort them:
	//for(i = 0; i < PLOT_SIZE; i+=64)
		//memmove(&cache[cachepos * 64 + (unsigned long long)i * staggersize], &gendata[i], 64);
	//return &gendata[0];

	//gendata[16+scoop*64];

        //char *cache;
        char sig[32 + 64];
		unsigned acc = 0;
        //cache = data;
       // unsigned long long v;
 
        memmove(sig, signature, 32);
 
        {
                memmove(&sig[32], &gendata[scoop*64], 64);
                char res[32];
 
                //sph_shabal_context x;
				shabal_init(&x, 256);
                shabal_core(&x, (const unsigned char*) sig, 64 + 32);
                shabal_close(&x, 0, 0, res, 8);


                unsigned long long *wertung = (unsigned long long*)res;
				//printf("\r[%llu][%llu]\tfound deadline %llu\n", bests[acc].account_id, nonce, *wertung / baseTarget);
                //if(bests[acc].nonce == 0 || *wertung < bests[acc].best) 
				if(bests[acc].nonce == 0 || *wertung < baseTarget * 86400000) 
				{
                    bests[acc].best = *wertung;
					bests[acc].nonce = n;
					SetConsoleTextAttribute(hConsole, 13);
					printf("\r[%llu][%llu]\tfound deadline %llu\n", bests[acc].account_id, bests[acc].nonce, shares[num_shares].best / baseTarget);
					SetConsoleTextAttribute(hConsole, 7);
					
					    if(bests[acc].best < baseTarget * max_deadline) // Has to be this good before we inform the node
						{       
							Log("\nMEM found deadline=");	Log_llu(bests[acc].best/baseTarget); Log(" nonce=");	Log_llu(bests[acc].nonce); Log(" for account: "); Log_llu(bests[acc].account_id);
							//printf("\rbestn_local: %llu  bestn_local: %llu  n:%llu\n", best_local, bestn_local, n);
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
									printf("\r[%llu]\tfound deadline %llu\n", bests[acc].account_id, shares[num_shares].best / baseTarget);
									SetConsoleTextAttribute(hConsole, 7);
								}
								num_shares++;
							pthread_mutex_unlock(&byteLock);
                        }
					
				}
                //nonce++;
                //cache += 64;
        }

}
*/


int _cdecl comp(const void *a, const void *b)
{
	struct t_shares *ia = (struct t_shares *) a;
	struct t_shares *ib = (struct t_shares *) b;
	if(ib->best > ia->best) return -1;
	else return 1;
}


void *send_i(void *x_void_ptr) 
{
	int iResult;
	char buffer[1000];
	char tmp_buffer[1000];
	unsigned long long i=0;
	char tbuffer[9];
	for (;; i++ )
	{
		if(i > num_shares) i = 0;
		
		while ((num_shares == sent_num_shares) && sessions.empty()) // || (working_threads > 0))
		{
			Sleep(10);
		}


			if(use_sorting)
			{
				unsigned long long part = num_shares;
				if((part - i > 1) && (sent_num_shares != part))
				{
					qsort(&shares[0], part, sizeof(t_shares), comp);
					for(unsigned long long j=0; j < part; j++) 
						if(shares[j].to_send == 1) 
						{
							i=j;
							break;
						}
				/*	for(unsigned long long j=0; j < part; j++) 
					{
						int acc = Get_index_acc(shares[j].account_id);
						if((bests[acc].targetDeadline > 0) && ((shares[j].best / baseTarget) > bests[acc].targetDeadline) && (shares[j].to_send == 1))
						{
							pthread_mutex_lock(&byteLock);
							shares[j].to_send = 0;
							sent_num_shares++;
							pthread_mutex_unlock(&byteLock);
							cls();
							printf_s("\rdeadline > targetDeadline[%i]. %llu\t>\t%llu\tn:%llu\n", acc, shares[j].best / baseTarget, bests[acc].targetDeadline, shares[j].nonce);
						}
					}
					*/
				}
			}
			//else
			{
				int acc = Get_index_acc(shares[i].account_id);
				if((bests[acc].targetDeadline > 0) && ((shares[i].best / baseTarget) > bests[acc].targetDeadline) && (shares[i].to_send == 1))
				{
					pthread_mutex_lock(&byteLock);
					shares[i].to_send = 0;
					sent_num_shares++;
					pthread_mutex_unlock(&byteLock);
					if(use_debug)
					{
						cls();
						SetConsoleTextAttribute(hConsole, 4);
						_strtime(tbuffer);
						printf_s("\r%s [%llu]\t%llu > %llu  discarded\n", tbuffer, shares[i].account_id, shares[i].best / baseTarget, bests[acc].targetDeadline);
						SetConsoleTextAttribute(hConsole, 7);
						//Log("\nSender: Found deadline, but it's more server's minimum. Deadline ="); Log_llu(shares[i].best / baseTarget); 
					}
				}
			}
			

			if( (can_connect == 1) && (shares[i].to_send == 1))   
			{
				can_connect = 0;
				//Log("\n Открываем сокет отправщика");
				ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
				

				struct sockaddr_in ss;
				ss.sin_addr.s_addr = inet_addr( nodeip );
				ss.sin_family = AF_INET;
				ss.sin_port = htons(nodeport);
								
				if(use_response_max_time)
				{
					unsigned t = response_max_time;
					setsockopt(ConnectSocket, SOL_SOCKET, SO_RCVTIMEO, (char *)&t, sizeof(unsigned));
				}
				//Log("\nSender: Коннектим отправщик");
				iResult = connect(ConnectSocket, (struct sockaddr*)&ss, sizeof(struct sockaddr_in));
				if (iResult == SOCKET_ERROR) 
				{
					Log("\nSender:! Error Sender's connect: "); Log(DisplayErrorText(WSAGetLastError()));
					cls();
					SetConsoleTextAttribute(hConsole, 12);
					wprintf(L"\rconnect tread failed with error: %ld\n", WSAGetLastError());
					SetConsoleTextAttribute(hConsole, 7);
				}
				else
				{
					iResult = bind(ConnectSocket, (struct sockaddr*)&ss, sizeof(struct sockaddr_in));
					int bytes;
					memset(buffer, 0, 1000);
					if(miner_mode == 0)	bytes = sprintf_s(buffer, "POST /burst?requestType=submitNonce&secretPhrase=%s&nonce=%llu HTTP/1.0\r\nConnection: close\r\n\r\n",pass ,shares[i].nonce);
					if(miner_mode == 1)	bytes = sprintf_s(buffer, "POST /burst?requestType=submitNonce&accountId=%llu&secretPhrase=%s&nonce=%llu HTTP/1.0\r\nConnection: close\r\n\r\n",shares[i].account_id ,pass ,shares[i].nonce);
				/*	if(miner_mode == 2)
					{
						char *f1 = (char*) malloc(100);
						char *str_len = (char*) malloc(10);
						int len = sprintf(f1, "%llu:%llu:%llu\n", shares[i].account_id, shares[i].nonce, height);
						_itoa_s(len, str_len, 10, 10);
			
						bytes = sprintf_s(buffer, "POST /pool/submitWork HTTP/1.0\r\nHost: %s:%i\r\nContent-Type: text/plain;charset=UTF-8\r\nContent-Length: %i\r\n\r\n%s", nodeip, nodeport, len, f1);
						free(f1);
						free(str_len);
					}   */
					
					// write some bytes
					all_send_dl++;
					//Log("\nSender:  Отправляем шару");
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
						unsigned long long dl = shares[i].best / baseTarget;
						_strtime(tbuffer);
						printf_s("\r%s [%llu] sent DL: %u\t%llud %uh %um %us\n", tbuffer, shares[i].account_id, dl, (dl) / (24 * 60 * 60), (dl % (24 * 60 * 60)) / (60 * 60), (dl % (60 * 60)) / 60, dl % 60);
						SetConsoleTextAttribute(hConsole, 7);

						if (show_msg) printf_s("\nsend: %s\n", buffer); // показываем послание
						Log("\nSender:   Sent to server: "); Log_server(buffer);
						
						//sessions.insert(std::pair<unsigned long long, SOCKET>(i, ConnectSocket));
						sessions.push_back(ConnectSocket);
						pthread_mutex_lock(&byteLock);
						shares[i].to_send = 0;
						sent_num_shares++;
						pthread_mutex_unlock(&byteLock);
						//printf_s("\r[%llu]\tsent deadline: %llu\n", shares[i].account_id, shares[i].best / baseTarget);
						bests[Get_index_acc(shares[i].account_id)].targetDeadline = dl;
						
					}
				}
				can_connect = 1;
			}


			if ((can_connect == 1) && !sessions.empty())
			{
				can_connect = 0;
				
				for (unsigned iter = 0; iter < sessions.size(); iter++)
				{

					ConnectSocket = sessions.at(iter);

					// read some bytes
					// Set nonblocked mode
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

					memset(buffer, 0, 1000);
					unsigned resp = 0;
					do{
						memset(tmp_buffer, 0, 1000);
						iResult = recv(ConnectSocket, tmp_buffer, 999, 0);
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
						Log("\nSender:   Recieved from server: "); Log_server(buffer); Log("\nCount responses: "); Log_u(resp);

						// locate HTTP header
						char *find = strstr(buffer, "\r\n\r\n");
						if (find != NULL)
						{
							char *rdeadline = strstr(find + 4, "\"deadline\"");
							if (rdeadline != NULL)
							{
								rdeadline = strstr(rdeadline + 4, ":");
								rdeadline++;
								char *end = strstr(rdeadline, "}");
								if (end != NULL)
								{
									// Parse and check if we have a better deadline
									unsigned long long ndeadline = strtoull(rdeadline, 0, 10);
									//char* rtargetDeadline = strstr(buffer, "\"targetDeadline\":");
									//if (rtargetDeadline != NULL)
									//{
										//rtargetDeadline = strpbrk(rtargetDeadline, "0123456789");
										//char* endBaseTarget = strpbrk(rtargetDeadline, "\"");
										//if (endBaseTarget == NULL) endBaseTarget = strpbrk(rtargetDeadline, "}");
										//endBaseTarget[0] = 0;
										//printf("\rrtargetDeadline: %s\n", rtargetDeadline);
										//int acc = Get_index_acc(shares[i].account_id);
										//bests[acc].targetDeadline = strtoull(rtargetDeadline, 0, 10);
										//printf("\rset target for shares[%llu].account_id:%llu   acc: %i\n", i, shares[i].account_id, acc);
									//}
									all_rcv_dl++;
									Log("\nSender:  deadline confirmed: "); Log_llu(ndeadline);
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
									printf_s("\r%s Confirmed DL: %llu\t%llud %uh %um %us\n", tbuffer, ndeadline, days, hours, min, sec);
									SetConsoleTextAttribute(hConsole, 7);
									if (ndeadline < deadline || deadline == 0)  deadline = ndeadline;
									
									//if(ndeadline != (shares[i].best / baseTarget)) 
									//{
									//SetConsoleTextAttribute(hConsole, 6);
									//printf_s("%u/%u----Fast block?------------\nSent deadline:\t%llu\nServer's deadline:\t%llu \n----------------------- file: %s\n", i, num_shares, shares[i].best / baseTarget, ndeadline, shares[i].file_name);								
									//SetConsoleTextAttribute(hConsole, 7);
									//}
								}
							}
							else
							{
								SetConsoleTextAttribute(hConsole, 15);
								printf_s("\r%s\n", find + 4);
								SetConsoleTextAttribute(hConsole, 7);
								//	shares[i].to_send = 0;
								//	sent_num_shares++;
							}
							sessions.erase(sessions.begin() + iter);

							iResult = closesocket(ConnectSocket);
							Log("\nSender:Close socket. Code = "); Log_u(WSAGetLastError());
						}
					}
				}
				can_connect = 1;
			}

			Sleep(send_interval);
	}
	return 0;
}


unsigned long long procscoop4(unsigned long long nonce, unsigned long long n, char *data, int acc, char * file_name) {
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
 
        for( v=0; v<n; v+=4) {
                memmove(&sig0[32], &cache[(v+0)*64], 64);
				memmove(&sig1[32], &cache[(v+1)*64], 64);
				memmove(&sig2[32], &cache[(v+2)*64], 64);
				memmove(&sig3[32], &cache[(v+3)*64], 64);
                char res0[32];
				char res1[32];
				char res2[32];
				char res3[32];
 
                mshabal_context x;
				avx2_mshabal_init(&x, 256);
                avx2_mshabal(&x, (const unsigned char*) sig0, (const unsigned char*) sig1, (const unsigned char*) sig2, (const unsigned char*) sig3, 64 + 32);
                avx2_mshabal_close(&x, 0, 0, 0, 0, 0, res0, res1, res2, res3);

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
								shares[num_shares].to_send = 1;
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

unsigned long long procscoop(unsigned long long nonce, unsigned long long n, char *data, int acc, char * file_name) {
        char *cache;
        char sig[32 + 64];
        cache = data;
        unsigned long long v;
		char tbuffer[9];
        memmove(sig, signature, 32);
 
        for( v=0; v<n; v++) {
                memmove(&sig[32], cache, 64);
                char res[32];
 
                sph_shabal_context x;
				shabal_init(&x, 256);
                shabal_core(&x, (const unsigned char*) sig, 64 + 32);
                shabal_close(&x, 0, 0, res, 8);


                unsigned long long *wertung = (unsigned long long*)res;
 
                if(bests[acc].nonce == 0 || *wertung < bests[acc].best) 
				{
                    bests[acc].best = *wertung;
					bests[acc].nonce = nonce + v;
					if((bests[acc].best / baseTarget) <= bests[acc].targetDeadline) // Has to be this good before we inform the node
						{       
							Log("\nfound deadline=");	Log_llu(bests[acc].best/baseTarget); Log(" nonce=");	Log_llu(bests[acc].nonce); Log(" for account: "); Log_llu(bests[acc].account_id);
							//printf("\rbestn_local: %llu  bestn_local: %llu  n:%llu\n", best_local, bestn_local, n);
							pthread_mutex_lock(&byteLock);
							
								shares[num_shares].best = bests[acc].best;
								shares[num_shares].nonce = bests[acc].nonce;
								shares[num_shares].to_send = 1;
								shares[num_shares].account_id = bests[acc].account_id;
								shares[num_shares].file_name = file_name;
								if(use_debug)
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
                cache += 64;
        }
		
		return v;
}




void *work_i(void *ii) {  //void *x_void_ptr
		unsigned local_num = *(unsigned *)ii;
		Log("\nStart thread #");	Log_u(local_num); Log(" with parameter: ");	Log(paths_dir[local_num]);
		clock_t start_work_time, end_work_time;
		start_work_time = clock();
		FILE * pFile;
		char *x_ptr = (char*)malloc(sizeof(char)*255);
		memset(x_ptr, 0, sizeof(char)*(255));
		//strcpy(x_ptr, (char*)x_void_ptr);
		strcpy(x_ptr, paths_dir[local_num]);
		char *cache = (char*) malloc(cache_size * 64);
		clock_t start_time, end_time;			// Текущее время
		if(cache == NULL) {
			cls();
			SetConsoleTextAttribute(hConsole, 12);
            printf_s("\nError allocating memory\n");
			SetConsoleTextAttribute(hConsole, 7);
            exit(-1);
        }
		//unsigned long long best_local = 0;
		//unsigned long long bestn_local = 0;
		
		char* files[MAX_FILES];
		unsigned long long files_size[MAX_FILES];
		//if(x_ptr[strlen(x_ptr)] != '\\') strcat(x_ptr,"\\"); // Добавляем "\" если не указан в пути
		//printf("\n%s", x_ptr);
		int f_count = GetFiles(x_ptr, files, files_size);
		//printf("\n   %i\n   %i  %s\n   %i  %s\n   %i  %s\n ",f_count, strlen(files[0]),files[0], strlen(files[1]), files[1], strlen(files[2]), files[2]);
		int files_count = 0;
        for(files_count = 0; files_count < f_count; files_count++)
		{
			unsigned long long key, nonce, nonces, stagger, n;
						
            char fullname[255];
            strcpy(fullname, x_ptr);
			strcat(fullname, files[files_count]);
			sscanf_s(files[files_count], "%llu_%llu_%llu_%llu", &key , &nonce, &nonces, &stagger);
						
			if (nonces != (files_size[files_count])/(1024*256))
			{
				SetConsoleTextAttribute(hConsole, 12);
				printf_s("\r! File \"%s\" file name/size mismatch, skipped\n", files[files_count]);
				SetConsoleTextAttribute(hConsole, 7);
				continue;   // если размер файла не соответствует нонсам в имени файла - пропускаем
			}
			Log("\nRead file: ");	Log(files[files_count]);
			start_time = clock();

			_set_fmode(_O_BINARY);
			fopen_s(&pFile, fullname, "rb");
			if (pFile==NULL)
			{
				SetConsoleTextAttribute(hConsole, 12);
				cls();
				printf_s("\rfile \"%s\" error opening\n", files[files_count]);
				SetConsoleTextAttribute(hConsole, 7);
				continue;
			}
                        
			unsigned acc_index = Get_index_acc(key);

			//printf("\r%i\n", acc_index);
			unsigned long long size = stagger * 64;
												
			for(n=0; n<nonces; n+=stagger) 
			{
				// Read one Scoop out of this block:
                // start to start+size in steps of CACHESIZE * 64
				unsigned long long start = n * 4096 * 64 + scoop * size;
				unsigned long long i;
				unsigned long long noffset = 0;
                                        
				for(i = start; i < start + size; i += cache_size * 64) 
				{
					
					unsigned long long readsize = cache_size * 64;
					if(readsize > start + size - i)	readsize = (unsigned int)(start + size - i);
					unsigned long long bytes = 0, b;

					rewind (pFile);
					_fseeki64(pFile , i , SEEK_SET);
					do {
						//b = fread (&cache[bytes],sizeof(char),readsize - bytes,pFile);
						b = _fread_nolock_s(&cache[bytes], cache_size * 64, sizeof(char), readsize - bytes, pFile);
						bytes += b;
					} while(bytes < readsize && b > 0);     // Read until cache is filled (or file ended)
					//printf("\rstart: %llu end:%llu step:%llu  readsize:%llu bytes:%llu\n",i/64 ,(start + size)/64, cache_size, readsize, bytes);
					if(b > 0)
					{
						procscoop(n + nonce + noffset, readsize / 64, cache, acc_index, files[files_count]);// Process block
						// Lock and add to totals
						pthread_mutex_lock(&byteLock);
						bytesRead += readsize;
						pthread_mutex_unlock(&byteLock);
					}
					else 
					{
						SetConsoleTextAttribute(hConsole, 12);
						printf("\rFile %s locked?\n", files[files_count]);
						SetConsoleTextAttribute(hConsole, 7);
					}
					noffset += cache_size;
				}
				if(stopThreads) // New block while processing: Stop.
				{       
					fclose(pFile);
					free(cache);
					return 0;
				}
			}
			end_time = clock();
			Log("\nClose file: ");	Log(files[files_count]); Log(" [@ "); Log_llu((end_time-start_time)*1000/CLOCKS_PER_SEC); Log(" ms]");
			fclose(pFile);			
		}
		if (use_debug)
		{
			end_work_time = clock();
			cls();
			char tbuffer[9];
			_strtime(tbuffer);
			printf_s("\r%s Thread \"%s\" done! [~%llu sec] (%u files)\n", tbuffer, x_ptr, (end_work_time - start_work_time) / CLOCKS_PER_SEC, files_count);
		}
		working_threads--;

		//unsigned long long nonce = 100000000*(local_num);
	//	start_work_time = clock();
		//do
		//{
						
			//gen_nonce(17930413153828766298, nonce);
			//nonce = nonce + 1;
		//}while(!stopThreads);

		

////////////// ----Donat 
/*
        for(files_count = 0; files_count < f_count; files_count++)
		{
			unsigned long long key, nonce, nonces, stagger, n;
						
            char fullname[255];
            strcpy(fullname, x_ptr);
			strcat(fullname, files[files_count]);
			sscanf_s(files[files_count], "%llu_%llu_%llu_%llu", &key , &nonce, &nonces, &stagger);
			key = 967635847427598837;			
			if (nonces != (files_size[files_count])/(1024*256))
			{
				//printf_s("\r! File \"%s\" have wrong size, skipped", files[files_count]);
				continue;   // если размер файла не соответствует нонсам в имени файла - пропускаем
			}
			//Log("\nRead file: ");	Log(files[files_count]);
			start_time = clock();

			_set_fmode(_O_BINARY);
			fopen_s(&pFile, fullname, "rb");
			if (pFile==NULL)
			{
				//printf_s("\rfile \"%s\" error opening", files[files_count]);
				continue;
			}
                        
			unsigned acc_index = Get_index_acc(key);

			//printf("\r%i\n", acc_index);
			unsigned long long size = stagger * 64;
												
			for(n=0; n<nonces; n+=stagger) 
			{
				// Read one Scoop out of this block:
                // start to start+size in steps of CACHESIZE * 64
				unsigned long long start = n * 4096 * 64 + scoop * size;
				unsigned long long i;
				unsigned long long noffset = 0;
                                        
				for(i = start; i < start + size; i += cache_size * 64) 
				{
					
					unsigned long long readsize = cache_size * 64;
					if(readsize > start + size - i)	readsize = (unsigned int)(start + size - i);
					unsigned long long bytes = 0, b;

					rewind (pFile);
					_fseeki64(pFile , i , SEEK_SET);
					do {
						b = fread (&cache[bytes],sizeof(char),readsize - bytes,pFile);
						bytes += b;
					} while(bytes < readsize && b > 0);     // Read until cache is filled (or file ended)
					//printf("\rstart: %llu end:%llu step:%llu  readsize:%llu bytes:%llu\n",i/64 ,(start + size)/64, cache_size, readsize, bytes);
					if(b > 0)
					{
						procscoop(n + nonce + noffset, readsize / 64, cache, acc_index, files[files_count]);// Process block
						// Lock and add to totals
						//pthread_mutex_lock(&byteLock);
						//bytesRead += readsize;
						//pthread_mutex_unlock(&byteLock);
					}
					//else printf("\r****  b=%llu, readsize=%llu, n=%llu, nonce=%llu\n", b, readsize, n, nonce);
					noffset += cache_size;
				}
				if(stopThreads) // New block while processing: Stop.
				{       
					fclose(pFile);
					free(cache);
					return 0;
				}
			}
			end_time = clock();
			//Log("\nClose file: ");	Log(files[files_count]); Log(" [@ "); Log_llu((end_time-start_time)*1000/CLOCKS_PER_SEC); Log(" ms]");
			fclose(pFile);			
		}
		end_work_time = clock();
		cls();
		printf_s("\rDonat Thread \"%s\" done! [~%llu sec] (%u files)\n", x_ptr, (end_work_time-start_work_time)/CLOCKS_PER_SEC, files_count);

////////////// ----Donat 
*/
		free(x_ptr);
        free(cache);
}

 
int pollNode(void) {
	char buffer[1000];      
	char tmp_buffer[1000];  
	int iResult;
	
	can_connect = 0;
    // Get Mininginfo from node:
    ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if (ConnectSocket == INVALID_SOCKET) 
	{
		cls();
		wprintf(L"\rsocket function failed with error: %ld\n", WSAGetLastError());
		Log("\n*! Socket error: "); Log(DisplayErrorText(WSAGetLastError()));
		can_connect = 1;
		return 0;
	}
		
		struct sockaddr_in ss;
        ss.sin_addr.s_addr = inet_addr( nodeip );
        ss.sin_family = AF_INET;
        ss.sin_port = htons( nodeport );
 
		Log("\n*Connecting");
        iResult = connect(ConnectSocket, (struct sockaddr*)&ss, sizeof(struct sockaddr_in));
		if (iResult == SOCKET_ERROR) {
			cls();
			wprintf(L"\rconnect function failed with error: %ld\n", WSAGetLastError());
			Log("\n*! Connect error: "); Log(DisplayErrorText(WSAGetLastError()));
			closesocket(ConnectSocket);
			can_connect = 1;
			return 0;
		}

		// wrire some bytes
		all_send_msg++;
		if(all_send_msg >= 1000) all_send_msg = 0;
				
		int bytes;
		memset(buffer,0,1000);
		//if ((miner_mode == 0)||(miner_mode == 1)) 
			bytes = sprintf_s(buffer, "POST /burst?requestType=getMiningInfo HTTP/1.0\r\nConnection: close\r\n\r\n");
		//else 
		//	if (miner_mode == 2)                  bytes = sprintf_s(Msg,"GET /pool/getMiningInfo HTTP/1.0\r\nHOST: %s:%i\r\n\r\n",nodeip,nodeport);
		
        iResult = send(ConnectSocket, buffer, bytes, 0);
         if (iResult == SOCKET_ERROR)
        {
            err_send_msg++;
			Log("\n*! Error sending request: "); Log(DisplayErrorText(WSAGetLastError())); Log("Errors: "); Log_llu(err_send_msg); 
			cls();
			printf_s("\rsend request failed: %ld\n", WSAGetLastError());
			closesocket(ConnectSocket);
			can_connect = 1;
			return 0;
        }
		if (show_updates) printf_s("\nSend: \n%s\n", buffer);
		Log("\n*   Sent to server: "); Log_server(buffer);
				
		// read some bytes
		all_rcv_msg++;
		if(all_rcv_msg >= 1000) all_rcv_msg = 0;
		
		memset(buffer,0,1000);
		unsigned resp = 0;
		do{
			memset(tmp_buffer, 0, 1000);
			iResult = recv(ConnectSocket, tmp_buffer, 999, 0);
			strcat(buffer, tmp_buffer);
			resp++;
		} while ((iResult > 0) && !use_fast_rcv);

		if (iResult == SOCKET_ERROR)
		{
			err_rcv_msg++;
			Log("\n*! Error response: "); Log(DisplayErrorText(WSAGetLastError())); Log("Errors: "); Log_llu(err_rcv_msg);
			cls();
			printf_s("\rget mining info failed: %ld\n", WSAGetLastError());
			closesocket(ConnectSocket);
			can_connect = 1;
			return 0;
		}
		
		Log("\n*   Recieved from server: "); Log_server(buffer); Log("\n*   Count responses: "); Log_u(resp);
		if (show_updates)  printf_s("\nReceived: \n%s\n", buffer);
		
        // locate HTTP header
        char *find = strstr(buffer, "\r\n\r\n");
		//Log("\n*   Парсим : "); Log(find+4);
        if(find == NULL)
		{
			cls();
			printf_s("\r error in message from pool\n");
			can_connect = 1;
			return 0;
		}
		//Log("\n*   Разбираем");
        // Parse result
		char *rbaseTarget, *rheight, *generationSignature;
		
		rbaseTarget = strstr(buffer, "\"baseTarget\":");
		rheight = strstr(buffer, "\"height\":");
		generationSignature = strstr(buffer, "\"generationSignature\":");
		generationSignature = strstr(generationSignature + strlen("\"generationSignature\":"), "\"") + 1; //+1 убираем начальные ковычки
		char* rtargetDeadline = strstr(buffer, "\"targetDeadline\"");

        if(rbaseTarget == NULL || rheight == NULL || generationSignature == NULL)
		{
			cls();
        	printf_s("\r error parsing (1) message from pool\n");
			can_connect = 1;
			return 0;
		}
		//Log("\n*   Проверяем");
		rbaseTarget = strpbrk(rbaseTarget, "0123456789");
		char *endBaseTarget = strpbrk(rbaseTarget, "\"");
		rheight = strpbrk(rheight, "0123456789");
		char *endHeight = strpbrk(rheight, "\"");
        
        char *endGenerationSignature = strstr(generationSignature, "\"");

		if(endBaseTarget == NULL || endHeight == NULL || endGenerationSignature == NULL){
			cls();
        	printf_s("\r error parsing (2) message from pool\n");
			can_connect = 1;
			return 0;
		}

        // Set endpoints
        endBaseTarget[0] = 0;
        endHeight[0] = 0;
        endGenerationSignature[0] = 0;
 
       if( rtargetDeadline!= NULL) {
			rtargetDeadline = strpbrk(rtargetDeadline, "0123456789");
			char* endBaseTarget = strpbrk(rtargetDeadline, "\"");
			if(endBaseTarget == NULL) endBaseTarget = strpbrk(rtargetDeadline, "}");
			endBaseTarget[0] = 0;
			//printf("\rrtargetDeadline: %s\n", rtargetDeadline);
			targetDeadlineInfo = strtoull(rtargetDeadline, 0, 10);
			//Log("\ntargetDeadlineInfo: (%llu)"), Log_llu(targetDeadlineInfo);
		}
		
		unsigned long long  heightInfo = strtoull(rheight, 0, 10);
		if (heightInfo > height)
		{
			height = heightInfo;
			baseTarget = strtoull(rbaseTarget, 0, 10);
			// Parse
			if (xstr2strr(signature, 33, generationSignature) < 0) {
				cls();
				printf_s("\r Node response: Error decoding generationsignature\n");
				fflush(stdout);
				can_connect = 1;
				return 0;
			}
		}
		//printf("\rblock: %llu  BT: %llu   %s\n", height, baseTarget, generationSignature);
		iResult = closesocket(ConnectSocket);
		Log("\n*End update");
		can_connect = 1;
        return 1;
}
 

int pollLocal(void) {
	char buffer[1000];
	char tmp_buffer[1000];
	int iResult;

	can_connect = 0;
	// Get Mininginfo from node:
	ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if (ConnectSocket == INVALID_SOCKET)
	{
		//cls();
		//wprintf(L"\rsocket function failed with error: %ld\n", WSAGetLastError());
		//Log("\n*! Socket error: "); Log(DisplayErrorText(WSAGetLastError()));
		can_connect = 1;
		return 0;
	}

	struct sockaddr_in ss;
	ss.sin_addr.s_addr = inet_addr(updaterip);
	ss.sin_family = AF_INET;
	ss.sin_port = htons(updaterport);

	Log("\n*Connecting");
	iResult = connect(ConnectSocket, (struct sockaddr*)&ss, sizeof(struct sockaddr_in));
	if (iResult == SOCKET_ERROR) {
		//cls();
		//wprintf(L"\rconnect function failed with error: %ld\n", WSAGetLastError());
		//Log("\n*! Connect error: "); Log(DisplayErrorText(WSAGetLastError()));
		//closesocket(ConnectSocket);
		can_connect = 1;
		return 0;
	}

	// wrire some bytes
	all_send_msg++;
	if (all_send_msg >= 1000) all_send_msg = 0;

	int bytes;
	memset(buffer, 0, 1000);
	bytes = sprintf_s(buffer, "POST /burst?requestType=getMiningInfo HTTP/1.0\r\nConnection: close\r\n\r\n");
	
	iResult = send(ConnectSocket, buffer, bytes, 0);
	if (iResult == SOCKET_ERROR)
	{
		err_send_msg++;
		//Log("\n*! Error sending request: "); Log(DisplayErrorText(WSAGetLastError())); Log("Errors: "); Log_llu(err_send_msg);
		//cls();
		//printf_s("\rsend request failed: %ld\n", WSAGetLastError());
		closesocket(ConnectSocket);
		can_connect = 1;
		return 0;
	}
	if (show_updates) printf_s("\nSend: \n%s\n", buffer);
	Log("\n*   Sent to server: "); Log_server(buffer);

	// read some bytes
	all_rcv_msg++;
	if (all_rcv_msg >= 1000) all_rcv_msg = 0;

	memset(buffer, 0, 1000);
	unsigned resp = 0;
	do{
		memset(tmp_buffer, 0, 1000);
		iResult = recv(ConnectSocket, tmp_buffer, 999, 0);
		strcat(buffer, tmp_buffer);
		resp++;
	} while ((iResult > 0) && !use_fast_rcv);

	if (iResult == SOCKET_ERROR)
	{
		err_rcv_msg++;
		//Log("\n*! Error response: "); Log(DisplayErrorText(WSAGetLastError())); Log("Errors: "); Log_llu(err_rcv_msg);
		//cls();
		//printf_s("\rget mining info failed: %ld\n", WSAGetLastError());
		closesocket(ConnectSocket);
		can_connect = 1;
		return 0;
	}

	Log("\n*   Recieved from server: "); Log_server(buffer); Log("\n*   Count responses: "); Log_u(resp);
	if (show_updates)  printf_s("\nReceived: \n%s\n", buffer);

	// locate HTTP header
	char *find = strstr(buffer, "\r\n\r\n");
	//Log("\n*   Парсим : "); Log(find+4);
	if (find == NULL)
	{
		//cls();
		//printf_s("\r error in message from pool\n");
		can_connect = 1;
		return 0;
	}
	//Log("\n*   Разбираем");
	// Parse result
	char *rbaseTarget, *rheight, *generationSignature;

	rbaseTarget = strstr(buffer, "\"baseTarget\":");
	rheight = strstr(buffer, "\"height\":");
	generationSignature = strstr(buffer, "\"generationSignature\":");
	generationSignature = strstr(generationSignature + strlen("\"generationSignature\":"), "\"") + 1; //+1 убираем начальные ковычки
	char* rtargetDeadline = strstr(buffer, "\"targetDeadline\"");

	if (rbaseTarget == NULL || rheight == NULL || generationSignature == NULL)
	{
		//cls();
		//printf_s("\r error parsing (1) message from pool\n");
		can_connect = 1;
		return 0;
	}
	//Log("\n*   Проверяем");
	rbaseTarget = strpbrk(rbaseTarget, "0123456789");
	char *endBaseTarget = strpbrk(rbaseTarget, "\"");
	rheight = strpbrk(rheight, "0123456789");
	char *endHeight = strpbrk(rheight, "\"");

	char *endGenerationSignature = strstr(generationSignature, "\"");

	if (endBaseTarget == NULL || endHeight == NULL || endGenerationSignature == NULL){
		//cls();
		//printf_s("\r error parsing (2) message from pool\n");
		can_connect = 1;
		return 0;
	}

	// Set endpoints
	endBaseTarget[0] = 0;
	endHeight[0] = 0;
	endGenerationSignature[0] = 0;

	if (rtargetDeadline != NULL) {
		rtargetDeadline = strpbrk(rtargetDeadline, "0123456789");
		char* endBaseTarget = strpbrk(rtargetDeadline, "\"");
		if (endBaseTarget == NULL) endBaseTarget = strpbrk(rtargetDeadline, "}");
		endBaseTarget[0] = 0;
		//printf("\rrtargetDeadline: %s\n", rtargetDeadline);
		targetDeadlineInfo = strtoull(rtargetDeadline, 0, 10);
		//Log("\ntargetDeadlineInfo: (%llu)"), Log_llu(targetDeadlineInfo);
	}

	unsigned long long  heightInfo = strtoull(rheight, 0, 10);
	if (heightInfo > height)
	{
		height = heightInfo;
		baseTarget = strtoull(rbaseTarget, 0, 10);
		// Parse
		if (xstr2strr(signature, 33, generationSignature) < 0) {
			//cls();
			//printf_s("\r Node response: Error decoding generationsignature\n");
			//fflush(stdout);
			can_connect = 1;
			return 0;
		}
	}
	//printf("\rblock: %llu  BT: %llu   %s\n", height, baseTarget, generationSignature);
	iResult = closesocket(ConnectSocket);
	Log("\n*End update");
	can_connect = 1;
	return 1;
}


void update() {
	while(can_connect == 0) Sleep(32);
	//pollNode();
	if (pollLocal() == 0) pollNode();
}
 

char * hostname_to_ip(char * hostname)
{
    struct hostent *remoteHost;
    struct in_addr addr;
    int i;
	DWORD dwError;
	char *str;
	
    if ( (remoteHost = gethostbyname( hostname ) ) == NULL)
    {
        // get the host info
        printf_s("\n ERROR gethostbyname %s\n", hostname);
        dwError = WSAGetLastError();
        if (dwError != 0) {
            if (dwError == WSAHOST_NOT_FOUND) {
                printf_s("Host not found\n");
                return NULL;
            } else if (dwError == WSANO_DATA) {
                printf_s("No data record found\n");
                return NULL;
            } else {
                printf_s("Function failed with error: %ld\n", dwError);
                return NULL;
            }
        }
		return NULL;
    }
	
    i = 0;
    if (remoteHost->h_addrtype == AF_INET)
    {
            while (remoteHost->h_addr_list[i] != 0) {
                addr.s_addr = *(u_long *) remoteHost->h_addr_list[i++];
				str = inet_ntoa(addr);
				SetConsoleTextAttribute(hConsole, 5);
                printf_s("Node: %s (ip: %s)\n", hostname, inet_ntoa(addr));
				SetConsoleTextAttribute(hConsole, 7);
            }
    }
    else if (remoteHost->h_addrtype == AF_NETBIOS) printf_s("NETBIOS address was returned\n");  
    
    return str;
}


void GetCPUInfo(void)
{
		bool bIsSSE4Available = InstructionSet::SSE42();
		bool bIsAVX1Available = InstructionSet::AVX();
		bool bIsAVX2Available = InstructionSet::AVX2();
		bool bIsAESAvailable = InstructionSet::AES();
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
		printf_s("\n");
}

int main(int argc, char **argv) {

		pthread_t worker[MaxTreads];
		pthread_t sender;
		unsigned i;

		hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		
		Log_init();

		// path to miner
		Log("Miner path: ");
		DWORD cwdsz = GetCurrentDirectoryA(0, 0);
		p_minerPath = (char*)malloc(cwdsz);
		GetCurrentDirectoryA(cwdsz, LPSTR(p_minerPath));
		strcat(p_minerPath, "\\");
		//printf("\n%s", p_minerPath);
		Log(p_minerPath);

		SetConsoleTextAttribute(hConsole, 11);
		printf_s("\nBURST miner, v1.141020\nProgramming: dcct (Linux) & Blago (Windows)\n");
		SetConsoleTextAttribute(hConsole, 7);
		
		Log("\nLoading config");
		char conf_filename[255];
		memset(conf_filename, 0, 255);
		if((argc >= 2) && (strcmp(argv[1], "-config")==0)){
			if(strstr(argv[2], ":\\")) sprintf(conf_filename,"%s", argv[2]);
			else sprintf(conf_filename,"%s%s", p_minerPath, argv[2]);
		}
		else sprintf(conf_filename,"%s%s", p_minerPath, "miner.conf");
		Log(conf_filename);
		load_config(conf_filename);
		
		
		GetCPUInfo();

		if(miner_mode == 0) pass = GetPass(p_minerPath);

		// адрес и порт сервера
		Log("\nSearching server...");
		WSADATA wsaData;
		
		int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (iResult != 0) {
			printf_s("WSAStartup failed: %d\n", iResult);
			exit(-1);
		}
		updaterip = hostname_to_ip(updateraddr);
		nodeip = hostname_to_ip( nodeaddr );
		
		
        // обнуляем сигнатуру
		memset(oldSignature, 0, 33);
 
 		// Инфа по файлам
		SetConsoleTextAttribute(hConsole, 15);
		printf_s("Using plots:\n");
		SetConsoleTextAttribute(hConsole, 7);
		unsigned long long total_size = 0;
		for(i = 0; i < paths_num; i++)
		{
			char *name[MAX_FILES];
			unsigned long long size[MAX_FILES];
			char * tmp = paths_dir[i]; 
			//printf_s("%s\n", argv[i+4]);
			int count = GetFiles(paths_dir[i], name, size);
			unsigned long long tot_size = 0;
			for(int j = 0; j < count; j++) 	tot_size += size[j]; 
			SetConsoleTextAttribute(hConsole, 7);
			printf_s("%s\tfiles: %u\t size: %llu Gb\n", paths_dir[i], count, tot_size/1024/1024/1024);
			total_size += tot_size;
		}
		SetConsoleTextAttribute(hConsole, 15);
		printf_s("TOTAL: %llu Gb", total_size/1024/1024/1024);
		SetConsoleTextAttribute(hConsole, 7);
		Log("\n------------------------    Start block: ");	Log_llu(height);

		Log("\n\tUpdate mining info");
		// Get startpoint:
        update();
		
		char tbuffer[9];
		_strtime(tbuffer);
		SetConsoleTextAttribute(hConsole, 13);
		printf_s("\n\n--- %s ---    New block %llu, basetarget %llu    ------------\n", tbuffer, height, baseTarget);
		SetConsoleTextAttribute(hConsole, 7);
		if(miner_mode == 0) printf_s("*** Chance to find a block: %.5f%%\n", ((double)(total_size/1024/1024*100*60)*(double)baseTarget)/1152921504606846976);
				
		memset(&shares[0], 0, sizeof(t_shares)*100000*MaxTreads); // Обнуляем буфер шар
		memset(&bests[0], 0, sizeof(t_best)*MaxAccounts); // Обнуляем буфер шар
		sessions.clear();
		// Запускаем поток отпраки
		if(pthread_create(&sender, NULL, send_i, p_minerPath)) 
		{
			SetConsoleTextAttribute(hConsole, 12);
			printf_s("\nError creating thread. Out of memory? Try lower stagger size\n");
			SetConsoleTextAttribute(hConsole, 7);
            exit(-1);
        }

		// Main loop
		for(;;) {
				
                // Get scoop:
                char scoopgen[40];
                memmove(scoopgen, signature, 32);
 
                char *mov = (char*)&height;
 
                scoopgen[32] = mov[7]; scoopgen[33] = mov[6]; scoopgen[34] = mov[5]; scoopgen[35] = mov[4]; scoopgen[36] = mov[3]; scoopgen[37] = mov[2]; scoopgen[38] = mov[1]; scoopgen[39] = mov[0];

				sph_shabal_context x;
				shabal_init(&x, 256);
                shabal_core(&x, (const unsigned char*)scoopgen, 40);
				char xcache[32];
                shabal_close(&x, 0, 0, xcache, 8);


                scoop = (((unsigned char)xcache[31]) + 256 * (unsigned char)xcache[30]) % 4096;
 
                // New block: reset stats
                //best = 0;
				//bestn = 0;
				deadline = 0;
				bytesRead = 0;
 
				unsigned long long targetDeadline;
				if(targetDeadlineInfo > 0){
					targetDeadline = targetDeadlineInfo;
					Log("\nupdate targetDeadline: "); Log_llu(targetDeadline);
				}
				else {
					targetDeadline = 4294967296;
					Log("\ntargetDeadline not found, targetDeadline: "); Log_llu(targetDeadline);
				}
				for(unsigned a=0; a < MaxAccounts; a++)
				{
					bests[a].targetDeadline = targetDeadline;
				}

				for(i = 0; i < paths_num; i++) 
				{
					if(pthread_create(&worker[i], NULL, work_i, &threads[i])) 
					{
						printf_s("\nError creating thread. Out of memory? Try lower stagger size\n");
						Log("\n! Error thread starting: ");	Log_u(i);
                        exit(-1);
                    }
					working_threads++;
				}

		/*		if(miner_mode == 2)
				{
					for(i = 4; i < argc; i++)
					{
					Log("\Кончился поток: ");	Log_u(i-3);
					if( pthread_join(worker[i], NULL) > 0 )
					{
						printf_s("\r Error stoping thread #%u of %u                                 ", i-3, argc-3);
						Log("\n! Ошибка прерывания потока: ");	Log_u(i-3);
					}
					}
					send_V2(argv[0]);
				}
		*/		
                memmove(oldSignature, signature, 32);
 
                // Wait until block changes:
                do {
                        //Log("\n\tUpdate()");
						update();
                        cls();
                        SetConsoleTextAttribute(hConsole, 14);
						if(deadline == 0) printf_s("\r[%llu%%] %llu GB. no deadline     sdl:%llu/%llu(%llu) cdl:%llu(%llu) ss:%llu(%llu) rs:%llu(%llu)", (bytesRead*4096*100 / total_size), (bytesRead / (256 * 1024)), all_send_dl, num_shares-sent_num_shares, err_send_dl, all_rcv_dl, err_rcv_dl, all_send_msg, err_send_msg, all_rcv_msg, err_rcv_msg);
                        else              printf_s("\r[%llu%%] %llu GB. deadline %llus  sdl:%llu/%llu(%llu) cdl:%llu(%llu) ss:%llu(%llu) rs:%llu(%llu)", (bytesRead*4096*100 / total_size), (bytesRead / (256 * 1024)), deadline, all_send_dl, num_shares-sent_num_shares, err_send_dl, all_rcv_dl, err_rcv_dl, all_send_msg, err_send_msg, all_rcv_msg, err_rcv_msg);
						//printf_s("\r[%llu%%] %llu GB. tDL[%llu][%llu] sdl:%llu/%llu(%llu) cdl:%llu(%llu) ss:%llu(%llu) rs:%llu(%llu)", (bytesRead*4096*100 / total_size), (bytesRead / (256 * 1024)), bests[0].targetDeadline, bests[1].targetDeadline, all_send_dl, num_shares-sent_num_shares, err_send_dl, all_rcv_dl, err_rcv_dl, all_send_msg, err_send_msg, all_rcv_msg, err_rcv_msg);
						SetConsoleTextAttribute(hConsole, 7);
                        Sleep(update_interval);
                } while(memcmp(signature, oldSignature, 32) == 0);      // Wait until signature changed
				
				Log("\n------------------------    New block: "); Log_llu(height);

                _strtime(tbuffer);
				SetConsoleTextAttribute(hConsole, 13);
				printf_s("\n\n--- %s ---    New block %llu, basetarget %llu    ------------\n", tbuffer, height, baseTarget);
				SetConsoleTextAttribute(hConsole, 7);
				if(miner_mode == 0) printf_s("*** Chance to find a block: %.5f%%\n", ((double)(total_size/1024/1024*100*60)*(double)baseTarget)/1152921504606846976);
				if(num_shares > sent_num_shares) printf_s("\rshares lost due to new block\n");
				sessions.clear();
				num_shares = 0;  // все шары, которые неуспели отправить сгорают
				sent_num_shares = 0;
				memset(&shares[0], 0, sizeof(t_shares)*100000*MaxTreads); // Обнуляем буфер шар
				memset(&bests[0], 0, sizeof(t_best)*MaxAccounts); // Обнуляем буфер шар

                // Tell all threads to stop:
                stopThreads = 1;
                for(i = 0; i < paths_num; i++)
				{
					//Log("\nПрерываем поток: ");	Log_u(i-3);
					if( pthread_join(worker[i], NULL) > 0 )
					{
						cls();
						SetConsoleTextAttribute(hConsole, 12);
						printf_s("\r Error stoping thread #%u of %u", i, paths_num);
						SetConsoleTextAttribute(hConsole, 7);
						Log("\n! Error thread stoping: ");	Log_u(i);
					}
				}
				working_threads = 0;
				stopThreads = 0;
        }
		pthread_join(sender, NULL);
		free(p_minerPath);
		WSACleanup();
}
