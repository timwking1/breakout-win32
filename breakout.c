/*=============================================================================
*       breakout.c
*       timwking1
*       28-Mar 2025
=============================================================================*/

#include "breakout.h"

/*=============================================================================
*   WinMain 
*       The entry point of a win32 application
*       ***gcc REQUIRES the -mwindows flag to recognize this as the entry point!***
=============================================================================*/
int FAR PASCAL WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevious, LPSTR lpCmdLine, int nCmdShow)
{
    HWND hWnd;
    WNDCLASSEX wc;
    wc.cbSize = sizeof(WNDCLASSEX);                    //class struct size in bytes
    wc.style = CS_HREDRAW | CS_VREDRAW;                //class style (CS_)
    wc.lpfnWndProc = WindowProc;                       //long pointer to window procedure function
    wc.cbClsExtra = 0;                                 //extra bytes to allocate for the class
    wc.cbWndExtra = 0;                                 //extra bytes to allocate for the window instance
    wc.hInstance = hInstance;                          //instance handle
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);        //icon (IDI_)
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);          //curosr (IDC_)
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW +1);      //background brush handle
    wc.lpszMenuName = NULL;                            //resource name of class menu
    wc.lpszClassName = "Breakout";                     //class name string
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);      //small version of the icon (IDI_)
    
    /*
    *   The Window class only needs to be registered once.
    *   If there is a previous instance handle we do not register the class again.
    */
    if (!hPrevious)
    {
        //Register the window class we just initialized (wc)
        if(!RegisterClassEx(&wc))
        {
            //This should never happen, but we inform the user and return gracefully in case it does.
            MessageBox(NULL, "Window Registration Failed", "Error", MB_ICONERROR | MB_OK);
            return 0;
        }
    }

    //Initialize the menu bar
    HMENU hMenu = CreateMenu();

    //Initialize the File submenu
    HMENU hGameMenu = CreatePopupMenu();
    AppendMenu(hGameMenu, MF_STRING, IDM_RESET, "&Reset Game");
    AppendMenu(hGameMenu, MF_STRING, IDM_EXIT, "E&xit");

    //Initialize the Help submenu
    HMENU hHelpMenu = CreatePopupMenu();
    AppendMenu(hHelpMenu, MF_STRING, IDM_ABOUT, "&About");

    //Append the File submenu and about button to the menu bar
    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hGameMenu, "&Game");
    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hHelpMenu, "&Help");

    //Initialize the main window
    hWnd = CreateWindowEx
    (
        WS_EX_CLIENTEDGE,                              //Extended window style (WS_EX_)
        "Breakout",                                    //Class name
        "breakout",                                    //Title bar text
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX, //Window style (WS_)
        CW_USEDEFAULT, CW_USEDEFAULT,                  //Default X and Y screen position of window
        WINDOW_WIDTH, WINDOW_HEIGHT,                   //Width and Height of the window
        NULL,                                          //Parent window (there isn't one, this is the main window)
        hMenu,                                         //Menu handle
        hInstance,                                     //Instance Handle
        NULL                                           //lParam (not used here)
    );

    if(hWnd == NULL)
    {
        //Again, this should never happen since we just created hWnd, but just in case, inform the user and return gracefully.
        MessageBox(NULL, "Window creation failed.", "Error", MB_ICONERROR | MB_OK);
        return 0;
    }

    /*
    *   Now that the main window handle is set up, we can start initializing the individual ui-element "windows" in our main window
    *   For organization sake, we do this in the InitializeUI function.
    */
    InitializeUI(hWnd);

    //Finally, now that the window is fully initialized, we can show it and begin ticking the message loop.
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    MSG msg;
    while(GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{    
    PAINTSTRUCT ps;
    switch(msg)
    {
        case WM_CREATE:
        {
            break;
        }
        case WM_COMMAND:
        {
            switch(LOWORD(wParam))
            {
                //Reset the game
                case IDM_RESET:
                {
                    InitializeUI(hwnd);
                    break;
                }
                case IDM_EXIT:
                {
                    DestroyWindow(hwnd);
                    break;
                }

                //Show an about dialog
                case IDM_ABOUT:
                {
                    MessageBox(hwnd, "Breakout for Win32\ntimwking1\n2025", "breakout", MB_OK | MB_ICONINFORMATION);
                    break;
                }
            }
            break;
        }
        //When a Loss message is received pause the game and show a messagebox
        case WM_LOSS:
        {
            isGameActive = FALSE;
            MessageBox(hwnd, "You lose!", "Oof...", MB_OK | MB_ICONERROR);
            break;
        }

        //When a Win message is received pause the game and show a messagebox
        case WM_WIN:
        {
            isGameActive = FALSE;
            MessageBox(hwnd, "You Won!!!", "Yay!", MB_OK | MB_ICONEXCLAMATION);
            break;
        }

        //Paddle input
        case WM_KEYDOWN:
        {
            switch(wParam)
            {
                case VK_LEFT:
                {
                    isMovingLeft = TRUE;
                    break;
                }
                case VK_RIGHT:
                {
                    isMovingRight = TRUE;
                    break;
                }
            }
            //Begin game tick on input
            SetTimer(hwnd, TIMER_ID, 10, NULL);
            break;
        }
        case WM_KEYUP:
        {
            switch(wParam)
            {
                case VK_LEFT:
                {
                    isMovingLeft = FALSE;
                    break;
                }
                case VK_RIGHT:
                {
                    isMovingRight = FALSE;
                    break;
                }
            }
            break;
        }

        case WM_TIMER:
        {
            switch(wParam)
            {
                case TIMER_ID:
                {
                    if(isGameActive)
                    {
                        //Move the ball
                        MoveBall(hwnd);

                        //Move the paddle
                        if(isMovingLeft)
                        {
                            if(paddle.transform.position.x > 0)
                            {
                                paddle.transform.position.x-=PADDLE_SPEED;
                            }
                        }
                        if(isMovingRight)
                        {
                            if(paddle.transform.position.x < WINDOW_WIDTH - BLOCK_WIDTH)
                            {
                                paddle.transform.position.x+=PADDLE_SPEED;
                            }
                        }

                        //Update the screen
                        InvalidateRect(hwnd, NULL, FALSE);
                    }
                    break;
                }
            }
            break;
        }

        case WM_PAINT:
        {
            /*
            *   RECT reference:
            *   https://learn.microsoft.com/en-us/windows/win32/api/windef/ns-windef-rect
            *   Petzold, Page 583
            */

            HDC hdc = BeginPaint(hwnd, &ps);

            //Set up brush handles
            HBRUSH hRedBrush = CreateSolidBrush(COLOR_RED);
            HBRUSH hBlueBrush = CreateSolidBrush(COLOR_BLUE);
            HBRUSH hGreenBrush = CreateSolidBrush(COLOR_GREEN);
            HBRUSH hBackgroundBrush = CreateSolidBrush(COLOR_GRAY);

            //Paint Background
            FillRect(hdc, &ps.rcPaint, hBackgroundBrush);

            //Paint Paddle
            paddle.rect = GetRectBounds(paddle.transform.position, paddle.transform.size, ORIGIN_ZERO);
            RECT rec = paddle.rect;
            FillRect(hdc, &rec, hRedBrush);

            //Paint Ball
            ball.rect = GetRectBounds(ball.transform.position, ball.transform.size, ORIGIN_CENTER);
            rec = ball.rect;
            FillRect(hdc, &rec, hGreenBrush);

            //Paint Blocks
            for(int i=0; i<TOTAL_BLOCKS; i++)
            {
                if(blocks[i].active == TRUE && blocks[i].initialized == TRUE)
                {
                    FillRect(hdc, &blocks[i].rect, hBlueBrush);
                }
            }

            //Free brush handles
            DeleteObject(hRedBrush);
            DeleteObject(hBlueBrush);
            DeleteObject(hGreenBrush);
            DeleteObject(hBackgroundBrush);

            EndPaint(hwnd, &ps);
            break;
        }

        case WM_DESTROY:
        {
            PostQuitMessage(0);
            break;
        }
        default:
        {
            return DefWindowProc(hwnd, msg, wParam, lParam);
        }
    }
    return 0;
}

/*=============================================================================
*   GetRectBounds [RECT]
*           Returns a RECT of fixed size relative to an input position
*       Parameters:
*           Vector2 rectPos - x and y position with respect to the origin
*           Vector2 rectSize - width and height of the rect
*           Origin o - Origin offset type
*
=============================================================================*/
RECT GetRectBounds(Vector2 rectPos, Vector2 rectSize, Origin o)
{
    switch(o)
    {
        case ORIGIN_ZERO:
        {
            RECT bounds = 
            {
                rectPos.x,                              //Top-left corner X
                rectPos.y,                              //Top-left corner Y
                rectPos.x+rectSize.x,                   //Bottom-right corner X
                rectPos.y+rectSize.y                    //Bottom-right corner Y
            };
            return bounds;
        }
        case ORIGIN_CENTER:
        {
            RECT bounds = 
            {
                rectPos.x-(rectSize.x/2),               //Top-left corner X
                rectPos.y-(rectSize.y/2),               //Top-left corner Y
                rectPos.x+(rectSize.x/2),               //Bottom-right corner X
                rectPos.y+(rectSize.y/2)                //Bottom-right corner Y
            };
            return bounds;
        }
        case ORIGIN_MAX:
        {
            RECT bounds = 
            {
                rectPos.x-rectSize.x,                   //Top-left corner X
                rectPos.y+rectSize.y,                   //Top-left corner Y
                rectPos.x,                              //Bottom-right corner X
                rectPos.y                               //Bottom-right corner Y
            };
            return bounds;
        }
    }
}

/*=============================================================================
*   GetBlockPosition [Vector2]
*           Used to place columns and rows of blocks 
*           with a set amount of space between them
*       Parameters:
*           int row
*           int column
*           int spacing
*
=============================================================================*/
Vector2 GetBlockPosition(int row, int column, int spacing)
{
    Vector2 NewPos = {column * (BLOCK_WIDTH+spacing), row * (BLOCK_HEIGHT+spacing)};
    return NewPos;
}

/*=============================================================================
*   InsertBlocks [void]
*           Initialize the blocks and place them into rows and columns
*
=============================================================================*/
void InsertBlocks()
{
    //Reset state of blocks
    size_t n = sizeof(blocks) / sizeof(Block);
    for(int i=0; i<n; i++)
    {
        blocks[i].initialized = FALSE;
    }

    //The index of where we are in the blocks array
    int arrayIndex = 0;
    for(int k=0; k<BLOCK_ROWS; k++)
    {
        for(int i=0; i<BLOCKS_PER_ROW; i++)
        {
            Vector2 blockPosition = GetBlockPosition(k, i, 1);
            Vector2 blockSize = {BLOCK_WIDTH, BLOCK_HEIGHT};

            blocks[arrayIndex].initialized = TRUE;
            blocks[arrayIndex].transform.position = blockPosition;
            blocks[arrayIndex].rect = GetRectBounds(blockPosition, blockSize, ORIGIN_ZERO);
            blocks[arrayIndex].active = TRUE;
            arrayIndex++;
        }
    }
    return;
}

/*=============================================================================
*   ApplyBallVelocity [void]
*           Moves the ball based on it's velocity
*       Parameters:
*           bool negate - reverse the velocity
*
=============================================================================*/
void ApplyBallVelocity(bool negate)
{
    if(!negate)
    {
        ball.transform.position.x+=(ball.velocity.x*BALL_SPEED);
        ball.transform.position.y+=(ball.velocity.y*BALL_SPEED);
    }
    else
    {
        ball.transform.position.x-=(ball.velocity.x*BALL_SPEED);
        ball.transform.position.y-=(ball.velocity.y*BALL_SPEED);
    }
}

/*=============================================================================
*   isColliding [bool]
*           Overlap check of two given rects
*       Parameters:
*           RECT RectA
*           RECT RectB
*
=============================================================================*/
bool isColliding(RECT RectA, RECT RectB)
{
    if(RectA.left < RectB.right && RectA.right > RectB.left && RectA.top < RectB.bottom && RectA.bottom > RectB.top)
    {
        return true;
    }
    return false;
}

/*=============================================================================
*   MoveBall [void]
*           Ball physics procedure
*       Parameters:
*           HWND hwnd - window handle to send win and loss messages
*                       when the ball hits the top/bottom of screen
*
=============================================================================*/
void MoveBall(HWND hwnd)
{
    if(isColliding(ball.rect, paddle.rect))
    {
        if(ball.rect.right >= paddle.rect.left && ball.rect.left <= paddle.rect.right)
        {
            ball.velocity.y = -ball.velocity.y;
        }
        else
        {
            ball.velocity.x = -ball.velocity.x;
        }
    }

    for (int i = 0; i < TOTAL_BLOCKS; i++)
    {
        if (blocks[i].active && isColliding(ball.rect, blocks[i].rect))
        {
            blocks[i].active = FALSE;
            if(ball.rect.bottom >= blocks[i].rect.top && ball.rect.top <= blocks[i].rect.bottom)
            {
                ball.velocity.y = -ball.velocity.y;
            }
            else
            {
                ball.velocity.x = -ball.velocity.x;
            }
            break;
        }
    }

    //Collision with window edges
    if(ball.transform.position.x <= 0)
    {
        ball.velocity.x = (ball.velocity.x)*(-1);
        ball.transform.position.x+=2;
    }
    if(ball.transform.position.x >= WINDOW_WIDTH)
    {
        ball.velocity.x = -ball.velocity.x;
        ball.transform.position.x--;
    }
    //Lose condition when the ball is beyond the bottom of the window
    if(ball.transform.position.y > WINDOW_HEIGHT)
    {
        SendMessage(hwnd, WM_LOSS, 0, 0);
        ball.velocity = FLOAT_VECTOR2_ZERO;
    }
    //Win condition when the ball hits the top of the window
    if(ball.transform.position.y <= 0)
    {
        SendMessage(hwnd, WM_WIN, 0, 0);
        ball.velocity = FLOAT_VECTOR2_ZERO;
    }
    ApplyBallVelocity(FALSE);

    return;
}

/*=============================================================================
*   InitializeUI [void]
*           Initialization of game objects is done here.
*           This gets called at the start of the game and also whenever the
*           game is reset.
*       Parameters:
*           HWND hwnd - window handle for ui elements
*
=============================================================================*/
void InitializeUI(HWND hwnd)
{
    //Set up paddle
    paddlePosition = (Vector2){WINDOW_WIDTH/2, WINDOW_HEIGHT - (BLOCK_HEIGHT*4)};
    paddleSize = (Vector2){BLOCK_WIDTH, BLOCK_HEIGHT};
    paddleTransform = (Transform){paddlePosition, paddleSize};
    paddleRect = GetRectBounds(paddlePosition, paddleSize, ORIGIN_ZERO);

    paddle = (Paddle){paddleTransform, paddleRect};
    

    //Set up ball
    ballPosition = (Vector2){WINDOW_WIDTH/2, WINDOW_HEIGHT - (BLOCK_HEIGHT*6)};
    ballSize = (Vector2){15, 15};
    ballTransform = (Transform){ballPosition, ballSize};
    ballVelocity = (f_Vector2){1, BALL_SPEED};
    ballRect = GetRectBounds(ball.transform.position, ball.transform.size, ORIGIN_CENTER);
    
    ball = (Ball){ballTransform, ballVelocity, ballRect};

    //Set up blocks
    InsertBlocks();

    //Start the game
    isGameActive = TRUE;
    return;
}