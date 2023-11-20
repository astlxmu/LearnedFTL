#pragma GCC push_options
#pragma GCC optimize(0)
#include "../nvme.h"

#include<stdio.h>
#include "util.h"


// void quick_sort(uint64_t lpns[], int low, int high){
//     if(low < high) {
//         int i = low;
//         int j = high;
//         uint64_t k = lpns[low];
//         while (i < j) {
//             while (i < j && lpns[j] >= k) {
//                 j--;
//             }
//             while (i < j && lpns[i] < k) {
//                 i++;
//             }

//             if (i < j) {
//                 lpns[j--] = lpns[i];
//             }
//         }
//         lpns[i] = k;
//         quick_sort(lpns, low, i-1);
//         quick_sort(lpns, i+1, high);
//     }
    
// }



static int findPos (uint64_t * a, int low, int high){
	uint64_t val = a[low];
 
	while(low < high){
		while(low < high && a[high] >= val){
			--high;
		}
		a[low] = a[high];
 
		while(low < high && a[low] <= val){
			++low;
		}
		a[high] = a[low];
	}
	a[low] = val;
	return low;
}

void quick_sort(uint64_t * a, int low, int high){
	int pos;
	if(low < high){
		pos = findPos(a, low, high);
		quick_sort(a, low, pos-1);
		quick_sort(a, pos+1, high);
	}
}


// static uint64_t re_standardlize(float pred_y, uint64_t max_y, uint64_t min_y) {
// 	return (uint64_t) (pred_y * (max_y - min_y) + min_y);
// }	

// void reclusiveRegression(uint64_t *x, uint64_t *y, int num, lr_node *wbs, int *total) {
// 	// find all continuously datas
// 	// int num_of_trains = sizeof(x)/(sizeof(uint64_t));
// 	int train_success_num = 0;
// 	uint64_t last_start = 0;
// 	for (int i = 1; i < num; i++) {
// 		if (x[i] - 1 == x[i-1]) continue;
		
// 		train_success_num += least_square(x, y, last_start, i+1-last_start, &wbs[*total].w, &wbs[*total].b);
// 		last_start = i+1;
// 		(*total) ++;
// 	}

// 	printf("train_success_num: %d ; total models: %d\n", train_success_num, *total);
// }

int least_square(uint64_t *x, uint64_t *y, uint64_t start, int num, float *w, float *b) {
	float t1=0, t2=0, t3=0, t4=0;  
	for(int i = start; i < start+num; ++i) {  
		t1 += x[i]*x[i];  
		t2 += x[i];  
		t3 += x[i]*y[i];  
		t4 += y[i];  
	}  
	*w = (t3*num - t2*t4) / (t1*num - t2*t2);  
	//b = (t4 - a*t2) / num;  
	*b = (t1*t4 - t2*t3) / (t1*num - t2*t2);  
	// printf("training result: %f %f\n", *w, *b);
	int train_success_num = 0;
	for (int i = start; i < start+num; i++) {
		float yy = predict(x[i], w, b);
		int ya = y[i];
		if (abs(yy - ya) < 1) {
			train_success_num++;
		}
	}
	return train_success_num;
}

void LeastSquare(uint64_t *x, uint64_t *y, int num, float *w, float *b) {  

		// // first standardlize the data
        float t1=0, t2=0, t3=0, t4=0;  
        for(int i=0; i<num; ++i) {  
            t1 += x[i]*x[i];  
            t2 += x[i];  
            t3 += x[i]*y[i];  
            t4 += y[i];  
        }  
        *w = (t3*num - t2*t4) / (t1*num - t2*t2);  
        //b = (t4 - a*t2) / num;  
        *b = (t1*t4 - t2*t3) / (t1*num - t2*t2);  
		// printf("training result: %f %f\n", *w, *b);
		
		// int train_success_num = 0;
		// for (int i = 0; i < num; i++) {
		// 	float yy = predict(x[i], w, b);
		// 	int ya = y[i];
		// 	if (abs(yy - ya) < 1) {
		// 		train_success_num++;
		// 	}
		// }
		// printf("train success: %d ; %d\n", train_success_num, num);
}  


int LeastSquareNew(uint64_t *x, uint64_t *y, int num, float *w, float *b) {  

        float t1=0, t2=0, t3=0, t4=0;  
        for(int i=0; i<num; ++i) {  
            t1 += x[i]*x[i];  
            t2 += x[i];  
            t3 += x[i]*y[i];  
            t4 += y[i];  
        }  
        *w = (t3*num - t2*t4) / (t1*num - t2*t2);  
        //b = (t4 - a*t2) / num;  
        *b = (t1*t4 - t2*t3) / (t1*num - t2*t2);  
		// printf("training result: %f %f\n", *w, *b);
		
		int train_success_num = 0;

		return train_success_num;
		// printf("train success: %d ; %d\n", train_success_num, num);
}  


// float* regression(uint64_t *x, uint64_t *y, float learning_rate, int num_iterations, float *w, float *b){

// 	float *cost = (float*) malloc(sizeof(float)*num_iterations);
// 	int n = 10;

// 	for(int i=0; i<num_iterations; i++){
// 		cost[i] = batch_gradient_descent(x, y, n, learning_rate, w, b);
// 	}

// 	return cost;
// }


float batch_gradient_descent(uint64_t *x, uint64_t *y, int n, float learning_rate, float *w, float *b){

	float db = 0.0;
	float dw = 0.0;
	float cost = 0.0;

	for(int i=0; i<n; i++){

		cost += pow(((*w * x[i] + *b) - y[i]),2);
		db += (*w * x[i] + *b) - y[i];
		dw += ((*w * x[i] + *b) - y[i])*x[i];
	}

	cost /= n;
	*w = *w - learning_rate*(dw)/n;
	*b = *b - learning_rate*(db)/n;

	return cost;
}


float predict(uint64_t x, float *w, float *b){

	float y = *w * x + *b;
	return y;

}


uint64_t hash_func(uint64_t key) {
  key = (~key) + (key << 21); // key = (key << 21) - key - 1;
  key = key ^ (key >> 24);
  key = (key + (key << 3)) + (key << 8); // key * 265
  key = key ^ (key >> 14);
  key = (key + (key << 2)) + (key << 4); // key * 21
  key = key ^ (key >> 28);
  key = key + (key << 31);
  return key;

}

#pragma GCC pop_options