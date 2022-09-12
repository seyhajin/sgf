#pragma once

#include <geom/geom.hh>

namespace sgf {

class ActorType : public ObjectType {
public:
	uint const typeId;

	ActorType(const char* name, ObjectType* super, uint typeId) noexcept : ObjectType(name, super), typeId(typeId) {
	}
};

#define SGF_ACTOR_TYPE(name, super, typeId)                                                                            \
	using Super = super;                                                                                               \
	static inline ActorType staticType{#name, &Super::staticType, typeId};                                             \
	ActorType* dynamicType() const override { return &staticType; }

} // namespace sgf
