using System;
using System.Text.RegularExpressions;

namespace SdCardDataInterpreter
{
    [Flags]
    public enum RunningMode
    {
        AUTO,
        QUIET,
        MANUAL
    };
    [Flags]
    public enum AirQuality
    {
        VERY_GOOD,
        GOOD,
        MODERATE,
        BAD,
        UNHEALTHY,
        VERY_UNHEALTHY,
        HAZARDOUS,
        COMPUTING
    };
    public class InputTxtData
    {
        public TimeSpan Timestamp;
        public RunningMode Mode;
        public int PM1Value;
        public int PM25Value;
        public int PM10Value;
        public int SpeedPercent;
        public int RPM;
        public AirQuality AirQualityValue;
        public int HotEndTemperature;

        private const string SMODE      = "MODE:";
        private const string SPM1       = "PM1:";
        private const string SPM25      = "PM2.5:";
        private const string SPM10      = "PM10:";
        private const string SSPEED     = "SPEED:";
        private const string SRPM       = "RPM:";
        private const string SAQ        = "AQ:";
        private const string STEMP      = "T:";
        private const string extratModeRegex    = @"^.*" + SMODE + "([A-Z]+)";
        private const string extratPM1Regex     = @"^.*" + SPM1 + @"(\d+)";
        private const string extratPM25Regex    = @"^.*" + SPM25 + @"(\d+)";
        private const string extratPM10Regex    = @"^.*" + SPM10 + @"(\d+)";
        private const string extratSpeedRegex   = @"^.*" + SSPEED + @"(\d+)%";
        private const string extratRpmRegex     = @"^.*" + SRPM + @"(\d+)";
        private const string extratAqRegex      = @"^.*" + SAQ + @"([A-Z]+_?[A-Z]+)";
        private const string extratTempRegex    = @"^.*" + STEMP + @"(\d+)";
        private const string extractTime        = @"^(\d+)[J](\d+)[H]:(\d+)[m]:(\d+)[s]";

        public InputTxtData(string InputLineFrom3DtoxDataFile)
        {
            string inputData = InputLineFrom3DtoxDataFile.Trim().Replace(" ", "");
            

            ExtractDateTime(inputData, extractTime);

            ExtractIntFromRegex(inputData, extratPM1Regex, out PM1Value);
            ExtractIntFromRegex(inputData, extratPM25Regex, out PM25Value);
            ExtractIntFromRegex(inputData, extratPM10Regex, out PM10Value);
            ExtractIntFromRegex(inputData, extratSpeedRegex, out SpeedPercent);
            ExtractIntFromRegex(inputData, extratRpmRegex, out RPM);
            ExtractIntFromRegex(inputData, extratTempRegex, out HotEndTemperature);

            string sMode;
            ExtractStringFromRegex(inputData, extratModeRegex, out sMode);
            ConvertStringToType(sMode, out Mode);

            string sAq;
            ExtractStringFromRegex(inputData, extratAqRegex, out sAq);
            ConvertStringToType(sAq, out AirQualityValue);
        }

        private void ExtractIntFromRegex(string inputData, string regex, out int result)
        {
            Match output = Regex.Match(inputData, regex);
            result = 0;
            if (output.Groups.Count > 1)
            {
                int.TryParse(output.Groups[1].Value, out result);
            }
        }
        private void ExtractStringFromRegex(string inputData, string regex, out string result)
        {
            Match output = Regex.Match(inputData, regex);
            result = String.Empty;
            if (output.Groups.Count > 0)
            {
                result = output.Groups[1].Value;
            }
        }
        private void ExtractDateTime(string inputData, string regex)
        {
            Match result = Regex.Match(inputData, regex);
            if (result.Length > 0)
            {
                Match DateData = Regex.Match(result.Value, regex);
                if (DateData.Success && DateData.Groups.Count == 5)
                {
                    int RunningDays, RunningHours, RunningMinutes, RunningSeconds;

                    int.TryParse(DateData.Groups[1].Value, out RunningDays);
                    int.TryParse(DateData.Groups[2].Value, out RunningHours);
                    int.TryParse(DateData.Groups[3].Value, out RunningMinutes);
                    int.TryParse(DateData.Groups[4].Value, out RunningSeconds);
                    Timestamp = new TimeSpan(RunningDays, RunningHours, RunningMinutes,RunningSeconds);
                }
            }
        }

        private void ConvertStringToType<T>(string data, string replaceString, out T result)
        {
            string workString = data.ToUpper().Replace(replaceString, "");
            result = (T)Enum.Parse(typeof(T), workString, true);
        }

        private void ConvertStringToType<T>(string data, out T result)
        {
            result = (T)Enum.Parse(typeof(T), data, true);
        }

    }


}
