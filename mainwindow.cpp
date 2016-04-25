#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QRegExp>
#include <QTcpSocket>
#include <QString>
#include <QListWidgetItem>
#include <QListWidget>
#include <QLineEdit>
#include <QTextEdit>
#include <QAbstractItemView>
#include <QWidget>
#include <QObject>
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    socket = new QTcpSocket(this);
    //socket = new QTcpSocket(this);
    //ne mora SLOT(fja) qt sam prepozna da treba start fja
    //connect(socket, SIGNAL(connected()), this, SLOT(connected()));
    //when socket has text ready for read
    connect(socket, SIGNAL(readyRead()),this, SLOT(readyRead()));
}
//napraviti mozda prvi ekran gde se unosi ip adresa

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_sayButton_clicked()
{
    QString message = (ui->sayText)->toPlainText().trimmed();
    if(!message.isEmpty())
    {
        socket->write(QString(message + "\n").toUtf8());
    }
    ui->sayText->clear();
    ui->sayText->setFocus();
}
void MainWindow::readyRead(){

    while(socket->canReadLine()){
        //sa ovim Utf8 moze da se koriste i nasa slova
        QString line = QString::fromUtf8(socket->readLine()).trimmed();

        //normalne poruke sa servera su "korisnickoime: poruka..."
        QRegExp messageRegex("^([^:]+):(.*)$");

        //poruke servera "/users:..." salju nam korisnike , koje stavljamo u listu
        QRegExp usersRegex("^/users:(.*)$");

        QRegExp usersRegex2("^/users:(.*)$");

        if(usersRegex.indexIn(line) != -1) // -1 ako ne nadje
        {
            QStringList users = usersRegex.cap(1).split(",");
            (ui->listWidget)->clear();
            //brisem sve korisnike iz liste , zbog update
           /* int c = ui->listWidget->count();
            for(int row = 0; row < c; row++)
            {
                delete ui->listWidget->takeItem(0);
                qDebug() << "count after iteration" << row << "is " << listWidget->count();
            }
            */
            foreach(QString user, users)
                new QListWidgetItem(user, ui->listWidget);
                //QPixmap(":/user.png"), kao prvi argument za ikonice korisnika
        }

        else if(messageRegex.indexIn(line) != -1)
        {
            QString user = messageRegex.cap(1);
            QString message = messageRegex.cap(2);

            ui->textEdit->append("<b>" + user + "</b>: " + message);
        }
    }

}
void MainWindow::connected(){
    //saljemo korisnicko ime na server
    socket->write(QString("/me:" + ui->username->toPlainText() + "\n").toUtf8());
}


void MainWindow::on_login_clicked()
{
    socket->connectToHost("127.0.0.1", 1234);
    //obrada izuzetaka
}
