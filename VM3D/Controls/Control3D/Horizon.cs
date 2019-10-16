using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.IO;
using System.Text;
using System.Windows.Forms;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using Drawing = System.Drawing;

namespace VM3D.Control3D
{
    sealed class Horizon : Object3D, IComparable<Horizon>
    {
        public Horizon(HorizonWellManager Manager)
            : base(Manager)
        {
            VertDeclaration = new VertexDeclaration(Manager.Device, VertexPositionNormalColored.VertexElements);
            ShowIntervalData = true;
        }

        public override void Initialise()
        {
            FindMinMaxValues();
            SetDataType(MyManager.CurrentDataType);
        }

        //main fields
        public int Identity;

        [Browsable(false)]
        public GridType ThisGridType { get; private set; }

        public int GridWidth = 0;
        public int GridHeight = 0;
        public HorizonPosition[,] Data;
        public Vector2 XYIncrement = Vector2.Zero;

        public MinMax Depth;
        public MinMax Time;
        public MinMax IntervalVel;
        public MinMax AverageVel;

        public bool HasDepthData = false;
        public bool HasTimeData = false;
        public bool HasIntervalVelData = false;
        public bool HasAverageVelData = false;

        int NumberOfGridTriangles = 0;

        //slice fields
        int NumberOfSliceVertices = 0;
        VertexBuffer vbSlice;
        IndexBuffer ibSlice;

        //interval fields
        int NumberOfIntervalTriangles = 0;
        VertexBuffer vbInterval;
        public Horizon AboveHorizon;
        //list of xy points to slice through
        List<Point> SlicePoints = new List<Point>();

        #region properties for the property grid

        float transparency = 1f;
        [Description("1 is opaque, 0 is fully transparent"), Category("Options")]
        public float Transparency
        {
            get { return transparency; }
            set
            {
                if (transparency != value)
                {
                    transparency = MathHelper.Clamp(value, 0, 1);
                    CalculateColors();
                    SetupVertices();
                }
            }
        }

        [Description("Sets whether interval data is visible when viewing slice"), Category("Options")]
        public bool ShowIntervalData { get; set; }

        HorizonColoring coloring = HorizonColoring.RainbowAll;
        [Description("Colors of the Horizon"), Category("Options")]
        public HorizonColoring Coloring
        {
            get { return coloring; }
            set
            {
                if (coloring != value)
                {
                    coloring = value;
                    CalculateColors();
                    SetupVertices();

                    MyManager.ColorsChanged();
                }
            }
        }

        [Description("Horizon maximum, minimum and mean depth"), Category("Information")]
        public string DepthStatistics
        {
            get
            {
                if (HasDepthData)
                    return Depth.ToString();
                else
                    return "No depth data";
            }
        }

        [Description("Horizon maximum, minimum and mean time"), Category("Information")]
        public string TimeStatistics
        {
            get
            {
                if (HasTimeData)
                    return Time.ToString();
                else
                    return "No time data";
            }
        }

        [Description("Horizon maximum, minimum and mean interval velocity"), Category("Information")]
        public string IntervalVelocityStatistics
        {
            get
            {
                if (HasIntervalVelData)
                    return IntervalVel.ToString();
                else
                    return "No interval velocity data";
            }
        }

        [Description("Horizon maximum, minimum and mean Average velocity"), Category("Information")]
        public string AverageVelocityStatistics
        {
            get
            {
                if (HasAverageVelData)
                    return AverageVel.ToString();
                else
                    return "No average velocity data";
            }
        }

        #endregion

        #region Initialisation

        public void SetGridType(GridType T)
        {
            ThisGridType = T;
            if (ThisGridType == GridType.Fault_plane)
            {
                Coloring = HorizonColoring.Purple;
            }
        }

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

                    for (int x = 0; x < GridWidth; x++)
                    {
                        for (int y = 0; y < GridHeight; y++)
                        {
                            Data[x, y].Value.Z = Data[x, y].Time;
                            Data[x, y].Null = Data[x, y].TimeNull;
                        }
                    }
                    break;
                case DataType.Depth:
                    MaxValue.Z = Depth.Max;
                    MinValue.Z = Depth.Min;

                    for (int x = 0; x < GridWidth; x++)
                    {
                        for (int y = 0; y < GridHeight; y++)
                        {
                            Data[x, y].Value.Z = Data[x, y].Depth;
                            Data[x, y].Null = Data[x, y].DepthNull;
                        }
                    }
                    break;
                case DataType.Interval_velocity:
                    MaxValue.Z = IntervalVel.Max;
                    MinValue.Z = IntervalVel.Min;

                    for (int x = 0; x < GridWidth; x++)
                    {
                        for (int y = 0; y < GridHeight; y++)
                        {
                            Data[x, y].Value.Z = Data[x, y].IntervalVelocity;
                            Data[x, y].Null = Data[x, y].IntervalVelNull;
                        }
                    }
                    break;
                case DataType.Average_velocity:
                    MaxValue.Z = AverageVel.Max;
                    MinValue.Z = AverageVel.Min;

                    for (int x = 0; x < GridWidth; x++)
                    {
                        for (int y = 0; y < GridHeight; y++)
                        {
                            Data[x, y].Value.Z = Data[x, y].AverageVelocity;
                            Data[x, y].Null = Data[x, y].AverageVelNull;
                        }
                    }
                    break;
            }
        }

        int NumberOfDepthValues = 0;
        int NumberOfTimeValues = 0;
        int NumberOfIntervalValues = 0;
        int NumberOfAverageVelValues = 0;

        /// <summary>
        /// Finds the minimum and maximum time and depth values
        /// </summary>
        protected override void FindMinMaxValues()
        {
            Depth.Reset();
            Time.Reset();
            IntervalVel.Reset();
            AverageVel.Reset();

            MinValue.X = float.MaxValue;
            MinValue.Y = float.MaxValue;
            MaxValue.X = float.MinValue;
            MaxValue.Y = float.MinValue;

            NumberOfDepthValues = 0;
            NumberOfTimeValues = 0;
            NumberOfIntervalValues = 0;
            NumberOfAverageVelValues = 0;

            double depth = 0;
            double time = 0;
            double interval = 0;
            double averagevel = 0;

            for (int x = 0; x < GridWidth; x++)
            {
                for (int y = 0; y < GridHeight; y++)
                {
                    //set min max x and y values of grid
                    MinValue.X = Math.Min(Data[x, y].Value.X, MinValue.X);
                    MinValue.Y = Math.Min(Data[x, y].Value.Y, MinValue.Y);
                    MaxValue.X = Math.Max(Data[x, y].Value.X, MaxValue.X);
                    MaxValue.Y = Math.Max(Data[x, y].Value.Y, MaxValue.Y);

                    if (!Data[x, y].DepthNull)
                    {
                        Depth.Min = Math.Min(Data[x, y].Depth, Depth.Min);
                        Depth.Max = Math.Max(Data[x, y].Depth, Depth.Max);

                        depth += (float)Data[x, y].Depth;
                        NumberOfDepthValues++;
                    }

                    if (!Data[x, y].TimeNull)
                    {
                        Time.Min = Math.Min(Data[x, y].Time, Time.Min);
                        Time.Max = Math.Max(Data[x, y].Time, Time.Max);

                        time += Data[x, y].Time;
                        NumberOfTimeValues++;
                    }

                    if (!Data[x, y].IntervalVelNull)
                    {
                        IntervalVel.Min = Math.Min(Data[x, y].IntervalVelocity, IntervalVel.Min);
                        IntervalVel.Max = Math.Max(Data[x, y].IntervalVelocity, IntervalVel.Max);

                        interval += Data[x, y].IntervalVelocity;
                        NumberOfIntervalValues++;
                    }

                    if (!Data[x, y].AverageVelNull)
                    {
                        AverageVel.Min = Math.Min(Data[x, y].AverageVelocity, AverageVel.Min);
                        AverageVel.Max = Math.Max(Data[x, y].AverageVelocity, AverageVel.Max);

                        averagevel += Data[x, y].AverageVelocity;
                        NumberOfAverageVelValues++;
                    }

                }
            }

            depth /= (double)NumberOfDepthValues;
            time /= (double)NumberOfTimeValues;
            interval /= (double)NumberOfIntervalValues;
            averagevel /= (double)NumberOfAverageVelValues;

            Depth.Average = (float)depth;
            Time.Average = (float)time;
            IntervalVel.Average = (float)interval;
            AverageVel.Average = (float)averagevel;
        }

        public void CalculateColors()
        {
            //set colors
            for (int x = 0; x < GridWidth; x++)
            {
                for (int y = 0; y < GridHeight; y++)
                {
                    //ignore nulls
                    if (Data[x, y].Null)
                        continue;

                    SetColour(ref Data[x, y]);
                }
            }
        }

        public void CalculateIntervalColors()
        {
            //set colors
            for (int x = 0; x < GridWidth; x++)
            {
                for (int y = 0; y < GridHeight; y++)
                {
                    if (Data[x, y].Null)
                        continue;

                    Data[x, y].IntervalColour = Helper.RainbowColorFromRange(Data[x, y].IntervalVelocity, MyManager.IntervalVelcocity.Min, MyManager.IntervalVelcocity.Max, 1);
                }
            }
        }

        private void SetColour(ref HorizonPosition hp)
        {
            switch (coloring)
            {
                case HorizonColoring.RainbowAll:
                    hp.vertex.Color = Helper.RainbowColorFromRange(hp.Value.Z, MyManager.MinHorizonValues.Z, MyManager.MaxHorizonValues.Z, Transparency);
                    break;
                case HorizonColoring.RainbowPerHorizon:
                    hp.vertex.Color = Helper.RainbowColorFromRange(hp.Value.Z, MinValue.Z, MaxValue.Z, Transparency);
                    break;
                case HorizonColoring.BlueToRed:
                    hp.vertex.Color = Helper.DualColorFromRange(hp.Value.Z, MinValue.Z, MaxValue.Z, Transparency, Color.Blue, Color.Red);
                    break;
                case HorizonColoring.GreenToBlue:
                    hp.vertex.Color = Helper.DualColorFromRange(hp.Value.Z, MinValue.Z, MaxValue.Z, Transparency, Color.Green, Color.Blue);
                    break;
                case HorizonColoring.RedToGreen:
                    hp.vertex.Color = Helper.DualColorFromRange(hp.Value.Z, MinValue.Z, MaxValue.Z, Transparency, Color.Red, Color.Green);
                    break;
                case HorizonColoring.GreyScale:
                    hp.vertex.Color = Helper.DualColorFromRange(hp.Value.Z, MinValue.Z, MaxValue.Z, Transparency, Color.Black, Color.White);
                    break;
                case HorizonColoring.Red:
                    hp.vertex.Color = Color.Red;
                    break;
                case HorizonColoring.Green:
                    hp.vertex.Color = Color.Green;
                    break;
                case HorizonColoring.Blue:
                    hp.vertex.Color = Color.Blue;
                    break;
                case HorizonColoring.Purple:
                    hp.vertex.Color = Color.Purple;
                    break;
                default:
                    hp.vertex.Color = Helper.RainbowColorFromRange(hp.Value.Z, MinValue.Z, MaxValue.Z, Transparency);
                    break;
            }
        }

        public override void SetupScaledValues()
        {
            for (int x = 0; x < GridWidth; x++)
            {
                for (int y = 0; y < GridHeight; y++)
                {
                    //don't bother with nulls
                    if (Data[x, y].Null)
                        continue;

                    //calculate scaled value
                    Data[x, y].vertex.Position.X = (Data[x, y].Value.X - MyManager.MinValues.X) * MyManager.XYScale;
                    Data[x, y].vertex.Position.Y = (Data[x, y].Value.Y - MyManager.MinValues.Y) * MyManager.XYScale;
                    Data[x, y].vertex.Position.Z = (Data[x, y].Value.Z - MyManager.MinValues.Z) * MyManager.ZScale;
                }
            }

            float x1, x2, y1, y2;
            //set normals
            for (int x = 1; x < GridWidth - 1; x++)
            {
                for (int y = 1; y < GridHeight - 1; y++)
                {
                    //ignore nulls
                    if (Data[x, y].Null)
                        continue;

                    //if value beside this is null, then set it to be same as this
                    if (Data[x - 1, y].Null)
                        x1 = Data[x, y].vertex.Position.Z;
                    else
                        x1 = Data[x - 1, y].vertex.Position.Z;

                    if (Data[x + 1, y].Null)
                        x2 = Data[x, y].vertex.Position.Z;
                    else
                        x2 = Data[x + 1, y].vertex.Position.Z;

                    if (Data[x, y - 1].Null)
                        y1 = Data[x, y].vertex.Position.Z;
                    else
                        y1 = Data[x, y - 1].vertex.Position.Z;

                    if (Data[x, y + 1].Null)
                        y2 = Data[x, y].vertex.Position.Z;
                    else
                        y2 = Data[x, y + 1].vertex.Position.Z;

                    //create normal vector
                    Data[x, y].vertex.Normal = new Vector3((x1 - x2) / 2, (y1 - y2) / 2, 1);
                    Data[x, y].vertex.Normal.Normalize();
                }
            }

        }

        bool hasHorizonToDraw = false;
        public override void SetupVertices()
        {
            List<VertexPositionNormalColored> VertexList = new List<VertexPositionNormalColored>(NumberOfGridTriangles / 2);
            List<int> IndicesList = new List<int>(NumberOfGridTriangles * 3);

            NumberOfGridTriangles = 0;

            int xMin = 0;
            int xMax = GridWidth;
            int yMin = 0;
            int yMax = GridHeight;

            //set index for each point
            int index = 0;
            for (int x = 0; x < GridWidth; x++)
            {
                for (int y = 0; y < GridHeight; y++)
                {
                    if (!Data[x, y].Null)
                    {
                        Data[x, y].Index = index;
                        VertexList.Add(Data[x, y].vertex);

                        index++;
                    }
                }
            }

            for (int x = xMin + 1; x < xMax; x++)
            {
                for (int y = yMin + 1; y < yMax; y++)
                {
                    if (Data[x, y].Null)
                        continue;

                    if (Data[x - 1, y - 1].Null)
                        continue;

                    if (!Data[x - 1, y].Null)
                    {
                        //triangle 1
                        IndicesList.Add(Data[x, y].Index);
                        IndicesList.Add(Data[x - 1, y].Index);
                        IndicesList.Add(Data[x - 1, y - 1].Index);
                        NumberOfGridTriangles++;
                    }

                    if (!Data[x, y - 1].Null)
                    {
                        //triangle 2
                        IndicesList.Add(Data[x, y].Index);
                        IndicesList.Add(Data[x, y - 1].Index);
                        IndicesList.Add(Data[x - 1, y - 1].Index);
                        NumberOfGridTriangles++;
                    }
                }
            }

            if (NumberOfGridTriangles < 1)
            {
                hasHorizonToDraw = false;
            }
            else
            {
                hasHorizonToDraw = true;

                if (vb != null)
                    vb.Dispose();
                if (ib != null)
                    ib.Dispose();

                //set vertices
                vb = new VertexBuffer(device, VertexPositionNormalColored.SizeInBytes * VertexList.Count, BufferUsage.WriteOnly);
                vb.SetData(VertexList.ToArray());
                
                //set indices
                ib = new IndexBuffer(device, typeof(int), IndicesList.Count, BufferUsage.WriteOnly);
                ib.SetData(IndicesList.ToArray());
            }
        }

        public void SetXSlicePoints()
        {
            SlicePoints.Clear();
            Point p;

            int xSlicePosition = (int)MathHelper.Clamp(MyManager.XSlicePosition, 0, GridWidth - 1);

            for (int x = 0; x < GridHeight; x++)
            {
                p = new Point(xSlicePosition, x);
                SlicePoints.Add(p);
            }

            SetupSliceLineVertices();
            SetupIntervalVertices();
        }

        public void SetYSlicePoints()
        {
            SlicePoints.Clear();
            Point p;

            int ySlicePosition = (int)MathHelper.Clamp(MyManager.YSlicePosition, 0, GridHeight - 1);

            for (int y = 0; y < GridWidth; y++)
            {
                p = new Point(y, ySlicePosition);
                SlicePoints.Add(p);
            }

            SetupSliceLineVertices();
            SetupIntervalVertices();
        }

        public void SetAngledSlicePoints()
        {
            SlicePoints.Clear();

            HorizonPosition[] points = MyManager.AngledSlicePoints;

            for (int j = 0; j < points.Length - 1; j++)
            {
                Vector2 aPos = new Vector2(points[j].GridPosition.X, points[j].GridPosition.Y);
                Vector2 bPos = new Vector2(points[j + 1].GridPosition.X, points[j + 1].GridPosition.Y);

                Vector2 direction = bPos - aPos;
                direction.Normalize();

                float distance = Vector2.Distance(aPos, bPos);

                for (int i = 0; i < distance; i++)
                {
                    Vector2 cPos = new Vector2(aPos.X, aPos.Y);

                    cPos += (direction * i);

                    Point p = new Point((int)cPos.X, (int)cPos.Y);

                    SlicePoints.Add(p);
                }
            }

            SetupSliceLineVertices();
            SetupIntervalVertices();
        }

        bool hasSliceToDraw = false;
        private void SetupSliceLineVertices()
        {
            List<VertexPositionColor> VertexList = new List<VertexPositionColor>();
            HorizonPosition hp;
            VertexPositionColor v;
            NumberOfSliceVertices = 0;

            foreach (Point p in SlicePoints)
            {
                hp = Data[p.X, p.Y];

                if (hp.Null == true)
                    continue;

                v = new VertexPositionColor();
                v.Position = hp.vertex.Position;
                v.Color = hp.vertex.Color;
                VertexList.Add(v);
                NumberOfSliceVertices++;
            }

            if (NumberOfSliceVertices < 2)
            {
                hasSliceToDraw = false;
            }
            else
            {
                hasSliceToDraw = true;
                vbSlice = new VertexBuffer(device, VertexPositionColor.SizeInBytes * VertexList.Count, BufferUsage.WriteOnly);
                vbSlice.SetData(VertexList.ToArray());

                //set up indices
                int[] indices = new int[VertexList.Count];
                for (int x = 0; x < indices.Length; x++)
                {
                    indices[x] = x;
                }

                ibSlice = new IndexBuffer(device, typeof(int), indices.Length, BufferUsage.WriteOnly);
                ibSlice.SetData(indices);
            }
        }

        bool hasIntervalToDraw = false;
        private void SetupIntervalVertices()
        {
            List<VertexPositionColor> VertexList = new List<VertexPositionColor>();
            HorizonPosition hp;
            HorizonPosition hp1;
            VertexPositionColor v;
            NumberOfIntervalTriangles = 0;

            for (int j = 0; j < SlicePoints.Count - 1; j++)
            {
                hp = Data[SlicePoints[j].X, SlicePoints[j].Y];
                hp1 = Data[SlicePoints[j + 1].X, SlicePoints[j + 1].Y];

                float hpIntervalHeight = 0;
                float hp1IntervalHeight = 0;

                Color hpIntervalColor = Color.Black;
                Color hp1IntervalColor = Color.White;

                //check points are not null
                if (hp.Null)
                    continue;
                if (hp1.Null)
                    continue;
                if (hp.IntervalVelNull)
                    continue;
                if (hp1.IntervalVelNull)
                    continue;

                //check there is a horizon above this one
                if (AboveHorizon != null)
                {
                    //check above horizon point is not null
                    if (AboveHorizon.Data[SlicePoints[j].X, SlicePoints[j].Y].Null)
                        continue;
                    if (AboveHorizon.Data[SlicePoints[j + 1].X, SlicePoints[j + 1].Y].Null)
                        continue;

                    hpIntervalHeight = AboveHorizon.Data[SlicePoints[j].X, SlicePoints[j].Y].vertex.Position.Z;
                    hp1IntervalHeight = AboveHorizon.Data[SlicePoints[j + 1].X, SlicePoints[j + 1].Y].vertex.Position.Z;
                }

                //1st triangle
                v = new VertexPositionColor();
                v.Position = hp.vertex.Position;
                v.Color = hp.IntervalColour;
                VertexList.Add(v);

                v = new VertexPositionColor();
                v.Position = new Vector3(hp.vertex.Position.X, hp.vertex.Position.Y, hpIntervalHeight);
                v.Color = hp.IntervalColour;
                VertexList.Add(v);

                v = new VertexPositionColor();
                v.Position = hp1.vertex.Position;
                v.Color = hp1.IntervalColour;
                VertexList.Add(v);

                NumberOfIntervalTriangles++;

                //second 
                v = new VertexPositionColor();
                v.Position = new Vector3(hp.vertex.Position.X, hp.vertex.Position.Y, hpIntervalHeight);
                v.Color = hp.IntervalColour;
                VertexList.Add(v);

                v = new VertexPositionColor();
                v.Position = new Vector3(hp1.vertex.Position.X, hp1.vertex.Position.Y, hp1IntervalHeight);
                v.Color = hp1.IntervalColour;
                VertexList.Add(v);

                v = new VertexPositionColor();
                v.Position = hp1.vertex.Position;
                v.Color = hp1.IntervalColour;
                VertexList.Add(v);

                NumberOfIntervalTriangles++;
            }

            if (NumberOfIntervalTriangles < 1)
            {
                hasIntervalToDraw = false;
            }
            else
            {
                hasIntervalToDraw = true;
                vbInterval = new VertexBuffer(device, VertexPositionColor.SizeInBytes * VertexList.Count, BufferUsage.WriteOnly);
                vbInterval.SetData(VertexList.ToArray());
            }

        }

        /// <summary>
        /// Sub samples map data, maps can be too large to display reasonably.
        /// </summary>
        public void SubSample(int newWidth, int newHeight)
        {
            //check newHeight is lower than existing grid height
            if (newHeight >= GridHeight && newWidth >= GridWidth)
                return;

            HorizonPosition[,] DataSub = new HorizonPosition[newWidth, newHeight];

            XYIncrement.X = (MaxValue.X - MinValue.X) / (float)(newWidth - 1);
            XYIncrement.Y = (MaxValue.Y - MinValue.Y) / (float)(newHeight - 1);

            for (int x = 0; x < newWidth; x++)
            {
                for (int y = 0; y < newHeight; y++)
                {
                    float xpos = (float)x / (float)newWidth;
                    float ypos = (float)y / (float)newHeight;

                    HorizonPosition h;

                    h = GetHorizonPositionFromPoint(new Vector2(GridWidth * xpos, GridHeight * ypos));

                    h.GridPosition = new Point(x, y);
                    h.Value.X = MinValue.X + (x * XYIncrement.X);
                    h.Value.Y = MinValue.Y + (y * XYIncrement.Y);

                    DataSub[x, y] = h;
                }
            }

            Data = DataSub;
            GridWidth = newWidth;
            GridHeight = newHeight;
            XYIncrement.X = (MaxValue.X - MinValue.X) / (float)(GridWidth - 1);
            XYIncrement.Y = (MaxValue.Y - MinValue.Y) / (float)(GridHeight - 1);
        }

        public override void Reset()
        {
            Visible = true;

            if (ThisGridType == GridType.Horizon)
                Coloring = HorizonColoring.RainbowAll;
            else
                Coloring = HorizonColoring.Purple;
        }

        #endregion

        #region drawing

        public override void Draw()
        {
            if (!Visible || !hasHorizonToDraw)
                return;

            MyManager.Effect3D.Begin();
            MyManager.Effect3D.CurrentTechnique.Passes[0].Begin();

            device.Vertices[0].SetSource(vb, 0, VertexPositionNormalColored.SizeInBytes);

            device.Indices = ib;
            device.VertexDeclaration = VertDeclaration;

            device.DrawIndexedPrimitives(PrimitiveType.TriangleList, 0, 0, NumberOfGridTriangles * 3, 0, NumberOfGridTriangles);

            MyManager.Effect3D.CurrentTechnique.Passes[0].End();
            MyManager.Effect3D.End();
        }

        public void DrawSlice()
        {
            if (!Visible)
                return;

            MyManager.EffectLine.Begin();
            MyManager.EffectLine.CurrentTechnique.Passes[0].Begin();

            if (hasSliceToDraw)
            {
                device.Vertices[0].SetSource(vbSlice, 0, VertexPositionColor.SizeInBytes);
                device.Indices = ibSlice;
                device.VertexDeclaration = new VertexDeclaration(device, VertexPositionColor.VertexElements);

                device.DrawIndexedPrimitives(PrimitiveType.LineStrip, 0, 0, NumberOfSliceVertices, 0, NumberOfSliceVertices - 1);
            }

            if (ShowIntervalData && hasIntervalToDraw)
            {
                device.Vertices[0].SetSource(vbInterval, 0, VertexPositionColor.SizeInBytes);
                device.VertexDeclaration = new VertexDeclaration(device, VertexPositionColor.VertexElements);

                device.DrawPrimitives(PrimitiveType.TriangleList, 0, NumberOfIntervalTriangles);
            }

            MyManager.EffectLine.CurrentTechnique.Passes[0].End();
            MyManager.EffectLine.End();
        }

        #endregion

        #region IComparable<Horizon> Members

        public int CompareTo(Horizon other)
        {
            if (other.Identity > this.Identity)
                return -1;
            else if (other.Identity < this.Identity)
                return 1;
            else
                return 0;

            //if (other.MinValue.Z > this.MinValue.Z)
            //    return -1;
            //else if (other.MinValue.Z < this.MinValue.Z)
            //    return 1;
            //else
            //    return 0;
        }

        #endregion

        private HorizonPosition GetHorizonPositionFromPoint(Vector2 position)
        {
            //make sure coords are within horizon
            position.X = MathHelper.Clamp(position.X, 0, GridWidth);
            position.Y = MathHelper.Clamp(position.Y, 0, GridHeight);

            HorizonPosition newPos = Data[(int)position.X, (int)position.Y];

            return newPos;
        }

        private void InterpolateIntervalValue(float x, float y)
        {
            int LowerX = (int)(x - 0.5f);
            int UpperX = (int)(x + 0.5f);

            int LowerY = (int)(y - 0.5f);
            int UpperY = (int)(y + 0.5f);

        }

        public override void Dispose()
        {
            if (vbSlice != null)
                vbSlice.Dispose();
            if (ibSlice != null)
                ibSlice.Dispose();
            if (vbInterval != null)
                vbInterval.Dispose();

            AboveHorizon = null;
            Data = null;

            base.Dispose();
        }
    }

    class HorizonPosition
    {
        Horizon owner;

        //Current values
        public Vector3 Value;
        public bool Null;

        //scaled units for drawing
        public VertexPositionNormalColored vertex;
        public int Index;
        public Color IntervalColour;

        public HorizonPosition(Horizon owner)
        {
            this.owner = owner;
        }

        #region Properties for property grid

        [Description("Horizon"), Category("Information")]
        public string Horizon
        {
            get { return owner.Name; }
        }

        [Description("X, Y coordinates"), Category("Information"), ReadOnly(true)]
        public Vector2 Coordinates
        {
            get { return new Vector2(Value.X, Value.Y); }
        }

        [Description("The xy position in data grid"), Category("Information"), ReadOnly(true)]
        public Point GridPosition
        { get; set; }

        [Description("Depth at selected point"), Category("Information"), ReadOnly(true)]
        public float Depth
        { get; set; }

        [Description("Time at selected point"), Category("Information"), ReadOnly(true)]
        public float Time
        { get; set; }

        [Description("Interval at selected point"), Category("Information"), ReadOnly(true)]
        public float IntervalVelocity
        { get; set; }

        [Description("Average velocity at selected point"), Category("Information"), ReadOnly(true)]
        public float AverageVelocity
        { get; set; }

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
        public bool IntervalVelNull
        {
            get { return float.IsNaN(IntervalVelocity); }
        }

        [Browsable(false)]
        public bool AverageVelNull
        {
            get { return float.IsNaN(AverageVelocity); }
        }

        #endregion        

        public override string ToString()
        {
            string s = "";

            s += "Time: " + (TimeNull ? "Null" : Time.ToString("F"));
            s += " Depth: " + (DepthNull ? "Null" : Depth.ToString("F"));
            s += " Interval: " + (IntervalVelNull ? "Null" : IntervalVelocity.ToString("F"));

            return s;
        }
    }

    abstract class Object3D : IDisposable
    {
        protected Object3D(HorizonWellManager manager)
        {
            MyManager = manager;
            device = manager.Device;
            CurrentDataType = DataType.Depth;
            MyTreeNode = new TreeNode();
            MyTreeNode.Tag = this;
            MyTreeNode.NodeFont = new System.Drawing.Font("Courier New", 9, visible ? Drawing.FontStyle.Bold : System.Drawing.FontStyle.Regular);
            MyTreeNode.Checked = visible;
        }

        protected GraphicsDevice device;
        protected VertexDeclaration VertDeclaration;
        protected VertexBuffer vb;
        protected IndexBuffer ib;
        protected HorizonWellManager MyManager;
        [Browsable(false)]
        public DataType CurrentDataType { get; private set; }

        public Vector3 MinValue = new Vector3(float.MaxValue);
        public Vector3 MaxValue = new Vector3(float.MinValue);

        [Browsable(false)]
        public TreeNode MyTreeNode { get; private set; }

        string name = "";
        [Description("Name of the item"), ReadOnly(true), Category("Information")]
        public string Name
        {
            get { return name; }
            set
            {
                if (name != value)
                {
                    name = value;
                    MyTreeNode.Text = name;
                    MyTreeNode.ToolTipText = name;
                }
            }
        }

        bool visible = true;
        [Description("Is the item visible"), Category("Options")]
        public bool Visible
        {
            get { return visible; }
            set
            {
                if (visible != value)
                {
                    visible = value;
                    MyManager.NeedToRecalibrate();

                    MyTreeNode.Checked = visible;
                    MyTreeNode.NodeFont = new System.Drawing.Font("Courier New", 9, visible ? Drawing.FontStyle.Bold : System.Drawing.FontStyle.Regular);
                }
            }
        }

        public void SelectNode()
        {
            MyTreeNode.TreeView.SelectedNode = MyTreeNode;
            MyTreeNode.EnsureVisible();
        }

        public abstract void Initialise();
        protected abstract void FindMinMaxValues();
        public virtual void SetDataType(DataType t)
        {
            CurrentDataType = t;
        }
        public abstract void Reset();
        public abstract void SetupScaledValues();
        public abstract void SetupVertices();
        public abstract void Draw();

        #region IDisposable Members

        public virtual void Dispose()
        {
            if (vb != null)
                vb.Dispose();
            if (ib != null)
                ib.Dispose();
            device = null;
            MyManager = null;
        }

        #endregion

    }
}
