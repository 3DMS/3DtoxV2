using System;
using System.Collections.Generic;
using System.Drawing;

namespace SdCardDataInterpreter.Drawers
{
    public class DrawerAqMode : ADrawer, IDrawer
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
            long durationInSeconds = (long)(endingTime.TotalSeconds - statingTime.TotalSeconds);
            float yScale = (float)imageHeight / (float)MaxValue;
            float xScale = (float)imageWidth / (float)durationInSeconds;
            InputTxtData previousData = null;
            int previousValue = MinValue;

            int averageGroupSize = data.Count / imageWidth;
            int averageCounterIndex = 0;
            float averageValue = 0;


            for (int i = 0; i < data.Count; i++)
            {
                int currentValue = (int)data[i].AirQualityValue;
                if (averageCounterIndex < averageGroupSize)
                {
                    averageValue += currentValue;
                    averageCounterIndex++;
                    continue;
                }
                else
                {
                    averageValue /= averageGroupSize;
                    averageCounterIndex = 0;
                    currentValue = (int)Math.Round(averageValue,MidpointRounding.AwayFromZero);
                }

                
                if (previousData != null)
                {
                    _currentPen = GetPen(currentValue);
                    Brush currentBrush = new SolidBrush(_currentPen.Color);
                    float x1 = (float)(previousData.Timestamp - statingTime).TotalSeconds * xScale;
                    float x2 = (float)(data[i].Timestamp - statingTime).TotalSeconds * xScale;
                    float width = x2 - x1;
                    float height = imageHeight - (currentValue * yScale);
                    g.DrawRectangle(_currentPen,
                                    x1,
                                    0,
                                    width,
                                    imageHeight);// - height);
                }
                previousData = data[i];
                previousValue = currentValue;
            }
        }

        private Pen GetPen(int CurrentAqMode)
        {
            switch (CurrentAqMode)
            {
                case (int)AirQuality.VERY_GOOD:
                    return new Pen(Color.Blue);
                case (int)AirQuality.GOOD:
                    return new Pen(Color.Green);
                case (int)AirQuality.MODERATE:
                    return new Pen(Color.Yellow);
                case (int)AirQuality.BAD:
                    return new Pen(Color.Orange);
                case (int)AirQuality.UNHEALTHY:
                case (int)AirQuality.VERY_UNHEALTHY:
                case (int)AirQuality.HAZARDOUS:
                default:
                    return new Pen(Color.Red);
            }
        }

        public string GetDrawerName()
        {
            return "Air Quality";
        }

        public override string ToString()
        {
            return GetDrawerName();
        }
    }
}
