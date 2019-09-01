using System;
using System.Collections.Generic;
using System.Drawing;

namespace SdCardDataInterpreter.Drawers
{
    public class DrawerSpeedPercent : ADrawer, IDrawer
    {
        Pen _currentPen = new Pen(Color.Brown);
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
            data.ForEach(x => dataValues.Add(x.SpeedPercent));
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
            return "Speed (%)";
        }
        public override string ToString()
        {
            return GetDrawerName();
        }
    }
}
