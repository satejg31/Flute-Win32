#include <windows.h>
#include <stdio.h>
#include <math.h>

#define SUCCESS 1
#define MAX_LINE_LEN 10

char line[MAX_LINE_LEN];

int RENDER_SMOOTHING_FLAG=0;	
int G_POINTS_PLOTED=0;

struct tagMyPoint 
{
	int xPos;
	int yPos;
};

struct tagVector 
{
	struct tagMyPoint **list;
	size_t size;	
};

struct tagVector *createVector(void);
int pushPoint(struct tagVector *pVector, struct tagMyPoint *pPoint);
void DestroyVector(struct tagVector *pVector);
struct tagMyPoint *createPoint(int xco_ord, int yco_ord);
double getDistance(int x1, int y1, int x2, int y2);

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
struct tagVector *pointVector = NULL;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow) 
{

	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("Flute");

	pointVector = createVector();
	FILE *fp = fopen("Flute.txt", "r");

	int xCoordiante = 0;
	int yCoordinate = 0;
	char *token = NULL;
	int isCurrentTokenX = 0;
	struct tagMyPoint *pPoint;

	while(fgets(line, MAX_LINE_LEN, fp) != NULL)
	{
		token = strtok(line, ",");
		while(token != NULL)
		{
			if(isCurrentTokenX == 0)
			{
				xCoordiante = atoi(token);
				isCurrentTokenX = 1;
			}

			else if(isCurrentTokenX == 1)
			{
				yCoordinate = atoi(token);
				isCurrentTokenX = 0;
			}

			token = strtok(NULL, ",");
		}

		pPoint = createPoint(xCoordiante, yCoordinate);
		pushPoint(pointVector, pPoint);
	}

	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;

	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hInstance = hInstance;
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;


	RegisterClassEx(&wndclass);

	hwnd = CreateWindow(szAppName,
			TEXT("KRISHNA FLUTE"),
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			NULL,
			NULL,
			hInstance,
			NULL);

	ShowWindow(hwnd, SW_MAXIMIZE);
	UpdateWindow(hwnd);

	while(GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	DestroyVector(pointVector);
	pointVector = NULL;

	return ((int)msg.wParam);
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{

	HDC hdc;
	HPEN hLinePen;
	HPEN hOldPen;
	PAINTSTRUCT ps;
	int pointIndex;
	double point_distance;


	switch(iMsg)
	{

		case WM_CREATE:
			PlaySound(TEXT("audio.wav"), NULL, SND_FILENAME | SND_ASYNC );
			SetTimer(hwnd, 1, 150, NULL);
			break;

		case WM_PAINT:
			hdc = BeginPaint(hwnd, &ps);

			if(RENDER_SMOOTHING_FLAG == 1)
			{
				hLinePen = CreatePen(PS_SOLID, 4, RGB(250, 180, 0));
				hOldPen = (HPEN)SelectObject(hdc, hLinePen);

				for(pointIndex = 1;
						pointIndex < G_POINTS_PLOTED;
						pointIndex++)
				{

					point_distance = getDistance(pointVector->list[pointIndex - 1]->xPos,
										pointVector->list[pointIndex - 1]->yPos,
										pointVector->list[pointIndex]->xPos,
										pointVector->list[pointIndex]->yPos);

					if(point_distance <= 20) 
					{
						MoveToEx(hdc, pointVector->list[pointIndex - 1]->xPos+40, pointVector->list[pointIndex-1]->yPos-100, NULL);
						LineTo(hdc, pointVector->list[pointIndex]->xPos+40, pointVector->list[pointIndex]->yPos-100);
								
					}
				}

				DeleteObject(hLinePen);
				SelectObject(hdc, hOldPen);

				EndPaint(hwnd, &ps);
				hdc = NULL;

				RENDER_SMOOTHING_FLAG = 0;
			}
			break;
		
		case WM_TIMER:
			if(G_POINTS_PLOTED != pointVector->size && RENDER_SMOOTHING_FLAG == 0)
			{
				RENDER_SMOOTHING_FLAG = 1;
				G_POINTS_PLOTED++;
				InvalidateRect(hwnd, NULL, FALSE);
			}

			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;

	}

	return (DefWindowProc(hwnd, iMsg, wParam, lParam));
}


struct tagVector *createVector(void)
{

	struct tagVector *pVector = NULL;
	pVector = (struct tagVector*)malloc(sizeof(struct tagVector));

	pVector->list = NULL;
	pVector->size = 0;

	return (pVector);
}

int pushPoint(struct tagVector *pVector, struct tagMyPoint *pPoint)
{
	pVector->size = pVector->size + 1;
	pVector->list = (struct tagMyPoint**)realloc(pVector->list,(pVector->size)*sizeof(struct tagMyPoint*));
	pVector->list[pVector->size - 1] = pPoint;
	return (SUCCESS);
}

void DestroyVector(struct tagVector *pVector)
{
	free(pVector->list);
	free(pVector);
}

struct tagMyPoint *createPoint(int xco_ord, int yco_ord)
{
	struct tagMyPoint *pPoint = NULL;
	pPoint = (struct tagMyPoint*)malloc(sizeof(struct tagMyPoint));
	pPoint->xPos = xco_ord;
	pPoint->yPos = yco_ord;

	return (pPoint);
}

double getDistance(int x1, int y1, int x2, int y2)
{
	double distance;

	int delta_x = x2 - x1;
	int delta_y = y2 - y1;

	distance = pow(pow(delta_x, 2) + pow(delta_y, 2), (0.5f));

	return (distance);
}