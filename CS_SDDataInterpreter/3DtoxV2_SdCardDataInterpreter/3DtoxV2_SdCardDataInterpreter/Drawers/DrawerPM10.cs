using System;
using System.Collections.Generic;
using System.Drawing;

namespace SdCardDataInterpreter.Drawers
{
    public class DrawerPM10 : ADrawer, IDrawer
    {
        Pen _currentPen = new Pen(Color.DeepSkyBlue);
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
            data.ForEach(x => dataValues.Add(x.PM10Value));
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
            return "PM 10";
        }
        public override string ToString()
        {
            return GetDrawerName();
        }
    }
}
