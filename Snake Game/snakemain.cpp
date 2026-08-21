#include <raylib.h>
#include <iostream>
#include <deque>
#include <raymath.h>

Color light = { 255,218,185,255 };
Color dark = { 255,140,0,255 };

//Grid
float cellSize = 30;
float cellCount = 20;
int offset = 75;

double lastUpdateTime = 0;
float intervalTime = 0.2;

//diese Funktion ist dafür da, das Movement der Schlange auf ein Intervall einzuschränken, damit sie nicht wegspeeded
bool eventTriggered(double interval) {
	double currentTime = GetTime();
	if (currentTime - lastUpdateTime >= interval) {
		lastUpdateTime = currentTime;
		return true;
	}
	return false;
}

bool ElementInDeque(Vector2 element, std::deque<Vector2> deque)
{
	for (unsigned int i = 0; i < deque.size(); i++) {
		if (Vector2Equals(deque[i], element)) {
			return true;
		}
	}return false;
}


//class für Futter
class Food {
public:
	//Vektor, um Position darzustellen
	Vector2 position = { 5,6 };

	Texture2D texture;


	Food(std::deque<Vector2>snakeBody) {
		position = GenerateRandomPos(snakeBody);
	}


	void Draw() {
		DrawRectangleRounded(Rectangle{ offset + position.x * cellSize, offset + position.y * cellSize, cellSize, cellSize }, 0.8, 0, dark);
	}

	//Funktion um random Zellen zu definieren
	Vector2 GenerateRandomCell() {
		float x = GetRandomValue(0, cellCount - 1);
		float y = GetRandomValue(0, cellCount - 1);
		return Vector2{ x,y };
	}

	//Random Position geben
	Vector2 GenerateRandomPos(std::deque<Vector2>snakeBody) {

		Vector2 position = GenerateRandomCell();
		//hier wird geprüft, ob die random Position in einem deque (also der Schlange) ist -> dann wird neu generiert
		while (ElementInDeque(position, snakeBody)) {
			position = GenerateRandomCell();
		}
		//position wird erst nach erfolgreichem Durchlaufen der while schleife returned
		return position;

	}

};

class Snake {
public:
	//Deque für den Körper der Schlange vom Type Vector2
	std::deque<Vector2> body = { Vector2{6,9}, Vector2{5,9}, Vector2{4,9} };
	Vector2 direction = { 1,0 };
	bool addSegment = false;

	void Draw() {
		//Schlange mit for-loop zeichnen
		for (unsigned int i = 0; i < body.size(); i++) {
			float x = body[i].x;
			float y = body[i].y;
			Rectangle segment = Rectangle{ offset + x * cellSize, offset + y * cellSize, (float)cellSize, (float)cellSize };
			DrawRectangleRounded(segment, 0.3, 0, dark);
		}
	}

	void Move() {

		body.push_front(Vector2Add(body[0], direction));

		if (addSegment == true) {

			addSegment = false;
		}
		else {

			body.pop_back();

		}
	}

	void Reset() {
		body = { Vector2{6,9}, Vector2{5,9}, Vector2{4,9} };
		direction = { 1,0 };

	}
};

class Game {
public:
	Snake snake = Snake();
	Food food = Food(snake.body);
	bool running = true;
	int score = 0;

	void Draw() {
		food.Draw();
		snake.Draw();
	}

	void Move() {
		if (running) {
			snake.Move();
			CheckCollisionFood();
			CheckCollisionEdges();
			CheckCollisionTail();
		}
	}

	void CheckCollisionFood() {

		if (Vector2Equals(snake.body[0], food.position)) {
			food.position = food.GenerateRandomPos(snake.body);
			snake.addSegment = true;
			score++;
			intervalTime = intervalTime - 0.003;
		}
	}

	void CheckCollisionEdges() {
		if (snake.body[0].x == cellCount || snake.body[0].x == -1 || snake.body[0].y == cellCount || snake.body[0].y == -1) {
			GameOver();
		}
	}

	void CheckCollisionTail() {

		std::deque<Vector2> tail = snake.body;
		tail.pop_front();
		if (ElementInDeque(snake.body[0], tail)) {
			GameOver();
		}
	}

	void GameOver() {

		snake.Reset();
		score = 0;
		intervalTime = 0.3;
		food.position = food.GenerateRandomPos(snake.body);
		running = false;

	}




};


int main() {

	InitWindow(2 * offset + cellSize * cellCount, 2 * offset + cellSize * cellCount, "Snake Game");
	SetTargetFPS(60);

	//Klassen initialisieren
	Game game = Game();

	while (!WindowShouldClose()) {

		BeginDrawing();

		if (!game.running) {
			DrawText("Game Over", 10 * cellCount, 10 * cellCount, 20, dark);
			DrawText("Press SPACE to restart", 10 * cellCount, 10 * cellCount + 30, 15, dark);

			if (IsKeyPressed(KEY_SPACE)) {
				game.running = true;
			}
		}

		//Schlange bewegen 
		if (eventTriggered(intervalTime)) {
			game.Move();
		}
		//Richtung der Schlange ändern
		if (IsKeyPressed(KEY_UP) && game.snake.direction.y != 1) {
			game.snake.direction = { 0, -1 };
			game.running = true;
		}
		if (IsKeyPressed(KEY_DOWN) && game.snake.direction.y != -1) {
			game.snake.direction = { 0, 1 };
			game.running = true;
		}
		if (IsKeyPressed(KEY_LEFT) && game.snake.direction.x != 1) {
			game.snake.direction = { -1,0 };
			game.running = true;
		}
		if (IsKeyPressed(KEY_RIGHT) && game.snake.direction.x != -1) {
			game.snake.direction = { 1,0 };
			game.running = true;
		}

		ClearBackground(light);
		DrawRectangleLinesEx(Rectangle{ (float)offset - 5,(float)offset - 5, cellSize * cellCount + 10, cellSize * cellCount + 10 }, 5, dark);
		DrawText("Snake", offset - 5, 20, 40, dark);
		DrawText(TextFormat("%i", game.score), cellSize * cellCount - offset, 20, 40, dark);
		game.Draw();

		EndDrawing();

	}


	CloseWindow();
	return 0;
}