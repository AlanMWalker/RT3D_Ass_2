#ifndef MACRO_H
#define MACRO_H

#ifndef FREE
#define SAFE_FREE(ptr) if ( ptr != nullptr ) { delete ptr; ptr = nullptr; }
#endif

#ifndef FREE_ARR
#define SAFE_FREE_ARR(ptr) if ( ptr != nullptr) { delete[] ptr; ptr = nullptr;}
#endif

#define TESTING_ENABLED 0

#define INDEX_NONE -1

#ifdef _MSC_VER
#define DX_ALIGNED __declspec(align(16)) // For classes storing SIMD-optimised data structures (such as XMVECTOR or XMMATRIX)
#endif


#define OP_NEW void* operator new(size_t i)\
{\
	return _mm_malloc(i, 16);\
}\

#define OP_DEL void operator delete(void* p) \
{\
	_mm_free(p);\
}\


#endif