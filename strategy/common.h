#pragma once
#include "bits/stdc++.h"
#include "MyStrategy.hpp"
#include "model/Specialty.hpp"
using namespace model;
using namespace std;
#define int long long
#define vi vector<int>
#define vvi vector<vi>
#define INF 100000000000000LL
#define dict unordered_map
#define ResSet dict<Resource, int>
#define BotSet dict<Specialty, int>
#ifdef NDEBUG
#define log(X) ;
#else
#define log(X) cout<<(X)<<endl;
#endif
using Specialty::LOGISTICS;
using Specialty::PRODUCTION;
using Specialty::COMBAT;
template<typename T> bool rmax(T& a, T b); // a=max(a, b); return 1, if there was replace, and 0 if not
template<typename T> bool rmin(T& a, T b); // Analog
template<typename T> T mabs(T a); // Math absolute funcition
template<typename T, typename U> dict<T, int> to_ll(const dict<T, U>&); // converts from [T->int] to [T->long long]
template<typename T, typename U> U sum(const dict<T, U>&); // sums value
int mrand(); // Get determinic random for all the runs;
template<class RandomIt> void mrandom_shuffle(RandomIt first, RandomIt last);

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
template<typename T, typename U> dict<T, int> to_ll(const dict<T, U>& in){
	dict<T, int> ans;
	for(const auto& p:in){
		ans[p.first]=p.second;
	}
	return ans;
}
template<typename T, typename U> U sum(const dict<T, U>& in){
	U ans=U();
	for(const auto& p:in){
		ans+=p.second;
	}
	return ans;
}
template<class RandomIt> void mrandom_shuffle(RandomIt first, RandomIt last){
	typename std::iterator_traits<RandomIt>::difference_type i, n;
	n=last-first;
	for(i=n-1;i>0;--i){
		swap(first[i], first[mrand()%(i+1)]);
	}
}
