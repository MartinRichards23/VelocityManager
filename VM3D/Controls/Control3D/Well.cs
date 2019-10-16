using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.IO;
using System.Text;
using System.Windows.Forms;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;

namespace VM3D.Control3D
{
    sealed class Well : Object3D
    {
        public Well(HorizonWellManager manager)
            : base(manager)
        {
            VertDeclaration = new VertexDeclaration(manager.Device, VertexPositionColor.VertexElements);
            ShowLabels = true;
        }

        public override void Initialise()
        {
            FindMinMaxValues();
            SetDataType(MyManager.CurrentDataType);
            Coloring = Helper.GetNextWellColour();
        }

        public WellPosition[] Data;

        int NumberOfVertices = 0;

        Color coloring = Color.White;
        Colour colouringEnum = Colour.White;
        float labelSize = 1f;

        public MinMax Depth;
        public MinMax Time;
        public MinMax AverageVel;
        public MinMax IntervalVel;

        #region Properties

        [Browsable(false)]
        public bool WithinGridBounds { get; set; }

        [Description("Colors of the Well"), Category("Options")]
        public Colour Coloring
        {
            get { return colouringEnum; }
            set
            {
                colouringEnum = value;
                coloring = Helper.ColorFromEnum(colouringEnum);
                MyTreeNode.BackColor = Helper.ConvertSystemToXNAColor(coloring);
                SetupVertices();
            }
        }

        [Description("Show well names"), Category("Options")]
        public bool ShowLabels { get; set; }

        [Description("Position of first point"), Category("Information")]
        public string SurfacePosition
        {
            get
            {
                string s = "";
                if (Data != null)
                {
                    s = "X: " + Data[0].Value.X.ToString();
                    s += " Y: " + Data[0].Value.Y.ToString();
                }
                return s;
            }
        }

        [Description("Type of well"), Category("Information"), ReadOnly(true)]
        public WellType Type { get; set; }

        [Description("Maximum well depth"), Category("Information")]
        public float MaximumDepth
        {
            get { return Depth.Max; }
        }

        [Description("Maximum well time"), Category("Information")]
        public float MaximumTime
        {
            get { return Time.Max; }
        }

        #endregion

        #region initialise

        /// <summary>
        /// Sets the datatype, choose between time and depth data
        /// </summary>
        public override void SetDataType(DataType t)
        {
            base.SetDataType(t);

            switch (CurrentDataType)
            {
                case DataType.Time:
                    MaxValue.Z = Time.Max;
                    MinValue.Z = Time.Min;

                    for (int i = 0; i < Data.Length; i++)
                    {
                        Data[i].Value.Z = Data[i].Time;
                        Data[i].Null = Data[i].TimeNull;
                    }
                    break;

                case DataType.Depth:
                    MaxValue.Z = Depth.Max;
                    MinValue.Z = Depth.Min;

                    for (int i = 0; i < Data.Length; i++)
                    {
                        Data[i].Value.Z = Data[i].Depth;
                        Data[i].Null = Data[i].DepthNull;
                    }
                    break;

                case DataType.Average_velocity:
                    MaxValue.Z = AverageVel.Max;
                    MinValue.Z = AverageVel.Min;

                    for (int i = 0; i < Data.Length; i++)
                    {
                        Data[i].Value.Z = Data[i].AverageVel;
                        Data[i].Null = Data[i].AverageVelNull;
                    }
                    break;

                case DataType.Interval_velocity:
                    MaxValue.Z = IntervalVel.Max;
                    MinValue.Z = IntervalVel.Min;

                    for (int i = 0; i < Data.Length; i++)
                    {
                        Data[i].Value.Z = Data[i].IntervalVel;
                        Data[i].Null = Data[i].IntervalVelNull;
                    }
                    break;
            }
        }

        /// <summary>
        /// Finds the minimum and maximum time and depth values
        /// </summary>
        protected override void FindMinMaxValues()
        {
            Depth.Reset();
            Time.Reset();
            AverageVel.Reset();
            IntervalVel.Reset();

            MaxValue.X = float.MinValue;
            MaxValue.Y = float.MinValue;
            MinValue.X = float.MaxValue;
            MinValue.Y = float.MaxValue;

            for (int i = 0; i < Data.Length; i++)
            {
                if (WellPosition.ValidCoord(Data[i]))
                {
                    MinValue.X = Math.Min(Data[i].Value.X, MinValue.X);
                    MaxValue.X = Math.Max(Data[i].Value.X, MaxValue.X);
                    MinValue.Y = Math.Min(Data[i].Value.Y, MinValue.Y);
                    MaxValue.Y = Math.Max(Data[i].Value.Y, MaxValue.Y);
                }

                if (!Data[i].DepthNull)
                {
                    Depth.Min = Math.Min(Data[i].Depth, Depth.Min);
                    Depth.Max = Math.Max(Data[i].Depth, Depth.Max);
                }

                if (!Data[i].TimeNull)
                {
                    Time.Min = Math.Min(Data[i].Time, Time.Min);
                    Time.Max = Math.Max(Data[i].Time, Time.Max);
                }

                if (!Data[i].AverageVelNull)
                {
                    AverageVel.Min = Math.Min(Data[i].AverageVel, AverageVel.Min);
                    AverageVel.Max = Math.Max(Data[i].AverageVel, AverageVel.Max);
                }

                if (!Data[i].IntervalVelNull)
                {
                    IntervalVel.Min = Math.Min(Data[i].IntervalVel, IntervalVel.Min);
                    IntervalVel.Max = Math.Max(Data[i].IntervalVel, IntervalVel.Max);
                }
            }

        }

        public override void SetupScaledValues()
        {
            for (int i = 0; i < Data.Length; i++)
            {
                if (Data[i].Null)
                    continue;

                Data[i].ValueScaled.X = (Data[i].Value.X - MyManager.MinValues.X) * MyManager.XYScale;
                Data[i].ValueScaled.Y = (Data[i].Value.Y - MyManager.MinValues.Y) * MyManager.XYScale;
                Data[i].ValueScaled.Z = (Data[i].Value.Z - MyManager.MinValues.Z) * MyManager.ZScale;
            }
        }

        bool hasWellToDraw = false;
        public override void SetupVertices()
        {
            List<VertexPositionColor> VertexList = new List<VertexPositionColor>();
            NumberOfVertices = 0;

            for (int x = 0; x < Data.Length; x++)
            {
                if (Data[x].Null)
                    continue;

                VertexPositionColor v = new VertexPositionColor();
                v.Position = Data[x].ValueScaled;
                v.Color = coloring;

                VertexList.Add(v);
                NumberOfVertices++;
            }

            if (VertexList.Count < 2)
            {
                hasWellToDraw = false;
            }
            else
            {
                hasWellToDraw = true;

                if (vb != null)
                    vb.Dispose();
                if (ib != null)
                    ib.Dispose();

                //set vertices
                vb = new VertexBuffer(device, VertexPositionColor.SizeInBytes * NumberOfVertices, BufferUsage.WriteOnly);
                vb.SetData(VertexList.ToArray());

                //set up indices
                int[] indices = new int[NumberOfVertices];
                for (int x = 0; x < indices.Length; x++)
                {
                    indices[x] = x;
                }

                ib = new IndexBuffer(device, typeof(int), indices.Length, BufferUsage.WriteOnly);
                ib.SetData(indices);
            }
        }

        public override void Reset()
        {
            Visible = WithinGridBounds;
        }

        #endregion

        public override void Draw()
        {
            if (!Visible)
                return;

            if (!hasWellToDraw)
                return;

            // don't show some data types for wells
            // if (CurrentDataType == DataType.Interval_velocity)
            //     return;

            MyManager.EffectLine.Begin();
            MyManager.EffectLine.CurrentTechnique.Passes[0].Begin();

            device.Vertices[0].SetSource(vb, 0, VertexPositionColor.SizeInBytes);
            device.Indices = ib;
            device.VertexDeclaration = VertDeclaration;

            device.DrawIndexedPrimitives(PrimitiveType.LineStrip, 0, 0, NumberOfVertices, 0, NumberOfVertices - 1);

            MyManager.EffectLine.CurrentTechnique.Passes[0].End();
            MyManager.EffectLine.End();

            if (ShowLabels)
            {
                Vector3 v = Data[0].ValueScaled;
                v = MyManager.Device.Viewport.Project(v, MyManager.EffectLine.Projection, MyManager.EffectLine.View, MyManager.EffectLine.World);

                //draw if in front of camera
                if (v.Z < 1)
                {
                    MyManager.MyViewer.spriteBatch.Begin(SpriteBlendMode.AlphaBlend, SpriteSortMode.FrontToBack, SaveStateMode.SaveState);
                    MyManager.MyViewer.spriteBatch.DrawString(MyManager.MyViewer.spriteFontCourierF10, Name, new Vector2(v.X, v.Y), coloring, 0, Vector2.Zero, labelSize, SpriteEffects.None, 1);
                    MyManager.MyViewer.spriteBatch.End();
                }
            }
        }

    }

    class WellPosition
    {
        //xy coords and z height value
        public Vector3 Value;
        public bool Null;

        //scaled units 
        public Vector3 ValueScaled;

        Well owner;

        public WellPosition(Well owner)
        {
            Owner = owner;
        }

        [Browsable(false)]
        public Well Owner
        {
            get { return owner; }
            private set { owner = value; }
        }

        public float Depth { get; set; }
        public float Time { get; set; }
        public float AverageVel { get; set; }
        public float IntervalVel { get; set; }

        [Browsable(false)]
        public bool DepthNull
        {
            get { return float.IsNaN(Depth); }
        }

        [Browsable(false)]
        public bool TimeNull
        {
            get { return float.IsNaN(Time); }
        }

        [Browsable(false)]
        public bool AverageVelNull
        {
            get { return float.IsNaN(AverageVel); }
        }

        [Browsable(false)]
        public bool IntervalVelNull
        {
            get { return float.IsNaN(IntervalVel); }
        }

        public override string ToString()
        {
            string s = "";

            s += "Time: " + (TimeNull ? "Null" : Time.ToString());
            s += " Depth: " + (DepthNull ? "Null" : Depth.ToString());

            return s;
        }

        public static bool ValidCoord(WellPosition wp)
        {
            if (wp.Value.X < Helper.MaxCoord && wp.Value.Y < Helper.MaxCoord)
                return true;
            else
                return false;
        }
    }
}
