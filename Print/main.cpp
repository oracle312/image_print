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
	// GDI+ Bitmap 객체를 사용하여 이미지를 로드
	Bitmap* image = Bitmap::FromFile(imagePath.c_str());
	if (!image || image->GetLastStatus() != Ok) {
		std::wcerr << L"이미지 로드 실패 : " << imagePath << std::endl;
		delete image;
		return;
	}

	// 이미지 크기 가져오기
	int width = image->GetWidth();
	int height = image->GetHeight();

	// 프린터 해상도 가져오기
	int printerWidth = GetDeviceCaps(hdc, HORZRES);
	int printerHeight = GetDeviceCaps(hdc, VERTRES);

	// 비율 계산
	float scaleX = static_cast<float>(printerWidth) / width;
	float scaleY = static_cast<float>(printerHeight) / height;
	float scale = min(scaleX, scaleY);

	// 새 크기 계산
	int newWidth = static_cast<int>(width * scale);
	int newHeight = static_cast<int>(height * scale);

	// 이미지 렌더링
	Graphics graphics(hdc);
	graphics.DrawImage(image, 0, 0, newWidth, newHeight);

	delete image;
}

void PrintImages(const std::vector<std::wstring>& imagePaths) {
	// GDI+ 초기화
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	// 기본 프린터 가져오기
	DWORD size = 0;
	GetDefaultPrinter(nullptr, &size);
	std::vector<wchar_t> printerName(size);
	if (!GetDefaultPrinter(printerName.data(), &size)) {
		std::cerr << "기본프린터를 가져오지 못했습니다, 기본프린터를 설정해주세요." << std::endl;
		GdiplusShutdown(gdiplusToken);
		return;
	}

	// 프린터 DC 가져오기
	HDC hdc = CreateDC(nullptr, printerName.data(), nullptr, nullptr);
	if (!hdc) {
		std::cerr << "프린터 DC를 생성하지 못했습니다." << std::endl;
		GdiplusShutdown(gdiplusToken);
		return;
	}

	DOCINFO di;
	ZeroMemory(&di, sizeof(di));
	di.cbSize = sizeof(di);
	di.lpszDocName = L"Image Print";

	// 문서 시작
	if (StartDoc(hdc, &di) <= 0) {
		std::cerr << "프린터를 시작하지 못했습니다." << std::endl;
		DeleteDC(hdc);
		GdiplusShutdown(gdiplusToken);
		return;
	}

	for (const auto& imagePath : imagePaths) {
		// 페이지 시작
		if (StartPage(hdc) <= 0) {
			std::cerr << "페이지를 시작하지 못했습니다" << std::endl;
			continue;
		}

		// 이미지 인쇄
		PrintImage(hdc, imagePath);

		// 페이지 끝
		EndPage(hdc);
	}

	// 문서 끝
	EndDoc(hdc);
	DeleteDC(hdc);

	// GDI+ 종료
	GdiplusShutdown(gdiplusToken);
}

int main() {
	SetConsoleTitle(L"디오시스템");
	SetColor(SKYBLUE);
	cout << "______________________________________________" << endl;
	cout << "                 디오시스템                    " << endl;
	SetColor(GREEN);
	cout << "                          Code By. SIYUL PARK" << endl;
	SetColor(SKYBLUE);
	cout << "______________________________________________" << endl;
	cout << endl;
	SetColor(RED);
	cout << "[알림] 잉크막힘을 방지하기위한 정기적 인쇄입니다." << endl;
	cout << "[알림] 확인을 눌러 인쇄해주세요 !!" << endl;

	std::vector<std::wstring> imagePaths = {
		L"print1",
		L"print2",
		L"print3",
		L"print4",
		L"print5",
		// 더 많은 이미지 경로 추가
	};

	PrintImages(imagePaths);

	return 0;
}