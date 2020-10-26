//
// Created by Tobias on 10/14/2020.
//

#ifndef SD3D_FILEOPENDIALOG_H
#define SD3D_FILEOPENDIALOG_H

#include <string>


namespace sd3d::io {

[[deprecated("This uses a very old WinApi")]] std::string request_file_dialog(
	const char *filter = nullptr);

}

#endif // SD3D_FILEOPENDIALOG_H
