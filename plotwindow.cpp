/***************************************************************************
**                                                                        **
**  QCustomPlot, an easy to use, modern plotting widget for Qt            **
**  Copyright (C) 2011-2016 Emanuel Eichhammer                            **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see http://www.gnu.org/licenses/.   **
**                                                                        **
****************************************************************************
**           Author: Emanuel Eichhammer                                   **
**  Website/Contact: http://www.qcustomplot.com/                          **
**             Date: 13.09.16                                             **
**          Version: 2.0.0-beta                                           **
****************************************************************************/

#include "mainwindow.h"
#include "ui_mainwindow.h"

PlotWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
 p_ui(new Ui::MainWindow)
{
 p_ui->setupUi(this);
  setupPlot();
  
  // configure scroll bars:
  // Since scroll bars only support integer values, we'll set a high default range of -500..500 and
  // divide scroll bar position values by 100 to provide a scroll range -5..5 in floating point
  // axis coordinates. if you want to dynamically grow the range accessible with the scroll bar,
  // just increase the the minimum/maximum values of the scroll bars as needed.
 p_ui->horizontalScrollBar->setRange(-500, 500);
 p_ui->verticalScrollBar->setRange(-500, 500);
  
  // create connection between axes and scroll bars:
  connect(p_ui->horizontalScrollBar, SIGNAL(valueChanged(int)), this, SLOT(horzScrollBarChanged(int)));
  connect(p_ui->verticalScrollBar, SIGNAL(valueChanged(int)), this, SLOT(vertScrollBarChanged(int)));
  connect(p_ui->plot->xAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(xAxisChanged(QCPRange)));
  connect(p_ui->plot->yAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(yAxisChanged(QCPRange)));
  
  // initialize axis range (and scroll bar positions via signals we just connected):
  p_uiui->plot->xAxis->setRange(0, 6, Qt::AlignCenter);
  p_ui->plot->yAxis->setRange(0, 10, Qt::AlignCenter);
}

PlotWindow::~MainWindow()
{
  delete p_ui;
}

void MainWindow::setupPlot()
{
  // The following plot setup is mostly taken from the plot demos:
  m_ui->plot->addGraph();
  m_ui->plot->graph()->setPen(QPen(Qt::blue));
  m_ui->plot->graph()->setBrush(QBrush(QColor(0, 0, 255, 20)));
 m_ui->plot->addGraph();
  m_ui->plot->graph()->setPen(QPen(Qt::red));
  QVector<double> x(500), y0(500), y1(500);
  for (int i=0; i<500; ++i)
  {
    x[i] = (i/499.0-0.5)*10;
    y0[i] = qExp(-x[i]*x[i]*0.25)*qSin(x[i]*5)*5;
    y1[i] = qExp(-x[i]*x[i]*0.25)*5;
  }
  m_ui->plot->graph(0)->setData(x, y0);
  m_ui->plot->graph(1)->setData(x, y1);
  m_ui->plot->axisRect()->setupFullAxesBox(true);
  m_ui->plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
}

void MainWindow::horzScrollBarChanged(int value)
{
  if (qAbs(ui->plot->xAxis->range().center()-value/100.0) > 0.01) // if user is dragging plot, we don't want to replot twice
  {
    m_ui->plot->xAxis->setRange(value/100.0, ui->plot->xAxis->range().size(), Qt::AlignCenter);
    m_ui->plot->replot();
  }
}

void MainWindow::vertScrollBarChanged(int value)
{
  if (qAbs(p_ui->plot->yAxis->range().center()+value/100.0) > 0.01) // if user is dragging plot, we don't want to replot twice
  {
    m_ui->plot->yAxis->setRange(-value/100.0, ui->plot->yAxis->range().size(), Qt::AlignCenter);
    m_ui->plot->replot();
  }
}

void MainWindow::xAxisChanged(QCPRange range)
{
  m_ui->horizontalScrollBar->setValue(qRound(range.center()*100.0)); // adjust position of scroll bar slider
  m_ui->horizontalScrollBar->setPageStep(qRound(range.size()*100.0)); // adjust size of scroll bar slider
}

void MainWindow::yAxisChanged(QCPRange range)
{
 m_ui->verticalScrollBar->setValue(qRound(-range.center()*100.0)); // adjust position of scroll bar slider
  m_ui->verticalScrollBar->setPageStep(qRound(range.size()*100.0)); // adjust size of scroll bar slider
}






