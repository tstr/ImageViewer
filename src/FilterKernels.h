/*
	Image filter kernels
*/

#pragma once

/*
	A filter kernel is represented by an n*m matrix
*/
template<uint n, uint m>
struct Kernel
{
	static_assert(n % 2 == 1, "Kernel must have odd dimensions");
	static_assert(m % 2 == 1, "Kernel must have odd dimensions");

	int v[n * m];
};

struct KernelView
{
	const int* v;
	uint n;
	uint m;

	KernelView() :
		v(nullptr), n(0), m(0)
	{}

	template<uint n, uint m>
	KernelView(const Kernel<n, m>& k)
	{
		this->v = k.v;
		this->n = n;
		this->m = m;
	}

	KernelView(const KernelView& other) :
		v(other.v),
		n(other.n),
		m(other.m)
	{}

	const int* operator[](size_t idx) const
	{
		return v + (m * idx);
	}
};

/*
	Common filters
*/
namespace kernels
{
	const Kernel<3,3> box = {
		1, 1, 1,
		1, 1, 1,
		1, 1, 1
	};

	const Kernel<3, 3> edgesV = {
		 1, 2, 1,
		 0, 0, 0,
		-1,-2,-1
	};

	const Kernel<3, 3> edgesH = {
		1, 0, -1,
		2, 0, -2,
		1, 0, -1
	};

	const Kernel<3, 3> edges2 = {
		-1, -1, -1,
		-1, 8, -1,
		-1, -1, -1,
	};

	const Kernel<3, 3> gaussian3 = {
		1, 2, 1,
		2, 4, 2,
		1, 2, 1
	};

	const Kernel<5, 5> gaussian5 = {
		1,  4,  7,  4, 1,
		4, 16, 26, 16, 4,
		7, 26, 41, 26, 7,
		4, 16, 26, 16, 4,
		1,  4,  7,  4, 1,
	};

	const Kernel<3, 3> sharpen = {
		 0, -1,  0,
		-1,  5, -1,
		 0, -1,  0
	};

	const Kernel<3, 3> emboss = {
		+1, 0, 0,
		 0, 0, 0,
		 0, 0, -1,
	};
}
