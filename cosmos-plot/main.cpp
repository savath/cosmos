#include "mainwindow.h"
#include <QApplication>
#include "plot.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QStringList args = a.arguments();

    MainWindow w;

    if (args.count() != 2) // need to find arguments before plotting
    {
        //cout << "Usage: json file " << endl;
        //exit (1);
        w.init_without_json();

    }
    else    {

        qDebug ("json file to load: %s", argv[1]);

        w.init_from_json(argv[1]);
        w.setup_plot_widget();
    }

    //w.setup_init_widget();

    w.setup_find_widget();
    //w.startTimers();
    //w.setup_plot_widget();
    w.show();

    return a.exec();
}
