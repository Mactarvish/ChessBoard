#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QPainter>
#include <QKeyEvent>
#include <QDebug>
#include <QPixmap>
#include <QPicture>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QTime>
#include <QStack>
#include <QThread>
#include <QDialog>
#include <QGroupBox>
#include <QRadioButton>

#define fuck qDebug() <<

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:

    enum ChessColor {BLACK, WHITE, NONE};
    enum GameMode   {PVP, PVC, CVC};

    explicit Widget(QWidget *parent = 0);
    ~Widget();

    bool MoveChess(const int& x, const int& y);
    bool MoveChess(const QPoint& point);


    struct ChessInfo
    {
        int x;
        int y;
        bool taken;
        QLabel* chess;
        ChessColor color;
    };

public slots:
    void RestartGame();
    void ReTract();

private:
    QPoint CalculatePlacement(const QPoint& pos);
    void MovePlacementTip(const int& x, const int& y);
    void MovePlacementTip(const QPoint& point);
    void DrawVictoryLine(const int &x_start, const int &y_start, const int& x_last, const int& y_last);

    bool JudgeVictory(const int& x, const int& y, int &x_start, int &y_start, int& x_last, int& y_last) const;

    bool VerticalSearch(const int& x, const int& y, const ChessColor& winerColor) const;
    bool IsSameColor(const int& x, const int& y, const ChessColor& color) const;
    bool IsValidPosition(const int& x, const int& y) const;
    // 判赢坐标调整
    void AdjustCoordinate(int& x_current, int& y_current,
                          const int& x, const int& y, const int& i, const int &operation) const;
    void DeleteChess(const int& x, const int& y);
    void AIMove();

    Ui::Widget *ui;
    QLabel* chessboard;
    QLabel* placementTip;
    QLabel* victoryLine;
    QPixmap* whiteChess;
    QPixmap* blackChess;
    QPixmap chessPicture[2];
    QPixmap* picture;

    QLabel* gameInfo;
    QPushButton* restartButton;
    QPushButton* optionButton;
	QDialog* optionDialog;
	QGroupBox* modeGroupBox;

    bool firstMove;
    bool gameOver;
    ChessColor color[2];
    GameMode mode;
    ChessInfo info[20][20];
    QStack<ChessInfo> retractInfo;

    QPoint lastChess;

protected:
    void keyPressEvent(QKeyEvent *event);
    void mouseMoveEvent(QMouseEvent* event);
    void mousePressEvent(QMouseEvent* event);
};

class GobangThread : public QThread
{
     Q_OBJECT
protected:
     void run();
};

#endif // WIDGET_H
