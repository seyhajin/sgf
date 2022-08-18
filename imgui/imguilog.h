#pragma once

#include <imgui.h>

namespace sgf::ImGuiEx {

class ImGuiLog {
public:
	ImGuiLog();

	~ImGuiLog();

	void AddLog(const char* fmt, ...) IM_FMTARGS(2);

	void Draw(const char* title, bool* p_open = nullptr);

private:
	struct Rep;
	Rep* m_rep;
};

}
