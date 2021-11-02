#pragma once
#include "bits/stdc++.h"
#include "MyStrategy.hpp"
using namespace model;
using namespace std;
#define int long long
#define vi vector<int>
#define vvi vector<vi>
#define INF 100000000000000LL
#define dict unordered_map
template<typename T> bool rmax(T& a, T b); // a=max(a, b); return 1, if there was replace, and 0 if not
template<typename T> bool rmin(T& a, T b); // Analog
template<typename T> T mabs(T a); // Math absolute funcition

// Template realization
template<typename T> T mabs(T a){
	return(a<0?-a:a);
};
template<typename T> bool rmin(T& a, T b){
	if(b<a){
		a=b;
		return 1;
	}
	return 0;
};
template<typename T> bool rmax(T& a, T b){
	if(a<b){
		a=b;
		return 1;
	}
	return 0;
};
