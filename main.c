#include <stdio.h>
#include <raylib.h>

#define ROW_LEN 3
#define GRID_LEN (ROW_LEN * ROW_LEN)
#define GRID_GAP 4
#define CELL_SIZE 128
#define CANVAS_SIZE (ROW_LEN * CELL_SIZE + (ROW_LEN - 1) * GRID_GAP)

#define CALC_X(i) (i % ROW_LEN)
#define CALC_Y(i) (i / ROW_LEN)
#define CALC_COORD(i) (i * CELL_SIZE + i * GRID_GAP)

#define TEXT_SIZE 24

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
	WHITE,
	BLUE,
	RED,
};

Vector2 origin(Font fnt, const char* text, float size, float canvas);
Vector2 cellOrigin(Font fnt, const char c, float size, Vector2 pos, float cellSize);
void debugCells(cellType grid[]);

int main(void) {

	cellType grid[GRID_LEN] = { N };

	// grid[4] = X;
	// grid[5] = O;
	// debugCells(grid);

	int x = 0, y = 0;
	Vector2 mousePos = { 0, 0 };

	int cellActive = -1;
	int gameDone = 1;
	int currentPlayer = X;
	char currentSymbol = cellName[currentPlayer];

	char* winnerText = "X Wins!";
	Vector2 textOrigin = { 0, 0 };

	SetTraceLogLevel(LOG_ERROR);
	InitWindow(CANVAS_SIZE, CANVAS_SIZE, "XnO");

	// Font loading + setup
	Font fnt = LoadFont("font/FiraMono-Regular.ttf");
	SetTextureFilter(fnt.texture, TEXTURE_FILTER_BILINEAR);

	while (!WindowShouldClose()) {
		gameDone = 1;

		// Check if we're done playing
		for (int i = 0; i < GRID_LEN; i++) {
			if (grid[i] == N) {
				gameDone--;
				break;
			}
		}

		if (IsMouseButtonPressed(0)) {
			// Get the click coords
			mousePos = GetMousePosition();
			// TraceLog(LOG_ERROR, "CLICK %d, %d", (int)mousePos.x, (int)mousePos.y);

			// cellIndex = (Vector2){ mousePos.x / (CELL_SIZE + GRID_GAP), mousePos.y / (CELL_SIZE + GRID_GAP) };
			// TraceLog(LOG_ERROR, "INDEX %d, %d", (int)cellIndex.x, (int)cellIndex.y);
			
			// Get the cell index to update
			cellActive = ((int)(mousePos.y / (CELL_SIZE + GRID_GAP)) * ROW_LEN) + (int)(mousePos.x / (CELL_SIZE + GRID_GAP));
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
			}
		}

		// Reset the game when Space is pressed
		if (IsKeyPressed(KEY_SPACE)) {
			for (int i = 0; i < GRID_LEN; i++) {
				grid[i] = N;
			}
			// reset first player to X
			currentPlayer = X;
			// TODO: reset to the last winner
		}

		BeginDrawing(); {
			ClearBackground(BLACK);
			for (int i = 0; i < GRID_LEN; i++) {
				x = CALC_X(i);
				y = CALC_Y(i);
				DrawRectangle(CALC_COORD(x), CALC_COORD(y), CELL_SIZE, CELL_SIZE, cellColour[grid[i]]);
				// Draw the player symbol on the cell
				if (grid[i] != N) {
					// this has to be assigned to a variable otherwise DrawText() draws "OX" instead of 'O', but oddly 'X' works just fine ??
					currentSymbol = cellName[grid[i]];
					textOrigin =  cellOrigin(fnt, currentSymbol, TEXT_SIZE, (Vector2){ CALC_COORD(x), CALC_COORD(y) }, CELL_SIZE);
					DrawTextEx(fnt, &currentSymbol, textOrigin, TEXT_SIZE, 0, BLACK);
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

	CloseWindow();

	return 0;
}

Vector2 origin(Font fnt, const char* text, float size, float canvas) {
	Vector2 s = MeasureTextEx(fnt, text, size, 0);
	return (Vector2){ canvas / 2 - s.x / 2, canvas / 2 - s.y / 2 };
}

Vector2 cellOrigin(Font fnt, const char c, float size, Vector2 pos, float cellSize) {
	Vector2 s = MeasureTextEx(fnt, &c, size, 0);
	return (Vector2){ pos.x + (cellSize / 2 - s.x / 2), pos.y + (cellSize / 2 - s.y / 2) };
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

// TODO: check if we have a winner
// TODO: player's turn indicator
// TODO: player score