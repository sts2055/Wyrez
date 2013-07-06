//
//  DebuggingHelpers.h
//  Wyrez
//
//  Created by stefan on 7/6/13.
//
//

#ifndef Wyrez_DebuggingHelpers_h
#define Wyrez_DebuggingHelpers_h

static inline void printFileAtLocation(std::string s) {
    FILE *in;
    char c;
    in = fopen(s.c_str(), "r");
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

#endif
