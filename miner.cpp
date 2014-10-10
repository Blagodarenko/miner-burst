
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

#include "sph_shabal.h"
#include "shabal.c"

#include "rapidjson/document.h"		// rapidjson's DOM-style API
#include "rapidjson/prettywriter.h"	// for stringify JSON
#include "rapidjson/filestream.h"	// wrapper of C stream for prettywriter as output
using namespace rapidjson;


#define strtoll     _strtoi64
#define strtoull    _strtoui64

#pragma warning( disable : 4715 4996)

// These are fixed for BURST. Dont change!
#define HASH_SIZE       32
#define HASH_CAP        4096
#define PLOT_SIZE       (HASH_CAP * HASH_SIZE * 2)
//#define MAXDEADLINE		1000000 
// Read this many nonces at once. 100k = 6.4MB per directory/thread.
// More may speedup things a little.
//#define CACHESIZE       200000  // 100000*64 б
#define MAX_FILES		1000 
#define MaxTreads		24


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
 
char nodeaddr[100] = "127.0.0.1";	// адрес пула
char *nodeip = "127.0.0.1";		// IP пула
unsigned nodeport = 8125;		// порт пула
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
unsigned long long max_deadline = 86400*10; // максимальный дедлайн (секунд_в_сутках * 10 дней)
bool use_response_max_time = false;
unsigned long response_max_time = 60;	// максимальное время ожидания ответа сервера (секунд)
SYSTEMTIME cur_time;			// Текущее время
unsigned work_done = 0;			// Поток закончил работу

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
} shares[10000*MaxTreads];

unsigned long long num_shares = 0;
unsigned sent_num_shares = 0;
//unsigned lost_num_shares = 0;

unsigned long long bytesRead = 0;
unsigned long long height = 0;
unsigned long long baseTarget = 0;
unsigned long long targetDeadline = 0; // Максимальный дедлайн пула
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
	
	char json[1024];
	fopen_s(&pFile, filename, "rt");
	if (pFile==NULL) {
		printf_s("\nminer.conf not found");
		return -1;
	}
	
	len = 0;
	int i = -1;
	memset(json, 0, 1024);
	do{
		i++;
		len += fread (&json[i],sizeof(char),1,pFile);
	}while(json[i] != EOF);
	json[i] = 0;
	
	fclose(pFile);
//	free(filename);
  //printf("\n%s",json);

	Document document;	// Default template parameter uses UTF8 and MemoryPoolAllocator.
	if (document.Parse<0>(json).HasParseError()){
		printf("\nJSON format error\n");
		return 0;
	}

	if (document.ParseInsitu<0>(json).HasParseError()){
		printf("\nJSON format error (Insitu)\n");
		return 0;
	}

	
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
		DWORD dwBytesWritten; 
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
  if (pFile==NULL) {printf_s("passphrases.txt not found"); exit (1);}

  fseek (pFile , 0 , SEEK_END);
  lSize = ftell (pFile);
  rewind (pFile);

  buffer = (char*) malloc (sizeof(char)*(lSize));
  if (buffer == NULL) {printf_s("Memory error"); exit (2);}
  
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


//int comp(t_shares a, t_shares b)
//{
//	return (b.best > a.best) ;
//}


void *send_i(void *x_void_ptr) 
{
	int iResult;
	char buffer[1000];
	char tmp_buffer[1000];
	unsigned int i=0;
	for (;; i++ )
	{
		if(i > num_shares) i = 0;
		
		while(num_shares == sent_num_shares){
			Sleep(10);
		}


		if( (can_connect == 1) && (shares[i].to_send == 1))
		{
			if((num_shares - i > 1) && (sent_num_shares != num_shares) && use_sorting){
				//std::sort(&shares[i], &shares[num_shares-1], comp);
				std::sort(&shares[i], &shares[num_shares-1]);
				//printf_s("\rSorted------ %u - %u ---------\n", i, num_shares);
			}
	
			if((targetDeadline > 0) && ((shares[i].best / baseTarget) > targetDeadline))
			{
				shares[i].to_send = 0;
				sent_num_shares++;
				//cls();
				//printf_s("\rdeadline > targetDeadline. %llu > %llu\tn:%llu\n", shares[i].best / baseTarget, targetDeadline, shares[i].nonce);
				Log("\nSender: Found deadline, but it's more server's minimum. Deadline ="); Log_llu(shares[i].best / baseTarget); 
				//continue;
			}
			else   
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
					timeval tv;
					tv.tv_sec = response_max_time;
					tv.tv_usec = 0;
					setsockopt(ConnectSocket, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(struct timeval));
				}
				//Log("\nSender: Коннектим отправщик");
				iResult = connect(ConnectSocket, (struct sockaddr*)&ss, sizeof(struct sockaddr_in));
				if (iResult == SOCKET_ERROR) 
				{
					Log("\nSender:! Error Sender's connect: "); Log(DisplayErrorText(WSAGetLastError()));
					cls();
					wprintf(L"\rconnect tread failed with error: %ld\n", WSAGetLastError());
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
					cls();
					printf_s("\rsending... deadline: %u  (nonce:%llu) acc# %llu", shares[i].best / baseTarget, shares[i].nonce, shares[i].account_id);

					// write some bytes
					all_send_dl++;
					//Log("\nSender:  Отправляем шару");
					iResult = send(ConnectSocket, buffer, bytes, 0);
					if (iResult == SOCKET_ERROR)
					{
						err_send_dl++;
						Log("\nSender: ! Error deadline's sending: "); Log(DisplayErrorText(WSAGetLastError()));
						cls();
						printf_s("\rsend failed: %ld\n", WSAGetLastError());
					}
					else
					{
						if (show_msg) printf_s("\nsend: %s\n", buffer); // показываем послание
						Log("\nSender:   Sent to server: "); Log_server(buffer);
						

						// read some bytes
						all_rcv_dl++;
						//Log("\nSender:  Получаем подтверждение шары");
						memset(buffer,0,1000);
						
						cls();
						printf_s("\rrecieving data...");
						do{
							memset(tmp_buffer,0,1000);
							iResult = recv(ConnectSocket, tmp_buffer, 999, 0);
							strcat(buffer, tmp_buffer);
						}while((iResult > 0));
						
						if (iResult == SOCKET_ERROR)
						{
							err_rcv_dl++;
							Log("\nSender: ! Error getting deadline's confirmation: "); Log(DisplayErrorText(WSAGetLastError()));
							cls();
							printf_s("\rreceiving confirmation failed: %ld\n", WSAGetLastError());
						}
						else
						{
							if (show_msg) printf_s("\nReceived: %s\n", buffer);  // показываем ответ
							Log("\nSender:   Recieved from server: "); Log_server(buffer);
							
							// locate HTTP header
							char *find = strstr(buffer, "\r\n\r\n");
							//Log("\nSender:   Парсим строку: "); Log(find+4);
							//cls();
							//printf_s("\%s\n",find+4);
							if(find != NULL) 
							{
								char *rdeadline = strstr(find + 4, "\"deadline\"");
						/*		if(miner_mode == 2)  // Для V2 пула
								{
									if(strstr(buffer, "OK") != NULL)
									{
										shares[i].to_send = 0;
										sent_num_shares++;
										deadline = shares[i].best;
										cls();
										printf_s("\rsent deadline: %llu\t(%s)\n", shares[i].best / baseTarget, shares[i].file_name);
									}
								} 
								else */
								if(rdeadline != NULL) 
								{
									rdeadline = strstr(rdeadline + 4, ":");
									rdeadline++;
									char *end = strstr(rdeadline, "}");
									if(end != NULL) 
									{
										// Parse and check if we have a better deadline
										unsigned long long ndeadline = strtoull(rdeadline, 0, 10);
										char* rtargetDeadline = strstr(buffer, "\"targetDeadline\":");
										if( rtargetDeadline!= NULL)
										{
											rtargetDeadline = strpbrk(rtargetDeadline, "0123456789");
											char* endBaseTarget = strpbrk(rtargetDeadline, "\"");
											if(endBaseTarget == NULL) endBaseTarget = strpbrk(rtargetDeadline, "}");
											endBaseTarget[0] = 0;
											//printf("\rrtargetDeadline: %s\n", rtargetDeadline);
											targetDeadline = strtoull(rtargetDeadline, 0, 10);
										}
										Log("\nSender:  deadline confirmed: "); Log_llu(ndeadline);
										cls();
										printf_s("\rconfirmed deadline: %llu\t(%s)\n", ndeadline, shares[i].file_name);
										if(ndeadline < deadline || deadline == 0)  deadline = ndeadline;
										//pthread_mutex_lock(&byteLock);
											shares[i].to_send = 0;
											sent_num_shares++;
										//pthread_mutex_unlock(&byteLock);
										
										//if(deadline != (shares[i].best / baseTarget)) printf_s("%u/%u---------------------------\nSent deadline:\t%llu\nServer's deadline:\t%llu \n---------------------------file: %s\n", i, num_shares, shares[i].best / baseTarget, deadline, shares[i].file_name);								
									}
								}
								else{
									printf_s("\r%s\n", find+4);
									shares[i].to_send = 0;
									sent_num_shares++;
								}
							}
						}
					}
				}
			
			//	iResult = shutdown(ConnectSocket, 2);
			//	Log("\nSender: Обрубаем сокет отправщика. Код = "); Log_u(WSAGetLastError());
			
				iResult = closesocket(ConnectSocket);
				Log("\nSender:Close socket. Code = "); Log_u(WSAGetLastError());
				can_connect = 1;
				Sleep(send_interval);
			}
		}
	}
	return 0;
}

/*

void send_V2(void *x_void_ptr) 
{
	int iResult;
	//char buffer[10000];
	unsigned int i=0;
	//for (;; i++ )
	{
		//if(i > num_shares) i = 0;
		
		//if( (can_connect == 1) && (shares[i].to_send == 1))
		{
			//if((miner_mode == 2) && ((shares[i].best / baseTarget) > targetDeadline))
			//{
				//cls();
				//printf_s("\rfound deadline \"%llu\", but it's bigger pool's targetDeadline \"%llu\"\n", shares[i].best / baseTarget, targetDeadline);
				//shares[i].to_send = 0;
				//sent_num_shares++;
				//Log("\nSender: Найден дедлайн, но он больше минимума сервера. Deadline ="); Log_llu(shares[i].best / baseTarget); 
				//continue;
			//}
			//else
			{
				can_connect = 0;
				//Log("\n Открываем сокет отправщика");
				ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
 
				struct sockaddr_in ss;
				ss.sin_addr.s_addr = inet_addr( nodeip );
				ss.sin_family = AF_INET;
				ss.sin_port = htons(nodeport);
								
				//timeval tv;
				//tv.tv_sec = 10;  // 30 sec timeout
				//tv.tv_usec = 0;
				//setsockopt(ConnectSocket, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(struct timeval));
				Log("\nSender: Коннектим отправщик");
				iResult = connect(ConnectSocket, (struct sockaddr*)&ss, sizeof(struct sockaddr_in));
				if (iResult == SOCKET_ERROR) 
				{
					Log("\nSender:! Ошибка коннекта отправщика");
					cls();
					wprintf(L"\rconnect tread failed with error: %ld\n", WSAGetLastError());
				}
				else
				{
					int bytes;
				//	if(miner_mode == 0)	bytes = sprintf_s(buffer, "POST /burst?requestType=submitNonce&secretPhrase=%s&nonce=%llu HTTP/1.0\r\nConnection: close\r\n\r\n",pass ,shares[i].nonce);
				//	if(miner_mode == 1)	bytes = sprintf_s(buffer, "POST /burst?requestType=submitNonce&accountId=%llu&secretPhrase=%s&nonce=%llu HTTP/1.0\r\nConnection: close\r\n\r\n",shares[i].account_id ,pass ,shares[i].nonce);
				//	if(miner_mode == 2)
					//{
						int len = 0;
						char *str_len = (char*) malloc(10);
						char *f1 = (char*) malloc(100*50);
						char *buffer = (char*) malloc(100*500);
						for(i=0; i<num_shares; i++)
						{	
							shares[i].to_send = 0;	
							if((shares[i].best / baseTarget) <= targetDeadline)
								len += sprintf(&f1[len], "%llu:%llu\n", shares[i].account_id, shares[i].nonce);
								
							printf("%llu  %u\n", shares[i].best / baseTarget, len);
						}
						_itoa_s(len, str_len, 10, 10);
						bytes = sprintf(buffer, "POST /pool/submitWork HTTP/1.0\r\nHost: %s:%i\r\nContent-Type: text/plain;charset=UTF-8\r\nContent-Length: %i\r\n\r\n{\"%s\", %u}", nodeip, nodeport, len + 6 + strlen(str_len) , f1, len);
						free(f1);
						free(str_len);
					//}
					buffer[bytes] = 0;
					//printf_s("\rsending... deadline: %i  (nonce:%llu)                         ", shares[i].best / baseTarget, shares[i].nonce);

					// write some bytes
					all_send_dl++;
					Log("\nSender:  Отправляем шару");
					iResult = send(ConnectSocket, buffer, bytes, 0);
					if (iResult == SOCKET_ERROR)
					{
						err_send_dl++;
						Log("\nSender: ! Ошибка отправки шары");
						cls();
						printf_s("\rsend failed: %d\n", WSAGetLastError());
					}
					else
					{
						if (show_stat == 1) printf_s("\nsend: %s\n", buffer); // показываем послание
						Log("\nSender:   Sent to server: "); Log_server(buffer);
						//iResult = shutdown(ConnectSocket, SD_SEND);

						// read some bytes
						all_rcv_dl++;
						Log("\nSender:  Получаем подтверждение шары");
						memset(buffer,0,1000);
						iResult = recv(ConnectSocket, buffer, 999, 0);
						buffer[iResult] = 0;
						if (iResult == 0)
						{
							err_rcv_dl++;
							Log("\nSender: ! Ошибка получения подтверждения шары");
							cls();
							printf_s("\rreceive failed: %d\n", WSAGetLastError());
						}
						else
						{
							if (show_stat == 1)  // показываем ответ
								printf_s("\nReceived: %s\n", buffer);
							
							Log("\nSender:   Recieved from server: "); Log_server(buffer);
							
							// locate HTTP header
							char *find = strstr(buffer, "\r\n\r\n");
							Log("\nSender:   Парсим");
							cls();
							printf_s("\%s\n",find+4);
							if(find != NULL) 
							{
								char *rdeadline = strstr(find + 4, "\"deadline\"");
								if(miner_mode == 2)  // Для V2 пула
								{
									if(strstr(buffer, "OK") != NULL)
									{
										shares[i].to_send = 0;
										sent_num_shares++;
										deadline = shares[i].best;
										cls();
										printf_s("\rsent deadline: %llu\t(%s)\n", shares[i].best / baseTarget, shares[i].file_name);
									}
								} 
								else 
								if(rdeadline != NULL) 
								{
									rdeadline = strstr(rdeadline + 4, ":");
									rdeadline++;
									char *end = strstr(rdeadline, "}");
									if(end != NULL) 
									{
										// Parse and check if we have a better deadline
										unsigned long long ndeadline = strtoull(rdeadline, 0, 10);
										Log("\nSender:  Подтверждение шары: "); Log_llu(ndeadline);
										cls();
										printf_s("\rconfirmed deadline: %llu\t(%s)\n", ndeadline, shares[i].file_name);
										if(ndeadline < deadline || deadline == 0)  deadline = ndeadline;
										//pthread_mutex_lock(&byteLock);
											shares[i].to_send = 0;
											sent_num_shares++;
										//pthread_mutex_unlock(&byteLock);
										if(deadline != (shares[i].best / baseTarget)) printf_s("\nYour deadline does was not confirmed by server, proebali.\nSent deadline: %llu\nServer's deadline: %llu \nfile: %s\n", shares[i].best / baseTarget, deadline, shares[i].file_name);								
									}
								}
							}
						}
					}
				}
			
				iResult = shutdown(ConnectSocket, 2);
				Log("\nSender: Обрубаем сокет отправщика. Код = "); Log_u(WSAGetLastError());
			
				iResult = closesocket(ConnectSocket);
				Log("\nSender:Закрываем сокет отправщика. Код = "); Log_u(WSAGetLastError());
				can_connect = 1;
				Sleep(update_interval);
			}
		}
	}
	//return 0;
}

*/

unsigned long long procscoop(unsigned long long nonce, unsigned long long n, char *data, unsigned long long best_local, unsigned long long bestn_local, unsigned long long account_id, char * file_name) {
        char *cache;
        char sig[32 + 64];
        cache = data;
        unsigned long long v;
 
        memmove(sig, signature, 32);
 
        for( v=0; v<n; v++) {
                memmove(&sig[32], cache, 64);
                char res[32];
 
                sph_shabal_context x;
				shabal_init(&x, 256);
                shabal_core(&x, (const unsigned char*) sig, 64 + 32);
                shabal_close(&x, 0, 0, res, 8);
				 
                unsigned long long *wertung = (unsigned long long*)res;
 
                if(bestn_local == 0 || *wertung < best_local) 
				{
                    best_local = *wertung;
                    bestn_local = nonce+v;
					    if(best_local < baseTarget * max_deadline) // Has to be this good before we inform the node
						{       
							Log("\nfound deadline=");	Log_llu(best_local/baseTarget); Log("\nnonce=");	Log_llu(bestn_local); Log(" for account: "); Log_llu(account_id);
							pthread_mutex_lock(&byteLock);
								shares[num_shares].best = best_local;
								shares[num_shares].nonce = bestn_local;
								shares[num_shares].to_send = 1;
								shares[num_shares].account_id = account_id;
								shares[num_shares].file_name = file_name;
								num_shares++;
							pthread_mutex_unlock(&byteLock);
                        }
				}
                nonce++;
                cache += 64;
        }
		
		return v;
}
 


void *work_i(void *x_void_ptr) {
		clock_t start_work_time, end_work_time;
		start_work_time = clock();
		FILE * pFile;
		char *x_ptr = (char*)malloc(sizeof(char)*255);
		memset(x_ptr, 0, sizeof(char)*(255));
		strcpy(x_ptr, (char*)x_void_ptr);

		char *cache = (char*) malloc(cache_size * HASH_SIZE * 2);
		clock_t start_time, end_time;			// Текущее время
		Log("\nThread started");
        if(cache == NULL) {
			cls();
            printf_s("\nError allocating memory\n");
            exit(-1);
        }
		unsigned long long best_local = 0;
		unsigned long long bestn_local = 0;
		
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
				printf_s("\r! File \"%s\" have wrong size, skipped", files[files_count]);
				continue;   // если размер файла не соответствует нонсам в имени файла - пропускаем
			}
			Log("\nRead file: ");	Log(files[files_count]);
			start_time = clock();

			_set_fmode(_O_BINARY);
			fopen_s(&pFile, fullname, "rb");
			if (pFile==NULL)
			{
				printf_s("\rfile \"%s\" error opening", files[files_count]);
				continue;
			}
                        
			unsigned long long size = stagger * 64;
												
			for(n=0; n<nonces; n+=stagger) 
			{
				// Read one Scoop out of this block:
                // start to start+size in steps of CACHESIZE * HASH_SIZE * 2
				unsigned long long start = n * HASH_CAP * 64 + scoop * size;
				unsigned long long i;
				unsigned long long noffset = 0;
                                        
				for(i = start; i < start + size; i += cache_size * 64) 
				{
					//printf("start: %llu end:%llu step:%llu  ",i,start + size, CACHESIZE * HASH_SIZE * 2);
					unsigned long long readsize = cache_size * 64;
					if(readsize > start + size - i)	readsize = (unsigned int)(start + size - i);
					unsigned long long bytes = 0, b;

					rewind (pFile);
					_fseeki64(pFile , i , SEEK_SET);
					do {
						b = fread (&cache[bytes],sizeof(char),readsize - bytes,pFile);
						bytes += b;
					} while(bytes < readsize && b > 0);     // Read until cache is filled (or file ended)
					
					if(b > 0)
					{
						procscoop(n + nonce + noffset, readsize / 64, cache, best_local, bestn_local, key, files[files_count]);// Process block
						// Lock and add to totals
						pthread_mutex_lock(&byteLock);
						bytesRead += readsize;
						pthread_mutex_unlock(&byteLock);
					}
					else printf("\r****  b=%llu, readsize=%llu, n=%llu, nonce=%llu\n", b, readsize, n, nonce);
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
		end_work_time = clock();
		cls();
		printf_s("\rThread \"%s\" done! [~%llu sec] (%u files)\n", x_ptr, (end_work_time-start_work_time)/CLOCKS_PER_SEC, files_count);
		free(x_ptr);
        free(cache);
}

 
int pollNode(void) {
	char buffer[1000];      // Should be plenty
	char tmp_buffer[1000];      // Should be plenty
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
 
		if(use_response_max_time)
		{
			timeval tv;
			tv.tv_sec = response_max_time;
			tv.tv_usec = 0;
			setsockopt(ConnectSocket, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(struct timeval));
		}
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
		//Log("\n*  Отправка запроса. Всего отправлено запросов: "); Log_llu(all_send_msg);
		
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
		//Log("\n*  Получение ответа. Всего получено ответов: "); Log_llu(all_rcv_msg);
		memset(buffer,0,1000);
		
		unsigned pos = 0;
		do{
			memset(tmp_buffer,0,1000);
			iResult = recv(ConnectSocket, tmp_buffer, 999, 0);
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
			strcat(buffer, tmp_buffer);
			//pos += iResult; 
		}while(iResult != 0);
		Log("\n*   Recieved from server: "); Log_server(buffer);
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
 
        // Parse
		if(xstr2strr(signature, 33, generationSignature) < 0) {
			cls();
			printf_s("\r Node response: Error decoding generationsignature\n");
            fflush(stdout);
			can_connect = 1;
            return 0;
        }
		//Log("\n*   Обновляем");
	
		//char* rtargetDeadline = strstr(buffer, "\"targetDeadline\"");
		//printf("\rrtargetDeadline: %s\n", buffer);
		if( rtargetDeadline!= NULL) {
			rtargetDeadline = strpbrk(rtargetDeadline, "0123456789");
			char* endBaseTarget = strpbrk(rtargetDeadline, "\"");
			if(endBaseTarget == NULL) endBaseTarget = strpbrk(rtargetDeadline, "}");
			endBaseTarget[0] = 0;
			//printf("\rrtargetDeadline: %s\n", rtargetDeadline);
			targetDeadlineInfo = strtoull(rtargetDeadline, 0, 10);
			//Log("\ntargetDeadline: (%llu)"), Log_llu(targetDeadlineInfo);
		}
		
		height = strtoull(rheight, 0, 10);
        baseTarget = strtoull(rbaseTarget, 0, 10);
		//printf("\rblock: %llu  BT: %llu   %s\n", height, baseTarget, generationSignature);
		//Log("\n* Закрываем сокет");
		iResult = closesocket(ConnectSocket);
		Log("\n*End update");
		can_connect = 1;
        return 1;
}
 
void update() {
	while(can_connect == 0) Sleep(50);
	pollNode();
}
 

char * hostname_to_ip(char * hostname)
{
    struct hostent *remoteHost;
    struct in_addr addr;
    int i, iResult;
	DWORD dwError;
    WSADATA wsaData;
	char *str;
	
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf_s("WSAStartup failed: %d\n", iResult);
        return NULL;
    }
	
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
                printf_s("Node: %s (ip: %s)\n", hostname, inet_ntoa(addr));
            }
    }
    else if (remoteHost->h_addrtype == AF_NETBIOS) printf_s("NETBIOS address was returned\n");  
    
    return str;
}


int main(int argc, char **argv) {

		pthread_t worker[MaxTreads];
		pthread_t sender;
		unsigned i;
		
		Log_init();

		// находим путь к майнеру
		Log("Miner path: ");
		DWORD cwdsz = GetCurrentDirectoryA(0, 0);
		p_minerPath = (char*)malloc(cwdsz);
		GetCurrentDirectoryA(cwdsz, LPSTR(p_minerPath));
		strcat(p_minerPath, "\\");
		//printf("\n%s", p_minerPath);
		Log(p_minerPath);

		// проверка на количество аргументов
		printf_s("\nBURST miner, v1.141009\n Programming: dcct (Linux) & Blago (Windows)\n");
/*		if(argc < 4) {
                printf_s("Usage: miner.exe solo <node url> <node port> <plot dir> [<plot dir> ..]\n");
				printf_s("or: miner.exe pool <node url> <node port> <plot dir> [<plot dir> ..]\n");
				//printf_s("or: miner.exe poolV2 <node url> <node port> <plot dir> [<plot dir> ..]\n");
                exit(-1);
        }
*/
		Log("\nLoading config");
		char conf_filename[255];
		memset(conf_filename, 0, 255);
		if((argc >= 2) && (strcmp(argv[1], "-config")==0)){
			if(strstr(argv[2], ":\\")) sprintf(conf_filename,"%s", argv[2]);
			else sprintf(conf_filename,"%s%s", p_minerPath, argv[2]);
		}
		else sprintf(conf_filename,"%s%s", p_minerPath, "miner.conf");
		load_config(conf_filename);
		
		

/*		// определяем тип: соло или пул
		if(strcmp(argv[1], "solo")==0)
		{
			miner_mode = 0;
			pass = GetPass(p_minerPath);
		}
		else
			if(strcmp(argv[1], "pool")==0)
			{
				miner_mode = 1;
			}
			else 
				if(strcmp(argv[1], "solo+show")==0)
				{
					miner_mode = 0;
					pass = GetPass(p_minerPath);
					show_stat = 1;
				}
				else
					if(strcmp(argv[1], "pool+show")==0) 
					{
						miner_mode = 1;
						show_stat = 1;
					}
					else
						if(strcmp(argv[1], "poolV2")==0)
						{
							miner_mode = 2;
							exit(-1);
						}
						else
							if(strcmp(argv[1], "poolV2+show")==0) 
							{
								miner_mode = 2;
								show_stat = 1;
								exit(-1);
							}
							else
							{ 
								printf_s("\n use parameter \"solo\" or \"pool\" \n");
								exit(-1);
							}
*/	
		if(miner_mode == 0) pass = GetPass(p_minerPath);

		// адрес и порт сервера
		nodeip = hostname_to_ip( nodeaddr);
		//nodeport = atoi(argv[3]);
		
        // обнуляем сигнатуру
		memset(oldSignature, 0, 33);
 
 		// Инфа по файлам
		printf_s("Using plots:\n");
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
			printf_s("%s\tfiles: %u\t size: %llu Gb\n", paths_dir[i], count, tot_size/1024/1024/1024);
			total_size += tot_size;
		}
		printf_s("\t \t \t \t \t \tTOTAL: %llu Gb", total_size/1024/1024/1024);

		Log("\n------------------------    Start block: ");	Log_llu(height);

		Log("\n\tUpdate mining info");
		// Get startpoint:
        update();
		if(miner_mode) max_deadline = 0xFFFFFFFFFFFFFFFE;		
		printf_s("\n\n--------------    New block %llu, basetarget %llu    --------------\n", height, baseTarget);
		if(miner_mode == 0) printf_s("*** Chance to find a block: %.5f%%\n", ((double)(total_size/1024/1024*100*60)*(double)baseTarget)/1152921504606846976);
				
		memset(&shares, 0, sizeof(t_shares)*100); // Обнуляем буфер шар

	
		// Запускаем поток отпраки
		if(pthread_create(&sender, NULL, send_i, p_minerPath)) 
		{
			printf_s("\nError creating thread. Out of memory? Try lower stagger size\n");
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

                scoop = (((unsigned char)xcache[31]) + 256 * (unsigned char)xcache[30]) % HASH_CAP;
 
                // New block: reset stats
                //best = 0;
				//bestn = 0;
				deadline = 0;
				bytesRead = 0;
 
				if(targetDeadlineInfo > 0){
					targetDeadline = targetDeadlineInfo;
					Log("\nupdate targetDeadline: "); Log_llu(targetDeadline);
				}
				else {
					targetDeadline = baseTarget;
					Log("\ntargetDeadline did not use, baseTarget,: "); Log_llu(baseTarget);
				}

				for(i = 0; i < paths_num; i++) 
				{
					Log("\nStart thread #");	Log_u(i); Log(" with parameter: ");	Log(paths_dir[i]);
					if(pthread_create(&worker[i], NULL, work_i, paths_dir[i])) 
					{
						printf_s("\nError creating thread. Out of memory? Try lower stagger size\n");
						Log("\n! Error thread starting: ");	Log_u(i);
                        exit(-1);
                    }
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
                        //cls();
                        //if(deadline == 0) printf_s("\r[%llu%%] %llu GB. no deadline     sdl:%llu/%llu(%llu) cdl:%llu(%llu) ss:%llu(%llu) rs:%llu(%llu)", (bytesRead*4096*100 / total_size), (bytesRead / (256 * 1024)), all_send_dl, num_shares, err_send_dl, all_rcv_dl, err_rcv_dl, all_send_msg, err_send_msg, all_rcv_msg, err_rcv_msg);
                        //else              printf_s("\r[%llu%%] %llu GB. deadline %llus  sdl:%llu/%llu(%llu) cdl:%llu(%llu) ss:%llu(%llu) rs:%llu(%llu)", (bytesRead*4096*100 / total_size), (bytesRead / (256 * 1024)), deadline, all_send_dl, num_shares, err_send_dl, all_rcv_dl, err_rcv_dl, all_send_msg, err_send_msg, all_rcv_msg, err_rcv_msg);
						printf_s("\r[%llu%%] %llu GB. targetDL: %llu  sdl:%llu/%llu(%llu) cdl:%llu(%llu) ss:%llu(%llu) rs:%llu(%llu)", (bytesRead*4096*100 / total_size), (bytesRead / (256 * 1024)), targetDeadline, all_send_dl, num_shares, err_send_dl, all_rcv_dl, err_rcv_dl, all_send_msg, err_send_msg, all_rcv_msg, err_rcv_msg);
						
                        Sleep(update_interval);
                } while(memcmp(signature, oldSignature, 32) == 0);      // Wait until signature changed
				
				Log("\n------------------------    New block: "); Log_llu(height);

                
				printf_s("\n\n--------------    New block %llu, basetarget %llu    --------------\n", height, baseTarget);
				if(miner_mode == 0) printf_s("*** Chance to find a block: %.5f%%\n", ((double)(total_size/1024/1024*100*60)*(double)baseTarget)/1152921504606846976);
				if(num_shares > sent_num_shares) printf_s("\r %u shares had not time to send in the previous block\n", num_shares - sent_num_shares);
				num_shares = 0;  // все шары, которые неуспели отправить сгорают
				sent_num_shares = 0;
				memset(&shares, 0, sizeof(t_shares)*1000*MaxTreads); // Обнуляем буфер шар
				
                // Tell all threads to stop:
                stopThreads = 1;
                for(i = 0; i < paths_num; i++)
				{
					//Log("\nПрерываем поток: ");	Log_u(i-3);
					if( pthread_join(worker[i], NULL) > 0 )
					{
						cls();
						printf_s("\r Error stoping thread #%u of %u", i, paths_num);
						Log("\n! Error thread stoping: ");	Log_u(i);
					}
				}
				stopThreads = 0;
        }
		pthread_join(sender, NULL);
		free(p_minerPath);
		WSACleanup();
}
