using System;
using System.Collections.Generic;
using System.Drawing;

namespace SdCardDataInterpreter.Drawers
{
    public class DrawerMode : ADrawer, IDrawer
    {
        public void DrawChart(ref List<InputTxtData> data,
                              ref Graphics g,
                              TimeSpan statingTime,
                              TimeSpan endingTime,
                              int MinValue,
                              int MaxValue,
                              int imageWidth,
                              int imageHeight)
        {
            throw new NotImplementedException();
        }

        public string GetDrawerName()
        {
            return "Mode";
        }

        public override string ToString()
        {
            return GetDrawerName();
        }
    }
}
