//
// Created by Tobias on 10/14/2020.
//

#ifndef SD3D_FILEOPENDIALOG_H
#define SD3D_FILEOPENDIALOG_H

#include <string>

// TODO also look at https://github.com/AirGuanZ/imgui-filebrowser
// TODO also look at
// https://docs.microsoft.com/en-us/previous-versions/windows/desktop/legacy/bb776913(v=vs.85)

namespace sd3d::io {

[[deprecated("This uses a very old WinApi")]] std::string request_file_dialog(
	const char *filter = nullptr);

}

#endif // SD3D_FILEOPENDIALOG_H
