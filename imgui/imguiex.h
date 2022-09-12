#pragma once

#include <core/core.hh>
#include <geom/geom.hh>

namespace sgf {

class Window;

namespace ImGuiEx {

void CreateContext(Window* window);

void DestroyContext();

void NewFrame();

void Render();

DebugStream Debug();

DebugStream DebugRow();

bool Checkbox(const char* label, sgf::Property<bool>& v);

bool InputInt(const char* label, sgf::Property<int>& v, int step = 1, int step_fast = 100,
			  ImGuiInputTextFlags flags = 0);

bool DragFloat(const char* label, sgf::Property<float>& v, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f,
			   const char* format = "%.3f", ImGuiSliderFlags flags = 0);

bool FilePath(const char* label, sgf::Property<sgf::String>& v, const char* dialogTitle, const char* vFilters);

bool ColorEdit4(const char* label, sgf::Property<sgf::Vec4f>& v, ImGuiColorEditFlags flags = 0);

bool ColorPicker4(const char* label, sgf::Property<sgf::Vec4f>& v, ImGuiColorEditFlags flags = 0,
				  const float* ref_col = nullptr);

} // namespace ImGuiEx

} // namespace sgf
