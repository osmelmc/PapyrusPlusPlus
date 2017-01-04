/*
This file is part of Papyrus++

Copyright (C) 2016 - 2017 Tschilkroete <tschilkroete@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "resources.hpp"

#include "npp\DockingDlgInterface.h"
#include "npp\PluginInterface.h"
#include <windows.h>

#include <string>
#include <vector>

struct Error {
	std::wstring file;
	std::wstring message;
	int line;
	int column;
};

class WindowErrors : public DockingDlgInterface
{
public:
	WindowErrors(const NppData& nppData, HINSTANCE instance);
	void show(std::vector<Error> errors);
	void clear();
protected:
	virtual INT_PTR CALLBACK run_dlgProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	void resize();
	HWND listView;
	const NppData& nppData;
	std::vector<Error> errors;
};
