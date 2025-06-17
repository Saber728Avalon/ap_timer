/**********************************************************************平台宏 开始**************************************************************/
#if defined( _WIN32) ||  defined( _WIN64)
//define something for Windows (32-bit and 64-bit, this part is common)
	#define DUAN_DLL_EXPORT _declspec(dllexport)
	#define DUAN_DLL_IMPORT _declspec(dllimport)
#elif defined(__APPLE__)
    #define DUAN_DLL_EXPORT __attribute__((visibility("default")))
    #if TARGET_IPHONE_SIMULATOR
	// iOS Simulator
	#elif TARGET_OS_IPHONE
	// iOS device
	#elif TARGET_OS_MAC
	// Other kinds of Mac OS
	#else
	// Unsupported platform
	#endif
#elif defined(__linux__)
// linux
	#define DUAN_DLL_EXPORT __attribute__((visibility("default")))
#elif defined(__unix__) // all unices not caught above
// Unix
#elif defined(__posix)
// POSIX
#elif defined(__ANDROID__)
//android
#elif defined(_AIX)
//aix
#endif

#include <iostream>
#include <map>
#include <thread>
#include <memory>
#include <mutex>
#if defined( _WIN32) ||  defined( _WIN64)
	#include <Windows.h>
#else
	#include <stdint.h>
	#include <mutex>
	#include <sys/time.h>
	#include <unistd.h>
#endif

#define MAX_TIMER_COUNT (10 * 1024)


typedef void(*ap_timer_cb)(int timer_id, void* param);


namespace ap_timer {

	//describe a timer in manager list
	struct timer_manager_item {
		//last execute time
		uint64_t timer_last_time;
		//How often is it executed 
		uint32_t timer_interval;
		//id. manager this timer
		int timer_id;
		//callback function
		ap_timer_cb cb;
		//extend param. if you want to call C++ member function. you can give this;
		void* param;
	};

	//global param
	static std::map<int, timer_manager_item> g_map_manager_timer;
	static std::shared_ptr<std::thread> g_ptr_thread;
	static uint64_t g_timer_last_time = 0;
	static int g_exit = 0;
	static std::mutex g_mutex;


	//get current timer
	uint64_t get_current_time() {
		uint64_t u64_timer = 0;
#if defined( _WIN32) ||  defined( _WIN64)
		u64_timer = GetTickCount64();
#else
		struct timeval tv;
		if (gettimeofday(&tv, NULL) != 0)
			return 0;

		u64_timer = (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
#endif
		return u64_timer;
	}

	//Sleep thread
	void ap_sleep() {
#if defined( _WIN32) ||  defined( _WIN64)
		Sleep(0);
#else
		usleep(50 * 1000);
#endif
	}

	// thread deal with timer event
	static void thread_timer() {
		// no exist
		while (!g_exit) {
			std::map<int, timer_manager_item> list_run_tmp;
			do {
				//lock
				std::lock_guard<std::mutex> lock(g_mutex);
				//iter
				for (auto iter = g_map_manager_timer.begin(); iter != g_map_manager_timer.end(); iter++) {
					uint64_t u64_cur = get_current_time();
					if (u64_cur - iter->second.timer_last_time > iter->second.timer_interval) {
						//find item to execute
						list_run_tmp[iter->first] = iter->second;
					}
				}
			} while (0);
			//execute timer callback function
			for (auto iter = list_run_tmp.begin(); iter != list_run_tmp.end(); iter++) {
				//call function
				try {
					iter->second.cb(iter->second.timer_id, iter->second.param);
				}
				catch (...) {
				}
				{
					//update last execute time
					std::lock_guard<std::mutex> lock(g_mutex);
					auto find = g_map_manager_timer.find(iter->first);
					if (g_map_manager_timer.end() != find) {
						find->second.timer_last_time = get_current_time();
					}
				}
			}
			

			// release thread cpu.
			ap_sleep();
		}
		return;
	}

	//start timer thread.
	void ap_start_timer() {
		// not exit timer thread
		g_exit = 0;
		//get start timer time
		g_timer_last_time = get_current_time();
		//start thread
		g_ptr_thread = std::make_shared<std::thread>(thread_timer);
		g_ptr_thread->detach();
		return;
	}

	//add a timer to thread
	int ap_add_timer(uint32_t u32_interval_millis, ap_timer_cb cb, void* param) {
		//lock
		std::lock_guard<std::mutex> lock(g_mutex);

		// get new timer id
		int new_timer_id = 0;
		for (int i = 1; i < MAX_TIMER_COUNT; i++) {
			bool find = false;
			//iter
			for (auto iter = g_map_manager_timer.begin(); iter != g_map_manager_timer.end(); iter++) {
				//check
				if (i == iter->second.timer_id) {
					find = true;
					break;
				}

			}
			// not used
			if (!find) {
				new_timer_id = i;
				break;
			}
		}
		if (0 == new_timer_id) {
			return -1;
		}

		//add this timer
		timer_manager_item item;
		item.cb = cb;
		item.param = param;
		item.timer_id = new_timer_id;
		item.timer_interval = u32_interval_millis;
		item.timer_last_time = get_current_time();
		g_map_manager_timer.emplace(new_timer_id, item);
		return new_timer_id;
	}

	//delete a timer in thread
	void ap_del_timer(int timer_id) {
		//lock
		std::lock_guard<std::mutex> lock(g_mutex);
		//iter
		auto find = g_map_manager_timer.find(timer_id);
		if (g_map_manager_timer.end() == find) {
			return;
		}
		g_map_manager_timer.erase(find);
		return;
	}

	//close timer thread
	void ap_close_timer() {
		// exit timer thread
		g_exit = 1;
	}
}