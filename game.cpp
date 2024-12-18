#define UNICODE
#define _UNICODE


#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <Windows.h> // For Windows-specific APIs

using namespace std;

int nScreenWidth = 120;         // Console Screen Size X (columns)
int nScreenHeight = 40;         // Console Screen Size Y (rows)
int nMapWidth = 16;             // World Dimensions
int nMapHeight = 16;

float fPlayerX = 14.7f;         // Player Start Position
float fPlayerY = 5.09f;
float fPlayerA = 0.0f;          // Player Start Rotation
float fFOV = 3.14159f / 4.0f;   // Field of View
float fDepth = 16.0f;           // Maximum rendering distance
float fSpeed = 5.0f;            // Walking Speed

int main()
{
    // Create Screen Buffer
   // wchar_t* screen = new wchar_t[nScreenWidth * nScreenHeight];
   wchar_t* screen = new wchar_t[nScreenWidth * nScreenHeight];

    HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(hConsole);
    DWORD dwBytesWritten = 0;

    // Create Map of world space # = wall block, . = space
    wstring map;
    map += L"#########.......";
	map += L"#...............";
	map += L"#.......########";
	map += L"#..............#";
	map += L"#......##......#";
	map += L"#......##......#";
	map += L"#..............#";
	map += L"###............#";
	map += L"##.............#";
	map += L"#......####..###";
	map += L"#......#.......#";
	map += L"#......#.......#";
	map += L"#..............#";
	map += L"#......#########";
	map += L"#..............#";
	map += L"################";

    auto tp1 = chrono::system_clock::now();
    auto tp2 = chrono::system_clock::now();

    while (1)
    {
        // Time differential per frame
        tp2 = chrono::system_clock::now();
        chrono::duration<float> elapsedTime = tp2 - tp1;
        tp1 = tp2;
        float fElapsedTime = elapsedTime.count();

        // Handle CCW Rotation
        if (GetAsyncKeyState((unsigned short)'A') & 0x8000)
            fPlayerA -= (fSpeed * 0.75f) * fElapsedTime;

        // Handle CW Rotation
        if (GetAsyncKeyState((unsigned short)'D') & 0x8000)
            fPlayerA += (fSpeed * 0.75f) * fElapsedTime;

        // Handle Forwards movement & collision
        if (GetAsyncKeyState((unsigned short)'W') & 0x8000)
        {
            fPlayerX += sinf(fPlayerA) * fSpeed * fElapsedTime;
            fPlayerY += cosf(fPlayerA) * fSpeed * fElapsedTime;
            if (map.c_str()[(int)fPlayerY * nMapWidth + (int)fPlayerX] == '#')
            {
                fPlayerX -= sinf(fPlayerA) * fSpeed * fElapsedTime;
                fPlayerY -= cosf(fPlayerA) * fSpeed * fElapsedTime;
            }
        }

        // Handle backwards movement & collision
        if (GetAsyncKeyState((unsigned short)'S') & 0x8000)
        {
            fPlayerX -= sinf(fPlayerA) * fSpeed * fElapsedTime;
            fPlayerY -= cosf(fPlayerA) * fSpeed * fElapsedTime;
            if (map.c_str()[(int)fPlayerY * nMapWidth + (int)fPlayerX] == '#')
            {
                fPlayerX += sinf(fPlayerA) * fSpeed * fElapsedTime;
                fPlayerY += cosf(fPlayerA) * fSpeed * fElapsedTime;
            }
        }

        for (int x = 0; x < nScreenWidth; x++)
        {
            float fRayAngle = (fPlayerA - fFOV / 2.0f) + ((float)x / (float)nScreenWidth) * fFOV;

            float fDistanceToWall = 0.0f;
            float fStepSize = 0.1f;
            bool bHitWall = false;
            bool bBoundary = false;

            float fEyeX = sinf(fRayAngle);
            float fEyeY = cosf(fRayAngle);

            while (!bHitWall && fDistanceToWall < fDepth)
            {
                fDistanceToWall += fStepSize;
                int nTestX = (int)(fPlayerX + fEyeX * fDistanceToWall);
                int nTestY = (int)(fPlayerY + fEyeY * fDistanceToWall);

                if (nTestX < 0 || nTestX >= nMapWidth || nTestY < 0 || nTestY >= nMapHeight)
                {
                    bHitWall = true;
                    fDistanceToWall = fDepth;
                }
                else
                {
                    if (map.c_str()[nTestY * nMapWidth + nTestX] == '#')
                    {
                        bHitWall = true;

                        vector<pair<float, float>> p;
                        for (int tx = 0; tx < 2; tx++)
                            for (int ty = 0; ty < 2; ty++)
                            {
                                float vy = (float)nTestY + ty - fPlayerY;
                                float vx = (float)nTestX + tx - fPlayerX;
                                float d = sqrt(vx * vx + vy * vy);
                                float dot = (fEyeX * vx / d) + (fEyeY * vy / d);
                                p.push_back(make_pair(d, dot));
                            }

                        sort(p.begin(), p.end(), [](const pair<float, float>& left, const pair<float, float>& right) { return left.first < right.first; });

                        float fBound = 0.01;
                        if (acos(p[0].second) < fBound) bBoundary = true;
                        if (acos(p[1].second) < fBound) bBoundary = true;
                    }
                }
            }

            int nCeiling = (float)(nScreenHeight / 2.0) - nScreenHeight / ((float)fDistanceToWall);
            int nFloor = nScreenHeight - nCeiling;

            wchar_t nShade = L' ';
            if (fDistanceToWall <= fDepth / 4.0f)        nShade = 0x2588;
            else if (fDistanceToWall < fDepth / 3.0f)   nShade = 0x2593;
            else if (fDistanceToWall < fDepth / 2.0f)   nShade = 0x2592;
            else if (fDistanceToWall < fDepth)         nShade = 0x2591;

            if (bBoundary) nShade = L' ';

            for (int y = 0; y < nScreenHeight; y++)
            {
                if (y <= nCeiling)
                    screen[y * nScreenWidth + x] = L' ';
                else if (y > nCeiling && y <= nFloor)
                    screen[y * nScreenWidth + x] = nShade;
                else
                {
                    float b = 1.0f - (((float)y - nScreenHeight / 2.0f) / ((float)nScreenHeight / 2.0f));
                    if (b < 0.25)      nShade = L'#';
                    else if (b < 0.5)  nShade = L'x';
                    else if (b < 0.75) nShade = L'.';
                    else if (b < 0.9)  nShade = L'-';
                    else               nShade = L' ';
                    screen[y * nScreenWidth + x] = nShade;
                }
            }
        }

        swprintf_s(screen, nScreenWidth, L"X=%3.2f, Y=%3.2f, A=%3.2f FPS=%3.2f ", fPlayerX, fPlayerY, fPlayerA, 1.0f / fElapsedTime);

        for (int nx = 0; nx < nMapWidth; nx++)
            for (int ny = 0; ny < nMapHeight; ny++)
                screen[(ny + 1) * nScreenWidth + nx] = map[ny * nMapWidth + nx];
        screen[((int)fPlayerY + 1) * nScreenWidth + (int)fPlayerX] = L'P';

        screen[nScreenWidth * nScreenHeight - 1] = L'\0';
       WriteConsoleOutputCharacterW(hConsole, screen, nScreenWidth * nScreenHeight, { 0, 0 }, &dwBytesWritten);

    }

    return 0;
}
