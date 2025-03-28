/*=============================================================================
*       breakout.h
*       timwking1
*       28-Mar 2025
=============================================================================*/

#include <stdbool.h>    //bool
#include <windows.h>    //Win32 API

/*=============================================================================
*   Preprocessor
=============================================================================*/
#define WIN32_LEAN_AND_MEAN

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define IDM_RESET 101
#define IDM_EXIT 102
#define IDM_ABOUT 103

#define WM_LOSS 201
#define WM_WIN 202

#define BLOCK_WIDTH 79
#define BLOCK_HEIGHT 25
#define BLOCK_ROWS 12

#define BLOCKS_PER_ROW (WINDOW_WIDTH/(BLOCK_WIDTH+1))
#define TOTAL_BLOCKS (BLOCKS_PER_ROW*BLOCK_ROWS)

#define TIMER_ID 1
#define PADDLE_SPEED 5

#define BALL_SPEED 2

/*=============================================================================
*   Type Definitions
=============================================================================*/
typedef struct
{
    int x;
    int y;
} Vector2;

typedef struct
{
    float x;
    float y;
} f_Vector2;

typedef struct
{
    Vector2 position;
    Vector2 size;
} Transform;

typedef struct
{
    bool initialized;
    Transform transform;
    RECT rect;
    bool active;
} Block;

typedef struct
{
    Transform transform;
    RECT rect;
} Paddle;

typedef struct
{
    Transform transform;
    f_Vector2 velocity;
    RECT rect;
} Ball;

typedef enum Origin
{
    ORIGIN_ZERO,        //The top left corner
    ORIGIN_CENTER,      //The center of the object
    ORIGIN_MAX          //The bottom right corner
} Origin;

/*=============================================================================
*   Declarations
=============================================================================*/
static Vector2 paddlePosition;
static Vector2 paddleSize;
static Transform paddleTransform;
static RECT paddleRect;
static Paddle paddle;

static Vector2 ballPosition;
static Vector2 ballSize;
static Transform ballTransform;
static f_Vector2 ballVelocity;
static RECT ballRect;
static Ball ball;

Block blocks[TOTAL_BLOCKS];

static bool isGameActive = FALSE;

static bool isMovingLeft = FALSE;
static bool isMovingRight = FALSE;

/*=============================================================================
*   Constants
=============================================================================*/
const Vector2 VECTOR2_ZERO = { 0, 0 };
const f_Vector2 FLOAT_VECTOR2_ZERO = { 0.0f, 0.0f };

const COLORREF COLOR_RED   =  0x006456FF;
const COLORREF COLOR_BLUE  =  0x00FAC98D;
const COLORREF COLOR_GREEN =  0x008DFCBB;
const COLORREF COLOR_GRAY  =  0x00242424;

/*=============================================================================
*   Procedures
=============================================================================*/
LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
RECT GetRectBounds(Vector2 blockPos, Vector2 blockSize, Origin o);
Vector2 GetBlockPosition(int row, int column, int spacing);
void InsertBlocks();
void MoveBall();
void ApplyBallVelocity(bool negate);
bool isColliding(RECT RectA, RECT RectB);
void InitializeUI(HWND);