using System;
using System.Collections.Generic;
using System.Drawing;

namespace SdCardDataInterpreter.Drawers
{
    public class DrawerPM25 : ADrawer, IDrawer
    {
        Pen _currentPen = new Pen(Color.DarkBlue);
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
            data.ForEach(x => dataValues.Add(x.PM25Value));
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
            return "PM 2.5";
        }
        public override string ToString()
        {
            return GetDrawerName();
        }
    }
}
