// Originally created in 1999 Erwin S. Andreasen <erwin@andreasen.org>
// Some modifications by Henner Zeller <foobar@to.com>
// Homepage: http://www.andreasen.org/LeakTracer/
// This code is Public Domain

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>

/**
 * Filedescriptor to write to. This should not be a low number,
 * because these often have special meanings (stdin, out, err)
 * and may be closed by the program (daemons)
 * So choose an arbitrary higher FileDescriptor .. e.g. 42
 */
#define FILEDESC    42

/**
 * allocate a more memory in order to check if there is a memory
 * overwrite. Either 0 or more than sizeof(unsigned int).
 * This seems to generate problems some time (resulting in segmentation 
 * faults), so disabled for now. -HZ
 */
#define SAVEBUFFER  0
#define SAVEVALUE   0xAA
#define MAGIC       0xAABBCCDDu

/**
 * Initial Number of memory allocations in our list.
 * Doubles for each re-allocation.
 */
#define INITIALSIZE 16

static class LeakTracer {
	struct Leak {
		const void *addr;   // Address of the memory
		size_t size;	    // Size of the memory
		const void *ret[3]; // Return addresses
		bool type;	    // array allocation?
	};
	
	int  newCount;      // how many memory blocks do we have
	int  leaksCount;    // amount of entries in the leaks array
	int  firstFreeSpot; // Where is the first free spot in the leaks array?
	int  currentAllocated; // currentAllocatedMemory
	int  maxAllocated;     // maximum Allocated
	unsigned long totalAllocations; // total number of allocations. stats.

	FILE *report;       // filedescriptor to write to
	Leak *leaks;        // our housekeeping about allocated memory
	
public:
	LeakTracer() 
		: newCount(0),
		  leaksCount(0),
		  firstFreeSpot(0),
		  currentAllocated(0),
		  maxAllocated(0),
		  leaks(0) 
	{
		const char *filename = getenv("LEAKTRACE_FILE") ? : "leak.out";
		int reportfd = open(filename, 
				    O_WRONLY|O_CREAT|O_TRUNC,S_IREAD|S_IWRITE);
		if (reportfd < 0) {
			fprintf(stderr, "LeakTracer: cannot open %s: %s\n", 
				filename, strerror(errno));
			report = stderr;
		}
		else {
			int dupfd = dup2(reportfd, FILEDESC);
			close(reportfd);
			report = fdopen(dupfd, "w");
			if (report == NULL)
				report = stderr;
		}
	}
	
	/*
	 * the workhorses:
	 */
	void *registerAlloc(size_t size, bool type);
	void  registerFree (void *p, bool type);

	/**
	 * write a hexdump of the given area.
	 */
	void  hexdump(const unsigned char* area, int size);
	
	/**
	 * write a Report over leaks, e.g. still pending deletes
	 */
	void writeLeakReport();

	~LeakTracer() {
		writeLeakReport();
		fclose(report);
	}
} leakTracer;

void* LeakTracer::registerAlloc (size_t size, bool type) {
	newCount++;
	totalAllocations++;
	currentAllocated += size;
	if (currentAllocated > maxAllocated)
		maxAllocated = currentAllocated;
	void *p = malloc(size + SAVEBUFFER);
	unsigned int *mag;
	
	memset((char*)p+size, SAVEVALUE, SAVEBUFFER);

	// Need to call the new-handler
	if (!p) {
		fprintf(report, "LeakTracer malloc: %m\n");
		_exit (1);
	}
	
	// set our magic to find out if the memory was overwritten
	if (SAVEBUFFER >= sizeof(unsigned int)) {
		mag = (unsigned int*)((char*)p + size);
		*mag = MAGIC;
	}
	
	for (;;) {
		for (int i = firstFreeSpot; i < leaksCount; i++)
			if (leaks[i].addr == NULL) {
				leaks[i].addr = p;
				leaks[i].size = size;
				leaks[i].type = type;
				leaks[i].ret[0] = __builtin_return_address(1);
				// is there any way to find out how deep
				// our stack is ? Otherwise this segfaults.. -HZ
				//leaks[i].ret[1] = __builtin_return_address(2);
				//leaks[i].ret[2] = __builtin_return_address(3);
				firstFreeSpot = i+1;
				return p;
			}
		
		// Allocate a bigger array
		// Note that leaksCount starts out at 0.
		int new_leaksCount = (leaksCount == 0) ? INITIALSIZE 
			                                 : leaksCount * 2;
		leaks = (Leak*)realloc(leaks, sizeof(Leak) * new_leaksCount);
		if (!leaks) {
			fprintf(report, "# LeakTracer realloc failed: %m\n");
			_exit(1);
		}
		memset(leaks+leaksCount, 0x00,
		       sizeof(Leak) * (new_leaksCount-leaksCount));
		leaksCount = new_leaksCount;
	}
}

void LeakTracer::hexdump(const unsigned char* area, int size) {
	fprintf(report, "# ");
	for (int j=0; j < size ; ++j) {
		fprintf (report, "%02x ", *(area+j));
		if (j % 16 == 15) {
			fprintf(report, "  ");
			for (int k=-15; k < 0 ; k++) {
				char c = (char) *(area + j + k);
				fprintf (report, "%c", isprint(c) ? c : '.');
			}
			fprintf(report, "\n# ");
		}
	}
	fprintf(report, "\n");
}

void LeakTracer::registerFree (void *p, bool type) {
	if (p == NULL)
		return;
	
	newCount--;
	for (int i = 0; i < leaksCount; i++)
		if (leaks[i].addr == p) {
			leaks[i].addr = NULL;
			currentAllocated -= leaks[i].size;
			if (i < firstFreeSpot)
				firstFreeSpot = i;
			
			unsigned int *mag;
			if (SAVEBUFFER >= sizeof(unsigned int)) {
				mag = (unsigned int*)((char*)p + leaks[i].size);
			}
			if (leaks[i].type != type) {
				fprintf(report, 
					"S %10p %10p  # new%s but delete%s "
					"; size %d\n",
					leaks[i].ret[0],
					__builtin_return_address(1),
					((!type) ? "[]" : " normal"),
					((type) ? "[]" : " normal"),
					leaks[i].size);

				//abort();
			}
			
			if ((SAVEBUFFER >= sizeof(unsigned int)) && 
			    *mag != MAGIC) {
				fprintf(report, "O %10p %10p  "
					"# memory overwritten beyond allocated"
					" %d bytes\n",
					leaks[i].ret[0],
					__builtin_return_address(1),
					leaks[i].size);
				fprintf(report, "# %d byte beyond area:\n",
					SAVEBUFFER);
				hexdump((unsigned char*)p+leaks[i].size,
					SAVEBUFFER);
				//abort();
			}
			free(p);
			return;
		}
	
	fprintf(report, "LeakTracer: delete on an already deleted value?\n");
	abort();
}


void LeakTracer::writeLeakReport() {
	if (newCount > 0) {
		fprintf(report, "# LeakReport\n");
		/*
		fprintf(report, "# %10s | %10s | %10s | %9s  # Pointer Addr\n",
			"from new @", "..from", "..from", "size");
		*/
		fprintf(report, "# %10s | %9s  # Pointer Addr\n",
			"from new @", "size");
	}
	for (int i = 0; i <  leaksCount; i++)
		if (leaks[i].addr != NULL) {
				// This ought to be 64-bit safe?
			/*
			fprintf(report, "L %10p   %10p   %10p   %9ld  # %p\n",
				leaks[i].ret[0], 
				leaks[i].ret[1], 
				leaks[i].ret[2],
				(long) leaks[i].size,
				leaks[i].addr);
			*/
			fprintf(report, "L %10p   %9ld  # %p\n",
				leaks[i].ret[0], 
				(long) leaks[i].size,
				leaks[i].addr);
		}
	fprintf(report, "# total allocation requests: %6ld ; max. mem used"
		" %d kBytes\n", totalAllocations, maxAllocated / 1024);
	fprintf(report, "# leak %6d Bytes\n", currentAllocated);
}

/** -- The actual new/delete operators -- **/

void* operator new(size_t size) {
	return leakTracer.registerAlloc(size,false);
}


void* operator new[] (size_t size) {
	return leakTracer.registerAlloc(size,true);
}


void operator delete (void *p) {
	leakTracer.registerFree(p,false);
}


void operator delete[] (void *p) {
	leakTracer.registerFree(p,true);
}

/* Emacs: 
 * Local variables:
 * c-basic-offset: 8
 * End:
 * vi:set tabstop=8 shiftwidth=8 nowrap: 
 */
