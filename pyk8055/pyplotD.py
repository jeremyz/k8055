
import sys
from Numeric import *
from PyQt4.Qt import *
from PyQt4.Qwt5 import *
from pyk8055 import k8055

class DataPlot(QwtPlot):

    def __init__(self, *args):
        QwtPlot.__init__(self, *args)
        self.setCanvasBackground(Qt.white)

        # Initialize data
        self.x = arrayrange(0.0, 100.1, 0.5)
        self.d1 = 0.0 + zeros(len(self.x), Float)
        self.d2 = 1.0 + zeros(len(self.x), Float)
        self.d3 = 2.0 + zeros(len(self.x), Float)
        self.d4 = 3.0 + zeros(len(self.x), Float)
        self.d5 = 4.0 + zeros(len(self.x), Float)

        self.setTitle("Simple K8055 datascope")
        self.insertLegend(Qwt.QwtLegend(), Qwt.QwtPlot.BottomLegend);

        self.curve1 = QwtPlotCurve("Input 1")
        self.curve2 = QwtPlotCurve("Input 2")
        self.curve3 = QwtPlotCurve("Input 3")
        self.curve4 = QwtPlotCurve("Input 4")
        self.curve5 = QwtPlotCurve("Input 5")

        self.curve1.attach(self)
        self.curve2.attach(self)
        self.curve3.attach(self)
        self.curve4.attach(self)
        self.curve5.attach(self)

        self.curve1.setPen(QPen(Qt.red))
        self.curve2.setPen(QPen(Qt.blue))
        self.curve3.setPen(QPen(Qt.green))
        self.curve4.setPen(QPen(Qt.black))
        self.curve5.setPen(QPen(Qt.cyan))

        # Make data plot shape square
        self.curve1.setStyle(QwtPlotCurve.Steps)
        self.curve2.setStyle(QwtPlotCurve.Steps)
        self.curve3.setStyle(QwtPlotCurve.Steps)
        self.curve4.setStyle(QwtPlotCurve.Steps)
        self.curve5.setStyle(QwtPlotCurve.Steps)

        # Fixed axis here from 0 to 5
        self.setAxisScale(QwtPlot.yLeft,0,5,1)

        self.setAxisTitle(QwtPlot.xBottom, "Time (seconds)")
        self.setAxisTitle(QwtPlot.yLeft, "Values")

        self.k = k8055(0)
        self.startTimer(50)
    # __init__()

    def timerEvent(self, e):

        # data moves from left to right:
        # shift data array right and assign new value data[0]

        self.d1 = concatenate((self.d1[:1], self.d1[:-1]), 1)
        self.d1[0] = self.k.ReadDigitalChannel(1) * 0.95

        self.d2 = concatenate((self.d2[:1], self.d2[:-1]), 1)
        self.d2[0] = self.k.ReadDigitalChannel(2) * 0.95 + 1

        self.d3 = concatenate((self.d3[:1], self.d3[:-1]), 1)
        self.d3[0] = self.k.ReadDigitalChannel(3) * 0.95 + 2

        self.d4 = concatenate((self.d4[:1], self.d4[:-1]), 1)
        self.d4[0] = self.k.ReadDigitalChannel(4) * 0.95 + 3

        self.d5 = concatenate((self.d5[:1], self.d5[:-1]), 1)
        self.d5[0] = self.k.ReadDigitalChannel(5) * 0.95 + 4

        self.curve1.setData(self.x, self.d1)
        self.curve2.setData(self.x, self.d2)
        self.curve3.setData(self.x, self.d3)
        self.curve4.setData(self.x, self.d4)
        self.curve5.setData(self.x, self.d5)

        self.replot()
    # timerEvent()

# class DataPlot

def make():
    demo = DataPlot()
    demo.resize(500, 300)
    demo.show()
    return demo
# make()

def main(args):
    app = QApplication(args)
    demo = make()
    sys.exit(app.exec_())
# main()

if __name__ == '__main__':
    main(sys.argv)

