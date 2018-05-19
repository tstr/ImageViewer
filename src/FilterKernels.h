/*
	Image filter kernels
*/

#pragma once

/*
	A filter kernel is represented by an n*m matrix
*/
template<size_t n, size_t m>
struct Kernel
{
	static_assert(n % 2 == 1, "Kernel must have odd dimensions");
	static_assert(m % 2 == 1, "Kernel must have odd dimensions");

	int v[n * m];

	operator int*() const
	{
		return v;
	}
};

/*
	Common filters
*/
namespace filters
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
		-1, -1, -1,
		-1,  8, -1,
		-1, -1, -1
	};
}