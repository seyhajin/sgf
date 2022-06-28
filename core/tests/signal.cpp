
#include <core/core.hh>

#include <type_traits>

using namespace sgf;

// std::enable_if_t<std::is_arithmetic<LhsTy>::value && std::is_base_of<Op, RhsTy>::value, bool> = true>

struct Value {};

// template <class ValueTy>// Can't do this 'coz SFINAE only works in 'immediate context'?
template <class ValueTy> inline constexpr bool is_evaluable_v = std::is_invocable_v<decltype(&ValueTy::eval), ValueTy&>;

template <class ValueTy> struct ConstExpr : public Value {

	ValueTy value;

	ConstExpr(const ValueTy& value) : value(value) {}

	auto eval() const { return value; }
};

template <class ValueTy> auto expr(const ValueTy& value) { return ConstExpr<ValueTy>(value); }

template <class ValueTy> struct PropertyExpr : public Value {

	const Property<ValueTy>& property;

	PropertyExpr(const Property<ValueTy>& property) : property(property) {}

	auto eval() const { return property.value(); }
};

template <class ValueTy> auto expr(const Property<ValueTy>& value) { return PropertyExpr<ValueTy>(value); }

template <class LhsTy, class RhsTy> struct AddExpr : public Value {

	LhsTy lhs;
	RhsTy rhs;

	AddExpr(const LhsTy& lhs, const RhsTy& rhs) : lhs(lhs), rhs(rhs) {}

	auto eval() const { return lhs.eval() + rhs.eval(); }
};

template <class LhsTy, class RhsTy,
		  std::enable_if_t<											 //
			  std::is_invocable_v<decltype(&LhsTy::eval), LhsTy&> && //
				  std::is_invocable_v<decltype(&RhsTy::eval), RhsTy&>,
			  bool> = true>
auto operator+(const LhsTy& lhs, const RhsTy& rhs) {

	return AddExpr<LhsTy, RhsTy>(lhs, rhs);
}

template <class LhsTy, class RhsTy,
		  std::enable_if_t<											 //
			  std::is_invocable_v<decltype(&LhsTy::eval), LhsTy&> && //
				  std::is_arithmetic_v<RhsTy>,
			  bool> = true>
auto operator+(const LhsTy& lhs, const RhsTy& rhs) {

	return AddExpr<LhsTy, ConstExpr<RhsTy>>(lhs, expr(rhs));
}

template <class LhsTy, class RhsTy,
		  std::enable_if_t<					 //
			  std::is_arithmetic_v<LhsTy> && //
				  std::is_invocable_v<decltype(&RhsTy::eval), RhsTy&>,
			  bool> = true>
auto operator+(const LhsTy& lhs, const RhsTy& rhs) {

	return AddExpr<ConstExpr<LhsTy>, RhsTy>(expr(lhs), rhs);
}

template <class LhsTy, class RhsTy> auto operator+(const ConstExpr<LhsTy>& lhs, const ConstExpr<RhsTy>& rhs) {

	return expr(lhs.eval() + rhs.eval());
}

template <class ExprTy> struct LazyExpr {};

template <class LhsTy, class RhsTy> auto operator+(const LazyExpr<LhsTy>& lhs, const LazyExpr<RhsTy>& rhs) {}

int main() {

	Property<int> x{5};

	auto y = expr(3);

	auto sum = expr(x) + 20 + y;

	debug() << sum.eval();

	x = 10;

	debug() << sum.eval();

	return 0;
}
