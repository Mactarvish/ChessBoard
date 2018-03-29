#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    connect(ui->retractButton, SIGNAL(clicked()), this, SLOT(ReTract()));

    // 设置手动添加的控件

    picture = new QPixmap(":/source/ChessBoard.png");
    chessboard = new QLabel(this);
    chessboard->setPixmap(QPixmap(":/source/ChessBoard.png"));
    chessboard->setAutoFillBackground(true);

    blackChess = new QPixmap(":/source/blackChess.png");
    whiteChess = new QPixmap(":/source/whiteChess.png");
    chessPicture[0] = QPixmap(":/source/blackChess.png");
    chessPicture[1] = QPixmap(":/source/whiteChess.png");

    victoryLine = new QLabel(this);
    victoryLine->hide();

    gameInfo = new QLabel(this);
    gameInfo->move(13 * 30 + 10, 20 * 30 + 10);
    gameInfo->hide();
    gameInfo->raise();

    restartButton = new QPushButton(this);
    restartButton->move(13 * 30 + 10, 20 * 30 + 10 + 30);
    restartButton->setText(u8"重新开始");
    restartButton->show();
    connect(restartButton, SIGNAL(clicked()), this, SLOT(RestartGame()));

    placementTip = new QLabel(this);
    placementTip->setPixmap(QPixmap(":/source/PlacementTip.png"));
    placementTip->raise();
    placementTip->hide();
    placementTip->setMouseTracking(true);

    ui->lineEdit->raise();
    ui->lineEdit->setReadOnly(true);
    ui->lineEdit->move(200, 600);
    ui->retractButton->raise();
    ui->retractButton->move(200, 620);

    optionButton = new QPushButton(this);
    optionButton->move(ui->retractButton->x(), ui->retractButton->y() + 20);
    optionButton->setText(u8"设置");
    optionButton->raise();
    optionButton->show();

	modeGroupBox = new QGroupBox(u8"游戏模式", this);
    //QRadioButton
//	optionDialog = new QDialog(this);

	

	this->setMouseTracking(true);
    chessboard->setMouseTracking(true);
    victoryLine->setMouseTracking(true);

    this->setFixedSize(chessboard->pixmap()->size());

    // 初始化棋盘信息
    mode = CVC;
    retractInfo.clear();
    firstMove = false;
    gameOver = false;
    color[0] = BLACK;
    color[1] = WHITE;
    for (int i = 0; i < 20; i++)
    {
        for (int j = 0; j < 20; j++)
        {
            info[i][j].taken = false;
            info[i][j].color = NONE;
            info[i][j].x = i;
            info[i][j].y = j;
        }
    }

    restartButton->raise();

}

Widget::~Widget()
{
    delete ui;
}

bool Widget::MoveChess(const int &x, const int &y)
{
    if (x < 0 || x > 19 || y < 0 || y > 19)
    {
        return false;
    }

    if (info[x][y].taken == true)
    {
        return false;
    }

    ChessInfo& currentPosInfo = info[x][y];
    firstMove = !firstMove;
    currentPosInfo.taken = true;
    currentPosInfo.color = color[firstMove];
    currentPosInfo.chess = new QLabel(this);
    currentPosInfo.chess->setPixmap(chessPicture[firstMove]);
    currentPosInfo.chess->move(x * 30 + 10 - 12, y * 30 + 10 - 12);
    currentPosInfo.chess->show();
    retractInfo.push(info[x][y]);

    int x_start = 0;
    int y_start = 0;
    int x_last = 0;
    int y_last = 0;
    if (JudgeVictory(x, y, x_start, y_start, x_last, y_last))
    {
        fuck x_start << y_start;
        DrawVictoryLine(x_start, y_start, x_last, y_last);

        if (info[x][y].color == BLACK)
        {
            gameInfo->setText(u8"<h2><font color=red>黑方胜！</font></h2>");
        }
        else
        {
            gameInfo->setText(u8"<h2><font color=red>白方胜！</font></h2>");
        }
        gameInfo->show();
        gameOver = true;
    }

    lastChess = QPoint(x, y);
    return true;
}

bool Widget::MoveChess(const QPoint &point)
{
    return MoveChess(point.x(), point.y());
}

void Widget::RestartGame()
{
    for (int i = 0; i <  20; i++)
    {
        for (int j = 0; j < 20; j++)
        {
            if (info[i][j].taken)
            {
                delete info[i][j].chess;
                info[i][j].color = NONE;
            }
            info[i][j].taken = false;
        }
    }
    victoryLine->setPixmap(QPixmap());
    victoryLine->hide();
    gameInfo->hide();
    gameOver = false;
    firstMove = false;
}

void Widget::ReTract()
{
    // 目前只支持PVP情况的悔棋，PVC尚未设计
    if (retractInfo.count() == 0)
    {
        return;
    }

    if (gameOver)
    {
        victoryLine->setPixmap(QPixmap());
        victoryLine->hide();
        gameInfo->hide();
    }

    ChessInfo lastChess =  retractInfo.pop();
    DeleteChess(lastChess.x, lastChess.y);
    firstMove = !firstMove;
    gameOver = false;
}

// 落点位置相对于棋盘左上角原点的坐标，以一格为单位。
QPoint Widget::CalculatePlacement(const QPoint &pos)
{
    int x_placement = 0;
    int y_placement = 0;

    int x_relative = pos.x() - 10;
    int y_relative = pos.y() - 10;

    int x_offset = (x_relative) % 30 - 15;
    if (x_offset < 0)
    {
        x_placement = x_relative - ((x_relative) % 30);
    }
    else
    {
        x_placement = x_relative - ((x_relative) % 30) + 30;
    }

    int y_offset = (y_relative) % 30 - 15;
    if (y_offset < 0)
    {
        y_placement = y_relative - ((y_relative) % 30);
    }
    else
    {
        y_placement = y_relative - ((y_relative) % 30) + 30;
    }

    //return QPoint(x_placement + 10, y_placement + 10);
    return QPoint(x_placement / 30, y_placement / 30);
}

void Widget::MovePlacementTip(const int &x, const int &y)
{
    placementTip->move(x * 30 + 10 - 15, y * 30 + 10 - 15);
    placementTip->show();
}

void Widget::MovePlacementTip(const QPoint &point)
{
    return MovePlacementTip(point.x(), point.y());
}

void Widget::DrawVictoryLine(const int &x_start, const int &y_start,const int &x_last, const int &y_last)
{
    QLine line(x_start * 30 + 10, y_start * 30 + 10,
               x_last * 30 + 10, y_last * 30 + 10);
    QPixmap topPicture(this->width(), this->height() - 50);
    topPicture.fill(Qt::transparent);
    QPainter painter(&topPicture);
    painter.setPen(QPen(Qt::red, 5, Qt::SolidLine, Qt::RoundCap));
    painter.drawLine(line);
    victoryLine->setPixmap(topPicture);

    victoryLine->raise();
    victoryLine->show();

}

// 判赢
bool Widget::JudgeVictory(const int &x, const int &y, int& x_start, int& y_start, int& x_last, int& y_last) const
{
    ChessColor winerColor = info[x][y].color;

    int seriesCount = 1;
    int x_current = x;
    int y_current = y;
    QPoint lastSameChess;

    int operation = 0;
    while (1)
    {
        x_current = x;
        y_current = y;

        // 执行完一个双向查找后，复位五连子的起始点
        if (operation % 2 == 0)
        {
            x_start = x;
            y_start = y;
            lastSameChess.setX(x);
            lastSameChess.setY(y);
        }

        for (int i = 1; i < 5; i++)
        {
            // 调整当前判定点的坐标
            AdjustCoordinate(x_current, y_current, x, y, i, operation);
            if (IsSameColor(x_current, y_current, winerColor))
            {
                seriesCount++;
                if (seriesCount == 5)
                {
                    // 回传最后的棋子的坐标
                    x_last = x_current;
                    y_last = y_current;
                    return true;
                }
                lastSameChess.setX(x_current);
                lastSameChess.setY(y_current);
            }
            else
            {
                x_start = lastSameChess.x();
                y_start = lastSameChess.y();
                operation++;
                if (operation == 8)
                {
                    return false;
                }
                else if (operation % 2 == 0)
                {
                    seriesCount = 1;
                }
                break;
            }
        }
    }
}

bool Widget::IsSameColor(const int &x, const int &y, const Widget::ChessColor &color) const
{
    return (IsValidPosition(x, y) && info[x][y].taken && info[x][y].color == color);
}

bool Widget::IsValidPosition(const int &x, const int &y) const
{
    return (x >= 0 && x < 20 && y >= 0 && y < 20);
}

void Widget::AdjustCoordinate(int &x_current, int &y_current, const int &x, const int &y, const int &i, const int &operation) const
{
    switch (operation)
    {
        // 横向查找
        case 0:
        {
            x_current = x - i;
            break;
        }
        case 1:
        {
            x_current = x + i;
            break;
        }
        // 左斜查找
        case 2:
        {
            x_current = x - i;
            y_current = y - i;
            break;
        }
        case 3:
        {
            x_current = x + i;
            y_current = y + i;
            break;
        }
        // 竖向查找
        case 4:
        {
            y_current = y - i;
            break;
        }
        case 5:
        {
            y_current = y + i;
            break;
        }
        // 右斜查找
        case 6:
        {
            x_current = x + i;
            y_current = y - i;
            break;
        }
        case 7:
        {
            x_current = x - i;
            y_current = y + i;
            break;
        }
    }
}

void Widget::DeleteChess(const int &x, const int &y)
{
    if (!info[x][y].taken)
    {
        qDebug() << "ERROR:cannot delete:Chess does not exist.";
    }
    delete info[x][y].chess;
    info[x][y].color = NONE;
    info[x][y].taken = false;
}

void Widget::AIMove()
{
    QTime t;
    t.start();
    while(t.elapsed() < 200)
    {
        QCoreApplication::processEvents();
    }

    // 把棋下在上一个棋子的周围一圈的任意位置
    int offset[3] = {-1, 0, 1};
    int x = lastChess.x() + offset[qrand() % 3];
    int y = lastChess.y() + offset[qrand() % 3];

    while (!MoveChess(x, y))
    {
        x = lastChess.x() + offset[qrand() % 3];
        y = lastChess.y() + offset[qrand() % 3];
        QCoreApplication::processEvents();
    }
}

void Widget::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape)
    {
        this->close();
    }
}

void Widget::mouseMoveEvent(QMouseEvent *event)
{
    ui->lineEdit->setText(QString("%1, %2").arg(CalculatePlacement(event->pos()).x()).arg(CalculatePlacement(event->pos()).y()));
    if (event->pos().x() < -10 || event->pos().y() < 0
     || event->pos().x() > 590 || event->pos().y() > 580)
    {
        return;
    }

    MovePlacementTip(CalculatePlacement(event->pos()));
}

void Widget::mousePressEvent(QMouseEvent *event)
{
    if (gameOver)
    {
        return;
    }
    if (event->button() != Qt::LeftButton)
    {
        return;
    }

    if (event->pos().x() < -10 || event->pos().y() < 0
            || event->pos().x() > 590 || event->pos().y() > 580)
    {
        return;
    }
    QPoint placement = CalculatePlacement(event->pos());
    //DrawPieces(picture, placement, firstMove);
    //chessboard->setPixmap(*picture);

    bool isValidMove = MoveChess(placement);
    if (isValidMove && mode == PVC)
    {
        AIMove();
    }
    else if (isValidMove && mode == CVC)
    {
        while (!gameOver)
        {
            AIMove();
        }
    }
}


void GobangThread::run()
{

}
