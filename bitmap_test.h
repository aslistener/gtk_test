#pragma once

#include "functions.h"

// test for XShm
void TestPaintSharedImage(int xshm_id);

// save ximage to png 
int save_ximage_pnm(XImage *img,char *pnmname,int type);

// create a xbitmap, and paint with it
void test_bitmap(); 


// test for shared memory bitmap
extern int x_shm_id;
extern void test_xshm(int shm_id);