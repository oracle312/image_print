#include <windows.h>
#include <gdiplus.h>
#include <vector>
#include <string>
#include <iostream>

#pragma comment (lib,"Gdiplus.lib")
#pragma comment (lib,"winspool.lib")

using namespace Gdiplus;
using namespace std;

enum {
	BLACK,
	DARK_BLUE,
	DARK_GREEN,
	DARK_SKYBLUE,
	DARK_RED,
	DARK_VOILET,
	DAKR_YELLOW,
	GRAY,
	DARK_GRAY,
	BLUE,
	GREEN,
	SKYBLUE,
	RED,
	VIOLET,
	YELLOW,
	WHITE,
};

void SetColor(unsigned short text)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), text);
}

void PrintImage(HDC hdc, const std::wstring& imagePath) {
	// GDI+ Bitmap ��ü�� ����Ͽ� �̹����� �ε�
	Bitmap* image = Bitmap::FromFile(imagePath.c_str());
	if (!image || image->GetLastStatus() != Ok) {
		std::wcerr << L"�̹��� �ε� ���� : " << imagePath << std::endl;
		delete image;
		return;
	}

	// �̹��� ũ�� ��������
	int width = image->GetWidth();
	int height = image->GetHeight();

	// ������ �ػ� ��������
	int printerWidth = GetDeviceCaps(hdc, HORZRES);
	int printerHeight = GetDeviceCaps(hdc, VERTRES);

	// ���� ���
	float scaleX = static_cast<float>(printerWidth) / width;
	float scaleY = static_cast<float>(printerHeight) / height;
	float scale = min(scaleX, scaleY);

	// �� ũ�� ���
	int newWidth = static_cast<int>(width * scale);
	int newHeight = static_cast<int>(height * scale);

	// �̹��� ������
	Graphics graphics(hdc);
	graphics.DrawImage(image, 0, 0, newWidth, newHeight);

	delete image;
}

void PrintImages(const std::vector<std::wstring>& imagePaths) {
	// GDI+ �ʱ�ȭ
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	// �⺻ ������ ��������
	DWORD size = 0;
	GetDefaultPrinter(nullptr, &size);
	std::vector<wchar_t> printerName(size);
	if (!GetDefaultPrinter(printerName.data(), &size)) {
		std::cerr << "�⺻�����͸� �������� ���߽��ϴ�, �⺻�����͸� �������ּ���." << std::endl;
		GdiplusShutdown(gdiplusToken);
		return;
	}

	// ������ DC ��������
	HDC hdc = CreateDC(nullptr, printerName.data(), nullptr, nullptr);
	if (!hdc) {
		std::cerr << "������ DC�� �������� ���߽��ϴ�." << std::endl;
		GdiplusShutdown(gdiplusToken);
		return;
	}

	DOCINFO di;
	ZeroMemory(&di, sizeof(di));
	di.cbSize = sizeof(di);
	di.lpszDocName = L"Image Print";

	// ���� ����
	if (StartDoc(hdc, &di) <= 0) {
		std::cerr << "�����͸� �������� ���߽��ϴ�." << std::endl;
		DeleteDC(hdc);
		GdiplusShutdown(gdiplusToken);
		return;
	}

	for (const auto& imagePath : imagePaths) {
		// ������ ����
		if (StartPage(hdc) <= 0) {
			std::cerr << "�������� �������� ���߽��ϴ�" << std::endl;
			continue;
		}

		// �̹��� �μ�
		PrintImage(hdc, imagePath);

		// ������ ��
		EndPage(hdc);
	}

	// ���� ��
	EndDoc(hdc);
	DeleteDC(hdc);

	// GDI+ ����
	GdiplusShutdown(gdiplusToken);
}

int main() {
	SetConsoleTitle(L"����ý���");
	SetColor(SKYBLUE);
	cout << "______________________________________________" << endl;
	cout << "                 ����ý���                    " << endl;
	SetColor(GREEN);
	cout << "                          Code By. SIYUL PARK" << endl;
	SetColor(SKYBLUE);
	cout << "______________________________________________" << endl;
	cout << endl;
	SetColor(RED);
	cout << "[�˸�] ��ũ������ �����ϱ����� ������ �μ��Դϴ�." << endl;
	cout << "[�˸�] Ȯ���� ���� �μ����ּ��� !!" << endl;

	std::vector<std::wstring> imagePaths = {
		L"print1",
		L"print2",
		L"print3",
		L"print4",
		L"print5",
		// �� ���� �̹��� ��� �߰�
	};

	PrintImages(imagePaths);

	return 0;
}