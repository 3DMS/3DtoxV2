using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO;
using System.Windows.Forms;
using System.Linq;

namespace SdCardDataInterpreter
{
    public partial class MainForm : Form
    {
        private List<InputTxtData> _inputData = new List<InputTxtData>();
        private int _maxPm1Value    = 0;
        private int _maxPm25Value   = 0;
        private int _maxPm10Value   = 0;
        private int _maxTemperature = 0;

        private Pen BlackPen = new Pen(Brushes.Black, 1);
        public MainForm()
        {
            InitializeComponent();
        }

        void UpdateMaxValues(InputTxtData data)
        {
            _maxPm1Value    = data.PM1Value > _maxPm1Value? data.PM1Value: _maxPm1Value;
            _maxPm25Value   = data.PM25Value > _maxPm25Value ? data.PM25Value : _maxPm25Value;
            _maxPm10Value   = data.PM10Value > _maxPm10Value ? data.PM10Value : _maxPm10Value;
            _maxTemperature = data.HotEndTemperature > _maxTemperature ? data.HotEndTemperature : _maxTemperature;
        }

        private void button1_Click(object sender, EventArgs e)
        {
            openFileDialog1.ShowDialog();
            string InputFilename = openFileDialog1.FileName;
            if (File.Exists(InputFilename) == false)
            {
                MessageBox.Show($"Input Filename {InputFilename} doesn't exist", "Error when loading input Data file", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
            _inputData.Clear();
            
            int CurrentProgressValue = 0;
            int PreviousProgressValue = -1;
            using (StreamReader fileStreamReader = new StreamReader(InputFilename))
            {
                while (fileStreamReader.EndOfStream == false)
                {
                    InputTxtData data = new InputTxtData(fileStreamReader.ReadLine());
                    UpdateMaxValues(data);
                    _inputData.Add(data);

                    CurrentProgressValue = (int)(fileStreamReader.BaseStream.Position * 100 / fileStreamReader.BaseStream.Length);
                    if (CurrentProgressValue > PreviousProgressValue)
                    {
                        PreviousProgressValue = CurrentProgressValue;
                        progressBar.Value = CurrentProgressValue;
                        progressBar.Refresh();
                    }
                }
            }
            UpdateCharts();
            UpdateGraphicsOptionList();

            //InputTxtData data = new InputTxtData(@"0001J 22H:45m:26s|MODE:AUTO     |PM1:  3| PM2.5:  4| PM10:  4| SPEED: 70%| RPM:11513| AQ:      MODERATE|T:185");

        }

        private Dictionary<int, Drawers.IDrawer> OptionListMap = new Dictionary<int, Drawers.IDrawer>()
        {
            {0, new Drawers.DrawerPM1()},
            {1, new Drawers.DrawerPM25()},
            {2, new Drawers.DrawerPM10()},
            {3, new Drawers.DrawerSpeedPercent()},
            {4, new Drawers.DrawerSpeedRpm() },
            {5, new Drawers.DrawerTemperature() },
            {6, new Drawers.DrawerAqMode() },
            {7, new Drawers.DrawerMode()}
        };


        private void UpdateGraphicsOptionList()
        {
            checkedListBox1.Items.Clear();
            for (int i = 0; i < OptionListMap.Count; i++)
            {
                Drawers.IDrawer Drawer;
                OptionListMap.TryGetValue(i, out Drawer);
                checkedListBox1.Items.Add(Drawer);
            }
            checkedListBox1.Visible = true;
        }

        private void UpdateCharts()
        {
            Bitmap image = new Bitmap(pictureBox1.Width, pictureBox1.Height);
            Graphics g = Graphics.FromImage(image);
            g.Clear(Color.White);
            g.PageUnit = GraphicsUnit.Pixel;

            DrawGrid(g, BlackPen, pictureBox1.Width, pictureBox1.Height);

            g.Dispose();
            this.pictureBox1.Image = image;
            this.pictureBox1.Refresh();
        }

        private void DrawGrid(Graphics g,Pen pen ,int width, int height)
        {
            int TotalLines = 10;
            int insetInPixel = 1;
            for (int i = 0; i <= TotalLines; i++)
            {
                //horizontal lines
                float X1 = insetInPixel;
                float X2 = width - insetInPixel;
                float Y1 = (i * (height - 2 * insetInPixel) / TotalLines) + insetInPixel;
                float Y2 = Y1;
                g.DrawLine(pen, X1, Y1, X2, Y2);
                //vertical lines
                X1 = (i * (width - 2 * insetInPixel) / TotalLines) + insetInPixel; ;
                X2 = X1;
                Y1 = insetInPixel;
                Y2 = height - insetInPixel;

                g.DrawLine(pen, X1, Y1, X2, Y2);
            }
        }

        private void checkedListBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
            for (int i = 0; i < checkedListBox1.Items.Count; i++)
            {
                if (i == checkedListBox1.SelectedIndex)
                {
                    checkedListBox1.SetItemChecked(i, true);
                }
                else
                {
                    checkedListBox1.SetItemChecked(i, false);
                }
            }
            RefreshChart();
        }

        private int GetMaxValueBasedOnDrawerType(Drawers.IDrawer drawer)
        {
            if (drawer is Drawers.DrawerPM1)
            {
                return _maxPm1Value;
            }
            else if (drawer is Drawers.DrawerPM10)
            {
                return _maxPm10Value;
            }
            else if (drawer is Drawers.DrawerPM25)
            {
                return _maxPm25Value;
            }
            else if (drawer is Drawers.DrawerTemperature)
            {
                return _maxTemperature;
            }
            else if (drawer is Drawers.DrawerSpeedPercent)
            {
                return 100;
            }
            else if (drawer is Drawers.DrawerMode)
            {
                return 4;
            }
            else if (drawer is Drawers.DrawerAqMode)
            {
                return 6;
            }
            else if (drawer is Drawers.DrawerSpeedRpm)
            {
                return 20000;
            }
            else
            {
                return 0;
            }
        }

        private void RefreshChart()
        {
            if (_inputData.Count == 0) // no data to draw
            {
                MessageBox.Show("No input data to display", "Error when drawing data", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            var selectedDrawers = checkedListBox1.CheckedItems;
            Bitmap image = new Bitmap(pictureBox1.Width, pictureBox1.Height);
            Graphics g = Graphics.FromImage(image);
            g.Clear(Color.White);
            DrawGrid(g, BlackPen, pictureBox1.Width, pictureBox1.Height);
            for (int i = 0; i < selectedDrawers.Count; i++)
            {
                Drawers.IDrawer drawer = (Drawers.IDrawer)selectedDrawers[i];
                progressBar.Value = (int)(i * 100 / selectedDrawers.Count);
                try
                {
                    drawer.DrawChart(ref _inputData,
                                     ref g,
                                     _inputData[0].Timestamp,
                                     _inputData[_inputData.Count - 1].Timestamp,
                                     0,
                                     GetMaxValueBasedOnDrawerType(drawer),
                                     pictureBox1.Width,
                                     pictureBox1.Height);
                }
                catch (Exception exception)
                {
                    Console.WriteLine($"Error:{exception.Message}");
                }
                progressBar.Update();
            }
            progressBar.Value = 100;
            pictureBox1.Image = image;
            pictureBox1.Refresh();
            g.Dispose();
        }

        private void refreshBtn_Click(object sender, EventArgs e)
        {
            RefreshChart();
        }
    }
}
