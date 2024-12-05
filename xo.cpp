#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>
#include <fstream>
#include <string>

using namespace std;

class ConnectFour
{
private:
  static int ROWS;
  static int COLS;
  static int DEPTH;
  bool computerStarts;
  vector<vector<char>> board;
  char player;
  char computer;

  bool isValidMove(int col) const
  {
    return col >= 0 && col < COLS && board[0][col] == ' ';
  }

  int dropPiece(int col, char piece)
  {
    for (int row = ROWS - 1; row >= 0; row--)
    {
      if (board[row][col] == ' ')
      {
        board[row][col] = piece;
        return row;
      }
    }
    return -1;
  }

  void removePiece(int row, int col)
  {
    board[row][col] = ' ';
  }

  bool checkWin(int row, int col, char piece) const
  {
    const int dx[] = {1, 0, 1, 1}; // направления: →, ↓, ↘, ↗
    const int dy[] = {0, 1, 1, -1};

    for (int dir = 0; dir < 4; dir++)
    {
      int count = 1;

      // Проверяем в одну сторону
      for (int i = 1; i < 4; i++)
      {
        int newRow = row + dy[dir] * i;
        int newCol = col + dx[dir] * i;

        if (newRow < 0 || newRow >= ROWS || newCol < 0 || newCol >= COLS)
          break;

        if (board[newRow][newCol] != piece)
          break;

        count++;
      }

      // Проверяем в противоположную сторону
      for (int i = 1; i < 4; i++)
      {
        int newRow = row - dy[dir] * i;
        int newCol = col - dx[dir] * i;

        if (newRow < 0 || newRow >= ROWS || newCol < 0 || newCol >= COLS)
          break;

        if (board[newRow][newCol] != piece)
          break;

        count++;
      }

      if (count >= 4)
        return true;
    }
    return false;
  }

  bool isBoardFull() const
  {
    for (int col = 0; col < COLS; col++)
    {
      if (board[0][col] == ' ')
        return false;
    }
    return true;
  }

  // Новая функция для проверки немедленного выигрыша
  int checkImmeditateWin(char piece) const
  {
    for (int col = 0; col < COLS; col++)
    {
      if (!isValidMove(col))
        continue;

      // Находим строку, куда упадет фишка
      int row = ROWS - 1;
      while (row >= 0 && board[row][col] != ' ')
        row--;
      if (row < 0)
        continue;

      // Временно размещаем фишку
      vector<vector<char>> tempBoard = board;
      tempBoard[row][col] = piece;

      // Проверяем выигрыш по всем направлениям
      const int dx[] = {1, 0, 1, 1}; // направления: →, ↓, ↘, ↗
      const int dy[] = {0, 1, 1, -1};

      for (int dir = 0; dir < 4; dir++)
      {
        int count = 1;
        // Проверяем в обе стороны
        for (int sign = -1; sign <= 1; sign += 2)
        {
          for (int i = 1; i < 4; i++)
          {
            int newRow = row + sign * dy[dir] * i;
            int newCol = col + sign * dx[dir] * i;

            if (newRow < 0 || newRow >= ROWS || newCol < 0 || newCol >= COLS)
              break;

            if (tempBoard[newRow][newCol] != piece)
              break;

            count++;
          }
        }
        if (count >= 4)
          return col;
      }
    }
    return -1;
  }

  int evaluateWindow(const vector<char> &window) const
  {
    int score = 0;
    int computerCount = count(window.begin(), window.end(), computer);
    int playerCount = count(window.begin(), window.end(), player);
    int emptyCount = count(window.begin(), window.end(), ' ');

    // Увеличиваем веса для более агрессивной игры
    if (computerCount == 4)
      score += 1000000;
    else if (computerCount == 3 && emptyCount == 1)
      score += 1000;
    else if (computerCount == 2 && emptyCount == 2)
      score += 100;

    if (playerCount == 3 && emptyCount == 1)
      score -= 10000; // Блокируем победу противника
    else if (playerCount == 2 && emptyCount == 2)
      score -= 100;

    return score;
  }

  int evaluatePosition() const
  {
    int score = 0;
    vector<char> window(4);

    // Проверяем все возможные окна по 4 клетки
    // Горизонтальные окна
    for (int r = 0; r < ROWS; r++)
    {
      for (int c = 0; c < COLS - 3; c++)
      {
        for (int i = 0; i < 4; i++)
        {
          window[i] = board[r][c + i];
        }
        score += evaluateWindow(window);
      }
    }

    // Вертикальные окна
    for (int c = 0; c < COLS; c++)
    {
      for (int r = 0; r < ROWS - 3; r++)
      {
        for (int i = 0; i < 4; i++)
        {
          window[i] = board[r + i][c];
        }
        score += evaluateWindow(window);
      }
    }

    // Диагональные окна (↘)
    for (int r = 0; r < ROWS - 3; r++)
    {
      for (int c = 0; c < COLS - 3; c++)
      {
        for (int i = 0; i < 4; i++)
        {
          window[i] = board[r + i][c + i];
        }
        score += evaluateWindow(window);
      }
    }

    // Диагональные окна (↗)
    for (int r = 3; r < ROWS; r++)
    {
      for (int c = 0; c < COLS - 3; c++)
      {
        for (int i = 0; i < 4; i++)
        {
          window[i] = board[r - i][c + i];
        }
        score += evaluateWindow(window);
      }
    }

    // Бонус за центральные позиции
    for (int r = 0; r < ROWS; r++)
    {
      if (board[r][COLS / 2] == computer)
        score += 50;
    }

    return score;
  }

  int minimax(int depth, int alpha, int beta, bool maximizingPlayer)
  {
    // Сначала проверяем немедленный выигрыш
    int winningMove = checkImmeditateWin(maximizingPlayer ? computer : player);
    if (winningMove != -1)
    {
      return maximizingPlayer ? 1000000 : -1000000;
    }

    if (depth == 0 || isBoardFull())
    {
      return evaluatePosition();
    }

    if (maximizingPlayer)
    {
      int maxEval = numeric_limits<int>::min();
      for (int col = 0; col < COLS; col++)
      {
        if (isValidMove(col))
        {
          int row = dropPiece(col, computer);
          int eval = minimax(depth - 1, alpha, beta, false);
          removePiece(row, col);
          maxEval = max(maxEval, eval);
          alpha = max(alpha, eval);
          if (beta <= alpha)
            break;
        }
      }
      return maxEval;
    }
    else
    {
      int minEval = numeric_limits<int>::max();
      for (int col = 0; col < COLS; col++)
      {
        if (isValidMove(col))
        {
          int row = dropPiece(col, player);
          int eval = minimax(depth - 1, alpha, beta, true);
          removePiece(row, col);
          minEval = min(minEval, eval);
          beta = min(beta, eval);
          if (beta <= alpha)
            break;
        }
      }
      return minEval;
    }
  }

public:
  ConnectFour() : board(ROWS, vector<char>(COLS, ' ')), player('O'), computer('X'), computerStarts(false)
  {
    loadConfiguration();
  }

  void printBoard() const
  {
    cout << "\n ";
    for (int col = 0; col < COLS; col++)
    {
      cout << " " << col + 1;
    }
    cout << "\n";

    for (int row = 0; row < ROWS; row++)
    {
      cout << "|";
      for (int col = 0; col < COLS; col++)
      {
        cout << " " << board[row][col];
      }
      cout << " |\n";
    }
    cout << "+---------------+\n";
  }

  int getBestMove()
  {
    // Сначала проверяем выигрышный ход
    int winningMove = checkImmeditateWin(computer);
    if (winningMove != -1)
    {
      return winningMove;
    }

    // Затем проверяем, нужно ли блокировать победу противника
    winningMove = checkImmeditateWin(player);
    if (winningMove != -1)
    {
      return winningMove;
    }

    int bestScore = numeric_limits<int>::min();
    int bestMove = 0;

    for (int col = 0; col < COLS; col++)
    {
      if (isValidMove(col))
      {
        int row = dropPiece(col, computer);
        int score = minimax(DEPTH, numeric_limits<int>::min(), numeric_limits<int>::max(), false);
        removePiece(row, col);

        if (score > bestScore)
        {
          bestScore = score;
          bestMove = col;
        }
      }
    }
    return bestMove;
  }

  bool makeMove(int col, char piece)
  {
    if (!isValidMove(col))
      return false;
    int row = dropPiece(col, piece);
    return row != -1;
  }

  bool isGameOver(char piece, int lastCol)
  {
    if (lastCol == -1)
      return false;

    int lastRow = 0;
    for (int row = 0; row < ROWS; row++)
    {
      if (board[row][lastCol] == piece)
      {
        lastRow = row;
        break;
      }
    }

    return checkWin(lastRow, lastCol, piece) || isBoardFull();
  }

  void setPosition(const vector<vector<char>> &newBoard)
  {
    board = newBoard;
  }

  void loadConfiguration()
  {
    ifstream configFile("config.txt");
    if (!configFile.is_open())
    {
      ofstream createConfigFile("config.txt");
      createConfigFile << "ROWS=6\n";
      createConfigFile << "COLS=7\n";
      createConfigFile << "DEPTH=8\n";
      createConfigFile << "FIRST=player\n";
      createConfigFile.close();

      ROWS = 6;
      COLS = 7;
      DEPTH = 8;
      computerStarts = false;
      return;
    }

    string line;
    while (getline(configFile, line))
    {
      if (line.find("FIRST=") != string::npos)
      {
        string value = line.substr(line.find("=") + 1);
        computerStarts = (value == "computer");
      }
      if (line.find("ROWS=") != string::npos)
        ROWS = stoi(line.substr(line.find("=") + 1));
      if (line.find("COLS=") != string::npos)
        COLS = stoi(line.substr(line.find("=") + 1));
      if (line.find("DEPTH=") != string::npos)
        DEPTH = stoi(line.substr(line.find("=") + 1));
    }
    configFile.close();
  }

  void play()
  {
    cout << "Добро пожаловать в игру '4 в ряд'!\n";
    cout << "Вы играете 'O', компьютер играет 'X'\n";
    cout << "Для хода введите номер столбца (1-7)\n";

    // Если компьютер ходит первым, делаем его ход
    if (computerStarts)
    {
      int computerCol = getBestMove();
      makeMove(computerCol, computer);
      cout << "Компьютер начинает и делает ход в столбец " << computerCol + 1 << "\n";
    }

    bool gameOver = false;
    int lastMove = -1;

    while (!gameOver)
    {
      printBoard();

      // Ход игрока
      int playerCol;
      do
      {
        cout << "Ваш ход (1-7): ";
        cin >> playerCol;
        playerCol--; // Преобразуем в 0-based индекс
      } while (!makeMove(playerCol, player));

      if (isGameOver(player, playerCol))
      {
        printBoard();
        cout << "Поздравляем! Вы победили!\n";
        break;
      }

      // Ход компьютера
      int computerCol = getBestMove();
      makeMove(computerCol, computer);
      cout << "Компьютер делает ход в столбец " << computerCol + 1 << "\n";

      if (isGameOver(computer, computerCol))
      {
        printBoard();
        cout << "Компьютер победил!\n";
        break;
      }

      if (isBoardFull())
      {
        printBoard();
        cout << "Ничья!\n";
        break;
      }
    }
  }
};

int ConnectFour::ROWS = 6;
int ConnectFour::COLS = 7;
int ConnectFour::DEPTH = 8;

int main()
{
  ConnectFour game;
  game.play();
  return 0;
}
