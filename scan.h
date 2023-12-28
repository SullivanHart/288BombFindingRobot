/*
 * scan Header
 *
 * 12\2\2023
 * @author Daniel, Laith, Logan, Sullivan
 */

#ifndef SCAN_H_
#define SCAN_H_

typedef struct {
    int pingData;
    int irData;
} ScanData;

void search(ScanData *scanData, int ang);

#endif /* SCAN_H_ */
