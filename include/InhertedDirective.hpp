#pragma once

template <typename T>
class InhertedDirective
{
	private:
		bool _hasValue;
		T* _value;
	public:
		InhertedDirective();
		InhertedDirective(T* val);
		InhertedDirective(const InhertedDirective& other);
		InhertedDirective& operator=(const InhertedDirective& other);
		~InhertedDirective();
};

#include "../template/InhertedDirective.tpp"
