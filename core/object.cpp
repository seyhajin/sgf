#include "object.h"

namespace sgf {

Object::~Object() {
	deleted.emit();
}

} // namespace sgf
