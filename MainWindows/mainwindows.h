#ifndef MAINWINDOWS_H
#define MAINWINDOWS_H

#include <QMainWindow>
#include<QLineEdit>
#include<QLabel>
#include <QVBoxLayout>
#include<QGridLayout>
#include<string>
#include<QMessageBox>




/*
 * &brief the MainWindows class
 * Represent the main application window that houses the over all UI,
 * such as menus, toolbars, and the central Sundoku widget
 */
class MainWindows : public QMainWindow
{
    Q_OBJECT
public:
    /*
     * &brief Constructor for the MainWindows
     * &pram parent widget which has a vlaue nullptr which indicate top-level windows
     */
    MainWindows(QWidget *parent = nullptr);


public:
    /*
     * &brief used to manage the main window on startup
     * &return a QSize object representing the ideal width and height
     */
    QSize sizeHint() const;

    /*
     * &brief Destructor to clean up dynamically allocated resources.
     */
    ~MainWindows();



private:
    // menu actions action to exit the application
   QAction *quitAction;
    //action to display game rules or about info
    QAction *help;

};





class welcomeWidget: public QWidget{


public:
    welcomeWidget(QWidget *parent = nullptr);




};


/*
 * @brief the sudoku class
 * a constum QWidget that contains teh 9x9 sudoku grid and handles all
 * the game logic, user input validation, and puzzle generation.
 */

class Sudoku: public QWidget {
public:
    /*
     * @brief Constructor to initialize the game board and UI layout
     * @param parent The parent widget
     */
    explicit Sudoku(QWidget *parent = nullptr);


    /*
     * @brief Analys the user's input and return true if it is an integer; otherwise, return false.
     * @param take a std::string referance which is a user input.
     */
    bool analys(std::string &);

    /*
     * @brief Chech if it is legally allowed to place a specific value is a given cell
     * validate against Sudoku rules
     * @param r the row index
     * @param c the column index
     * @param str is the valaue to chech, passed as a qString (from QLineEdit).
     * @return true if the the move is legal and false if it violates Sudoku sules.
     */
    bool isSafe( int r, int c ,const QString &str);

    /*
     * @brief clear the current board and generate a brand new Sudolu puzzle.
     */
    void generateNewGame();

    /*
     * @berif chech if the entire 9x9 grid is filled correctly
     * @return true if the board is complete and false if not.
     */
    bool complete();


    /*
     * @brief auto complete the board.
     */
    void completeTheBoard(int row, int col);

    /*
     * @berif genarate random numbers
     * @returnan integer.
     */
    int random();
    friend struct CellInputValidator;


private:

    /*
     * @berif the 9x9 grid of text input fields representing the sudoku board
     * QLineEdit
     */
    QLineEdit *cell[9][9];

};



/*
 * @berif this struct is used to create a vector of cordinate position to 9x9 cells.
 */

template<typename T>
struct cordinate {
    // the row index of the cell (0-8)
    T row;
    // the column index if cell (0-8)
    T col;

    cordinate(T r = 0, T c = 0): row(r), col(c){}
};


/*
 * @berif functor that evaluate a char wether it is a string och intger.
 * @param c is a char and user input.
 */
struct isDigitFunctor{
    bool operator()(char c){
        return std::isdigit(static_cast<unsigned char>(c));
    }
};

/*
 *@brief functor that evaluate the user input.
 *@param userInput is a QString referance.
 */
struct CellInputValidator {
    Sudoku* board; // Pointer to the main game board
    int row;
    int col;

    CellInputValidator(Sudoku* g, int r, int c): board(g), row(r), col(c){};


    void operator()(const QString &userinput) const {
        if (userinput.isEmpty()) return;


        try {
            std::string input = userinput.toStdString();
            if(!board->analys(input)){
                    throw std::invalid_argument("it is an invalid input: it has to be an integer: ");
                };

            if (!board->isSafe(row, col, userinput)) {

                throw std::logic_error("invalid move chose another number:");

            }
            else if (board->complete()) {
                QMessageBox::information(board, "Winner!", " Congratulations, you completed the game!");

            }


        } catch (const std::invalid_argument& e) {
            QString errorMessage = QString("<b>") + e.what() + "</b>";
            QMessageBox::information(board, "game over",errorMessage);
            board->cell[row][col]->clear();


        }catch(std::logic_error& e){
            QString errorMessage = QString("<b>") + e.what() + "</b>";
            QMessageBox::information(board, "try again", errorMessage);
            board->cell[row][col]->clear();
        }
    }
};

#endif // MAINWINDOWS_H
