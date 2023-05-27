#pragma once

namespace Atrium {
	struct Triangle{
	public:
		Triangle(unsigned int index0, unsigned int index1, unsigned int index2);
		Triangle operator+(unsigned int obj) const;
	public:
		unsigned int index0, index1, index2;
	};
}



