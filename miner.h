#define RAPIDJSON_NO_SIZETYPEDEFINE

namespace rapidjson { typedef size_t SizeType; }
using namespace rapidjson;

#include "rapidjson/document.h"		// rapidjson's DOM-style API
#include "rapidjson/error/en.h"


#include <string.h>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <map>
#include <vector>
#include <thread>

#pragma comment(lib,"Ws2_32.lib")
#include <ws2tcpip.h>
#include <mswsock.h> // Need for SO_UPDATE_CONNECT_CONTEXT

#include "curses.h" 
//#include "panel.h" 
#include "sph_shabal.h"
#include "mshabal.h"
#include "mshabal256.h"
#include "shabal_asm.h"
#include "InstructionSet.h"
#include "picohttpparser.h"

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

HANDLE hHeap;

bool exit_flag = false;
#ifdef __AVX2__
	char const *const version = "v1.170820_AVX2";
#else
	#ifdef __AVX__
		char const *const version = "v1.170820_AVX";
	#else
		char const *const version = "v1.170820";
	#endif
#endif 

unsigned long long startnonce = 0;
unsigned long nonces = 0;
unsigned int scoop = 0;
unsigned long long deadline = 0;
int network_quality = 100;
char signature[33];
char str_signature[65];
char oldSignature[33];
unsigned long long height = 0;
unsigned long long baseTarget = 0;
unsigned long long targetDeadlineInfo = 0;			// Максимальный дедлайн пула
unsigned long long my_target_deadline = MAXDWORD;	// 4294967295;
volatile int stopThreads = 0;
char *pass = nullptr;							// пароль

std::string nodeaddr = "localhost";	// адрес пула
std::string nodeport = "8125";		// порт пула

std::string updateraddr = "localhost";// адрес пула
std::string updaterport = "8125";		// порт пула

std::string infoaddr = "localhost";	// адрес пула
std::string infoport = "8125";		// порт пула

std::string proxyport = "8125";		// порт пула

char *p_minerPath = nullptr;		// путь к папке майнера
size_t miner_mode = 0;				// режим майнера. 0=соло, 1=пул
size_t cache_size = 100000;			// размер кэша чтения плотов
std::vector<std::string> paths_dir; // пути
//bool show_msg = false;				// Показать общение с сервером в отправщике
//bool show_updates = false;			// Показать общение с сервером в апдейтере
FILE * fp_Log = nullptr;			// указатель на лог-файл
size_t send_interval = 100;			// время ожидания между отправками
size_t update_interval = 1000;		// время ожидания между апдейтами
short win_size_x = 80;
short win_size_y = 60;
//bool use_fast_rcv = false;
bool use_debug = false;
bool enable_proxy = false;
//bool send_best_only = true;
bool use_wakeup = false;
bool use_log = true;				// Вести лог
bool use_boost = false;				// Использовать повышенный приоритет для потоков
bool show_winner = false;			// показывать победителя
//short can_generate = 0;				// 0 - disable; 1 - can start generate; 2 - already run generator


SYSTEMTIME cur_time;				// Текущее время
unsigned long long total_size = 0;	// Общий объем плотов

WINDOW * win_main;
//PANEL  *panel_main;

std::vector<std::thread> worker;

struct t_worker_progress{
	size_t Number;
	unsigned long long Reads_bytes;
	bool isAlive;
};

std::vector<t_worker_progress> worker_progress;

std::map <u_long, unsigned long long> satellite_size; // Структура с объемами плотов сателлитов

struct t_files{
	std::string Path;
	std::string Name;
	unsigned long long Size;// = 0;
	unsigned long long Key;
	unsigned long long StartNonce;
	unsigned long long Nonces;
	unsigned long long Stagger;
	//unsigned State;// = 0;
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
//	unsigned long long ID;
	unsigned long long deadline;
	t_shares body;
	//	unsigned long long best;// = 0;
//	unsigned long long nonce;// = 0;
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


CRITICAL_SECTION sessionsLock;	// обновление sessions
CRITICAL_SECTION bestsLock;		// обновление bests
CRITICAL_SECTION sharesLock;	// обновление shares

// ========== HEADERS ==========
void ShowMemErrorExit(void);
void Log_init(void);
void Log(char const *const strLog);
void Log_server(char const *const strLog);
void Log_llu(unsigned long long const llu_num);
void Log_u(size_t const u_num);
int load_config(char const *const filename);
//LPSTR DisplayErrorText(DWORD dwLastError);
int xdigit(char const digit);
size_t xstr2strr(char *buf, size_t const bufsize, const char *const in);
//std::wstring str2wstr(std::string &s);
void GetPass(char const *const p_strFolderPath);
size_t GetFiles(const std::string &str, std::vector <t_files> *p_files);
size_t Get_index_acc(unsigned long long const key);
//void gen_nonce(unsigned long long addr, unsigned long long start_nonce, unsigned long long count);
//void generator_i(size_t number);
void proxy_i(void);
void send_i(void);
void procscoop_m_4(unsigned long long const nonce, unsigned long long const n, char const *const data, size_t const acc, const std::string &file_name);
void procscoop_m256_8(unsigned long long const nonce, unsigned long long const n, char const *const data, size_t const acc, const std::string &file_name);
void procscoop_sph(const unsigned long long nonce, const unsigned long long n, char const *const data, const size_t acc, const std::string &file_name);
void procscoop_asm(const unsigned long long nonce, const unsigned long long n, char const *const data, const size_t acc, const std::string &file_name);
void work_i(const size_t local_num);
char* GetJSON(char const *const req);
void GetBlockInfo(unsigned const num_block);
void pollLocal(void);
void updater_i(void);
void hostname_to_ip(char const *const  in_addr, char* out_addr);
void GetCPUInfo(void);
//std::vector<std::shared_ptr<OpenclPlatform>> GPU_PlatformInfo(void);
//int GPU_DeviceInfo(unsigned platform);
//int GPU(void);
//int GPU1(void);
int main(int argc, char **argv);