#include "types.h"

namespace sgf {

void VertexLayout::addAttribs(CVector<AttribFormat> attribs, uint buffer, uint location, uint offset, uint divisor) {
	uint pitch = 0;
	for (auto attrib : attribs) pitch += bytesPerAttrib(attrib);
	for (auto attrib : attribs) {
		attribLayouts.push_back({attrib, buffer, location, offset, pitch, divisor});
		offset += bytesPerAttrib(attrib);
		++location;
	}
}

} // namespace sgf
