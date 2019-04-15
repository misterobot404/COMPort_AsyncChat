#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <iostream>
#include <string>
using namespace std;

LPCTSTR pcComPort = L"COM2";
HANDLE hCom;

VOID WINAPI ThreadSend(PVOID* dummy) {
	while (true) {
		OVERLAPPED lpOverlapped;
		lpOverlapped.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		lpOverlapped.Offset = 0;
		lpOverlapped.OffsetHigh = 0;

		string message;
		getline(cin, message);
		cout << "Отправлено сообщение \"" << message << "\"." << endl;
		message = "Получено сообщение от COM 2: " + message;
		WriteFile(hCom, message.c_str(), message.length() + 1, NULL, &lpOverlapped);
		WaitForSingleObject(lpOverlapped.hEvent, INFINITE);
	}
}

int main(void) {
	setlocale(LC_ALL, "Russian");
	DCB dcb;
	FILE* fout;
	hCom = CreateFile(pcComPort, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_NO_BUFFERING | FILE_FLAG_OVERLAPPED, NULL);
	if (hCom == INVALID_HANDLE_VALUE) {
		printf("Ошибка открытия COM 2!\n");
		system("pause");
		return 1;
	}
	GetCommState(hCom, &dcb);
	dcb.BaudRate = CBR_9600;
	dcb.ByteSize = 8;
	dcb.Parity = NOPARITY;
	dcb.StopBits = ONESTOPBIT;
	BOOL fSuccess = SetCommState(hCom, &dcb);
	if (!fSuccess) {
		printf("Попытка вызвать SetCommState провалилась!\n");
		system("pause");
		return 1;
	}
	cout << "COM 2 порт успешно сконфигурирован.\n";
	GetCommState(hCom, &dcb);
	cout << "Скорость порта равна " << dcb.BaudRate << endl;

	DWORD dwParam, dwThreadId;
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadSend, &dwParam, 0, &dwThreadId);
	cout << "Программа работает в асинхронном режиме." << endl;
	cout << "Принимайте и отправляйте сообщения в COM 1 (через Enter).\n" << endl;
	char sReceivedChar;

	while (true)
	{
		OVERLAPPED lpOverlapped;
		lpOverlapped.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		lpOverlapped.Offset = 0;
		lpOverlapped.OffsetHigh = 0;

		ReadFile(hCom, &sReceivedChar, 1, NULL, &lpOverlapped);

		WaitForSingleObject(lpOverlapped.hEvent, INFINITE);

		if (sReceivedChar == '\0')
		{
			cout << endl;
			continue;
		}
		cout << sReceivedChar;
	}
	CloseHandle(hCom);
	return 0;
}