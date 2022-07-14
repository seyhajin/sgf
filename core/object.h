#ifndef SGF_OBJECT_H
#define SGF_OBJECT_H

#include "signal.h" //NOLINT
#include "std.h"

namespace sgf {

class ObjectType {
public:
	String const name;
	ObjectType* const super;

	ObjectType(const char* name, ObjectType* super) noexcept : name(name),super(super) {
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

	virtual ObjectType* dynamicType() const {
		return &staticType;
	}

	Signal<> deleted;

	Object() = default;

	virtual ~Object();

	template <class ObjTy> bool instanceOf() const {
		return dynamicType()->extends(&ObjTy::staticType);
	}

	template <class ObjTy> ObjTy* cast() {
		return instanceOf<ObjTy>() ? static_cast<ObjTy*>(this) : nullptr;
	}

	template <class ObjTy> const ObjTy* cast() const {
		return instanceOf<ObjTy>() ? static_cast<ObjTy*>(this) : nullptr;
	}

private:
};

} // namespace sgf

#include "signal_impl.inl"

#endif
