using System;
using System.Collections.Generic;
using System.Drawing;

namespace SdCardDataInterpreter.Drawers
{
    public interface IDrawer
    {
        string GetDrawerName();
        void DrawChart(ref List<InputTxtData> data,
                       ref Graphics g,
                       TimeSpan statingTime,
                       TimeSpan endingTime,
                       int MinValue,
                       int MaxValue,
                       int imageWidth,
                       int imageHeight);
    }
}
