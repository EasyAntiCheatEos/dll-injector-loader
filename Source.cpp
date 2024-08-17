#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>

// keyauth
#include "Auth/auth.hpp"
#include "Auth/utils.hpp"
#include "Auth/skStr.h"

// dll injection
#include "Injection/injector.h"

using namespace KeyAuth;

// creds
std::string name = skCrypt("nurmolo").decrypt();
std::string ownerid = skCrypt("RR2z8bcHil").decrypt();
std::string secret = skCrypt("861eac1bd0fbbc506e0b6a8975474af2cea6af138cfacfa8794cc127a8cb1217").decrypt();
std::string version = skCrypt("1.0").decrypt();
std::string url = skCrypt("https://keyauth.win/api/1.2/").decrypt();
std::string path = skCrypt("").decrypt();

api auth(name, ownerid, secret, version, url, path);

struct userdata
{
	std::string subname;
	std::string expiry; // prob a string returned by keyauth???
} usrdat;
// get user sub
userdata getusersub()
{
	for (int i = 0; i < auth.user_data.subscriptions.size(); i++)
	{
		usrdat.subname = auth.user_data.subscriptions[i].name;
		usrdat.expiry = auth.user_data.subscriptions[i].expiry;

		return usrdat;
	}
}
DWORD FindProcessId(const std::wstring& processName)
{
	PROCESSENTRY32 processInfo;
	processInfo.dwSize = sizeof(processInfo);

	HANDLE processesSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (processesSnapshot == INVALID_HANDLE_VALUE)
		return 0;

	Process32First(processesSnapshot, &processInfo);
	if (!processName.compare(processInfo.szExeFile))
	{
		CloseHandle(processesSnapshot);
		return processInfo.th32ProcessID;
	}

	while (Process32Next(processesSnapshot, &processInfo))
	{
		if (!processName.compare(processInfo.szExeFile))
		{
			CloseHandle(processesSnapshot);
			return processInfo.th32ProcessID;
		}
	}

	CloseHandle(processesSnapshot);
	return 0;
}
std::string username;
std::string password;
std::string lickey;

INT selected;
INT logintype;

struct config
{
	std::wstring targetprocess = L"notepad.exe";
	INT targetpid;
	HANDLE targethandle;
	std::vector<BYTE> dllbytes;
}cfg;
auto main(void) -> int
{
	SetConsoleTitleW(L"Loader Login & Register");
	std::cout << "Options -> \n 1 : Register \n 2 : Login \n";
	std::cout << "\n \\_ Login -> ";

	std::cin >> selected;

	switch (selected)
	{
	case 1:
	{
		logintype = 1;
		break;
	}

	case 2:
	{
		logintype = 2;
		break;
	}

	default:
	{
		system("cls");
		system("color 4");
		std::cout << " \n Invaild selection...\n";
		Sleep(1500);
		return 420;
	}
	}
	// switch

	auth.init();
	if (!auth.response.success)
	{
		MessageBoxW(GetConsoleWindow(), L"Failed to initlize auth!!!", L"Loader", MB_OK | MB_ICONWARNING);
		return -1;
	}
	system("cls");
	switch (logintype)
	{
	case 1:
	{
		SetConsoleTitleW(L"Loader Register");
		std::cout << "\n Loader Register \\\n";

		std::cout << "\n License key -> ";
		std::cin >> lickey;

		std::cout << "\n Username -> ";
		std::cin >> username;

		std::cout << "\n Password -> ";
		std::cin >> password;


		auth.init();

		auth.check();

		auth.regstr(username, password, lickey);

		Sleep(1000);
		std::cout << "\n Loader -> Success\n";
		return -1;
		break;
	}

	case 2:
	{
		SetConsoleTitleW(L"Loader Login");
		std::cout << "\n Loader Login \\n";

		std::cout << "\n Username -> ";
		std::cin >> username;

		std::cout << "\n Password -> ";
		std::cin >> password;


		auth.login(username, password);
		if (auth.response.success)
		{
			SetConsoleTitleW(L"Loader Login Success");
			std::cout << "\n Loader -> Login success! \n";
		}
		else
		{
			return -1;
		}

		auth.log("User logged on");
		auth.check();

		system("cls");

		getusersub();

		// this is where you check for the user sub ex:gorrilatagsub

		if (usrdat.subname == "gorrilatag")
		{
			cfg.dllbytes = auth.download("835444");
			cfg.targetpid = FindProcessId(L"notepad.exe");
			while (cfg.targetpid == NULL)
				{
				cfg.targetpid = FindProcessId(L"notepad.exe");
				std::cout << "\n Injecter -> Please open process !!! \n";
					Sleep(1000);
				}
			cfg.targethandle = OpenProcess(PROCESS_ALL_ACCESS, false, cfg.targetpid);
			if (ManualMapDll(cfg.targethandle,cfg.dllbytes.data(),cfg.dllbytes.size()))
			{
				std::cout << "\n Injecter -> Injected! \n";
			}
			else
			{
				std::cout << "\n\n Injector -> FAILED!!!! \n";
				return -1;
			}
			// delete dll from memory
			ZeroMemory(&cfg.dllbytes, sizeof(cfg.dllbytes));
		}


		break;
	}
	default:
	{
		std::cout << "if u see this what the fuck have u done...\n";
		return 420;
	}
	}

	return 0;
}