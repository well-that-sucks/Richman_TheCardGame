#include <vector>
#include <iomanip>
#include <string>
#include <windowsx.h>
#include "framework.h"
#include "Lab6.h"

#define MAX_LOADSTRING 100

HBITMAP* bitmapArray;
HBITMAP cardBack;
int** deckP;
int** deckB;
std::string* translation;
std::vector<std::string> history;
int* openedCardsB;
int* openedCardsP;
double movingCardX;
double movingCardY;
bool hasStarted;
bool isDistrubutionInProcess;
bool showDeckOfCards;
int cardsP;
int cardsB;
int deckTopCard;
int topCardSuit;
int topCardsAmount;
int chosenCard;

HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];


void clearField(HWND);
int declarePlayerVictory(HWND);
int declareBotVictory(HWND);
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow) {
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_LAB6, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance (hInstance, nCmdShow)) {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LAB6));

    MSG msg;

    while (GetMessage(&msg, nullptr, 0, 0)) {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance) {
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LAB6));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_LAB6);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
   hInst = hInstance;

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd) {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

void fillElements(int* a, int n, int element) {
    for (int i = 0; i < n; i++)
        a[i] = element;
}

void copy2DArray(int** a, int** b, int n, int m) {
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            a[i][j] = b[i][j];
}

void copyArray(int* a, int* b, int n) {
    for (int i = 0; i < n; i++)
        a[i] = b[i];
}

int countOnesInRow(int* row, int n) {
    int oc = 0;
    for (int i = 0; i < n; i++)
        if (row[i] == 1)
            oc++;
    return oc;
}

bool isEmpty(int** deck, int n, int m) {
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            if (deck[i][j] == 1)
                return false;
    return true;
}

int estimatingFunction(int** deck) {
    double value = 0;
    int tcTotal = 0;
    for (int i = 0; i < 13; i++) {
        int tc = countOnesInRow(deck[i], 4);
        tcTotal += tc;
        value += (pow(2, i - 5) * tc);
    }
    return (int)(value * 100.0 + (26 - (double)tcTotal) * 50.0);
}

bool isPLayerUnableToBeat(int** deckP) {
    for (int i = deckTopCard + 1; i < 13; i++) {
        int tc = 0;
        for (int j = 0; j < 4; j++)
            if (deckP[i][j] == 1)
                tc++;
        if (tc >= topCardsAmount)
            return false;
    }
    return true;
}

int findSolution(int** deckB, int** deckP, int lastCard, int cards, int alpha, int beta, int step, int** bestTurnDeckB) {
    if (step > 9)
        return estimatingFunction(deckP) - estimatingFunction(deckB);
    if (isEmpty(deckB, 13, 4))
        return -100000;
    if (isEmpty(deckP, 13, 4))
        return 100000;
    if (step % 2 == 1) {
        int v = 9999999;
        int minV = 9999999;
        int k = 0;
        for (int i = lastCard + 1; i < 13; i++) {
            int tc = countOnesInRow(deckB[i], 4);
            if (tc >= cards) {
                k++;
                int* tArray = new int[4];
                int m = 0;
                copyArray(tArray, deckB[i], 4);
                if (lastCard != -1) {
                    for (int j = 0; j < 4 && m < cards; j++)
                        if (deckB[i][j] == 1) {
                            m++;
                            deckB[i][j] = 0;
                        }
                    v = min(v, findSolution(deckB, deckP, i, cards, alpha, beta, step + 1, bestTurnDeckB));
                } else {
                    fillElements(deckB[i], 4, 0);
                    v = min(v, findSolution(deckB, deckP, i, tc, alpha, beta, step + 1, bestTurnDeckB));
                }
                if (step == 1 && v < minV) {
                    minV = v;
                    copy2DArray(bestTurnDeckB, deckB, 13, 4);
                }
                copyArray(deckB[i], tArray, 4);
                delete[] tArray;
                if (v <= alpha)
                    return v;
                beta = min(v, beta);
                if (lastCard == -1)
                    break;
            }
        }
        if (k == 0) {
            v = min(v, findSolution(deckB, deckP, -1, 1, alpha, beta, step + 1, bestTurnDeckB));
            if (v <= alpha)
                return v;
            beta = min(v, beta);
        }
        return v;
    } else {
        int v = -9999999;
        int k = 0;
        for (int i = lastCard + 1; i < 13; i++) {
            int tc = countOnesInRow(deckP[i], 4);
            if (tc >= cards) {
                k++;
                int* tArray = new int[4];
                int m = 0;
                copyArray(tArray, deckP[i], 4);
                if (lastCard != -1) {
                    for (int j = 0; j < 4 && m < cards; j++)
                        if (deckP[i][j] == 1) {
                            m++;
                            deckP[i][j] = 0;
                        }
                    v = max(v, findSolution(deckB, deckP, i, cards, alpha, beta, step + 1, bestTurnDeckB));
                }
                else {
                    fillElements(deckP[i], 4, 0);
                    v = max(v, findSolution(deckB, deckP, i, tc, alpha, beta, step + 1, bestTurnDeckB));
                }
                copyArray(deckP[i], tArray, 4);
                delete[] tArray;
                if (v >= beta)
                    return v;
                alpha = max(v, alpha);
                if (lastCard == -1)
                    break;
            }
        }
        if (k == 0) {
            v = max(v, findSolution(deckB, deckP, -1, 1, alpha, beta, step + 1, bestTurnDeckB));
            if (v >= beta)
                return v;
            alpha = max(v, alpha);
        }
        return v;
    }
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_CREATE: {
            srand((unsigned int)time(NULL));
            deckP = new int*[13];
            deckB = new int*[13];
            openedCardsB = new int[26];
            openedCardsP = new int[26];
            translation = new std::string[13];
            bitmapArray = new HBITMAP[4];
            bitmapArray[0] = (HBITMAP)LoadImage(NULL, L"Bitmap1.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
            bitmapArray[1] = (HBITMAP)LoadImage(NULL, L"Bitmap.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
            bitmapArray[2] = (HBITMAP)LoadImage(NULL, L"Bitmap2.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
            bitmapArray[3] = (HBITMAP)LoadImage(NULL, L"Bitmap3.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
            cardBack = (HBITMAP)LoadImage(NULL, L"Bitmap4.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
            translation[0] = '3';
            translation[1] = '4';
            translation[2] = '5';
            translation[3] = '6';
            translation[4] = '7';
            translation[5] = '8';
            translation[6] = '9';
            translation[7] = "10";
            translation[8] = 'J';
            translation[9] = 'Q';
            translation[10] = 'K';
            translation[11] = 'A';
            translation[12] = '2';
            for (int i = 0; i < 13; i++) {
                deckP[i] = new int[4];
                deckB[i] = new int[4];
            }
            clearField(hWnd);
        }
        break;
        case WM_COMMAND: {
            int wmId = LOWORD(wParam);
            switch (wmId) {
                case IDM_EXIT:
                    DestroyWindow(hWnd);
                break;
                default:
                    return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
        case WM_MOUSEMOVE: {
            int mouseX = GET_X_LPARAM(lParam);
            int mouseY = GET_Y_LPARAM(lParam);
            int t = (mouseX - 100 - 1) / 40 + 1;
            if (mouseY > 600 && mouseY < 680 && mouseX >= 100 && t <= cardsP) {
                int tChosenCard = -1;
                int tc = 0;
                bool isFound = false;
                for (int i = 0; i < 13 && !isFound; i++)
                    for (int j = 0; j < 4 && !isFound; j++)
                        if (deckP[i][j] == 1) {
                            tc++;
                            if (tc >= t) {
                                isFound = true;
                                tChosenCard = i;
                            }
                        }
                if (isFound && tChosenCard != chosenCard) {
                    chosenCard = tChosenCard;
                    InvalidateRect(hWnd, NULL, TRUE);
                    UpdateWindow(hWnd);
                }
            } else {
                if (chosenCard != -1) {
                    chosenCard = -1;
                    InvalidateRect(hWnd, NULL, TRUE);
                    UpdateWindow(hWnd);
                }
            }
        }
        break;
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            HGDIOBJ original = SelectObject(hdc, GetStockObject(DC_PEN));
            int k = 0;
            HDC hMemDC = CreateCompatibleDC(hdc);
            SetBkMode(hdc, TRANSPARENT);
            SetBkColor(hdc, RGB(17, 107, 13));
            std::string tstr = "";
            if (deckTopCard != -1)
                tstr = translation[deckTopCard];
            if (topCardSuit == 0 || topCardSuit == 1) 
                SetTextColor(hdc, RGB(255, 0, 0));
            if (topCardSuit == 2 || topCardSuit == 3)
                SetTextColor(hdc, RGB(0, 0, 0));
            SelectObject(hMemDC, bitmapArray[topCardSuit]);
            for (int i = topCardsAmount - 1; i >= 0; i--)
                Rectangle(hdc, 100 + 5 * i, 310 + 5 * i, 150 + 5 * i, 390 + 5 * i);
            if (deckTopCard != -1) {
                BitBlt(hdc, 123, 316, 16, 16, hMemDC, 0, 0, SRCCOPY);
                TextOutA(hdc, 105, 315, tstr.c_str(), std::strlen(tstr.c_str()));
            }
            SelectObject(hMemDC, cardBack);
            if (isDistrubutionInProcess)
                BitBlt(hdc, (int)(round(movingCardX)), (int)(round(movingCardY)), 50, 80, hMemDC, 0, 0, SRCCOPY);
            if (showDeckOfCards)
                BitBlt(hdc, 600, 310, 50, 80, hMemDC, 0, 0, SRCCOPY);
            for (int i = 0; i < 13; i++) {
                for (int j = 0; j < 4; j++) {
                    if (deckB[i][j] == 1 && openedCardsB[k] == 1) {
                        SelectObject(hMemDC, cardBack);
                        BitBlt(hdc, 100 + 40 * k, 20, 50, 80, hMemDC, 0, 0, SRCCOPY);
                        std::string str = translation[i];
                        k++;
                    }
                }
            }
            k = 0;
            for (int i = 0; i < 13; i++) {
                for (int j = 0; j < 4; j++)
                    if (deckP[i][j] == 1 && openedCardsP[k] == 1) {
                        int raise = 0;
                        if (i == chosenCard)
                            raise = 20;
                        Rectangle(hdc, 100 + 40 * k, 600 - raise, 150 + 40 * k, 680 - raise);
                        std::string str = translation[i];
                        if (j == 0 || j == 1)
                            SetTextColor(hdc, RGB(255, 0, 0));
                        if (j == 2 || j == 3)
                            SetTextColor(hdc, RGB(0, 0, 0));
                        SelectObject(hMemDC, bitmapArray[j]);
                        BitBlt(hdc, 123 + 40 * k, 605 - raise, 16, 16, hMemDC, 0, 0, SRCCOPY);
                        TextOutA(hdc, 105 + 40 * k, 605 - raise, str.c_str(), std::strlen(str.c_str()));
                        k++;
                    }
            }
            SetTextColor(hdc, RGB(0, 0, 0));
            for (int i = 0; i < (int)history.size(); i++)
                TextOutA(hdc, 1000, 320 + 30 * i, history[i].c_str(), std::strlen(history[i].c_str()));
            if (isPLayerUnableToBeat(deckP) && !isDistrubutionInProcess) {
                Rectangle(hdc, 1200, 600, 1300, 650);
                SetTextColor(hdc, RGB(0, 0, 0));
                TextOutA(hdc, 1220, 615, "Skip turn", std::strlen("Skip turn"));
            }
            DeleteDC(hMemDC);
            SelectObject(hdc, original);
            EndPaint(hWnd, &ps);
        }
        break;
        case WM_LBUTTONUP: {
            if (!hasStarted) {
                history.clear();
                history.push_back("The game started");
                showDeckOfCards = true;
                isDistrubutionInProcess = true;
                for (int i = 0; i < 13; i ++) {
                    int destX = 100 + i * 80;
                    int destY = 20;
                    movingCardX = 600;
                    movingCardY = 310;
                    double stepX = (destX - movingCardX) / 25.0;
                    double stepY = (destY - movingCardY) / 25.0;
                    for (int j = 0; j < 25; j++) {
                        movingCardX += stepX;
                        movingCardY += stepY;
                        InvalidateRect(hWnd, NULL, TRUE);
                        UpdateWindow(hWnd);
                        Sleep(25);
                    }
                    openedCardsB[i * 2] = 1;
                    openedCardsB[i * 2 + 1] = 1;
                    InvalidateRect(hWnd, NULL, TRUE);
                    UpdateWindow(hWnd);
                }
                for (int i = 0; i < 13; i++) {
                    int destX = 100 + i * 80;
                    int destY = 600;
                    movingCardX = 600;
                    movingCardY = 310;
                    double stepX = (destX - movingCardX) / 25.0;
                    double stepY = (destY - movingCardY) / 25.0;
                    for (int j = 0; j < 25; j++) {
                        movingCardX += stepX;
                        movingCardY += stepY;
                        InvalidateRect(hWnd, NULL, TRUE);
                        UpdateWindow(hWnd);
                        Sleep(25);
                    }
                    openedCardsP[i * 2] = 1;
                    openedCardsP[i * 2 + 1] = 1;
                    InvalidateRect(hWnd, NULL, TRUE);
                    UpdateWindow(hWnd);
                }
                showDeckOfCards = false;
                isDistrubutionInProcess = false;
                int** bestTurn = new int*[13];
                for (int i = 0; i < 13; i++) {
                    bestTurn[i] = new int[4];
                    fillElements(bestTurn[i], 4, 0);
                }
                hasStarted = true;
                int sol = findSolution(deckB, deckP, -1, 1, -9999999, 9999999, 1, bestTurn);
                int k = 0;
                for (int i = 0; i < 13; i++) {
                    for (int j = 0; j < 4; j++) {
                        if (bestTurn[i][j] != deckB[i][j]) {
                            deckTopCard = i;
                            topCardsAmount++;
                            topCardSuit = j;
                        }
                    }
                }
                for (int i = 0; i <= deckTopCard; i++) {
                    for (int j = 0; j < 4; j++) {
                        if (deckB[i][j] == 1)
                            k++;
                    }
                }
                copy2DArray(deckB, bestTurn, 13, 4);
                for (int i = 0; i < 13; i++)
                    delete[] bestTurn[i];
                delete[] bestTurn;
                history.push_back("Bot put " + std::to_string(topCardsAmount) + " card(s) with the value of " + std::to_string(deckTopCard + 1));
                isDistrubutionInProcess = true;
                movingCardX = 100 + k * 40;
                movingCardY = 20;
                double stepX = (100 - movingCardX) / 25.0;
                double stepY = (310 - movingCardY) / 25.0;
                for (int j = 0; j < 25; j++) {
                    movingCardX += stepX;
                    movingCardY += stepY;
                    InvalidateRect(hWnd, NULL, TRUE);
                    UpdateWindow(hWnd);
                    Sleep(25);
                }
                isDistrubutionInProcess = false;
                InvalidateRect(hWnd, NULL, TRUE);
                UpdateWindow(hWnd);
            } else {
                bool turnSucceeded = false;
                if (deckTopCard != -1) {
                    if (chosenCard > deckTopCard) {
                        int t = 0;
                        for (int j = 0; j < 4; j++)
                            if (deckP[chosenCard][j] == 1)
                                t++;
                        if (t >= topCardsAmount) {
                            int k = 0;
                            for (int i = 0; i <= deckTopCard; i++)
                                for (int j = 0; j < 4; j++)
                                    if (deckP[i][j] == 1)
                                        k++;
                            deckTopCard = chosenCard;
                            t = topCardsAmount;
                            for (int j = 0; j < 4 && t > 0; j++) {
                                if (deckP[chosenCard][j] == 1) {
                                    deckP[chosenCard][j] = 0;
                                    t--;
                                }
                            }
                            history.push_back("The player put " + std::to_string(topCardsAmount) + " card(s) with the value of " + std::to_string(deckTopCard + 1));
                            if (history.size() > 3)
                                history.erase(history.begin());
                            isDistrubutionInProcess = true;
                            movingCardX = 100 + k * 40;
                            movingCardY = 600;
                            double stepX = (100 - movingCardX) / 25.0;
                            double stepY = (310 - movingCardY) / 25.0;
                            for (int j = 0; j < 25; j++) {
                                movingCardX += stepX;
                                movingCardY += stepY;
                                InvalidateRect(hWnd, NULL, TRUE);
                                UpdateWindow(hWnd);
                                Sleep(25);
                            }
                            isDistrubutionInProcess = false;
                            InvalidateRect(hWnd, NULL, TRUE);
                            UpdateWindow(hWnd);
                            turnSucceeded = true;
                            if (isEmpty(deckP, 13, 4)) {
                                declarePlayerVictory(hWnd);
                                turnSucceeded = false;
                                deckTopCard = chosenCard;
                                InvalidateRect(hWnd, NULL, TRUE);
                                UpdateWindow(hWnd);
                            }
                        }
                    }
                }
                else {
                    int imin = 0;
                    while (imin < 13 && countOnesInRow(deckP[imin], 4) == 0)
                        imin++;
                    if (chosenCard == imin) {
                        deckTopCard = imin;
                        topCardsAmount = countOnesInRow(deckP[imin], 4);
                        fillElements(deckP[imin], 4, 0);
                        history.push_back("Player put " + std::to_string(topCardsAmount) + " card(s) with the value of " + std::to_string(deckTopCard + 1));
                        if (history.size() > 3)
                            history.erase(history.begin());
                        isDistrubutionInProcess = true;
                        movingCardX = 100;
                        movingCardY = 600;
                        double stepX = (100 - movingCardX) / 25.0;
                        double stepY = (310 - movingCardY) / 25.0;
                        for (int j = 0; j < 25; j++) {
                            movingCardX += stepX;
                            movingCardY += stepY;
                            InvalidateRect(hWnd, NULL, TRUE);
                            UpdateWindow(hWnd);
                            Sleep(25);
                        }
                        isDistrubutionInProcess = false;
                        turnSucceeded = true;
                        if (isEmpty(deckP, 13, 4)) {
                            declarePlayerVictory(hWnd);
                            turnSucceeded = false;
                            deckTopCard = chosenCard;
                            InvalidateRect(hWnd, NULL, TRUE);
                            UpdateWindow(hWnd);
                        }
                    }
                }
                if (!turnSucceeded) {
                    int mouseX = GET_X_LPARAM(lParam);
                    int mouseY = GET_Y_LPARAM(lParam);
                    if (mouseX >= 1200 && mouseX <= 1300 && mouseY >= 600 && mouseY <= 650 && isPLayerUnableToBeat(deckP) && !isEmpty(deckP, 13, 4)) {
                        history.push_back("Player skipped his turn");
                        if (history.size() > 3)
                            history.erase(history.begin());
                        deckTopCard = -1;
                        topCardsAmount = 1;
                        turnSucceeded = true;
                    }
                }
                if (turnSucceeded) {
                    int** bestTurn = new int* [13];
                    for (int i = 0; i < 13; i++) {
                        bestTurn[i] = new int[4];
                        fillElements(bestTurn[i], 4, 0);
                    }
                    int sol = findSolution(deckB, deckP, deckTopCard, topCardsAmount, -9999999, 9999999, 1, bestTurn);
                    topCardsAmount = 0;
                    int t1 = 0;
                    int t2 = 0;
                    for (int i = 0; i < 13; i++) {
                        int tc = 0;
                        for (int j = 0; j < 4; j++) {
                            if (bestTurn[i][j] != 0)
                                t1++;
                            if (deckB[i][j] == 1)
                                tc++;
                        }
                        if (tc > 0)
                            t2++;
                    }
                    if (t1 != 0 || t2 == 1) {
                        for (int i = 0; i < 13; i++) {
                            for (int j = 0; j < 4; j++) {
                                if (bestTurn[i][j] != deckB[i][j]) {
                                    deckTopCard = i;
                                    topCardsAmount++;
                                    topCardSuit = j;
                                }
                            }
                        }
                        int k = 0;
                        for (int i = 0; i <= deckTopCard; i++)
                            for (int j = 0; j < 4; j++)
                                if (deckB[i][j] == 1)
                                    k++;
                        history.push_back("Bot put " + std::to_string(topCardsAmount) + " card(s) with the value of " + std::to_string(deckTopCard + 1));
                        if (history.size() > 3)
                            history.erase(history.begin());
                        isDistrubutionInProcess = true;
                        movingCardX = 100 + k * 40;
                        movingCardY = 20;
                        double stepX = (100 - movingCardX) / 25.0;
                        double stepY = (310 - movingCardY) / 25.0;
                        for (int j = 0; j < 25; j++) {
                            movingCardX += stepX;
                            movingCardY += stepY;
                            InvalidateRect(hWnd, NULL, TRUE);
                            UpdateWindow(hWnd);
                            Sleep(25);
                        }
                        isDistrubutionInProcess = false;
                        copy2DArray(deckB, bestTurn, 13, 4);
                        if (isEmpty(deckB, 13, 4)) {
                            InvalidateRect(hWnd, NULL, TRUE);
                            UpdateWindow(hWnd);
                            declareBotVictory(hWnd);
                        }
                    }
                    else {
                        history.push_back("Bot skipped his turn");
                        if (history.size() > 3)
                            history.erase(history.begin());
                        deckTopCard = -1;
                        topCardsAmount = 0;
                    }
                    for (int i = 0; i < 13; i++)
                        delete[] bestTurn[i];
                    delete[] bestTurn;
                    InvalidateRect(hWnd, NULL, TRUE);
                    UpdateWindow(hWnd);
                }
            }
        }
        break;
        case WM_DESTROY:
            PostQuitMessage(0);
        break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

void clearField(HWND hWnd) {
    for (int i = 0; i < 13; i++) {
        fillElements(deckP[i], 4, 0);
        fillElements(deckB[i], 4, 0);
    }
    std::vector<int> rndAssistanceVector;
    int m = 52;
    for (int i = 0; i < m; i++)
        rndAssistanceVector.push_back(i);
    for (int i = 0; i < 26; i++) {
        int index = rand() % m;
        int y = rndAssistanceVector[index] / 4;
        int x = rndAssistanceVector[index] % 4;
        deckB[y][x] = 1;
        rndAssistanceVector.erase(rndAssistanceVector.begin() + index);
        m--;
    }
    rndAssistanceVector.clear();
    for (int i = 0; i < 13; i++) {
        for (int j = 0; j < 4; j++) {
            if (deckB[i][j] == 0)
                deckP[i][j] = 1;
        }
    }
    hasStarted = false;
    showDeckOfCards = false;
    cardsP = 26;
    cardsB = 26;
    chosenCard = -1;
    deckTopCard = -1;
    topCardsAmount = 0;
    fillElements(openedCardsB, 26, 0);
    fillElements(openedCardsP, 26, 0);
}

int declarePlayerVictory(HWND hWnd) {
    int msgboxID = MessageBox(NULL, L"Player has won!", L"", MB_ICONINFORMATION | MB_OK);
    clearField(hWnd);
    InvalidateRect(hWnd, NULL, TRUE);
    UpdateWindow(hWnd);
    return msgboxID;
}

int declareBotVictory(HWND hWnd) {
    int msgboxID = MessageBox(NULL, L"Bot has won!", L"", MB_ICONINFORMATION | MB_OK);
    clearField(hWnd);
    InvalidateRect(hWnd, NULL, TRUE);
    UpdateWindow(hWnd);
    return msgboxID;
}