/*
 * @berif the main file Implementation MainWindows and Sudoku classes.
 * this file contain the logic for the main application windows including
 * the start menu, instruction, and the core 9x9 sudoku game board and functionality.
 */


#include <QPushButton>
#include <QLabel>
#include <QFont>
#include "mainwindows.h"
#include <vector>
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QStatusBar>
#include <QApplication>
#include <string>
#include <random>
#include <QPalette>
#include <QColor>
#include <QPixmap>
#include <algorithm>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QMessageBox>

// New headers for REST API communication
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QUrl>

/*
 * @brief Constructs the main login menu window for the Sudoku application with REST API integration.
 * @param parent The parent widget (default is nullptr for top-level window).
 */
MainWindows::MainWindows(QWidget *parent)
    : QMainWindow{parent}
{
    // Load and apply the background image
    QPixmap bkgnd(":/images/sudoku.jpg");
    bkgnd = bkgnd.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette pal = palette();
    pal.setBrush(QPalette::Window, QBrush(bkgnd));
    this->setAutoFillBackground(true);
    this->setPalette(pal);

    // Setup help/instructions Menu Action
    help = new QAction("instructions");
    connect(help, &QAction::triggered, this, [=](){
        QMessageBox::information(this, "Sudoku Game Rules",
                                 "<b>The Row Rule:</b><br>"
                                 "Every horizontal row must contain the numbers 1 through 9 exactly once.<br><br>"
                                 "<b>The Column Rule:</b><br>"
                                 "Every vertical column must contain the numbers 1 through 9 exactly once.<br><br>"
                                 "<b>The Block Rule:</b><br>"
                                 "Every 3x3 block must contain the numbers 1 through 9 exactly once.");
    });

    // Setup Quit Menu Action
    quitAction = new QAction("Quit Application");
    connect(quitAction, &QAction::triggered, this, [=](){
        QApplication::quit();
    });

    // Add Actions to menu bar
    QMenu *fileMenu0 = menuBar()->addMenu("quit");
    fileMenu0->addAction(quitAction);

    QMenu *fileMenu = menuBar()->addMenu("help");
    fileMenu->addAction(help);

    // UI element styling dimensions
    int standardWidth = 250;
    int standardHeight = 40;
    QFont font("Times", 10, QFont::Bold);

    // Heading label
    QLabel *label = new QLabel("    Please Login", this);
    label->setObjectName("labelStyle");
    QString labelstyle = "QLabel#labelStyle { "
                         "background-color: orange;"
                         "border-radius: 5px;"
                         "border: 1px solid gray;"
                         " }";
    label->setStyleSheet(labelstyle);
    label->setFixedSize(standardWidth, standardHeight);
    label->setFont(font);

    // Username and Password input fields
    QLineEdit *usernameInput = new QLineEdit(this);
    usernameInput->setPlaceholderText("Enter username...");
    usernameInput->setFixedSize(standardWidth, standardHeight);
    usernameInput->setFont(font);

    QLineEdit *passwordInput = new QLineEdit(this);
    passwordInput->setPlaceholderText("Enter password...");
    passwordInput->setEchoMode(QLineEdit::Password);
    passwordInput->setFixedSize(standardWidth, standardHeight);
    passwordInput->setFont(font);

    QPushButton *button = new QPushButton("Submit!", this);
    button->setFixedSize(standardWidth, standardHeight);
    button->setFont(font);

    // Central widget layout setup
    QWidget *central = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(central);
    layout->addWidget(label, 0, Qt::AlignCenter);
    layout->addWidget(usernameInput, 0, Qt::AlignCenter);
    layout->addWidget(passwordInput, 0, Qt::AlignCenter);
    layout->addWidget(button, 0, Qt::AlignCenter);
    layout->addStretch();

    setCentralWidget(central);

    // Login logic via REST API on submit click
    connect(button, &QPushButton::clicked, this, [=](){
        QString username = usernameInput->text().trimmed();
        QString password = passwordInput->text();

        if(username.isEmpty() || password.isEmpty()) {
            QMessageBox::warning(this, "Warning", "Username and password cannot be empty!");
            return;
        }

        // 1. Create a JSON payload with the credentials
        QJsonObject json;
        json["username"] = username;
        json["password"] = password;
        QJsonDocument doc(json);
        QByteArray data = doc.toJson();

        // 2. Setup Network Request to your Backend API Container
        // If testing locally outside docker, use http://localhost:3000/login
        QUrl url("http://backend:3000/login");
        QNetworkRequest request(url);
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

        // 3. Send the HTTP POST request
        QNetworkAccessManager *manager = new QNetworkAccessManager(this);
        QNetworkReply *reply = manager->post(request, data);

	//std::shared_ptr<welcomeWidget> welcome = std::make_shared<welcomeWidget>();


        // 4. Wait asynchronously for the backend API to respond
        connect(reply, &QNetworkReply::finished, this, [=]() {
            if (reply->error() == QNetworkReply::NoError) {

                // Parse the backend's JSON response
                QByteArray response_data = reply->readAll();
                QJsonDocument response_doc = QJsonDocument::fromJson(response_data);
                QJsonObject response_obj = response_doc.object();

                if (response_obj["status"].toString() == "success") {
                    QMessageBox::information(this, "Success", "Login successful! Launching Sudoku...");
                    

		welcomeWidget *welcome = new welcomeWidget(); 

		// 2. Tell Qt to clean up the memory only when the user clicks the X to close the window
		welcome->setAttribute(Qt::WA_DeleteOnClose);
                    welcome->show();
                    welcome->resize(500, 500);
                    this->close(); // Close login window
                } else {
                    // This handles incorrect passwords or missing users
                    QMessageBox::warning(this, "Login Failed", response_obj["message"].toString());
                }
            } else {
                QMessageBox::critical(this, "Network Error", "Could not connect to the Backend Auth API.");
            }

            // Clean up memory
            reply->deleteLater();
            manager->deleteLater();
        });
    });
}


welcomeWidget::welcomeWidget(QWidget *parent):QWidget(parent){

    QPixmap bkgnd(":/images/sudoku.jpg");

    //Scale the image to fit your initial window size
    bkgnd = bkgnd.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette pal = palette();
    pal.setBrush(QPalette::Window, QBrush(bkgnd));
    this->setAutoFillBackground(true);
    this->setPalette(pal);

    //UI element styling and creation
    int standardWidth = 250;
    int standardHeight = 40;
    QFont font("Times", 10, QFont::Bold);


    QLabel *label = new QLabel("        Welcome to Sudoku", this);
    label->setObjectName("labelStyle");
    QString labelstyle = "QLabel#labelStyle { "
                         "background-color: orange;"
                         "border-radius: 5px;"
                         "border: 1px gray;"
                         " }";
    label->setStyleSheet(labelstyle);
    QPushButton * button = new QPushButton("Press to Start the Game!", this);

    label->setFixedSize(standardWidth, standardHeight);
    button->setFixedSize(standardWidth, standardHeight);

    label->setFont(font);
    button->setFont(font);

Sudoku *sudoku = new Sudoku();

  QVBoxLayout *layout = new QVBoxLayout(this); // Assigns layout to central
    layout->addWidget(label, 0, Qt::AlignCenter);
    layout->addWidget(button, 0, Qt::AlignCenter);
    layout->addStretch();

    // lunch the sudoku game when the start button is clicked
    connect(button, &QPushButton::clicked, this, [sudoku, this](){
        sudoku->show();
        sudoku->resize(200, 200);
        this->close();
    });

}


/*
 * @berif suggest the defoult size of the main window
 * @return QSize representing the intial window dimensions.
 */

QSize MainWindows::sizeHint() const
{
    return QSize(500, 500);
}

/*
 * @berif destructor for MainWindows. frees allocated actions.
 */
MainWindows::~MainWindows()
{
    delete quitAction;
    delete help;
}


/*
 * @berif construct the sudoku game board widget.
 * @param parent The parent widget (default is nullptr)
 */
Sudoku::Sudoku(QWidget *parent): QWidget(parent){

    // background setup
    QPixmap bkgnd (":/images/sudoku.jpg");
    bkgnd = bkgnd.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette pal = QPalette();
    pal.setBrush(QPalette::Window, QBrush(bkgnd));
    this->setAutoFillBackground(true);
    this->setPalette(pal);


    this->setWindowFlags(Qt::Window);
    this->setWindowTitle("Sudoku Board");
    QFont font("Times", 16, QFont::Bold);


    QVBoxLayout * layout = new QVBoxLayout(this);
    QLabel * label = new QLabel("Sudoku Game",this);

    label->setFont(font);
    label->setStyleSheet("QLabel {"
                         "   border: 1px solid gray;"
                         "   border-radius: 10px;"
                         "background-color: yellow; "
                         "}");
    layout->addWidget(label, 0, Qt::AlignCenter);

    QGridLayout * gridLyout = new QGridLayout();
    gridLyout->setSpacing(0);


    // Initialize the 9x9 grid of QLineEdits
    for(int row = 0; row < 9; ++row){
        for(int column = 0; column < 9; ++column){

            cell[row][column] = new QLineEdit(this);

            cell[row][column]->setFixedSize(40, 40);

            cell[row][column]->setAlignment(Qt::AlignCenter);

            QFont cellFont("Arial", 10, QFont::Bold);
            cell[row][column]->setFont(cellFont);
            cell[row][column]->setMaxLength(1);




            QString style = "QLineEdit { border: 1px solid #ccc; ";


            if (column == 2 || column == 5) {
                style += "border-right: 3px solid black; ";
            }

            if (row == 2 || row == 5) {
                style += "border-bottom: 3px solid black; ";
            }


            if (column == 0) style += "border-left: 3px solid black; ";
            if (column == 8) style += "border-right: 3px solid black; ";
            if (row == 0) style += "border-top: 3px solid black; ";
            if (row == 8) style += "border-bottom: 3px solid black; ";


            style += "}";
            cell[row][column]->setStyleSheet(style);

            gridLyout->addWidget(cell[row][column], row, column);


            // connect text change signal to validate unser input instantly

            CellInputValidator validator(this, row, column);
            connect(cell[row][column], &QLineEdit::textChanged, this, validator);
        }
    }

    // setup reset/new game button
    QVBoxLayout *buttonLayout = new QVBoxLayout(this);
    QPushButton *btn = new QPushButton("Reset && start New game", this);
    btn->setObjectName("btn");
    btn->setFixedSize(200, 30);
    btn->setFont(QFont("Arial", 10, QFont::Bold));
    btn->setStyleSheet("QPushButton {"
                       "   background-color: #f0f0f0;"
                       "   border: 1px solid #999;"
                       "   border-radius: 10px;"
                       "   padding: 5px;"
                       "}"
                       "QPushButton:hover {"
                       "   background-color: lightblue;"
                       "   border: 1px solid #333;"
                       "}");


    // button that auto complete the game.
    QPushButton * buttonCompleteTheGame = new QPushButton("Auto Complete The game",this);
    buttonCompleteTheGame->setObjectName("btn");
    buttonCompleteTheGame->setFixedSize(200, 30);
    buttonCompleteTheGame->setFont(QFont("Arial", 10, QFont::Bold));
    buttonCompleteTheGame->setStyleSheet("QPushButton {"
                                         "   background-color: #f0f0f0;"
                                         "   border: 1px solid #999;"
                                         "   border-radius: 10px;"
                                         "   padding: 5px;"
                                         "}"
                                         "QPushButton:hover {"
                                         "   background-color: lightblue;"
                                         "   border: 1px solid #333;"
                                         "}");




    buttonLayout->addWidget(btn, 0, Qt::AlignCenter);
    buttonLayout->addWidget(buttonCompleteTheGame, 0, Qt::AlignCenter);

    layout->addLayout(gridLyout);
    layout->addLayout(buttonLayout);




    connect(btn, &QPushButton::clicked, [=](){
        generateNewGame();
    });

    // connect the button complete the game to the function that do the task but before that find the empty cell.
    connect(buttonCompleteTheGame, &QPushButton::clicked, [=](){


        for(int row = 0; row < 8; ++row){
            for(int col = 0; col < 8; ++col){
                if(cell[row][col]->text().isEmpty()){
                    completeTheBoard( row, col);
                }
            }
        }
    });

}



/*
 * @berif analyses a string to ensure it only contain digit characters
 * @param c the string input to check
 * @return true if the string is an integer, false otherwise
 */
bool Sudoku::analys(std::string &c)
{

   return  std::all_of(c.begin(), c.end(), isDigitFunctor());

}

/*
 * @berif validates if a number can safely be placed in a specific row and column.
 * @parm r the target row index.
 * @param c the target column index.
 * param str the sting (number) being checked.
 * @return true if the number does not conflict with existing row, column, or block.
 */
bool Sudoku::isSafe(int r, int c,const QString &str)
{

    // check the row for conflicts
    for(int i = 0; i <= 8; ++i){
        if(i != c && !cell[r][i]->text().isEmpty()){
            if(cell[r][i]->text() == str){
                return false;
            }
        }
    }

    // check column for conflicts
    for(int i = 0; i <= 8; ++i){
        if(i != r && !cell[i][c]->text().isEmpty()){
            if(cell[i][c]->text() == str){
                return false;
            }
        }
    }

    // check 3x3 block for conflicts
    int startRow = (r / 3) * 3;
    int startCol = (c / 3) * 3;

    for(int i = 0; i <= 2; ++i){
        for(int j = 0; j <= 2; ++j){

            int checkrow = startRow + i;
            int checkcol = startCol + j;
            if(checkrow != r && !cell[checkrow][checkcol]->text().isEmpty()){
                if(cell[checkrow][checkcol]->text() == str){
                    return false;
                }
            }
        }
    }

    return true;

}


/*
 * @berif Generate new Sudoku puzzle layout.
 * clear the bord and randomly fills a predefined set of coordinates.
 */
void Sudoku::generateNewGame()
{
    // 1. Clear the entire board first so you start fresh every time!
    for (int row = 0; row < 9; ++row) {
        for (int col = 0; col < 9; ++col) {
            cell[row][col]->clear();
            cell[row][col]->setReadOnly(false);
        }
    }


    std::vector<cordinate<int>>initCordinate;
    for(int row = 0; row < 9; ++row){
        for(int column = 0; column < 9; ++column ){
            cordinate<int> cord;
            cord.col = column;
            cord.row = row;
            initCordinate.push_back(cord);
        }
    }

    std::mt19937_64 rang(std::random_device{}());

    std::shuffle(initCordinate.begin(), initCordinate.end(), rang);

    int count = 0;

    // 4. Fill the board
    for (auto it : initCordinate) {

        if(count >= 30){
            break;
        }
        int row = it.row;
        int column = it.col;

        bool numberPlaced = false;
        int attempts = 0;

        // Keep guessing random numbers until we find a safe one!
        // (We limit attempts to 50 so the game doesn't freeze if it gets stuck)
        while (!numberPlaced && attempts < 50) {

            // Generate a random number from 1 to 9
            int randomValue = random();
            QString tx = QString::number(randomValue); // string conversion

            // fills the board but before that check if is safe.
            if (isSafe(row, column, tx)) {
                cell[row][column]->setText(tx);
                cell[row][column]->setReadOnly(true);
                numberPlaced = true;
                count++;

            }
            attempts++;
        }

    }
}

/*
 * @berif check if the Sudoku board is fully filled.
 * @return true if no cells are empty, false otherwise.
 */
bool Sudoku::complete()
{    
    for(int row = 0; row < 9; ++row){
        for(int col = 0; col < 9; ++col){        
                  if(cell[row][col]->text().isEmpty()){
                return false;
            }
        }
    }
    return true;
}

/*
 * @berif this function solve the board.
 * @param row and col is the cordinate of the empty cell.
 */

void Sudoku::completeTheBoard(int row, int col)
{

    // Systematically try every number from 1 to 9
    for (int guess = 1; guess <= 9; ++guess) {

        // Convert the integer guess to a QString
        QString textInt = QString::number(guess);

        // Check if this guess is safe
        if (isSafe(row, col, textInt)) {
            cell[row][col]->setText(textInt);
            cell[row][col]->setReadOnly(true);


            break;
        }
    }
}



/*
 * @berif genarate random integer between 1 and 9.
 * @return a random integer.
 */
int Sudoku::random()
{
    static  std::mt19937 rng{ std::random_device{}() };
    std::uniform_int_distribution<int> dist(1, 9);
    int value = dist(rng);
    return value;

}



























/*
#include<QPushButton>
#include<QLabel>
#include<QFont>
#include "mainwindows.h"
#include<vector>
#include<QAction>
#include<QMenu>
#include<QMenubar>
#include<QStatusBar>
#include<QApplication>
#include<string>
#include<random>
#include <QPalette>
#include <QColor>
#include<QPixmap>
#include<algorithm>
#include<QLineEdit>
#include<QVBoxLayout>
#include<QMessageBox>
#include<QSqlDatabase>
#include<QSqlQuery>
#include<QSqlError>


MainWindows::MainWindows(QWidget *parent)
    : QMainWindow{parent}
{
    // Load and apply the background image
    QPixmap bkgnd(":/images/sudoku.jpg");
    bkgnd = bkgnd.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette pal = palette();
    pal.setBrush(QPalette::Window, QBrush(bkgnd));
    this->setAutoFillBackground(true);
    this->setPalette(pal);

    // Setup help/instructions Menu Action
    help = new QAction("instructions");
    connect(help, &QAction::triggered, this, [=](){
        QMessageBox::information(this, "Sudoku Game Rules",
                                 "<b>The Row Rule:</b><br>"
                                 "Every horizontal row must contain the numbers 1 through 9 exactly once.<br><br>"
                                 "<b>The Column Rule:</b><br>"
                                 "Every vertical column must contain the numbers 1 through 9 exactly once.<br><br>"
                                 "<b>The Block Rule:</b><br>"
                                 "Every 3x3 block must contain the numbers 1 through 9 exactly once.");
    });

    // Setup Quit Menu Action
    quitAction = new QAction("Quit Application");
    connect(quitAction, &QAction::triggered, this, [=](){
        QApplication::quit();
    });

    // Add Actions to menu bar
    QMenu *fileMenu0 = menuBar()->addMenu("quit");
    fileMenu0->addAction(quitAction);

    QMenu *fileMenu = menuBar()->addMenu("help");
    fileMenu->addAction(help);

    // UI element styling dimensions
    int standardWidth = 250;
    int standardHeight = 40;
    QFont font("Times", 10, QFont::Bold);

    // Heading label
    QLabel *label = new QLabel("    Please Login", this);
    label->setObjectName("labelStyle");
    QString labelstyle = "QLabel#labelStyle { "
                         "background-color: orange;"
                         "border-radius: 5px;"
                         "border: 1px solid gray;"
                         " }";
    label->setStyleSheet(labelstyle);
    label->setFixedSize(standardWidth, standardHeight);
    label->setFont(font);

    // Username and Password input fields
    QLineEdit *usernameInput = new QLineEdit(this);
    usernameInput->setPlaceholderText("Enter username...");
    usernameInput->setFixedSize(standardWidth, standardHeight);
    usernameInput->setFont(font);

    QLineEdit *passwordInput = new QLineEdit(this);
    passwordInput->setPlaceholderText("Enter password...");
    passwordInput->setEchoMode(QLineEdit::Password);
    passwordInput->setFixedSize(standardWidth, standardHeight);
    passwordInput->setFont(font);

    QPushButton *button = new QPushButton("Submit!", this);
    button->setFixedSize(standardWidth, standardHeight);
    button->setFont(font);


    std::shared_ptr<welcomeWidget>welcome = std::make_shared<welcomeWidget>();

    // Central widget layout setup
    QWidget *central = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(central);
    layout->addWidget(label, 0, Qt::AlignCenter);
    layout->addWidget(usernameInput, 0, Qt::AlignCenter);
    layout->addWidget(passwordInput, 0, Qt::AlignCenter);
    layout->addWidget(button, 0, Qt::AlignCenter);
    layout->addStretch();

    setCentralWidget(central);

    // lunch the sudoku game when the start button is clicked
    connect(button, &QPushButton::clicked, this, [welcome](){
        welcome->show();
        welcome->resize(500, 500);
    });

    // Database initialization & login/registration logic on submit click
    connect(button, &QPushButton::clicked, this, [=](){
        QString username = usernameInput->text().trimmed();
        QString password = passwordInput->text();

        if(username.isEmpty() || password.isEmpty()) {
            QMessageBox::warning(this, "Warning", "Username and password cannot be empty!");
            return;
        }

        // Setup SQLite connection
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("sudoku_users.db");

        if (!db.open()) {
            QMessageBox::critical(this, "Database Error", "Could not open database connection.");
            return;
        }

        QSqlQuery query;
        query.exec("CREATE TABLE IF NOT EXISTS Users ("
                   "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                   "username TEXT UNIQUE, "
                   "password TEXT)");

        // Check if username exists in database
        query.prepare("SELECT password FROM Users WHERE username = :username");
        query.bindValue(":username", username);

        if (query.exec() && query.next()) {
            // Username found, verify password
            QString storedPassword = query.value(0).toString();
            if (storedPassword == password) {
                QMessageBox::information(this, "Success", "Login successful! Launching Sudoku...");

                // Launch the Sudoku game window
                std::shared_ptr<welcomeWidget> welcome = std::make_shared<welcomeWidget>();
                welcome->show();
                welcome->resize(400, 400);
                this->close(); // Close login window
            } else {
                QMessageBox::warning(this, "Login Failed", "Incorrect password. Please try again.");
            }
        } else {
            // Username NOT found -> Prompt user to register
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this, "User Not Found",
                                          "Username not found in the database. Would you like to register as a new user?",
                                          QMessageBox::Yes | QMessageBox::No);

            if (reply == QMessageBox::Yes) {
                QSqlQuery insertQuery;
                insertQuery.prepare("INSERT INTO Users (username, password) VALUES (:username, :password)");
                insertQuery.bindValue(":username", username);
                insertQuery.bindValue(":password", password);

                if (insertQuery.exec()) {
                    QMessageBox::information(this, "Registered", "Registration successful! You are now logged in.");
                    std::shared_ptr<welcomeWidget> welcome = std::make_shared<welcomeWidget>();
                    welcome->show();
                    welcome->resize(400, 400);
                    this->close();
                } else {
                    QMessageBox::critical(this, "Error", "Could not register user to database.");
                }
            }
        }
    });
}
*/


/*void Sudoku::completeTheBoard(int row, int col)
{


    bool numberPlaced = false;
    int attempts = 0;

    while(!numberPlaced && attempts < 50){
        int randomInt = random();
        QString textInt = QString::number(randomInt);

        if(isSafe(row, col, textInt)){
            cell[row][col]->setText(textInt);
            cell[row][col]->setReadOnly(true);
            numberPlaced = true;
        }

        attempts++;
    }

}*/
