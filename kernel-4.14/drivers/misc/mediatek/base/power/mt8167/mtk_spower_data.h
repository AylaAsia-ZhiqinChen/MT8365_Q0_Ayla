/*
 * Copyright (C) 2015 MediaTek Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

#ifndef MT_SPOWER_CPU_H
#define MT_SPOWER_CPU_H



#define VSIZE 8
#define TSIZE 17

/***************************/
/* "(WAT -8.6%)	   */
/* Leakage Power"	   */
/***************************/
#define CA7_TABLE_0							\
	/**/ {800, 850, 900, 950, 1000, 1050, 1100, 1150, \
		25, 24, 30, 39, 49, 61, 77, 97, 120, \
		30, 28, 36, 46, 58, 72, 90, 112, 138, \
		35, 34, 43, 54, 68, 84, 104, 129, 159, \
		40, 41, 52, 64, 79, 98, 121, 149, 183, \
		45, 50, 62, 76, 93, 115, 141, 172, 211, \
		50, 60, 74, 90, 110, 134, 164, 199, 243, \
		55, 72, 88, 107, 129, 156, 190, 230, 279, \
		60, 87, 105, 127, 152, 183, 221, 266, 322, \
		65, 105, 126, 150, 178, 213, 256, 308, 371, \
		70, 126, 150, 178, 209, 249, 298, 356, 427, \
		75, 152, 179, 210, 246, 291, 346, 412, 491, \
		80, 183, 213, 249, 289, 340, 402, 476, 566, \
		85, 220, 255, 295, 340, 397, 467, 550, 651, \
		90, 265, 304, 349, 400, 464, 543, 636, 750, \
		95, 319, 363, 413, 470, 542, 631, 735, 863, \
		100, 384, 433, 490, 552, 633, 733, 850, 994, \
		105, 463, 517, 580, 649, 740, 851, 983, 1145}

/******************/
/* "(WAT -3%)	  */
/* Leakage Power" */
/******************/
#define CA7_TABLE_1							\
	/**/ {800, 850, 900, 950, 1000, 1050, 1100, 1150, \
		25, 6, 8, 10, 12, 15, 19, 23, 29, \
		30, 7, 9, 12, 15, 18, 22, 27, 34, \
		35, 9, 11, 14, 17, 21, 26, 32, 39, \
		40, 11, 14, 17, 21, 25, 31, 37, 45, \
		45, 14, 17, 21, 25, 30, 36, 43, 52, \
		50, 18, 21, 25, 30, 36, 42, 51, 61, \
		55, 22, 26, 30, 36, 42, 50, 59, 71, \
		60, 27, 31, 37, 42, 50, 59, 69, 82, \
		65, 34, 38, 44, 51, 59, 69, 81, 95, \
		70, 42, 47, 53, 61, 70, 81, 95, 110, \
		75, 52, 57, 65, 73, 84, 95, 110, 128, \
		80, 64, 70, 78, 87, 99, 112, 129, 149, \
		85, 80, 86, 95, 104, 117, 132, 151, 173, \
		90, 99, 105, 115, 124, 139, 155, 176, 200, \
		95, 123, 129, 139, 149, 165, 183, 206, 233, \
		100, 152, 157, 168, 178, 196, 215, 240, 270, \
		105, 189, 192, 203, 213, 232, 253, 281, 313}

/******************/
/* "(WAT -2.5%)	  */
/* Leakage Power" */
/******************/
#define CA7_TABLE_2							\
	/**/	{800, 850,	900,	950,	1000, 1050, 1100, 1150, \
		25, 2, 3, 4, 5, 6, 8, 10, 12, \
		30, 3, 4, 5, 6, 8, 10, 12, 15, \
		35, 4, 5, 6, 7, 9, 11, 14, 17, \
		40, 5, 6, 8, 9, 11, 13, 16, 20, \
		45, 6, 8, 9, 11, 13, 16, 19, 23, \
		50, 8, 10, 11, 13, 16, 19, 23, 27, \
		55, 10, 12, 14, 16, 19, 23, 27, 32, \
		60, 13, 15, 17, 20, 23, 27, 31, 37, \
		65, 16, 18, 21, 24, 28, 32, 37, 43, \
		70, 20, 23, 26, 29, 33, 38, 44, 51, \
		75, 25, 29, 31, 35, 40, 45, 51, 59, \
		80, 32, 36, 39, 43, 48, 53, 60, 69, \
		85, 40, 44, 47, 52, 57, 63, 71, 81, \
		90, 51, 55, 58, 63, 69, 75, 84, 94, \
		95, 64, 68, 71, 77, 83, 90, 99, 110, \
		100, 81, 85, 87, 93, 100, 106, 116, 128, \
		105, 103, 106, 107, 113, 120, 126, 137, 150}

/***************************/
/* "(WAT 14.35%)	   */
/* Leakage Power"	   */
/***************************/
#define CA15L_TABLE_0							\
	/**/ {800, 850, 900, 950, 1000, 1050, 1100, 1150, \
		25, 65, 83, 104, 129, 172, 195, 241, 294, \
		30, 76, 97, 122, 151, 201, 229, 282, 344, \
		35, 89, 113, 142, 177, 235, 267, 330, 403, \
		40, 104, 132, 166, 207, 275, 313, 386, 471, \
		45, 122, 155, 195, 242, 321, 366, 451, 551, \
		50, 142, 181, 228, 283, 376, 428, 528, 645, \
		55, 166, 212, 266, 331, 440, 501, 617, 754, \
		60, 195, 248, 312, 387, 514, 586, 722, 882, \
		65, 228, 290, 365, 453, 602, 685, 845, 1032, \
		70, 266, 339, 427, 530, 704, 802, 989, 1207, \
		75, 312, 397, 499, 619, 823, 938, 1156, 1412, \
		80, 364, 464, 584, 725, 963, 1097, 1353, 1652, \
		85, 426, 543, 683, 848, 1127, 1284, 1582, 1933, \
		90, 499, 635, 799, 992, 1318, 1501, 1851, 2261, \
		95, 583, 743, 934, 1160, 1541, 1756, 2165, 2644, \
		100, 682, 869, 1093, 1357, 1803, 2054, 2532, 3093, \
		105, 798, 1017, 1278, 1587, 2109, 2402, 2962, 3618}

#define CA15L_TABLE_1							\
	/**/ {800, 850, 900, 950, 1000, 1050, 1100, 1150, \
		25, 27, 34, 43, 53, 71, 80, 99, 121, \
		30, 31, 40, 50, 63, 83, 95, 117, 143, \
		35, 37, 47, 59, 74, 98, 112, 138, 168, \
		40, 44, 56, 70, 87, 116, 132, 162, 198, \
		45, 52, 66, 83, 102, 136, 155, 191, 234, \
		50, 61, 77, 97, 121, 161, 183, 226, 275, \
		55, 72, 91, 115, 142, 189, 216, 266, 325, \
		60, 84, 108, 135, 168, 223, 254, 313, 383, \
		65, 99, 127, 159, 198, 263, 300, 369, 451, \
		70, 117, 149, 188, 233, 310, 353, 435, 532, \
		75, 138, 176, 221, 275, 365, 416, 513, 627, \
		80, 163, 208, 261, 324, 430, 490, 605, 738, \
		85, 192, 245, 307, 382, 507, 578, 712, 870, \
		90, 226, 288, 362, 450, 598, 681, 840, 1026, \
		95, 267, 340, 427, 530, 705, 803, 990, 1209, \
		100, 314, 400, 503, 625, 830, 946, 1166, 1425, \
		105, 370, 472, 593, 736, 979, 1115, 1375, 1679}

#define CA15L_TABLE_2							\
	/**/ {800, 850, 900, 950, 1000, 1050, 1100, 1150, \
		  25, 27, 34, 43, 53, 71, 80, 99, 121, \
		  30, 31, 40, 50, 63, 83, 95, 117, 143, \
		  35, 37, 47, 59, 74, 98, 112, 138, 168, \
		  40, 44, 56, 70, 87, 116, 132, 162, 198, \
		  45, 52, 66, 83, 102, 136, 155, 191, 234, \
		  50, 61, 77, 97, 121, 161, 183, 226, 275, \
		  55, 72, 91, 115, 142, 189, 216, 266, 325, \
		  60, 84, 108, 135, 168, 223, 254, 313, 383, \
		  65, 99, 127, 159, 198, 263, 300, 369, 451, \
		  70, 117, 149, 188, 233, 310, 353, 435, 532, \
		  75, 138, 176, 221, 275, 365, 416, 513, 627, \
		  80, 163, 208, 261, 324, 430, 490, 605, 738, \
		  85, 192, 245, 307, 382, 507, 578, 712, 870, \
		  90, 226, 288, 362, 450, 598, 681, 840, 1026, \
		  95, 267, 340, 427, 530, 705, 803, 990, 1209, \
		  100, 314, 400, 503, 625, 830, 946, 1166, 1425, \
		  105, 370, 472, 593, 36, 979, 1115, 1375, 1679}


#define GPU_TABLE_0							\
	/**/ {800, 850, 900, 950, 1000, 1050, 1100, 1150, \
		  25, 24, 31, 39, 48, 59, 73, 90, 110, \
		  30, 28, 36, 45, 55, 68, 85, 105, 128, \
		  35, 32, 41, 52, 64, 79, 98, 121, 148, \
		  40, 37, 47, 60, 74, 91, 113, 140, 171, \
		  45, 43, 55, 69, 86, 105, 131, 161, 197, \
		  50, 50, 63, 80, 99, 121, 151, 187, 228, \
		  55, 57, 73, 92, 114, 140, 175, 216, 263, \
		  60, 66, 85, 106, 132, 162, 202, 249, 304, \
		  65, 77, 98, 123, 153, 188, 234, 288, 352, \
		  70, 89, 113, 142, 177, 217, 270, 333, 406, \
		  75, 103, 131, 164, 204, 250, 312, 385, 470, \
		  80, 119, 151, 190, 236, 289, 361, 444, 543, \
		  85, 137, 175, 219, 272, 335, 417, 514, 627, \
		  90, 158, 202, 254, 315, 387, 482, 594, 725, \
		  95, 183, 233, 293, 364, 447, 557, 686, 838, \
		  100, 212, 270, 339, 421, 516, 643, 793, 969, \
		  105, 244, 312, 392, 486, 597, 743, 917, 1120}


#define GPU_TABLE_1							\
	/**/ {800, 850, 900, 950, 1000, 1050, 1100, 1150, \
		  25, 9, 11, 14, 18, 22, 27, 34, 41, \
		  30, 11, 13, 17, 21, 26, 32, 40, 48, \
		  35, 12, 16, 20, 25, 30, 38, 47, 57, \
		  40, 15, 19, 23, 29, 36, 44, 55, 67, \
		  45, 17, 22, 28, 34, 42, 52, 65, 79, \
		  50, 20, 26, 32, 40, 50, 62, 76, 93, \
		  55, 24, 30, 38, 47, 58, 73, 89, 109, \
		  60, 28, 36, 45, 56, 69, 85, 105, 129, \
		  65, 33, 42, 53, 66, 81, 101, 124, 151, \
		  70, 39, 50, 62, 77, 95, 118, 146, 178, \
		  75, 46, 58, 73, 91, 112, 139, 172, 210, \
		  80, 54, 69, 86, 107, 132, 164, 202, 247, \
		  85, 64, 81, 102, 126, 155, 193, 238, 291, \
		  90, 75, 95, 120, 149, 183, 227, 280, 342, \
		  95, 88, 112, 141, 175, 215, 268, 330, 403, \
		  100, 104, 132, 166, 206, 253, 315, 389, 475, \
		  105, 122, 156, 195, 243, 298, 371, 457, 559}


#define GPU_TABLE_2							\
	/**/ {800, 850, 900, 950, 1000, 1050, 1100, 1150, \
		  25, 9, 11, 14, 18, 22, 27, 34, 41, \
		  30, 11, 13, 17, 21, 26, 32, 40, 48, \
		  35, 12, 16, 20, 25, 30, 38, 47, 57, \
		  40, 15, 19, 23, 29, 36, 44, 55, 67, \
		  45, 17, 22, 28, 34, 42, 52, 65, 79, \
		  50, 20, 26, 32, 40, 50, 62, 76, 93, \
		  55, 24, 30, 38, 47, 58, 73, 89, 109, \
		  60, 28, 36, 45, 56, 69, 85, 105, 129, \
		  65, 33, 42, 53, 66, 81, 101, 124, 151, \
		  70, 39, 50, 62, 77, 95, 118, 146, 178, \
		  75, 46, 58, 73, 91, 112, 139, 172, 210, \
		  80, 54, 69, 86, 107, 132, 164, 202, 247, \
		  85, 64, 81, 102, 126, 155, 193, 238, 291, \
		  90, 75, 95, 120, 149, 183, 227, 280, 342, \
		  95, 88, 112, 141, 175, 215, 268, 330, 403, \
		  100, 104, 132, 166, 206, 253, 315, 389, 475, \
		  105, 122, 156, 195, 243, 298, 371, 457, 559}



int ca7_data[][VSIZE*TSIZE+VSIZE+TSIZE] = {
		CA7_TABLE_0,
		CA7_TABLE_1,
		CA7_TABLE_2,
};

int ca15l_data[][VSIZE*TSIZE+VSIZE+TSIZE] = {
		CA15L_TABLE_0,
		CA15L_TABLE_1,
		CA15L_TABLE_2,
};

int gpu_data[][VSIZE*TSIZE+VSIZE+TSIZE] = {
		GPU_TABLE_0,
		GPU_TABLE_1,
		GPU_TABLE_2,
};

struct spower_raw_t {
	int vsize;
	int tsize;
	int table_size;
	int *table[];
};

struct spower_raw_t ca7_spower_raw = {
	.vsize = VSIZE,
	.tsize = TSIZE,
	.table_size = 3,
	.table = { (int *)&ca7_data[0], (int *)&ca7_data[1],
		(int *)&ca7_data[2] },
};


struct spower_raw_t ca15l_spower_raw = {
	.vsize = VSIZE,
	.tsize = TSIZE,
	.table_size = 3,
	.table = { (int *)&ca15l_data[0], (int *)&ca15l_data[1],
		(int *)&ca15l_data[2] },
};

struct spower_raw_t gpu_spower_raw = {
	.vsize = VSIZE,
	.tsize = TSIZE,
	.table_size = 3,
	.table = { (int *)&gpu_data[0], (int *)&gpu_data[1],
		(int *)&gpu_data[2] },
};



struct vrow_t {
	int mV[VSIZE];
};

struct trow_t {
	int deg;
	int mA[VSIZE];
};


struct sptbl_t {
	int vsize;
	int tsize;
	/* array[VSIZE + TSIZE + (VSIZE*TSIZE)]; */
	int *data;
	/* pointer to voltage row of data */
	struct vrow_t *vrow;
	/* pointer to temperature row of data */
	struct trow_t *trow;
};

#define trow(tab, ti)		((tab)->trow[ti])
#define mA(tab, vi, ti)	((tab)->trow[ti].mA[vi])
#define mV(tab, vi)		((tab)->vrow[0].mV[vi])
#define deg(tab, ti)		((tab)->trow[ti].deg)
#define vsize(tab)		((tab)->vsize)
#define tsize(tab)		((tab)->tsize)
#define tab_validate(tab)	((tab)->data != NULL)

static inline void spower_tab_construct
	(struct sptbl_t (*tab)[], struct spower_raw_t *raw)
{
	int i;
	struct sptbl_t *ptab = (struct sptbl_t *)tab;

	for (i = 0; i < raw->table_size; i++) {
		ptab->vsize = raw->vsize;
		ptab->tsize = raw->tsize;
		ptab->data = raw->table[i];
		ptab->vrow = (struct vrow_t *)ptab->data;
		ptab->trow = (struct trow_t *)(ptab->data + ptab->vsize);
		ptab++;
	}
}

#define MAX_TABLE_SIZE 5

/**
 * @argument
 * dev: the enum of MT_SPOWER_xxx
 * voltage: the operating voltage
 * degree: the Tj
 * @return
 *  -1, means sptab is not yet ready.
 *  other value: the mW of leakage value.
 **/
extern int mt_spower_get_leakage(int dev, int voltage, int degree);

#endif


