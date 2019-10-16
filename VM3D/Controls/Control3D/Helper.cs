using System;
using System.Collections.Generic;
using System.Text;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using SysDraw = System.Drawing;

namespace VM3D.Control3D
{
    public enum DrawMode : byte { WholeMap, XSlice, YSlice, AngledSlice }
    public enum DataType : byte { Depth, Time, Average_velocity, Interval_velocity }
    public enum GridType : byte { Horizon, Fault_plane }

    public enum HorizonColoring : byte { RainbowPerHorizon, RainbowAll, RedToGreen, GreenToBlue, BlueToRed, GreyScale, Red, Green, Blue, Purple }
    public enum Colour : byte { Red, Green, Blue, White, Grey, Yellow, Purple }
    public enum BackgroundColour : byte { Black, White, Grey }
    public enum FileType : byte { DAT, PRJ, SurferGRD, BMP }

    public enum WellType : byte { Vertical, Deviated }

    static class Helper
    {
        static Helper()
        {
            Metric = true;
        }

        //values to set Null values
        public static float MaxCoord = 1000000000;
        public static float MaxDepth = 1000000000;
        public static float MaxTime = 1000000000;
        public static float MaxInterval = 50000;
        public static float MinInterval = 500;

        public static int SubSampleWidth = 800;
        public static int SubSampleHeight = 800;

        public static string TimeUnit = "seconds";
        public static string DistanceUnit = "meters";
        public static string VelocityUnit = "meters/second";
        public static string TimeUnitAbr = "(s)";
        public static string DistanceUnitAbr = "(m)";
        public static string VelocityUnitAbr = "(m/s)";
        public static bool Metric { get; private set; }

        public static void SetUnits(bool Metric)
        {
            Helper.Metric = Metric;

            if (Metric)
            {
                DistanceUnit = "meters";
                VelocityUnit = "meters/second";
                DistanceUnitAbr = "(m)";
                VelocityUnitAbr = "(m/s)";
            }
            else
            {
                DistanceUnit = "feet";
                VelocityUnit = "feet/second";
                DistanceUnitAbr = "(ft)";
                VelocityUnitAbr = "(ft/s)";
            }
        }

        /// <summary>
        /// Gets correct unit for datatype
        /// </summary>
        public static string GetUnit(DataType t, bool Abbreviation)
        {
            string s = DistanceUnit;

            switch (t)
            {
                case DataType.Time:
                    s = Abbreviation ? TimeUnitAbr : TimeUnit;
                    break;
                case DataType.Depth:
                    s = Abbreviation ? DistanceUnitAbr : DistanceUnit;
                    break;
                case DataType.Interval_velocity:
                    s = Abbreviation ? VelocityUnitAbr : VelocityUnit;
                    break;
                case DataType.Average_velocity:
                    s = Abbreviation ? VelocityUnitAbr : VelocityUnit;
                    break;
            }

            return s;
        }

        static int nextwell = 0;
        public static Colour GetNextWellColour()
        {
            Array ar = Enum.GetValues(typeof(Colour));

            nextwell++;
            if (nextwell >= ar.Length - 1)
            {
                nextwell = 0;
            }

            return (Colour)ar.GetValue(nextwell);
        }

        public static Color ColorFromEnum(Colour c)
        {
            switch (c)
            {
                case Colour.Red:
                    return Color.Red;
                case Colour.Green:
                    return Color.Green;
                case Colour.Blue:
                    return Color.Blue;
                case Colour.Grey:
                    return Color.Gray;
                case Colour.Yellow:
                    return Color.Yellow;
                case Colour.Purple:
                    return Color.Purple;

                default:
                    return Color.White;
            }
        }

        public static Color ColorFromEnum(BackgroundColour c)
        {
            switch (c)
            {
                case BackgroundColour.Black:
                    return Color.Black;
                case BackgroundColour.Grey:
                    return Color.Gray;
                case BackgroundColour.White:
                    return Color.White;

                default:
                    return Color.Black;
            }
        }

        public static System.Drawing.Color ConvertSystemToXNAColor(Color c)
        {
            return System.Drawing.Color.FromArgb(c.A, c.R, c.G, c.B);
        }

        public static Color GetColour(HorizonColoring C, float value, float min, float max, float transparency)
        {
            switch (C)
            {
                case HorizonColoring.RainbowPerHorizon:
                    return Helper.RainbowColorFromRange(value, min, max, transparency);
                case HorizonColoring.BlueToRed:
                    return Helper.DualColorFromRange(value, min, max, transparency, Color.Blue, Color.Red);
                case HorizonColoring.GreenToBlue:
                    return Helper.DualColorFromRange(value, min, max, transparency, Color.Green, Color.Blue);
                case HorizonColoring.RedToGreen:
                    return Helper.DualColorFromRange(value, min, max, transparency, Color.Red, Color.Green);
                case HorizonColoring.GreyScale:
                    return Helper.DualColorFromRange(value, min, max, transparency, Color.Black, Color.White);
                case HorizonColoring.Red:
                    return Color.Red;
                case HorizonColoring.Green:
                    return Color.Green;
                case HorizonColoring.Blue:
                    return Color.Blue;
                case HorizonColoring.Purple:
                    return Color.Purple;
                default:
                    return Helper.RainbowColorFromRange(value, min, max, transparency);
            }
        }

        const float quart = 0.25f;
        const float half = 0.5f;
        const float threequart = 0.75f;

        public static Color RainbowColorFromRange(float value, float min, float max, float transparency)
        {
            value = (value - min) / (max - min);
            transparency = MathHelper.Clamp(transparency, 0, 1);
            byte transp = (byte)(255 * transparency);

            float c = 0;

            if (value < quart)//high
            {
                c = value / quart;
                return new Color(255, (byte)(c * 255), 0, transp);
            }
            else if (value < half)//middle
            {
                c = (value - quart) / quart;
                return new Color((byte)(255 - (c * 255)), 255, 0, transp);
            }
            else if (value < threequart)//middle
            {
                c = (value - half) / quart;
                return new Color(0, 255, (byte)(c * 255), transp);
            }
            else//low
            {
                c = (value - threequart) / quart;
                return new Color(0, (byte)(255 - (c * 255)), 255, transp);
            }
        }

        public static Color SingleColorFromRange(float value, float min, float max, float transparency, Color col)
        {
            value = (value - min) / (max - min);
            transparency = MathHelper.Clamp(transparency, 0, 1);
            byte trans = (byte)(255 * transparency);

            return new Color((byte)(col.R * value), (byte)(col.G * value), (byte)(col.B * value), trans);
        }

        public static Color DualColorFromRange(float value, float min, float max, float transparency, Color colStart, Color colEnd)
        {
            value = (value - min) / (max - min);
            transparency = MathHelper.Clamp(transparency, 0, 1);
            byte trans = (byte)(255 * transparency);

            byte r, g, b = 0;

            r = (byte)(colStart.R * value);
            g = (byte)(colStart.G * value);
            b = (byte)(colStart.B * value);

            r += (byte)(colEnd.R * (1 - value));
            g += (byte)(colEnd.G * (1 - value));
            b += (byte)(colEnd.B * (1 - value));

            return new Color(r, g, b, trans);
        }

        static DateTime Time1;
        static DateTime Time2;
        public static void StartTimer()
        {
            Time1 = DateTime.Now;
        }

        public static void EndTimer()
        {
            EndTimer("");
        }

        public static void EndTimer(string s)
        {
            Time2 = DateTime.Now;
            TimeSpan d = Time2 - Time1;
            System.Windows.Forms.MessageBox.Show(s + "\r\n\r\n" + d.TotalMilliseconds.ToString());
        }

        /// <summary>
        /// Writes the Horizon data to a bmp file
        /// </summary>
        public static void WriteHorizonToBMP(Horizon h)
        {
            System.Windows.Forms.SaveFileDialog sf = new System.Windows.Forms.SaveFileDialog();

            sf.DefaultExt = "BMP";

            if (sf.ShowDialog() != System.Windows.Forms.DialogResult.OK)
                return;

            try
            {
                // create a Bitmap and get a Graphics
                SysDraw.Bitmap bmp = new SysDraw.Bitmap(h.GridWidth, h.GridHeight);

                for (int x = 0; x < h.GridWidth; x++)
                {
                    for (int y = 0; y < h.GridHeight; y++)
                    {

                        SysDraw.Color c = new System.Drawing.Color();

                        c = SysDraw.Color.FromArgb(255, h.Data[x, y].vertex.Color.R, h.Data[x, y].vertex.Color.G, h.Data[x, y].vertex.Color.B);

                        bmp.SetPixel(x, h.GridHeight - (y + 1), c);
                    }
                }
                bmp.Save(sf.FileName, System.Drawing.Imaging.ImageFormat.Bmp);
                bmp.Dispose();
            }
            catch
            {
                System.Windows.Forms.MessageBox.Show("An error occured in writing the file", "Error", System.Windows.Forms.MessageBoxButtons.OK, System.Windows.Forms.MessageBoxIcon.Error);
            }
        }
    }

    public struct MinMax
    {
        public float Min;
        public float Max;
        public float Average;

        public void Reset()
        {
            Min = float.MaxValue;
            Max = float.MinValue;
            Average = 0;
        }

        public override string ToString()
        {
            return "Min: " + Min.ToString() +
                " Max: " + Max.ToString() +
                " Mean: " + Average.ToString();
        }

        public static bool operator ==(MinMax a, MinMax b)
        {
            return (a.Min == b.Min && a.Max == b.Max && a.Average == b.Average);
        }

        public static bool operator !=(MinMax a, MinMax b)
        {
            return !(a == b);
        }
    }

    public struct VertexPositionNormalColored
    {
        public Vector3 Position;
        public Color Color;
        public Vector3 Normal;

        public static int SizeInBytes = 28;
        public static VertexElement[] VertexElements = new VertexElement[]
             {
                 new VertexElement(0, 0, VertexElementFormat.Vector3, VertexElementMethod.Default, VertexElementUsage.Position, 0),
                 new VertexElement(0, 12, VertexElementFormat.Color, VertexElementMethod.Default, VertexElementUsage.Color, 0),
                 new VertexElement(0, 16, VertexElementFormat.Vector3, VertexElementMethod.Default, VertexElementUsage.Normal, 0),
             };
    }

    abstract class ViewerComponent
    {
        protected ViewerComponent(Viewer MyViewer)
        {
            this.MyViewer = MyViewer;
        }

        [System.ComponentModel.Browsable(false)]
        public Viewer MyViewer { get; private set; }

        public abstract void Initialise();
        public abstract void Reset();
        public abstract void Update(float Time);
        public abstract void Draw();

    }


    [global::System.Serializable]
    public class ViewerException : Exception
    {
        //
        // For guidelines regarding the creation of new exception types, see
        //    http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpgenref/html/cpconerrorraisinghandlingguidelines.asp
        // and
        //    http://msdn.microsoft.com/library/default.asp?url=/library/en-us/dncscol/html/csharp07192001.asp
        //

        public ViewerException() { }
        public ViewerException(string message) : base(message) { }
        public ViewerException(string message, Exception inner) : base(message, inner) { }
        protected ViewerException(
          System.Runtime.Serialization.SerializationInfo info,
          System.Runtime.Serialization.StreamingContext context)
            : base(info, context) { }
    }
}
