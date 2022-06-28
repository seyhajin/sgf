#pragma once

#include <imgui/imgui.h>

#include <dev3d/dev3d.h>

namespace wb {
SGF_SHARED_CLASS(Image);
}

#if 0

namespace ImGuiEx {

bool Image(const char* label, wb::Property<wb::ImagePtr>& v, const ImVec2& size, const ImVec2& uv0 = ImVec2(0, 0),
		   const ImVec2& uv1 = ImVec2(1, 1), int frame_padding = -1, const ImVec4& bg_col = ImVec4(0, 0, 0, 0),
		   const ImVec4& tint_col =
			   ImVec4(1, 1, 1, 1)); // <0 frame_padding uses default frame padding settings. 0 for no padding

bool Texture(const char* label, wb::Property<wb::TexturePtr>& v, const ImVec2& size, const ImVec2& uv0 = ImVec2(0, 0),
			 const ImVec2& uv1 = ImVec2(1, 1), int frame_padding = -1, const ImVec4& bg_col = ImVec4(0, 0, 0, 0),
			 const ImVec4& tint_col =
				 ImVec4(1, 1, 1, 1)); // <0 frame_padding uses default frame padding settings. 0 for no padding

} // namespace ImGuiEx

#endif
