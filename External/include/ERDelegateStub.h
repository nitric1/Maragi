// XXX: No include guard

template<typename Return DELEGATE_TEMPLATE_COMMA DELEGATE_TEMPLATE_PARAM(DELEGATE_NUM_ARG)>
class ERDelegate<Return (DELEGATE_TEMPLATE_ARG(DELEGATE_NUM_ARG))>
{
public:
	virtual ~ERDelegate()
	{
	}

public:
	virtual Return invoke(DELEGATE_TEMPLATE_ARG(DELEGATE_NUM_ARG)) = 0;
	virtual Return operator ()(DELEGATE_TEMPLATE_ARG(DELEGATE_NUM_ARG)) = 0;
	virtual operator bool() = 0;
};

template<typename Return DELEGATE_TEMPLATE_COMMA DELEGATE_TEMPLATE_PARAM(DELEGATE_NUM_ARG)>
class ERDelegateImpl<Return (DELEGATE_TEMPLATE_ARG(DELEGATE_NUM_ARG))> : public ERDelegate<Return (DELEGATE_TEMPLATE_ARG(DELEGATE_NUM_ARG))>
{
private:
	std::vector<std::function<Return (DELEGATE_TEMPLATE_ARG(DELEGATE_NUM_ARG))>> fns;

public:
	ERDelegateImpl()
	{
	}

	ERDelegateImpl(const ERDelegateImpl<Return (DELEGATE_TEMPLATE_ARG(DELEGATE_NUM_ARG))> &obj)
		: fns(obj.fns)
	{
	}

	ERDelegateImpl(ERDelegateImpl<Return (DELEGATE_TEMPLATE_ARG(DELEGATE_NUM_ARG))> &&obj)
		: fns(std::move(obj.fns))
	{
	}

	ERDelegateImpl(Return (*fn)(DELEGATE_TEMPLATE_ARG(DELEGATE_NUM_ARG)))
		: fns(1, fn)
	{
	}

#define DELEGATE_BIND_ARG_ELEMENT(n) std::placeholders::_##n
#define DELEGATE_BIND_ARG(n) DELEGATE_REPEAT(n, DELEGATE_BIND_ARG_ELEMENT, DELEGATE_COMMA)

	template<typename FunctionClass, typename InstanceClass>
	ERDelegateImpl(Return (FunctionClass::*ifn)(DELEGATE_TEMPLATE_ARG(DELEGATE_NUM_ARG)), InstanceClass *ic)
		: fns(1, std::bind(ifn, ic DELEGATE_TEMPLATE_COMMA DELEGATE_BIND_ARG(DELEGATE_NUM_ARG)))
	{
	}

	template<typename FunctionClass, typename InstanceClass>
	ERDelegateImpl(Return (FunctionClass::*ifn)(DELEGATE_TEMPLATE_ARG(DELEGATE_NUM_ARG)) const, const InstanceClass *ic)
		: fns(1, std::bind(ifn, ic DELEGATE_TEMPLATE_COMMA DELEGATE_BIND_ARG(DELEGATE_NUM_ARG)))
	{
	}

	template<typename FunctionClass, typename InstanceClass>
	ERDelegateImpl(Return (FunctionClass::*ifn)(DELEGATE_TEMPLATE_ARG(DELEGATE_NUM_ARG)) volatile, volatile InstanceClass *ic)
		: fns(1, std::bind(ifn, ic DELEGATE_TEMPLATE_COMMA DELEGATE_BIND_ARG(DELEGATE_NUM_ARG)))
	{
	}

	template<typename FunctionClass, typename InstanceClass>
	ERDelegateImpl(Return (FunctionClass::*ifn)(DELEGATE_TEMPLATE_ARG(DELEGATE_NUM_ARG)) const volatile, const volatile InstanceClass *ic)
		: fns(1, std::bind(ifn, ic DELEGATE_TEMPLATE_COMMA DELEGATE_BIND_ARG(DELEGATE_NUM_ARG)))
	{
	}

	template<typename Functor>
	ERDelegateImpl(Functor fn)
		: fns(1, fn)
	{
	}

	virtual ~ERDelegateImpl()
	{
	}

public:
	ERDelegateImpl<Return (DELEGATE_TEMPLATE_ARG(DELEGATE_NUM_ARG))> &operator =(std::nullptr_t)
	{
		fns.clear();
		return *this;
	}

	ERDelegateImpl<Return (DELEGATE_TEMPLATE_ARG(DELEGATE_NUM_ARG))> &operator =(const ERDelegateImpl<Return (DELEGATE_TEMPLATE_ARG(DELEGATE_NUM_ARG))> &obj)
	{
		fns = obj.fns;
		return *this;
	}

	ERDelegateImpl<Return (DELEGATE_TEMPLATE_ARG(DELEGATE_NUM_ARG))> &operator =(ERDelegateImpl<Return (DELEGATE_TEMPLATE_ARG(DELEGATE_NUM_ARG))> &&obj)
	{
		fns = std::move(obj.fns);
		return *this;
	}

	ERDelegateImpl<Return (DELEGATE_TEMPLATE_ARG(DELEGATE_NUM_ARG))> &operator =(Return (*ifn)(DELEGATE_TEMPLATE_ARG(DELEGATE_NUM_ARG)))
	{
		fns.clear();
		fns.resize(1, fn);
		return *this;
	}

	template<typename FunctionClass, typename InstanceClass>
	ERDelegateImpl<Return (DELEGATE_TEMPLATE_ARG(DELEGATE_NUM_ARG))> &operator =(std::pair<Return (FunctionClass::*)(DELEGATE_TEMPLATE_ARG(DELEGATE_NUM_ARG)), InstanceClass *> fn)
	{
		fns.clear();
		fns.resize(1, std::bind(fn.first, fn.second DELEGATE_TEMPLATE_COMMA DELEGATE_BIND_ARG(DELEGATE_NUM_ARG)));
		return *this;
	}

	template<typename FunctionClass, typename InstanceClass>
	ERDelegateImpl<Return (DELEGATE_TEMPLATE_ARG(DELEGATE_NUM_ARG))> &operator =(std::pair<Return (FunctionClass::*)(DELEGATE_TEMPLATE_ARG(DELEGATE_NUM_ARG)) const, const InstanceClass *> fn)
	{
		fns.clear();
		fns.resize(1, std::bind(fn.first, fn.second DELEGATE_TEMPLATE_COMMA DELEGATE_BIND_ARG(DELEGATE_NUM_ARG)));
		return *this;
	}

	template<typename FunctionClass, typename InstanceClass>
	ERDelegateImpl<Return (DELEGATE_TEMPLATE_ARG(DELEGATE_NUM_ARG))> &operator =(std::pair<Return (FunctionClass::*)(DELEGATE_TEMPLATE_ARG(DELEGATE_NUM_ARG)) volatile, volatile InstanceClass *> fn)
	{
		fns.clear();
		fns.resize(1, std::bind(fn.first, fn.second DELEGATE_TEMPLATE_COMMA DELEGATE_BIND_ARG(DELEGATE_NUM_ARG)));
		return *this;
	}

	template<typename FunctionClass, typename InstanceClass>
	ERDelegateImpl<Return (DELEGATE_TEMPLATE_ARG(DELEGATE_NUM_ARG))> &operator =(std::pair<Return (FunctionClass::*)(DELEGATE_TEMPLATE_ARG(DELEGATE_NUM_ARG)) const volatile, const volatile InstanceClass *> fn)
	{
		fns.clear();
		fns.resize(1, std::bind(fn.first, fn.second DELEGATE_TEMPLATE_COMMA DELEGATE_BIND_ARG(DELEGATE_NUM_ARG)));
		return *this;
	}

	template<typename Functor>
	ERDelegateImpl<Return (DELEGATE_TEMPLATE_ARG(DELEGATE_NUM_ARG))> &operator =(Functor fn)
	{
		fns.clear();
		fns.resize(1, fn);
		return *this;
	}

	ERDelegateImpl<Return (DELEGATE_TEMPLATE_ARG(DELEGATE_NUM_ARG))> &operator +=(std::nullptr_t)
	{
		return *this;
	}

	ERDelegateImpl<Return (DELEGATE_TEMPLATE_ARG(DELEGATE_NUM_ARG))> &operator +=(const ERDelegateImpl<Return (DELEGATE_TEMPLATE_ARG(DELEGATE_NUM_ARG))> &obj)
	{
		fns.insert(std::end(fns), std::begin(obj.fns), std::end(obj.fns));
		return *this;
	}

	ERDelegateImpl<Return (DELEGATE_TEMPLATE_ARG(DELEGATE_NUM_ARG))> &operator +=(Return (*ifn)(DELEGATE_TEMPLATE_ARG(DELEGATE_NUM_ARG)))
	{
		fns.push_back(fn);
		return *this;
	}

	template<typename FunctionClass, typename InstanceClass>
	ERDelegateImpl<Return (DELEGATE_TEMPLATE_ARG(DELEGATE_NUM_ARG))> &operator +=(std::pair<Return (FunctionClass::*)(DELEGATE_TEMPLATE_ARG(DELEGATE_NUM_ARG)), InstanceClass *> fn)
	{
		fns.push_back(std::bind(fn.first, fn.second DELEGATE_TEMPLATE_COMMA DELEGATE_BIND_ARG(DELEGATE_NUM_ARG)));
		return *this;
	}

	template<typename FunctionClass, typename InstanceClass>
	ERDelegateImpl<Return (DELEGATE_TEMPLATE_ARG(DELEGATE_NUM_ARG))> &operator +=(std::pair<Return (FunctionClass::*)(DELEGATE_TEMPLATE_ARG(DELEGATE_NUM_ARG)) const, const InstanceClass *> fn)
	{
		fns.push_back(std::bind(fn.first, fn.second DELEGATE_TEMPLATE_COMMA DELEGATE_BIND_ARG(DELEGATE_NUM_ARG)));
		return *this;
	}

	template<typename FunctionClass, typename InstanceClass>
	ERDelegateImpl<Return (DELEGATE_TEMPLATE_ARG(DELEGATE_NUM_ARG))> &operator +=(std::pair<Return (FunctionClass::*)(DELEGATE_TEMPLATE_ARG(DELEGATE_NUM_ARG)) volatile, volatile InstanceClass *> fn)
	{
		fns.push_back(std::bind(fn.first, fn.second DELEGATE_TEMPLATE_COMMA DELEGATE_BIND_ARG(DELEGATE_NUM_ARG)));
		return *this;
	}

	template<typename FunctionClass, typename InstanceClass>
	ERDelegateImpl<Return (DELEGATE_TEMPLATE_ARG(DELEGATE_NUM_ARG))> &operator +=(std::pair<Return (FunctionClass::*)(DELEGATE_TEMPLATE_ARG(DELEGATE_NUM_ARG)) const volatile, const volatile InstanceClass *> fn)
	{
		fns.push_back(std::bind(fn.first, fn.second DELEGATE_TEMPLATE_COMMA DELEGATE_BIND_ARG(DELEGATE_NUM_ARG)));
		return *this;
	}

	template<typename Functor>
	ERDelegateImpl<Return (DELEGATE_TEMPLATE_ARG(DELEGATE_NUM_ARG))> &operator +=(Functor fn)
	{
		fns.push_back(fn);
		return *this;
	}

#undef DELEGATE_BIND_ARG_ELEMENT
#undef DELEGATE_BIND_ARG

public:
	virtual Return invoke(DELEGATE_FUNCTION_PARAM(DELEGATE_NUM_ARG))
	{
		if(fns.empty())
			return Return();
		else
		{
			auto it = std::begin(fns), ed = -- std::end(fns);
			for(; it != ed; ++ it)
				(*it)(DELEGATE_FUNCTION_ARG(DELEGATE_NUM_ARG));
			return (*it)(DELEGATE_FUNCTION_ARG(DELEGATE_NUM_ARG));
		}
	}

	virtual Return operator ()(DELEGATE_FUNCTION_PARAM(DELEGATE_NUM_ARG))
	{
		if(fns.empty())
			return Return();
		else
		{
			auto it = std::begin(fns), ed = -- std::end(fns);
			for(; it != ed; ++ it)
				(*it)(DELEGATE_FUNCTION_ARG(DELEGATE_NUM_ARG));
			return (*it)(DELEGATE_FUNCTION_ARG(DELEGATE_NUM_ARG));
		}
	}

	virtual operator bool()
	{
		return !fns.empty();
	}
};
