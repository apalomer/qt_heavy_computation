#include <QApplication>
#include <QStyleFactory>
#include "heavy_computation_widget.h"

int main(int argc, char** argv)
{
  QApplication app(argc, argv);
  app.setStyle(QStyleFactory::create("Fusion"));
  HeavyComputationWidget widget;
  widget.show();
  return app.exec();
}
