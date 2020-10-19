//
// Created by Tobias on 10/14/2020.
//

#include <windows.h>

#include <spdlog/spdlog.h>

#include "FileOpenDialog.h"

static constexpr const char *g_filterStr =
	"obj files (*.obj)\0*.obj\0fbx files (*.fbx)\0*.fbx";

namespace sd3d::io {

std::string request_file_dialog(const char *filter) {
	const auto *f{(!filter ? g_filterStr : filter)};
	OPENFILENAME ofn;
	char fileName[MAX_PATH] = "";
	ZeroMemory(&ofn, sizeof(ofn));

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = nullptr;
	ofn.lpstrFilter = f;
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST;
	ofn.lpstrDefExt = "";
	if (GetOpenFileName(&ofn)) {
		return fileName;
	} else {
		return "";
	}
}
} // namespace sd3d::io
