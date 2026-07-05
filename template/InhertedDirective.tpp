template <typename T>
InhertedDirective<T>::InhertedDirective() : _hasValue(false), _value(NULL)
{}

template <typename T>
InhertedDirective<T>::InhertedDirective(T* val) : _hasValue(false), _value(NULL)
{
	if (val != NULL)
	{
		_value = new T(*val);
		_hasValue = true;
	}
}

template <typename T>
InhertedDirective<T>::InhertedDirective(const InhertedDirective& other) : _hasValue(other._hasValue), _value(NULL)
{
	if (other._value != NULL)
		_value = new T(*other._value);
}

template <typename T>
InhertedDirective<T>& InhertedDirective<T>::operator=(const InhertedDirective& other)
{
	if (this != &other)
	{
		delete _value;
		_value = NULL;
		_hasValue = false;
		if (other._value != NULL)
		{
			_value = new T(*other._value);
			_hasValue = true;
		}
	}
	return *this;
}

template <typename T>
InhertedDirective<T>::~InhertedDirective()
{
	delete _value;
	_value = NULL;
	_hasValue = false;
}