using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SdCardDataInterpreter.Drawers
{
    public static class DrawerHelper
    {
        public static void DrawLineChart(ref List<InputTxtData> data,
                                     ref List<int> filteredDataValues,
                                     ref Graphics g,
                                     TimeSpan statingTime,
                                     TimeSpan endingTime,
                                     int MinValue,
                                     int MaxValue,
                                     int imageWidth,
                                     int imageHeight,
                                     Pen currentPen)
        {
            long durationInSeconds = (long)(endingTime.TotalSeconds - statingTime.TotalSeconds);
            float yScale = (float)imageHeight / (float)MaxValue;
            float xScale = (float)imageWidth / (float)durationInSeconds;
            InputTxtData previousData = null;
            int previousValue = MinValue;

            for( int i = 0; i < data.Count; i++)
            {
                if (previousData != null)
                {
                    g.DrawLine(currentPen, (float)(previousData.Timestamp - statingTime).TotalSeconds * xScale,
                                            imageHeight - (previousValue * yScale),
                                            (float)(data[i].Timestamp - statingTime).TotalSeconds * xScale,
                                            imageHeight - (filteredDataValues[i] * yScale));
                }
                previousData = data[i];
                previousValue = filteredDataValues[i];
            }
        }

        public static void DrawAirQualityChart( ref List<InputTxtData> data,
                                             ref List<int> filteredDataValues,
                                             ref Graphics g,
                                             TimeSpan statingTime,
                                             TimeSpan endingTime,
                                             int MinValue,
                                             int MaxValue,
                                             int imageWidth,
                                             int imageHeight,
                                             Pen currentPen)
        {
            
        }
    }
}
