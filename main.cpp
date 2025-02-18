#include <iostream>
#include <raylib.h>
#include <vector>
#include <raymath.h>
using namespace std;
// Implement
#define llui long long unsigned int
// frame scale
#define space 50 //->
#define cellSize 20
#define cellCount 30
int sPos = space + 5; // WorkSpace : W( (3->26)*cellSize , (3->26)*cellSize )
int scr_width = cellSize * cellCount;
int scr_height = cellSize * cellCount;
// Color
Color Green = {173, 204, 96, 255};
Color DarkGreen = {43, 51, 24, 255};
// time
static double lastUpdateTime = 0;
bool eventTriggerd(double interval)
{
    double currentTime = GetTime();
    if (currentTime - lastUpdateTime >= interval)
    {
        lastUpdateTime = currentTime;
        return true;
    }
    return false;
}

bool inContainer(Vector2 ele , vector<Vector2> container){  
    for ( llui i =  0 ; i < container.size() ; i++){
        if ( ele == container[i] ) return true ;
    }
    return false  ; 
}

class Snake
{
public:
    vector<Vector2> body = {Vector2{10, 15}, Vector2{11, 15}, Vector2{12, 15}};
    Vector2 currentDirection = {1, 0};
    Vector2 head = body.back();
    vector<Vector2> checkpoint = body;

    void Draw()
    {
        for (llui i = 0; i < body.size(); i++)
        {
            float x = body[i].x * cellSize;
            float y = body[i].y * cellSize;
            Rectangle segment = Rectangle{x, y, cellSize, cellSize};
            DrawRectangleRounded(segment, 0.5, 6, DarkGreen);
        }
    }
    void Move()
    {
        Vector2 newHead = Vector2Add(currentDirection, head);
        body.push_back(newHead);
        body.erase(body.begin());
        head = body.back();
    }

    void Reset()
    {
        body = checkpoint;
        currentDirection = Vector2{1, 0};
    }
};

class Food
{
public:
    Vector2 position;
    Texture2D texture;
    Vector2 checkpoint;
    Food(vector<Vector2> snakeBody)
    {
        Image image = LoadImage("Graphics/goldapp.jpg");
        texture = LoadTextureFromImage(image);
        UnloadImage(image);
        position = GenerateRandomPos(snakeBody);
        checkpoint = position;
    }

    ~Food()
    {
        UnloadTexture(texture);
    }
    void Draw()
    {
        DrawTexture(texture, position.x * cellSize, position.y * cellSize, WHITE);
    }

    Vector2 GenerateRandomCell()
    {
        float x = GetRandomValue(4, 25);
        float y = GetRandomValue(4, 25);
        return {x, y};
    }

    Vector2 GenerateRandomPos(vector<Vector2> snakeBody)
    {
        Vector2 position = GenerateRandomCell();
        if ( inContainer(position , snakeBody) ) position = GenerateRandomCell(); 
        return position;
    }

};

class GameManager
{
public:
    Snake Orochi;
    Food Organe;

    bool running = true;
    GameManager() : Organe(Orochi.body) {}
    void Draw()
    {
        Organe.Draw();
        Orochi.Draw();
       
    }

    void Update()
    {
        if (running)
        {   
            Orochi.Move();
            eat();
            CheckCollisionWithBody();
            CheckCollisionwithEdge();
            // checkCollisionHead_Body();
        }
    }

    void eat()
    {
        if (Vector2Equals(Organe.position, Orochi.head))
        {   
            Vector2 newMeat = Vector2{ Orochi.body[0].x - Orochi.body[1].x , Orochi.body[0].y - Orochi.body[1].y };
            Organe.position = Organe.GenerateRandomPos(Orochi.body);
            Orochi.body.insert( Orochi.body.begin() , Vector2Add( Orochi.body[0] , newMeat) ) ;
        }
    }

    void CheckCollisionwithEdge()
    {
        if (Orochi.head.x <= 2 || Orochi.head.x >= 28)
        {
            GameOver();
        }
        if (Orochi.head.y <= 2 || Orochi.head.y >= 28)
        {
            GameOver();
        }
    }

    void CheckCollisionWithBody(){
        vector<Vector2> endlessSnake = Orochi.body ; 
        endlessSnake.pop_back();
        if(  inContainer(Orochi.head, endlessSnake) ) GameOver();
    }
    void GameOver()
    {
        Orochi.Reset();
        Organe.position = Organe.checkpoint;
        running = false ; 
    }

};

int main()
{
    InitWindow(scr_width, scr_height, "RETRO SNAKE");
    SetTargetFPS(60);

    GameManager Game;

    while (!WindowShouldClose())
    {
        // Nếu game đang chạy -> cập nhật logic
        if (Game.running)
        {
            // Di chuyển rắn định kỳ
            if (eventTriggerd(0.2))
            {
                Game.Update();
            }

            // Xử lý phím điều khiển hướng (khi đang chạy)
            if (IsKeyDown(KEY_DOWN) && Game.Orochi.currentDirection.y != -1)
            {
                Game.Orochi.currentDirection = {0, 1};
            }
            else if (IsKeyDown(KEY_UP) && Game.Orochi.currentDirection.y != 1)
            {
                Game.Orochi.currentDirection = {0, -1};
            }
            else if (IsKeyDown(KEY_LEFT) && Game.Orochi.currentDirection.x != 1)
            {
                Game.Orochi.currentDirection = {-1, 0};
            }
            else if (IsKeyDown(KEY_RIGHT) && Game.Orochi.currentDirection.x != -1)
            {
                Game.Orochi.currentDirection = {1, 0};
            }
        }
        else
        {
            // Nếu game đã dừng, chờ người chơi nhấn phím để khởi động lại
            if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_UP)
                || IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_RIGHT))
            {
                // Bắt đầu chạy lại
                Game.running = true;
            }
        }

        // Vẽ game
        BeginDrawing();
        ClearBackground(Green);

        // Khung
        DrawRectangle(space, space, scr_width - 2 * space, scr_height - 2 * space, DarkGreen);
        DrawRectangle(sPos, sPos, scr_width - 2 * sPos, scr_height - 2 * sPos, Green);

        // Vẽ mọi thứ
        Game.Draw();
        DrawText(TextFormat("%i", Game.Orochi.body.size() - 3), space, scr_height - 40, 40, DarkGreen);
        DrawText("RETRO SNAKE", 10, 0, 30, DarkGreen);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}