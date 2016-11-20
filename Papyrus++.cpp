/*
This file is part of Papyrus++

Copyright (C) 2016 Tschilkroete <tschilkroete@gmail.com>

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

#pragma once

#include "papyrus++.hpp"
#include "settingsWindow.hpp"

#include <string>

BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID) {
	if (reason == DLL_PROCESS_ATTACH) {
		::instance = instance;
	} else if (reason == DLL_PROCESS_DETACH) {
		cleanUp();
	}
	return TRUE;
}

extern "C" __declspec(dllexport)  const TCHAR* getName() {
	return PLUGIN_NAME;
}

extern "C" __declspec(dllexport) BOOL isUnicode() {
	return true;
}

extern "C" __declspec(dllexport) void setInfo(NppData nppData) {
	::nppData = nppData;
	init();
}

extern "C" __declspec(dllexport) FuncItem* getFuncsArray(int* count) {
	*count = funcCount;
	return funcs;
}

extern "C" __declspec(dllexport) void beNotified(SCNotification* notification) {

}

extern "C" __declspec(dllexport) LRESULT messageProc(UINT message, WPARAM wParam, LPARAM lParam) {
	return TRUE;
}

void init() {
	funcs[0] = FuncItem{ L"Compile", compile, 0, false, new ShortcutKey{true, false, true, 0x43} };
	funcs[1] = FuncItem{ L"Settings", settingsWindow, 1, false, nullptr };
	funcs[2] = FuncItem{ L"About", about, 2, false, nullptr };

	wchar_t settingsPath[MAX_PATH * sizeof(wchar_t)];
	::SendMessage(nppData._nppHandle, NPPM_GETPLUGINSCONFIGDIR, MAX_PATH * sizeof(wchar_t), (LPARAM)settingsPath);
	if (!settings.load(std::wstring(settingsPath))) {
		::MessageBox(nppData._nppHandle, L"Confirm the compiler settings.\nYou can change them later if you want in Plugins > Papyrus++ > Settings", L"Papyrus++ setup", MB_OK);
		
		DWORD size;
		::RegGetValue(HKEY_LOCAL_MACHINE, L"SOFTWARE\\bethesda softworks\\skyrim", L"installed path", RRF_RT_ANY, nullptr, nullptr, &size);
		std::vector<wchar_t> skyrimPathC(size / sizeof(wchar_t));
		::RegGetValue(HKEY_LOCAL_MACHINE, L"SOFTWARE\\bethesda softworks\\skyrim", L"installed path", RRF_RT_ANY, nullptr, &skyrimPathC[0], &size);
		std::wstring skyrimPath(&skyrimPathC[0]);
		settings.putString(L"compilerPath", skyrimPath + L"Papyrus Compiler");
		settings.putString(L"importDirectories", skyrimPath + L"Data\\Scripts\\Source");
		settings.putString(L"outputDirectory", skyrimPath + L"Data\\Scripts");
		settings.putString(L"additionalArguments", L"");
		settings.save();

		settingsWindow();
	}
}

void cleanUp() {
}

void compile() {
}

void settingsWindow() {
	SettingsWindow(settings, instance, nppData._nppHandle);
}

void about() {
	::MessageBox(nullptr, L"Copyright 2016\r\n"
	                       "Contributors:\r\n"
	                       "Tschilkroete\r\n"
	                       "\r\n"
	                       "This plugin is licenced under the GNU General Public Licence 3 https://www.gnu.org/licenses/gpl.txt \r\n"
	                       "Get the source code: https://github.com/tschilkroete/PapyrusPlusPlus", L"Papyrus++ " VERSION, MB_OK);
}