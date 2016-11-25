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

#include "compilationThread.hpp"
#include "messages.hpp"

#include <fstream>
#include <string>

CompilationThread::CompilationThread(HWND window, const Settings& settings) : window(window), settings(settings) {
}

void CompilationThread::start(std::wstring inputFile) {
	if (!compilationThread.joinable())
		compilationThread = std::thread(run, std::ref(*this), inputFile);
}

void CompilationThread::run(CompilationThread& compilationThread, std::wstring inputFile) {
	const Settings& settings = compilationThread.settings;
	std::wstring path = settings.getString(L"compilerPath");
	if (std::ifstream(path).good()) {
		std::wstring commandLine = L"\"" + path + L"\" \"" + inputFile + L"\" -import=\"" + settings.getString(L"importDirectories") +
		                                                                 L"\" -output=\"" + settings.getString(L"outputDirectory") +
		                                                                 L"\" -flags=\"" + settings.getString(L"flagFile") +
		                                                                 L"\" " + settings.getString(L"additionalArguments");
		STARTUPINFO startupInfo = {};
		startupInfo.dwFlags = STARTF_USESTDHANDLES;
		HANDLE errorReadHandle;
		SECURITY_ATTRIBUTES attr = {};
		attr.bInheritHandle = TRUE;
		::CreatePipe(&errorReadHandle, &startupInfo.hStdError, &attr, 65536);
		PROCESS_INFORMATION compilationProcess = {};
		::CreateProcess(nullptr, &commandLine[0], nullptr, nullptr, TRUE, CREATE_NO_WINDOW | CREATE_UNICODE_ENVIRONMENT, nullptr, nullptr, &startupInfo, &compilationProcess);
		::WaitForSingleObject(compilationProcess.hProcess, INFINITE);

		DWORD size;
		::PeekNamedPipe(errorReadHandle, nullptr, 0, nullptr, &size, nullptr);
		if (size > 0) {
			std::vector<char> data(size);
			::ReadFile(errorReadHandle, &data[0], size, nullptr, nullptr);

			std::wstring errorOutput(data.begin(), data.end());
			::SendMessage(compilationThread.window, PPPM_COMPILATIONFAILED, reinterpret_cast<WPARAM>(&errorOutput[0]), 0);
		} else {
			::SendMessage(compilationThread.window, PPPM_COMPILATIONDONE, 0, 0);
		}
	} else {
		::SendMessage(compilationThread.window, PPPM_COMPILERNOTFOUND, 0, 0);
	}
	compilationThread.compilationThread.detach();
}