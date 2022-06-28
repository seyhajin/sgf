#include "imguiex.h"

#include "image.h"

using namespace wb;

#if 0

namespace ImGuiEx {

namespace {

Map<wb::Image*, TexturePtr> imageTextureCache;

}

bool Image(const char* label, wb::Property<ImagePtr>& v, const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1,
		   int frame_padding, const ImVec4& bg_col, const ImVec4& tint_col) {

	wb::Image* image = v.value();
	auto it = imageTextureCache.find(image);

	if (it == imageTextureCache.end()) {
		auto newImage = resizeImage(image, 128, 128);
		auto texture = graphicsDevice()->createTexture(newImage->data, TextureType::texture2d, newImage->width,
													   newImage->height, 0, newImage->format, TextureFlags::mipmap);
		it = imageTextureCache.insert(std::make_pair(image, texture)).first;
		image->deleted.connect([image]() { imageTextureCache.erase(image); });
	}

	auto texture = it->second.get();

	GLTexture* glTexture = static_cast<GLTexture*>(texture);
	assert(glTexture);
	String path = texturePath(glTexture);
	if (path.empty()) path = ".";

	String dialogKey = "[" + String(label) + "]";

	ImGui::Text("%s", label);
	//	SameLine();

	if (ImGui::ImageButton((ImTextureID)(size_t)glTexture->glTexture, size, uv0, uv1, frame_padding, bg_col,
						   tint_col)) {
		String dialogTitle = "Open image...";
		const char* vFilters = ".png,.jpg";
		ImGuiFileDialog::Instance()->OpenDialog(dialogKey, dialogTitle, vFilters, path);
	}

	if (!ImGuiFileDialog::Instance()->Display(dialogKey)) return false;

	// action if OK
	bool ok = false;
	if (ImGuiFileDialog::Instance()->IsOk()) {
		debug() << "### Loading image:" << ImGuiFileDialog::Instance()->GetFilePathName();
		auto newImage = loadImage(ImGuiFileDialog::Instance()->GetFilePathName(), texture->format);
		if (newImage) {
			v = newImage;
			ok = true;
		}
	}

	// close
	ImGuiFileDialog::Instance()->Close();
	return ok;
}

bool Texture(const char* label, wb::Property<TexturePtr>& v, const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1,
			 int frame_padding, const ImVec4& bg_col, const ImVec4& tint_col) {

	wb::Texture* texture = v.value();

	GLTexture* glTexture = static_cast<GLTexture*>(texture);
	assert(glTexture);
	String path = texturePath(glTexture);
	if (path.empty()) path = ".";

	String dialogKey = "[" + String(label) + "]";

	ImGui::Text("%s", label);
	//	SameLine();

	if (ImGui::ImageButton((ImTextureID)(size_t)glTexture->glTexture, size, uv0, uv1, frame_padding, bg_col,
						   tint_col)) {
		String dialogTitle = "Open texture...";
		const char* vFilters = ".png,.jpg";
		ImGuiFileDialog::Instance()->OpenDialog(dialogKey, dialogTitle, vFilters, path);
	}

	if (!ImGuiFileDialog::Instance()->Display(dialogKey)) return false;

	bool ok = false;
	// action if OK
	if (ImGuiFileDialog::Instance()->IsOk()) {
		debug() << "### Loading texture:" << ImGuiFileDialog::Instance()->GetFilePathName();
		auto newTexture = loadTexture(ImGuiFileDialog::Instance()->GetFilePathName(), texture->format, texture->flags);
		if (texture) {
			v = newTexture;
			ok = true;
		}
	}

	// close
	ImGuiFileDialog::Instance()->Close();
	return ok;
}

} // namespace ImGuiEx

#endif
