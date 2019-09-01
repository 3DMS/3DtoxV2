using System;
using System.Collections.Generic;
using System.Drawing;

namespace SdCardDataInterpreter.Drawers
{
    public class DrawerTemperature : ADrawer, IDrawer
    {
        Pen _currentPen = new Pen(Color.Red);
        public void DrawChart(ref List<InputTxtData> data,
                              ref Graphics g,
                              TimeSpan statingTime,
                              TimeSpan endingTime,
                              int MinValue,
                              int MaxValue,
                              int imageWidth,
                              int imageHeight)
        {
            List<int> dataValues = new List<int>();
            data.ForEach(x => dataValues.Add(x.HotEndTemperature));
            DrawerHelper.DrawLineChart(ref data,
                                 ref dataValues,
                                 ref g, statingTime,
                                 endingTime,
                                 MinValue,
                                 MaxValue,
                                 imageWidth,
                                 imageHeight,
                                 _currentPen);
        }

        public string GetDrawerName()
        {
            return "Temperature";
        }
        public override string ToString()
        {
            return GetDrawerName();
        }
    }
}
