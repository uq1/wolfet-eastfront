#include "g_local.h"
#include "etenhanced.h"

#if defined __linux__ 

	#include <string.h>
	#include <signal.h>
	#include <unistd.h>
	#include <execinfo.h>
	#define __USE_GNU
	#include <link.h>
	#include <sys/ucontext.h>
	#include <features.h>

	#if __GLIBC__ == 2 && __GLIBC_MINOR__ == 1
		#define GLIBC_21
	#endif

	extern char *strsignal (int __sig) __THROW;

	//use sigaction instead.
	__sighandler_t INTHandler (int signal, struct sigcontext ctx);
	void CrashHandler(int signal, siginfo_t *siginfo, ucontext_t *ctx);
	void (*OldHandler)(int signal);	
	struct sigaction oldact[NSIG];

	
	int segvloop = 0;
	
	void installcrashhandler() {

		struct sigaction act;

		memset(&act, 0, sizeof(act));
		memset(&oldact, 0, sizeof(oldact));
		act.sa_sigaction = (void *)CrashHandler;
		sigemptyset(&act.sa_mask);
		act.sa_flags = SA_SIGINFO;

		sigaction(SIGSEGV, &act, &oldact[SIGSEGV]); 
		sigaction(SIGILL, &act, &oldact[SIGILL]); 
		sigaction(SIGFPE, &act, &oldact[SIGFPE]); 
		sigaction(SIGBUS, &act, &oldact[SIGBUS]);

	}

	void restorecrashhandler() {
		sigaction(SIGSEGV, &oldact[SIGSEGV], NULL); 
	}

	void installinthandler() {
		// Trap Ctrl-C
		signal(SIGINT, (void *)INTHandler);
	}

	void ETEnhanced_siginfo(int signal, siginfo_t *siginfo) {
		G_LogPrintf("Signal: %s (%d)\n", strsignal(signal), signal); 
		G_LogPrintf("Siginfo: %p\n", siginfo);
		if(siginfo) {
			G_LogPrintf("Code: %d\n", siginfo->si_code);
			G_LogPrintf("Faulting Memory Ref/Instruction: %p\n",siginfo->si_addr);
		}
	}

	// tjw: i'm disabling ETEnhanced_dsnoinfo() because it depends on
	//      glibc 2.3.3 only  (it also won't build on earlier glibc 2.3
	//      versions).
	//      We're only doing glibc 2.1.3 release builds anyway.
	/*
	void ETEnhanced_dsoinfo() {
		struct link_map *linkmap = NULL;
	        void **linkp = (void *)&linkmap;
        	Dl_info dlinfo;

		if(!dladdr1(ETEnhanced_dsoinfo, &dlinfo, linkp, RTLD_DL_LINKMAP)) {
			return;
		}

		//rewind to top item.
  		while(linkmap->l_prev)
	                linkmap=linkmap->l_prev;

		G_LogPrintf("DSO Information\n");

		while(linkmap) {

			if(linkmap->l_addr) {

				if(strcmp(linkmap->l_name,"")==0) 
					G_LogPrintf("%#08x\t(unknown)\n", linkmap->l_addr);
				else
					G_LogPrintf("%#0x\t%s\n", linkmap->l_addr, linkmap->l_name);

			}			

			linkmap=linkmap->l_next;

		}
	}
	*/

	void ETEnhanced_backtrace(ucontext_t *ctx) {

		// See <asm/sigcontext.h>	

		// ctx.eip contains the actual value of eip 
		// when the signal was generated.

		// ctx.cr2 contains the value of the cr2 register 
		// when the signal was generated.

		// the cr2 register on i386 contains the address 
		// that caused the page fault if there was one.

		int i;

		char **strings;
		void *array[1024];
		size_t size = (size_t)backtrace(array, 1024);

		//Set the actual calling address for accurate stack traces.
		//If we don't do this stack traces are less accurate.
		#ifdef GLIBC_21
			G_LogPrintf("Stack frames: %Zd entries\n", size-1);
			array[1] = (void *)ctx->uc_mcontext.gregs[EIP];
		#else
			G_LogPrintf("Stack frames: %zd entries\n", size-1);
			array[1] = (void *)ctx->uc_mcontext.gregs[REG_EIP];
		#endif
		G_LogPrintf("Backtrace:\n");

		strings = (char **)backtrace_symbols(array, (int)size);

		//Start at one and climb up.
		//The first entry points back to this function.
		for (i = 1; i < (int)size; i++)
			G_LogPrintf("(%i) %s\n", i, strings[i]);

		free(strings);
	}

	__sighandler_t INTHandler(int signal, struct sigcontext ctx) {
		G_LogPrintf("------------------------------------------------\n");
		G_LogPrintf("Ctrl-C is not the proper way to kill the server.\n");
		G_LogPrintf("------------------------------------------------\n");
		return 0;
	}

	void CrashHandler(int signal, siginfo_t *siginfo, ucontext_t *ctx) {

		//we are real cautious here.
		restorecrashhandler();

		if(signal == SIGSEGV)
			segvloop++;

		if(segvloop < 2) {
			G_LogPrintf("-8<------- Crash Information ------->8-\n");
			G_LogPrintf(va("   Please forward to %s mod team.   \n", GAME_VERSION));
			G_LogPrintf("---------------------------------------\n");
			G_LogPrintf("Version: %s %s\n", GAMEVERSION, ETE_VERSION);
			G_LogPrintf("Map: %s\n",level.rawmapname);
			ETEnhanced_siginfo(signal, siginfo);
			//ETEnhanced_dsoinfo();
			ETEnhanced_backtrace(ctx);
			G_LogPrintf("-8<--------------------------------->8-\n\n");
			//pass control to the default handler.
			if(signal == SIGSEGV) {
				OldHandler = (void *)oldact[SIGSEGV].sa_sigaction;
				(*OldHandler)(signal);
			} else {
				exit(1);
			}
		} else {
			//end this madness we are looping.
			G_Error("Recursive segfault. Bailing out.");
			OldHandler = (void *)oldact[SIGSEGV].sa_sigaction;
			(*OldHandler)(signal);
		}

		return;

	}

#elif defined WIN32

#else //other platforms
//FIXME: Get rich, buy a mac, figure out how to do this on OSX.
#endif

void EnableCoreDumps() {
	#if defined __linux__

	#elif defined WIN32

	#else

	#endif
}

void DisableCoreDump() {
	#if defined __linux__

	#elif defined WIN32

	#else

	#endif
}

void EnableStackTrace() {
	#if defined __linux__
		installcrashhandler();
	#elif defined WIN32

	#else

	#endif
}

void DisableStackTrace() {
	#if defined __linux__
		restorecrashhandler();
	#elif defined WIN32

	#else

	#endif
}
