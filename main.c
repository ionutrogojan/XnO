#include <stdio.h>
#include <raylib.h>

#define ROW_LEN 3
#define GRID_LEN (ROW_LEN * ROW_LEN)
#define CELL_GAP 4
#define CELL_SIZE 128
#define CANVAS_SIZE (ROW_LEN * CELL_SIZE + (ROW_LEN - 1) * CELL_GAP)

#define CALC_X(i) (i % ROW_LEN)
#define CALC_Y(i) (i / ROW_LEN)
#define CALC_COORD(i) (i * CELL_SIZE + i * CELL_GAP)

#define TEXT_SIZE 64

typedef enum {
	N = 0,
	O = 1,
	X = 2,
} cellType;

const char cellName[] = {
	'N',
	'O',
	'X',
};

const Color cellColour[] = {
	{  12,  12,  12, 255 }, // LIGHT
	{   8, 128, 240, 255 }, // BLUE
	{ 248,  60,  60, 255 }, // RED
};

Texture2D cellSprites[2];

Vector2 origin(Font fnt, const char* text, float size, float canvas);
Vector2 cellOrigin(Font fnt, const char c, float size, Vector2 pos, float cellSize);
bool checkWin(cellType grid[], cellType player);
void debugCells(cellType grid[]);

int main(void) {

	Image image;

	cellType grid[GRID_LEN] = { N };

	// grid[4] = X;
	// grid[5] = O;
	// debugCells(grid);

	int x = 0, y = 0;
	Vector2 mousePos = { 0, 0 };

	int cellActive = -1;
	int gameDone = false;
	int currentPlayer = X;
	char currentSymbol = cellName[currentPlayer];

	int emptyCells = GRID_LEN;

	char winnerText[8];
	Vector2 spritePos = { 0, 0 };
	Vector2 textOrigin = { 0, 0 };

	Rectangle cellShape = { 0, 0, 0, 0 };

	SetTraceLogLevel(LOG_ERROR);
	InitWindow(CANVAS_SIZE, CANVAS_SIZE, "XnO");
	SetWindowState(FLAG_MSAA_4X_HINT);

	// Load Player Sprites
	image = LoadImage("sprites/o.png");
	cellSprites[0] = LoadTextureFromImage(image);

	image = LoadImage("sprites/x.png");
	cellSprites[1] = LoadTextureFromImage(image);

	UnloadImage(image);

	// Font loading + setup
	Font fnt = LoadFont("font/FiraMono-Regular.ttf");
	SetTextureFilter(fnt.texture, TEXTURE_FILTER_POINT);

	while (!WindowShouldClose()) {
		// gameDone = true;

// Game Win Check 
		if (checkWin(grid, X)) {
			gameDone = true;
			snprintf(winnerText, 8, "X Wins!");
			currentPlayer = X;
		} else if (checkWin(grid, O)) {
			gameDone = true;
			snprintf(winnerText, 8, "O Wins!");
			currentPlayer = O;
		} else if (emptyCells == 0) {
			// Check if we're have a draw
			gameDone = true;
			snprintf(winnerText, 8, "Draw ?!");
			// Default to player X when draw ??
			currentPlayer = X; 
		}

// Pointer Select Action
		if (IsMouseButtonPressed(0) && !gameDone) {
			// Get the click coords
			mousePos = GetMousePosition();
			// TraceLog(LOG_ERROR, "CLICK %d, %d", (int)mousePos.x, (int)mousePos.y);

			// cellIndex = (Vector2){ mousePos.x / (CELL_SIZE + GRID_GAP), mousePos.y / (CELL_SIZE + GRID_GAP) };
			// TraceLog(LOG_ERROR, "INDEX %d, %d", (int)cellIndex.x, (int)cellIndex.y);
			
			// Get the cell index to update
			cellActive = ((int)(mousePos.y / (CELL_SIZE + CELL_GAP)) * ROW_LEN) + (int)(mousePos.x / (CELL_SIZE + CELL_GAP));
			// TraceLog(LOG_ERROR, "ACTIVE %d", cellActive);

			// update the cell with the active player if empty
			if (grid[cellActive] == N) {
				grid[cellActive] = currentPlayer;
				// switch players
				if (currentPlayer == X) {
					currentPlayer = O;
				} else {
					currentPlayer = X;
				}
				// Remove 1 from the count of empty cells
				emptyCells--;
			}
		}

		// Reset the game when Space is pressed
		if (IsKeyPressed(KEY_SPACE)) {
			for (int i = 0; i < GRID_LEN; i++) {
				grid[i] = N;
			}
			// reset first player to X
			// currentPlayer = X; we do this in checkWin() to set the winner to the player start
			gameDone = false;
			emptyCells = GRID_LEN;
		}

		BeginDrawing(); {
			ClearBackground(BLACK);
			for (int i = 0; i < GRID_LEN; i++) {
				x = CALC_X(i);
				y = CALC_Y(i);
				cellShape = (Rectangle){ CALC_COORD(x), CALC_COORD(y), CELL_SIZE, CELL_SIZE };
				// DrawRectangle(CALC_COORD(x), CALC_COORD(y), CELL_SIZE, CELL_SIZE, cellColour[grid[i]]);
				DrawRectangleRounded(cellShape, 0.16, 4, cellColour[grid[i]]);
				// Draw the player symbol on the cell
				if (grid[i] != N) {
					// this has to be assigned to a variable otherwise DrawText() draws "OX" instead of 'O', but oddly 'X' works just fine ??
					currentSymbol = cellName[grid[i]];
					spritePos =  cellOrigin(fnt, currentSymbol, TEXT_SIZE, (Vector2){ CALC_COORD(x), CALC_COORD(y) }, CELL_SIZE);
					DrawTexture(cellSprites[grid[i]-1], spritePos.x, spritePos.y, WHITE);
					// DrawTextEx(fnt, &currentSymbol, textOrigin, TEXT_SIZE, 0, BLACK);
				}
				if (gameDone) {
					// Find the canvas origin with the text measures
					textOrigin = origin(fnt, winnerText, TEXT_SIZE, CANVAS_SIZE);
					// Draw the winner announcement
					DrawTextEx(fnt, winnerText, textOrigin, TEXT_SIZE, 0, GREEN);
				}
			}
		} EndDrawing();
	}

	UnloadFont(fnt);

	UnloadTexture(cellSprites[0]);
	UnloadTexture(cellSprites[1]);

	CloseWindow();

	return 0;
}

Vector2 origin(Font fnt, const char* text, float size, float canvas) {
	Vector2 s = MeasureTextEx(fnt, text, size, 0);
	return (Vector2){ canvas / 2 - s.x / 2, canvas / 2 - s.y / 2 };
}

Vector2 cellOrigin(Font fnt, const char c, float size, Vector2 pos, float cellSize) {
	return (Vector2){ pos.x + (cellSize / 2 - size / 2), pos.y + (cellSize / 2 - size / 2) };
}

bool checkWin(cellType grid[], cellType player) {
	if ((grid[0] == player && grid[3] == player && grid[6] == player) ||	// Vertical 0
		(grid[1] == player && grid[4] == player && grid[7] == player) ||	// Vertical 1
		(grid[2] == player && grid[5] == player && grid[8] == player) ||	// Vertical 2
		(grid[0] == player && grid[1] == player && grid[2] == player) ||	// Horizontal 0
		(grid[3] == player && grid[4] == player && grid[5] == player) ||	// Horizontal 1
		(grid[6] == player && grid[7] == player && grid[8] == player) ||	// Horizontal 2
		(grid[0] == player && grid[4] == player && grid[8] == player) ||	// Diagonal 0
		(grid[2] == player && grid[4] == player && grid[6] == player)) {	// Diagonal 1
		return true;
	}
	return false;
}

void debugCells(cellType grid[]) {
	int x = 0, y = 0;
	for (int i = 0; i < GRID_LEN; i++) {
		x = CALC_X(i);
		y = CALC_Y(i);
		printf("[ Y=%d | X=%d (%c) ]", y, x, cellName[grid[i]]);
		if (x == ROW_LEN - 1)
			printf("\n");
	}
}

// TODO: player's turn indicator
// TODO: player score