//
//  DebuggingHelpers.h
//  Wyrez
//
//  Created by stefan on 7/6/13.
//
//

#ifndef Wyrez_DebuggingHelpers_h
#define Wyrez_DebuggingHelpers_h

static inline void printFileContent(const char* path) {
    FILE *in;
    char c;
    in = fopen(path, "r");
    if(in != NULL)
    {
        while((c = fgetc(in)) != EOF)
            putchar(c);
        
        fclose(in);
    }
}

typedef unsigned long long timestamp_t;
static inline timestamp_t get_timestamp () {
    struct timeval now;
    gettimeofday (&now, NULL);
    return  now.tv_usec + (timestamp_t)now.tv_sec * 1000000;
}

static inline void printFileSize(const char* path)
{
    FILE *p_file = NULL;
    p_file = fopen(path,"rb");
    fseek(p_file,0,SEEK_END);
    int size = ftell(p_file);
    fclose(p_file);
    std::cout << "file size: " << size << "\n";
}

#endif
