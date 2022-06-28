#ifndef SGF_OBJECT_H
#define SGF_OBJECT_H

#include "signal.h" //NOLINT
#include "std.h"

namespace sgf {

class ObjectType {
public:
	String const name;
	ObjectType* const super;

	ObjectType(String name, ObjectType* super) : name(std::move(name)), super(super) {
	}

	bool extends(const ObjectType* thatType) const {
		for (auto thisType = this; thisType; thisType = thisType->super) {
			if (thisType == thatType) return true;
		}
		return false;
	}
};

#define SGF_OBJECT_TYPE(name, super)                                                                                    \
	static inline ObjectType staticType{#name, &super::staticType};                                                    \
	ObjectType* dynamicType() const override {                                                                         \
		return &staticType;                                                                                            \
	}

class Object {
public:
	// Special case manual expansion of SGF_OBJECT_TYPE for root object.
	static inline ObjectType staticType{"Object", nullptr};

	Signal<> deleted;

	virtual ObjectType* dynamicType() const {
		return &staticType;
	}

	Object() = default;
	virtual ~Object();

	template <class ObjTy> bool instanceOf() const {	//NOLINT (nodiscard)
		return dynamicType()->extends(&ObjTy::staticType);
	}

	template <class ObjTy> ObjTy* cast() {
		if (instanceOf<ObjTy>()) return static_cast<ObjTy*>(this);
		return nullptr;
	}

	template <class ObjTy> const ObjTy* cast() const {
		if (instanceOf<ObjTy>()) return static_cast<const ObjTy*>(this);
		return nullptr;
	}

private:
};

} // namespace sgf

#include "signal_impl.inl"

#endif
