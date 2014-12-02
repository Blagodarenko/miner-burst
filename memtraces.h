    /*  memtraces.h - replaces functions malloc, calloc, realloc and free with another
            ones that count memory being allocated. Also defines macro that allows to
            get the size of a memory block (memsize).
        Date: 2 August 2006
        Author: Jeremiah Shaulov
        Lisence: GPL
    */
    #ifndef MEMTRACES_INCLUDED
    #define MEMTRACES_INCLUDED
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
     
    static int memallocated=0, mempeak=0, memtraces_cnt=0, pmemtemp;
    static struct memtr{void *p; int size; int line; char file[20];} *memtraces_var=NULL;
    static FILE *memtraces_log=NULL;
     
    #define memsize(p) ((p)==NULL ? 0 : ((int*)(p))[-1])
     
    void *realloc2(void *p, int size)
    {   int *p2;
        if (p == NULL) p2 = (int*)malloc((unsigned)(size)+sizeof(int));
        else p2 = (int*)realloc((char*)(p)-sizeof(int), (unsigned)(size)+sizeof(int));
        if (p2 == NULL)
        {   fprintf(stderr, "\nFailed to allocate %u bytes", (unsigned)(size)+sizeof(int));
            exit(1);
        }
        *p2 = size;
        return p2+1;
    }
     
    void free2(void *p)
    {   if (p != NULL) free(((int*)p) - 1);
    }
     
    void *memtraces_add(void *p, int size, int line, const char *file)
    {   memallocated += size;
        if (memallocated > mempeak) mempeak = memallocated;
        if (p == NULL) return p;
        memtraces_cnt++;
		memtraces_var = (memtr*)realloc(memtraces_var, memtraces_cnt*sizeof(*memtraces_var));
        memtraces_var[memtraces_cnt-1].p = p;
        memtraces_var[memtraces_cnt-1].size = size;
        memtraces_var[memtraces_cnt-1].line = line;
        if (strlen(file) <= sizeof(memtraces_var->file)-1)
            strcpy(memtraces_var[memtraces_cnt-1].file, file);
        else
            strcpy(memtraces_var[memtraces_cnt-1].file, file+strlen(file)+1-sizeof(memtraces_var->file));
        if (memtraces_log != NULL)
            fprintf(memtraces_log, "add %p of size %d; %d allocated (line %d, source %s)\n",
                p, size, memallocated, line, file);
        return p;
    }
     
    void *memtraces_del(void *p, int line, const char *file)
    {   int i;
        if (p == NULL) return p;
        memallocated -= memsize(p);
        for (i=0; i<memtraces_cnt; i++) if (memtraces_var[i].p == p)
        {   memmove(memtraces_var+i, memtraces_var+i+1, (memtraces_cnt-i-1)*sizeof(*memtraces_var));
            memtraces_cnt--;
			memtraces_var = (memtr*)realloc(memtraces_var, memtraces_cnt*sizeof(*memtraces_var));
            if (memtraces_log != NULL)
                fprintf(memtraces_log, "del %p of size %d; %d allocated (line %d, source %s)\n",
                    p, memsize(p), memallocated, line, file);
            return p;
        }
        fprintf(stderr, "free(%p) - wrong pointer (line %d, source %s)", p, line, file);
        exit(1);
        return p;
    }
     
    void memtraces(FILE *fh)
    {   int i;
        fprintf(fh, "\n\nMEMORY STATUS:\n");
        for (i=0; i<memtraces_cnt; i++)
        {   fprintf(fh, "%p of size %d (line %d, source %s)\n", memtraces_var[i].p,
                memsize(memtraces_var[i].p), memtraces_var[i].line, memtraces_var[i].file);
        }
        fprintf(fh, "Allocated: %d; Peak: %d", memallocated, mempeak);
    }
     
    #ifdef NDEBUG
    #   define realloc(p, size) realloc2(p, size)
    #   define malloc(size) realloc2(NULL, size)
    #   define calloc(size) ( pmemtemp=(size), memset(realloc2(NULL, pmemtemp), 0, pmemtemp) )
    #   define free(p) free2(p)
    #else
    #   define realloc(p, size) ( pmemtemp=(size), memtraces_add(realloc2( \
            memtraces_del(p, __LINE__, __FILE__), pmemtemp), pmemtemp, __LINE__, __FILE__) )
    #   define malloc(size) ( pmemtemp=(size), memtraces_add(realloc2( \
            NULL, pmemtemp), pmemtemp, __LINE__, __FILE__) )
    #   define calloc(size) ( pmemtemp=(size), memset(memtraces_add(realloc2( \
            NULL, pmemtemp), pmemtemp, __LINE__, __FILE__), 0, pmemtemp) )
    #   define free(p) free2(memtraces_del(p, __LINE__, __FILE__))
    #endif
     
    #endif