#include "imgui.hh"

#include <window/glwindow.h>

namespace sgf::ImGuiEx {

void CreateContext(Window* window) {
	assert(window->instanceOf<GLWindow>());
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window->cast<GLWindow>()->glfwWindow(), true);
	ImGui_ImplOpenGL3_Init(nullptr);
}

void DestroyContext() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void NewFrame() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
//	ImGui::ShowDemoWindow();
}

void Render() {
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

DebugStream Debug() {
	return {[](CString output) { //
		ImGui::TextUnformatted(output.c_str(), nullptr);
	}};
}

DebugStream DebugRow() {
	return {[](CString output) {
				ImGui::TableNextRow();
				auto fields = split(output, "|");
				for (auto field : fields) {
					ImGui::TableNextColumn();
					ImGui::TextUnformatted(field.c_str(), nullptr);
				}
			},
			"|"};
}

bool Checkbox(const char* label, Property<bool>& v) {
	auto value = v.value();
	if (!ImGui::Checkbox(label, &value)) return false;
	v = value;
	return true;
}

bool InputInt(const char* label, Property<int>& v, int step, int step_fast, ImGuiInputTextFlags flags) {
	auto value = v.value();
	if (!ImGui::InputInt(label, &value, step, step_fast, flags)) return false;
	v = value;
	return true;
}

bool DragFloat(const char* label, sgf::Property<float>& v, float v_speed, float v_min, float v_max, const char* format,
			   ImGuiSliderFlags flags) {
	auto value = v.value();
	if (!ImGui::DragFloat(label, &value, v_speed, v_min, v_max, format, flags)) return false;
	v = value;
	return true;
}

bool FilePath(const char* label, sgf::Property<String>& v, const char* dialogTitle, const char* vFilters) {

	String dialogKey = "[" + String(label) + "]";

	if (ImGui::Button(label)) { ImGuiFileDialog::Instance()->OpenDialog(dialogKey, dialogTitle, vFilters, "."); }

	if (!ImGuiFileDialog::Instance()->Display(dialogKey)) return false;

	bool ok = false;
	// action if OK
	if (ImGuiFileDialog::Instance()->IsOk()) {
		v = ImGuiFileDialog::Instance()->GetFilePathName();
		ok = true;
	}

	// close
	ImGuiFileDialog::Instance()->Close();
	return ok;
}

bool ColorEdit4(const char* label, sgf::Property<sgf::Vec4f>& v, ImGuiColorEditFlags flags) {
	auto value = v.value();
	if (!ImGui::ColorEdit4(label, &value.x, flags)) return false;
	v = value;
	return true;
}

bool ColorPicker4(const char* label, sgf::Property<sgf::Vec4f>& v, ImGuiColorEditFlags flags, const float* ref_col) {
	auto value = v.value();
	if (!ImGui::ColorPicker4(label, &value.x, flags)) return false;
	v = value;
	return true;
}

} // namespace sgf::ImGuiEx
