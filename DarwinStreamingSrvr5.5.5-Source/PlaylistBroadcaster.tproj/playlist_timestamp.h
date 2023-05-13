#ifndef _TIMESTAMP_H_
#define _TIMESTAMP_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Get a 64 bit timestamp */
extern long long timestamp(void);

struct timescale {
    long long tsc_numerator;
    long long tsc_denominator;
};

extern void utimescale(struct timescale *tscp);

#ifdef __cplusplus
}
#endif

#endif /* _TIMESTAMP_H_ */
